"""
libra.clean — a tclean-equivalent image -> deconvolve loop over LibRA apps.

This is the Python analogue of CASA's ``task_tclean.py`` main major/minor cycle,
minus the bells and whistles (no mosaics, cubes, parallel-cube, mvc, automasking,
interactive, history, etc.). It drives the LibRA components in-process via their
pybind bindings:

    roadrunner  -> imaging derivative (weight / psf / residual gridding; GPU capable)
    dale        -> normalization (psf / residual / model) and primary beam
    hummbee     -> minor cycle (deconvolve) and restore

It is a faithful port of ``frameworks/htclean/libra_htclean.sh`` (fixed-``ncycle``
loop; no threshold/nsigma-based convergence yet). ``Hummbee`` returns the peak
residual each minor cycle, which is logged and returned, and is the intended hook
for real tclean-style convergence in a future version.

GPU: gridding runs on GPU when ``gridder="awphpg"`` (the default). This driver is
single-process / single-GPU; multi-GPU means splitting the MS and fanning out the
``roadrunner`` calls, which is left to the htcimager/slurm frameworks. """

import json
import logging
import os
import subprocess
import sys
from pathlib import Path

import libra

log = logging.getLogger("libra.clean")


# Subprocess worker: import libra (runs the *2py loader), then call
# module.callable(*args, **kwargs). If a result fd was passed, write the JSON
# return value to it. Kept as a source string so it runs via `python -c`.
_WORKER = (
    "import sys, json, os\n"
    "sys.path.insert(0, sys.argv[1])\n"
    "import libra\n"
    "spec = json.loads(sys.argv[2])\n"
    "mod = __import__(spec['module'])\n"
    "ret = getattr(mod, spec['callable'])(*spec['args'], **spec['kwargs'])\n"
    "fd = spec.get('result_fd')\n"
    "if fd is not None:\n"
    "    os.write(fd, json.dumps(ret).encode()); os.close(fd)\n"
)


def _run(module, callable_name, *args, want_result=False, **kwargs):
    """Run ONE LibRA component call ``module.callable(*args, **kwargs)`` in a
    fresh subprocess; return its return value (or ``None``).

    Every component runs out-of-process for one consistent reason: the ``*2py``
    bindings are not safe to call repeatedly in a single interpreter. roadrunner
    keeps non-reentrant file-scope globals; dale leaks casacore PagedImage
    handles into the process-global table cache (holding the ``.sumwt`` lock
    until process exit), which zeros the next roadrunner residual. A fresh
    process per call resets all of it — a single call is bit-identical to the
    standalone CLI, and this mirrors how ``libra_htclean.sh`` / htcimager already
    run each step as its own process. 
    stdout/stderr are inherited so the (chatty) component logs stream live. A
    return value comes back over a dedicated pipe fd (``want_result=True``), NOT
    stdout — the components write their logs to stdout, so the result needs its
    own clean channel. Only hummbee needs it (its peak residual); roadrunner and
    dale return ``None``.
    """
    libra_python = os.path.dirname(os.path.dirname(os.path.abspath(libra.__file__)))
    spec = {"module": module, "callable": callable_name,
            "args": list(args), "kwargs": kwargs}

    r = w = None
    if want_result:
        r, w = os.pipe()
        spec["result_fd"] = w

    cmd = [sys.executable, "-c", _WORKER, libra_python, json.dumps(spec)]
    try:
        result = subprocess.run(cmd, pass_fds=(w,) if want_result else ())
    finally:
        if w is not None:
            os.close(w)  # drop the parent's write end so the read end sees EOF

    if result.returncode != 0:
        if r is not None:
            os.close(r)
        raise RuntimeError(
            f"{module}.{callable_name} subprocess failed (exit {result.returncode}); "
            f"args={args} kwargs={kwargs}. If this is an ImportError the *2py bindings "
            f"did not load in this interpreter (Python "
            f"{'.'.join(map(str, sys.version_info[:2]))}) — rebuild them for this "
            f"Python or run under the matching one."
        )

    if want_result:
        data = b""
        while True:
            chunk = os.read(r, 65536)
            if not chunk:
                break
            data += chunk
        os.close(r)
        return json.loads(data.decode()) if data else None
    return None


class CleanParams:
    """Parameters for :func:`libra_tclean`, fanned out to the per-app calls.

    App argument names differ (e.g. ``dale`` uses ``imageName``); the fan-out
    happens inside :func:`libra_tclean`, not here.

    A plain class (not a ``dataclass``) so the module imports under Python 3.6,
    the ABI the ``*2py`` bindings are built against; ``dataclasses`` is 3.7+.
    """

    def __init__(
        self,
        # --- data selection / image definition ---
        vis, imagename, imsize, cell, phasecenter,
        datacolumn="data", stokes="I", reffreq="3GHz",
        field="*", spw="*", uvrange="",
        # --- gridding ---
        gridder="awphpg", cfcache="", weighting="natural", rmode="norm",
        robust=0.0, wprojplanes=1, wbawp=True, conjbeams=True,
        usepointing=False, pblimit=0.2, pbcor=True, sowimageext="sumwt",
        # --- normalization ---
        normtype="flatnoise",
        # --- deconvolution (minor cycle) ---
        deconvolver="hogbom", scales=None, largestscale=-1,
        fusedthreshold=0.0, nterms=1, gain=0.1, threshold=0.0, nsigma=0.0,
        cycleniter=-1, cyclefactor=1.0, mask="", specmode="mfs",
        # --- MT-MFS (multi-term, nterms>1) ---
        # spws: explicit SPW ids for the multi-SPW MT-MFS pipeline (per-SPW
        #   gridding + avgPB). None => single-term path (uses `spw` above).
        # reffreq_hz: Taylor reference freq in Hz (e.g. "1.5e9"), required by the
        #   taylor cube<->taylor conversion; distinct from `reffreq` ("3GHz") used
        #   for gridding coordinate setup.
        # cfcache with `spws`: a per-SPW template like "cf_spw{spw}.cf" or a list.
        spws=None, reffreq_hz="", avgpbname="", minfreqpbname="", avgpbmode="mean",
        # --- iteration control ---
        ncycle=10,
    ):
        self.vis = vis
        self.imagename = imagename
        self.imsize = imsize
        self.cell = cell
        self.phasecenter = phasecenter
        self.datacolumn = datacolumn
        self.stokes = stokes
        self.reffreq = reffreq
        self.field = field
        self.spw = spw
        self.uvrange = uvrange
        self.gridder = gridder
        self.cfcache = cfcache
        self.weighting = weighting
        self.rmode = rmode
        self.robust = robust
        self.wprojplanes = wprojplanes
        self.wbawp = wbawp
        self.conjbeams = conjbeams
        self.usepointing = usepointing
        self.pblimit = pblimit
        self.pbcor = pbcor
        self.sowimageext = sowimageext
        self.normtype = normtype
        self.deconvolver = deconvolver
        self.scales = scales if scales is not None else []
        self.largestscale = largestscale
        self.fusedthreshold = fusedthreshold
        self.nterms = nterms
        self.gain = gain
        self.threshold = threshold
        self.nsigma = nsigma
        self.cycleniter = cycleniter
        self.cyclefactor = cyclefactor
        self.mask = mask
        self.specmode = specmode
        self.spws = spws
        self.reffreq_hz = reffreq_hz
        self.avgpbname = avgpbname or (f"{imagename}.avgpb" if spws else "")
        self.minfreqpbname = minfreqpbname or (f"{imagename}.minfreqpb" if spws else "")
        self.avgpbmode = avgpbmode
        self.ncycle = ncycle


def _roadrunner(p, mode, modelimagename=""):
    """Grid ``mode`` (weight|psf|residual) with roadrunner, in a fresh subprocess.

    Gridding apps take the *extended* image name (``base.<mode>``) plus the mode;
    the app strips the extension to recover the prefix.
    """
    imagename = f"{p.imagename}.{mode}"
    log.info("roadrunner: mode=%s imagename=%s model=%s",
             mode, imagename, modelimagename or "<none>")
    _run("roadrunner2py", "Roadrunner", **dict(
        vis=p.vis,
        imagename=imagename,
        modelimagename=modelimagename,
        datacolumn=p.datacolumn,
        sowimageext=p.sowimageext,
        imsize=p.imsize,
        wprojplanes=p.wprojplanes,
        cell=p.cell,
        stokes=p.stokes,
        reffreq=p.reffreq,
        phasecenter=p.phasecenter,
        weighting=p.weighting,
        rmode=p.rmode,
        robust=p.robust,
        gridder=p.gridder,
        cfcache=p.cfcache,
        mode=mode,
        wbawp=p.wbawp,
        field=p.field,
        spw=p.spw,
        uvrange=p.uvrange,
        usepointing=p.usepointing,
        pbcor=p.pbcor,
        conjbeams=p.conjbeams,
        pblimit=p.pblimit,
    ))


def _dale(p, imtype):
    """Normalize ``imtype`` (psf|residual|model); computes the PB for psf.

    ``dale`` takes the bare base name and appends the extension internally.
    For ``model`` it writes ``{base}.divmodel``.
    """
    log.info("dale: imType=%s imagename=%s", imtype, p.imagename)
    # Pass the extended target name explicitly (e.g. "test.im.psf"). dale only
    # auto-appends ".<imtype>" when the name has no extension, and getExtension()
    # treats a dotted base like "test.im" as already-extensioned -> it would look
    # for a bare "test.im" table and fail. Constructing the name here makes it
    # robust to dots in the base; dale derives .weight/.sumwt/.pb/.divmodel from it.
    # Run in a subprocess: dale leaks casacore table handles/locks that otherwise
    # zero out the next roadrunner residual write (see _run).
    return _run("dale2py", "dale",
        imageName=f"{p.imagename}.{imtype}",
        normtype=p.normtype,
        imType=imtype,
        pblimit=p.pblimit,
        computePB=(imtype == "psf"),
    )


def _fix_model_subtype(imagename):
    """The sed-hack from libra_htclean.sh.

    ``dale`` decides an image still needs normalization by string-matching the
    word "normalized" in the CASA image's ``table.info`` SubType. ``hummbee
    deconvolve`` leaves the new ``.model`` tagged ``SubType =  normalized``, so
    the following ``dale`` model-normalize no-ops and skips the divide-by-weights.
    Strip the token to force the division.

    TODO: this is a label-vs-reality mismatch that belongs in the C++ (hummbee
    writing the correct subType, or dale not gating solely on this string). Keep
    faithfully for now. 
    """
    info = Path(f"{imagename}.model") / "table.info"
    if not info.exists():
        raise FileNotFoundError(
            f"{imagename}.model not found, possibly due to a failure in a "
            f"previous imaging step. Check the input parameters."
        )
    text = info.read_text()
    info.write_text(text.replace("SubType =  normalized", "SubType ="))


def _hummbee(p, mode):
    """Run hummbee minor cycle (deconvolve) or restore. Returns peak residual."""
    log.info("hummbee: mode=%s imagename=%s", mode, p.imagename)
    # want_result=True: hummbee returns the peak residual (the convergence hook),
    # read back over a pipe fd. roadrunner/dale return None (want_result=False).
    return _run("hummbee2py", "Hummbee",
        p.imagename,
        f"{p.imagename}.image",  # modelimagename (restored image name)
        p.deconvolver,
        list(p.scales),
        p.largestscale,
        p.fusedthreshold,
        p.nterms,
        p.gain,
        p.threshold,
        p.nsigma,
        p.cycleniter,
        p.cyclefactor,
        # hummbee's mask is std::vector<std::string>; the binding needs a list,
        # not the "" default (which fails the List[str] type check). Empty list
        # means "no mask"; a set mask is passed as a single-element list.
        [p.mask] if p.mask else [],
        p.specmode,
        p.pbcor,
        mode,
        want_result=True,
    )


# ---------------------------------------------------------------------------
# MT-MFS (multi-term, nterms>1) multi-SPW path.
#
# This mirrors ``scripts/run_taylor_fixed.py`` (the branch's validated MTMFS
# driver) stage-for-stage, but drives every component through the ``*2py``
# bindings (via ``_run``) instead of the CLI binaries. The scientific recipe —
# per-SPW gridding, avgPB, the taylor Way-In (removeFreqDepPB -> cube2taylor) and
# Way-Out (taylor2cube -> true-sky model handoff, NO ``.divmodel``) — is taken
# verbatim from that script; see its docstring for the four PB/avgPB bugs it
# fixed. Keep the two in lock-step. 
# ---------------------------------------------------------------------------

def _spw_images(p, suffix):
    """Per-SPW image names ``{imagename}_spw{spw}.{suffix}``."""
    return [f"{p.imagename}_spw{spw}.{suffix}" for spw in p.spws]


def _cfcache_for(p, spw):
    """Resolve the per-SPW cfcache from a list or a ``{spw}`` template."""
    cf = p.cfcache
    if isinstance(cf, (list, tuple)):
        return cf[p.spws.index(spw)]
    if isinstance(cf, str) and "{spw}" in cf:
        return cf.format(spw=spw)
    return cf


def _mtmfs_roadrunner(p, spw, mode, modelimagename=""):
    """Per-SPW gridding. Feeds roadrunner the TRUE-SKY model (its awp degridder
    applies PB(nu) itself), so no ``.divmodel``. ``reffreq=mean`` matches
    run_taylor_fixed (the awp reffreq for the per-SPW grid)."""
    base = f"{p.imagename}_spw{spw}"
    log.info("mtmfs roadrunner: spw=%s mode=%s model=%s",
             spw, mode, modelimagename or "<none>")
    _run("roadrunner2py", "Roadrunner", **dict(
        vis=p.vis, imagename=f"{base}.{mode}", modelimagename=modelimagename,
        datacolumn=p.datacolumn, sowimageext=p.sowimageext,
        imsize=p.imsize, wprojplanes=p.wprojplanes, cell=p.cell,
        stokes=p.stokes, reffreq="mean", phasecenter=p.phasecenter,
        weighting=p.weighting, rmode=p.rmode, robust=p.robust,
        gridder=p.gridder, cfcache=_cfcache_for(p, spw), mode=mode,
        wbawp=p.wbawp, field=p.field, spw=str(spw), uvrange=p.uvrange,
        usepointing=p.usepointing, pbcor=p.pbcor, conjbeams=p.conjbeams,
        pblimit=p.pblimit,
    ))


def _mtmfs_dale(p, spw, imtype):
    """Per-SPW normalization. ``taylorpsf`` normalizes the ``.psf`` and writes the
    PSF peak to ``.taylorwt`` (the stable Hessian weight); other imtypes write
    ``.taylorwt`` as the sum-of-weights image. Mirrors run_taylor_fixed._dale."""
    base = f"{p.imagename}_spw{spw}"
    file_ext = "psf" if imtype == "taylorpsf" else imtype
    sow = f"{base}.sumwt" if imtype == "taylorpsf" else f"{base}.taylorwt"
    log.info("mtmfs dale: spw=%s imtype=%s", spw, imtype)
    _run("dale2py", "dale",
         imageName=f"{base}.{file_ext}",
         wtimageName=f"{base}.weight",
         sowimageName=sow,
         normtype=p.normtype, imType=imtype,
         pblimit=1e-4, computePB=True)


def _taylor(p, mode, **kwargs):
    """Thin wrapper over the taylor2py binding for one ``mode`` call."""
    log.info("taylor: mode=%s", mode)
    _run("taylor2py", "taylor", mode=mode, overwrite=True,
         pblimit=p.pblimit, **kwargs)


def _taylor_computeavgpb(p):
    _taylor(p, "computeavgpb",
            cubeImage=_spw_images(p, "residual"),
            pbimage=_spw_images(p, "pb"),
            sumwtImage=_spw_images(p, "taylorwt"),
            avgpbname=p.avgpbname, minfreqpbname=p.minfreqpbname,
            avgpbmode=p.avgpbmode)


def _taylor_removefreqdepPB(p, imtype="residual"):
    """Way-In flatten: cube_chan -> cube_chan * avgPB / PB_chan, so the PB
    premultiply inside cube2taylor acts as a per-channel sensitivity weight."""
    _taylor(p, "removefreqdepPB",
            cubeImage=_spw_images(p, imtype),
            pbimage=_spw_images(p, "pb"),
            avgpbname=p.avgpbname)


def _taylor_cube2taylor(p, imtype):
    """Way-In: per-SPW cube -> Taylor terms. PB list is per-SPW PBs + avgPB last
    (taylor uses pbnames.back()=avgPB for the pblimit-thresholded divide, and
    per-SPW PB per channel because size>1). ``taylorwt`` (PSF peaks) is the
    Hessian weight for all imtypes; for ``sumwt`` it is also the cube input."""
    taylorwts = _spw_images(p, "taylorwt")
    cube_images = taylorwts if imtype == "sumwt" else _spw_images(p, imtype)
    _taylor(p, "cube2taylor",
            cubeImage=cube_images,
            taylorImages=[f"{p.imagename}.{imtype}"],
            pbimage=_spw_images(p, "pb") + [p.avgpbname],
            sumwtImage=taylorwts,
            reffreq=p.reffreq_hz, nTerms=p.nterms, imtype=imtype)


def _taylor_taylor2cube(p):
    """Way-Out: model Taylor terms -> per-SPW true-sky model cubes (tt0+tt1*w).
    ``pbimage=[]`` skips taylor's internal PB divide; roadrunner applies PB(nu)
    on degrid, so the bare polynomial is exactly what it must be fed."""
    taylor_images = [f"{p.imagename}.model.tt{i}" for i in range(p.nterms)]
    _taylor(p, "taylor2cube",
            cubeImage=_spw_images(p, "model"),
            taylorImages=taylor_images,
            pbimage=[],
            sumwtImage=_spw_images(p, "sumwt"),
            reffreq=p.reffreq_hz, nTerms=p.nterms, imtype="model")


def _strip_normalized_subtype(image_paths):
    """Strip the ``normalized`` token from each image's ``table.info`` SubType so
    dale's isNormalized() guard does not skip re-normalization. taylor2cube
    inherits the residual's ``... normalized`` SubType, which otherwise makes dale
    (and roadrunner's model read) treat the model as already-normalized."""
    for img in image_paths:
        info = Path(img) / "table.info"
        if not info.exists():
            continue
        out = []
        for line in info.read_text().splitlines():
            if line.startswith("SubType"):
                line = (line.replace(" normalized", "")
                            .replace("normalized", "").rstrip())
                if line.endswith("="):
                    line = "SubType = model"
            out.append(line)
        info.write_text("\n".join(out) + "\n")


def _create_model_containers(p):
    """Create the per-SPW ``.model`` cubes taylor2cube writes into, by copying the
    ``.residual`` (same shape/coords) and stripping the ``normalized`` SubType.
    Done once, before the first taylor2cube; taylor2cube overwrites in place."""
    import shutil
    for spw in p.spws:
        src = Path(f"{p.imagename}_spw{spw}.residual")
        dst = Path(f"{p.imagename}_spw{spw}.model")
        if not src.exists():
            raise FileNotFoundError(
                f"{src} missing — the residual gridding step did not produce it.")
        if dst.exists():
            shutil.rmtree(dst)
        shutil.copytree(src, dst)
    _strip_normalized_subtype(_spw_images(p, "model"))


def _mtmfs_stage_imaging(p, with_model):
    """Per-SPW imaging. Initial: psf -> weight -> dale(taylorpsf) -> residual ->
    dale(residual). Subsequent (with_model): residual(model) -> dale(residual)."""
    for spw in p.spws:
        if with_model:
            _mtmfs_roadrunner(p, spw, "residual",
                              modelimagename=f"{p.imagename}_spw{spw}.model")
            _mtmfs_dale(p, spw, "residual")
        else:
            _mtmfs_roadrunner(p, spw, "psf")
            _mtmfs_roadrunner(p, spw, "weight")
            _mtmfs_dale(p, spw, "taylorpsf")
            _mtmfs_roadrunner(p, spw, "residual")
            _mtmfs_dale(p, spw, "residual")


def _libra_tclean_mtmfs(p):
    """MT-MFS major/minor cycle loop, mirroring run_taylor_fixed.py via bindings.

    Initial: image -> computeavgpb -> cube2taylor(psf,pb,sumwt) ->
             removeFreqDepPB+cube2taylor(residual) -> deconvolve ->
             make model containers -> taylor2cube -> prepare model.
    Loop:    image(with model) -> removeFreqDepPB+cube2taylor(residual) ->
             deconvolve -> taylor2cube -> prepare model.
    Restore: image(with model) -> removeFreqDepPB+cube2taylor(residual) -> restore.

    Fixed ``ncycle`` (like the single-term path); flux/threshold convergence is a
    TODO (run_taylor_fixed does it via source-pixel tt0 readout — needs config the
    library API does not yet carry). Returns per-cycle peak residuals.
    """
    if not p.spws:
        raise ValueError("MT-MFS (nterms>1 / deconvolver='mtmfs') requires "
                         "explicit `spws=[...]` for per-SPW gridding + avgPB.")
    if not p.reffreq_hz:
        raise ValueError("MT-MFS requires `reffreq_hz` (Hz, e.g. '1.5e9') for the "
                         "taylor cube<->taylor conversion.")

    def _way_in_residual():
        _taylor_removefreqdepPB(p, "residual")
        _taylor_cube2taylor(p, "residual")

    # --- Initial setup ---
    _mtmfs_stage_imaging(p, with_model=False)
    _taylor_computeavgpb(p)
    for imtype in ("psf", "pb", "sumwt"):
        _taylor_cube2taylor(p, imtype)
    _way_in_residual()

    peak_residuals = []
    peak = _hummbee(p, "deconvolve")
    peak_residuals.append(peak)
    log.info("mtmfs cycle 0: peak residual = %s", peak)
    _create_model_containers(p)
    _taylor_taylor2cube(p)
    _strip_normalized_subtype(_spw_images(p, "model"))

    # --- Major/minor cycle loop (fixed ncycle) ---
    for i in range(1, p.ncycle):
        _mtmfs_stage_imaging(p, with_model=True)
        _way_in_residual()
        peak = _hummbee(p, "deconvolve")
        peak_residuals.append(peak)
        log.info("mtmfs cycle %d/%d: peak residual = %s", i, p.ncycle - 1, peak)
        _taylor_taylor2cube(p)
        _strip_normalized_subtype(_spw_images(p, "model"))

    # --- Final restore ---
    _mtmfs_stage_imaging(p, with_model=True)
    _way_in_residual()
    _hummbee(p, "restore")
    log.info("mtmfs done: %d cycles, final peak residual = %s",
             len(peak_residuals), peak_residuals[-1] if peak_residuals else "n/a")
    return peak_residuals


def libra_tclean(vis, imagename, imsize, cell, phasecenter, **kwargs):
    """Make an image via the LibRA major/minor cycle loop.

    Mirrors ``frameworks/htclean/libra_htclean.sh``:

        weight -> psf -> normalize(psf)+PB -> dirty(residual) -> normalize(residual)
        loop ncycle:  deconvolve -> [sed-hack] -> normalize(model)
                      -> residual(divmodel) -> normalize(residual)
        restore

    For **MT-MFS** (``deconvolver="mtmfs"`` or ``nterms>1``) this dispatches to the
    multi-SPW Taylor pipeline (:func:`_libra_tclean_mtmfs`), which mirrors
    ``scripts/run_taylor_fixed.py``. That path additionally requires ``spws=[...]``
    and ``reffreq_hz`` (see :class:`CleanParams`), and uses the ``taylor`` app +
    ``dale imtype=taylorpsf``; it hands roadrunner a true-sky model (no
    ``.divmodel``).

    Extra keyword arguments are forwarded to :class:`CleanParams`.

    Returns the list of per-cycle peak residuals (from ``hummbee deconvolve``).
    """
    p = CleanParams(vis=vis, imagename=imagename, imsize=imsize, cell=cell,
                    phasecenter=phasecenter, **kwargs)

    if p.deconvolver == "mtmfs" or p.nterms > 1:
        return _libra_tclean_mtmfs(p)

    # --- Setup: weights, PSF, PB, dirty image ---
    _roadrunner(p, "weight")
    _roadrunner(p, "psf")
    _dale(p, "psf")               # normalize PSF + make PB
    _roadrunner(p, "residual")    # dirty image
    _dale(p, "residual")

    # --- Major/minor cycle loop (fixed ncycle) ---
    peak_residuals = []
    for i in range(1, p.ncycle):
        peak = _hummbee(p, "deconvolve")
        peak_residuals.append(peak)
        log.info("cycle %d/%d: peak residual = %s", i, p.ncycle - 1, peak)

        _fix_model_subtype(p.imagename)
        _dale(p, "model")         # divide model by weights -> .divmodel
        _roadrunner(p, "residual", modelimagename=f"{p.imagename}.divmodel")
        _dale(p, "residual")

    # --- Restore ---
    _hummbee(p, "restore")
    log.info("done: %d cycles, final peak residual = %s",
             len(peak_residuals), peak_residuals[-1] if peak_residuals else "n/a")
    return peak_residuals
