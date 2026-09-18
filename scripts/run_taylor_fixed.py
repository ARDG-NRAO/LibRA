#!/usr/bin/env python3
"""
Taylor Pipeline (FIXED) for Multi-SPW MTMFS Imaging.

This is a corrected variant of run_taylor.py. The four math bugs identified
against the librattviacasa reference + SynthesisUtilMethods.cc are fixed:

Pipeline state (corrected):
    Way In:
        roadrunner + dale  ->  S(nu) * PB(nu)              [flatnoise]
        removeFreqDepPB    ->  S(nu) * avgPB               [flatnoise-at-avgPB]
        cubeToTaylorSum    ->  tt_i = sum_chan PB_chan * (S * avgPB) * w_i
                                       / sum sumwt / avgPB
                            =  sum_chan (PB_chan / avgPB) * S * avgPB * w_i
                            ~= CASA Sault-weighted Taylor sum

    Way Out (norm_mode='matched'):
        hummbee model      ->  flat noise (S*avgPB), same scale as the RHS
        taylorToCube       ->  divides by avgPB (pbimage=<avgpb>) giving the
                               true-sky per-SPW model (tt0 + tt1*w), fed
                               straight to roadrunner, whose awp degridder
                               applies PB(nu) itself during predict. No
                               dale divmodel (that would over-subtract by
                               a further 1/PB -- see Bug 7 in memory).

Usage (run from a clean working directory next to the MS):
    export CASAPATH='<casadata root> linux_64b'   # required by coyote
    python3 run_taylor_fixed.py --config config.json --iterations 20 \
        --threshold 1e-5

Required config keys: bin_path, mset, spws, imsize, cell, phasecenter,
reffreq (Hz, e.g. "1.5e9"), reffreq_str (e.g. "1.5GHz"), cfcache (per-SPW
template like "cf_spw{spw}.cf" for multi-SPW runs), casa_python (a python
with casatools, used for weight normalization and pixel reads).
Optional: field, conjbeams, source_pixel, stokes, wplanes, nterms, gain,
cycleniter, cyclefactor, pblimit, convergence_factor.
"""

import argparse
import os
import re
import subprocess
import sys
from pathlib import Path
from typing import List, Dict, Optional
import logging
import json
from enum import Enum

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)


class ImageState(Enum):
    GRIDDING = "S(nu) * PB(nu)"
    FLATNOISE_AVGPB = "S(nu) * avgPB"
    TAYLOR = "Taylor terms (Sault weighted)"
    MODEL_OVER_AVGPB = "S_recon / avgPB"
    MODEL_FLATNOISE = "S_recon * PB(nu) / avgPB"


class TaylorPipeline:
    CMD_LOG = "app_output.log"

    def __init__(self, config_file: Optional[str] = None):
        self.config = self._load_config(config_file)
        self.spws = self.config['spws']
        self.basename = self.config['basename']
        self.bin_path = Path(self.config['bin_path'])
        self.iteration = 0
        self.converged = False
        self.model_flux_history = []
        self.minor_cycle_records = []
        self.last_stdout = ""
        self.last_stderr = ""
        self.norm_mode = self.config['norm_mode']
        # (Way-In avgPB divide, Way-Out avgPB divide)
        modes = {'matched': (True, True),
                 'flatsky': (True, False),
                 'flatnoise': (False, True)}
        if self.norm_mode not in modes:
            raise SystemExit(f"norm_mode must be one of {sorted(modes)}, "
                             f"got {self.norm_mode!r}")
        self.divide_wayin, self.divide_wayout = modes[self.norm_mode]
        Path(self.CMD_LOG).write_text("")   # fresh per-run app-output log

    REQUIRED_KEYS = ['bin_path', 'mset', 'spws', 'imsize', 'cell',
                     'phasecenter', 'reffreq', 'reffreq_str', 'cfcache',
                     'casa_python']

    def _load_config(self, config_file: Optional[str]) -> Dict:
        default_config = {
            'stokes': 'I',
            'wplanes': '1',
            'nterms': 2,
            'gain': 0.1,
            'cycleniter': 100,
            'cyclefactor': 1.0,
            'pblimit': 1e-6,
            'field': '',
            # Keep 0, matching CASA specmode='mvc', which tclean REFUSES to run
            # with conjbeams=True ("requires frequency-dependent primary beams
            # during cube gridding"). mvc is the correct CASA analogue of this
            # pipeline: cube major cycles, per-channel PBs retained through
            # gridding, frequency dependence handled afterwards by the Taylor
            # transform. specmode='mfs' is NOT the right reference -- it collapses
            # all SPWs into one gridding pass, which is where conjbeams=True has a
            # fractional bandwidth to act on.
            # Note the setting is inert here either way: coyote is invoked once per
            # SPW, so nSpw==1 in SynthesisUtils::expandFreqSelection (Utils.cc:950),
            # s and cs both resolve to 0, and the conjugate frequency comes back as
            # the SPW's own frequency. Measured 2026-08-06: conjbeams 0 vs 1 give
            # bit-identical dirty_spw*.pb and .residual, and identical converged
            # tt0/alpha. 0 is kept because it states what actually happens.
            'conjbeams': 0,
            'basename': 'dirty',
            # There are TWO independent avgPB divides, and they must both be on.
            #
            #   Way In  (cube2taylor, the pblimit branch of normalizeTerms,
            #            taken when pblimit > 0):
            #       turns the Sault-weighted sum S*avgPB^2 into S*avgPB, i.e. the
            #       flat-noise RHS that CASA's mtmfs minor cycle is also given.
            #       Measured: pre-norm 6216.51 / wtsum 25045.9 = 0.2482, / avgPB
            #       0.49116 = 0.5053, against CASA's 0.4781. Matches.
            #
            #   Way Out (taylorToCube's trailing applyPB("divide"), taken when
            #            pbimage != ""):
            #       hummbee's model comes out at the same scale as the residual it
            #       was given, i.e. flat noise (S*avgPB). roadrunner's awp degridder
            #       applies PB(nu) itself, so it must be handed TRUE SKY. Without
            #       this divide it subtracts S*avgPB*PB instead of S*PB, i.e. under-
            #       subtracts by avgPB every major cycle.
            #
            # 'matched' turns both on and is the correct setting. The other two are
            # kept only to reproduce the measured failure modes -- each leaves
            # exactly one stray factor of avgPB, and both under-subtract identically:
            #   'flatsky'   Way In only  -> model read as true sky when it is flat
            #               noise. Removes 0.455 of the peak per major cycle where
            #               0.865 is expected; 0.865*avgPB = 0.425. This is the
            #               historical default and the cause of the ~10-cycle lag
            #               behind CASA.
            #   'flatnoise' Way Out only -> RHS is S*avgPB^2, one avgPB too small,
            #               so the minor cycle under-produces by the same factor.
            'norm_mode': 'matched',
            'convergence_factor': 0.01
        }
        if not config_file:
            raise SystemExit("A --config JSON file is required. See the "
                             "module docstring for the key list.")
        if not Path(config_file).exists():
            raise SystemExit(f"Config file not found: {config_file}")
        with open(config_file, 'r') as f:
            default_config.update(json.load(f))
        missing = [k for k in self.REQUIRED_KEYS if k not in default_config]
        if missing:
            raise SystemExit(f"Config is missing required keys: {missing}")
        cfg = default_config
        cfg.setdefault('avgpbname', f"{cfg['basename']}_spw{cfg['spws'][0]}.avgpb")
        cfg.setdefault('minfreqpbname', f"{cfg['basename']}_spw{cfg['spws'][0]}.minfreq")
        for key in ('bin_path', 'casa_python'):
            if not Path(cfg[key]).exists():
                raise SystemExit(f"Config '{key}' does not exist: {cfg[key]}")
        if len(cfg['spws']) > 1 and isinstance(cfg['cfcache'], str) \
                and '{spw}' not in cfg['cfcache']:
            raise SystemExit(
                "Multi-SPW run with a single shared cfcache path: once SPW "
                f"{cfg['spws'][0]}'s coyote call creates '{cfg['cfcache']}', "
                "the other SPWs are silently skipped and grid with the wrong "
                "CFs. Use a template like 'cf_spw{spw}.cf' (or a list).")
        if 'CASAPATH' not in os.environ:
            raise SystemExit(
                "CASAPATH is not set. coyote silently writes EMPTY CF images "
                "(exit 0, no error) without it. Export e.g. "
                "CASAPATH='/var/home/pjaganna/.casa linux_64b' first.")
        return cfg

    # -------- utilities --------

    def _run_command(self, cmd: List[str], description: str = "") -> bool:
        """Run an app. stdout/stderr are appended to CMD_LOG rather than
        discarded -- the apps' own per-invocation reports (notably hummbee's
        MinorCycleSummary line) are the only record of minor-cycle accounting.
        """
        try:
            logger.debug(f"Running: {' '.join(cmd[:3])}...")
            proc = subprocess.run(cmd, check=True, capture_output=True, text=True)
            self.last_stdout = proc.stdout or ""
            self.last_stderr = proc.stderr or ""
            with open(self.CMD_LOG, 'a') as f:
                f.write(f"\n{'=' * 70}\n# {description}\n# {' '.join(cmd)}\n{'=' * 70}\n")
                f.write(self.last_stdout)
                if proc.stderr:
                    f.write("\n--- stderr ---\n" + proc.stderr)
            return True
        except subprocess.CalledProcessError as e:
            self.last_stdout = e.stdout or ""
            self.last_stderr = e.stderr or ""
            with open(self.CMD_LOG, 'a') as f:
                f.write(f"\n{'=' * 70}\n# FAILED: {description}\n# {' '.join(cmd)}\n{'=' * 70}\n")
                f.write(self.last_stdout)
                if e.stderr:
                    f.write("\n--- stderr ---\n" + e.stderr)
            logger.error(f"Failed: {description}")
            logger.error(f"Error: {e.stderr[:500] if e.stderr else 'No error output'}")
            return False

    def _tool(self, name: str) -> str:
        return str(self.bin_path / name)

    def _spw_images(self, suffix: str) -> List[str]:
        return [f"{self.basename}_spw{spw}.{suffix}" for spw in self.spws]

    def _csv(self, items: List[str]) -> str:
        return ','.join(items)

    def _log_state(self, description: str, state: ImageState):
        logger.info(f"  -> {description}: {state.value}")

    # -------- tool wrappers --------

    def _cfcache_for(self, spw: int) -> str:
        cf = self.config['cfcache']
        if isinstance(cf, list):
            return cf[self.spws.index(spw)]
        if isinstance(cf, str) and '{spw}' in cf:
            return cf.format(spw=spw)
        return cf

    def _build_cfcaches(self) -> bool:
        tool = self._tool('coyote')
        for spw in self.spws:
            cfcache = self._cfcache_for(spw)
            if Path(cfcache).exists():
                logger.info(f"cfcache {cfcache} exists, skipping coyote for SPW {spw}")
                continue
            base = [
                tool, "help=noprompt",
                f"vis={self.config['mset']}", "telescope=EVLA",
                f"imsize={self.config['imsize']}", f"cell={self.config['cell']}",
                f"stokes={self.config['stokes']}", f"reffreq={self.config['reffreq_str']}",
                f"phasecenter={self.config['phasecenter']}", f"wplanes={self.config['wplanes']}",
                f"cfcache={cfcache}", "wbawp=1", "aterm=1", "psterm=0",
                f"conjbeams={self.config['conjbeams']}",
                "muellertype=diagonal", "dpa=360",
                f"field={self.config['field']}", f"spw={spw}",
                "buffersize=0", "oversampling=20"
            ]
            if not self._run_command(base + ["mode=dryrun", "cflist="], f"coyote dryrun SPW {spw}"):
                return False
            if not self._run_command(base + ["mode=fillcf", "cflist=CFS*"], f"coyote fillcf SPW {spw}"):
                return False
            # coyote writes empty CF images (.im metadata, no table.dat) and
            # exits 0 when the telescope/band lookup or CASAPATH data fails;
            # a full run then "succeeds" on garbage. Fail loudly instead.
            filled = list(Path(cfcache).glob('CFS*.im/table.dat'))
            if not filled:
                logger.error(
                    f"cfcache {cfcache} contains no filled CF images "
                    "(no CFS*.im/table.dat). Check CASAPATH and that "
                    "OBSERVATION.TELESCOPE_NAME is 'EVLA'.")
                return False
        return True

    def _roadrunner(self, mode: str, with_model: bool = False) -> bool:
        tool = self._tool('roadrunner')
        for spw in self.spws:
            # roadrunner's awp degridder applies PB(nu) itself, so it must be fed
            # the TRUE-SKY model (tt0 + tt1*w) straight from taylor2cube. The old
            # .divmodel path handed it Sintr/PB, which degrids to Sintr (no PB
            # attenuation) and over-subtracts by 1/PB (freq-dependent, ~1.5-2.8x).
            model_arg = f"{self.basename}_spw{spw}.model" if with_model else ""
            cmd = [
                tool, "help=noprompt",
                f"vis={self.config['mset']}",
                f"imagename={self.basename}_spw{spw}.{mode}",
                f"modelimagename={model_arg}",
                "datacolumn=data",
                "sowimageext=sumwt",
                f"imsize={self.config['imsize']}",
                f"cell={self.config['cell']}",
                f"stokes={self.config['stokes']}",
                f"phasecenter={self.config['phasecenter']}",
                "weighting=natural",
                "robust=0",
                f"wprojplanes={self.config['wplanes']}",
                "gridder=awphpg",
                f"cfcache={self._cfcache_for(spw)}",
                f"mode={mode}",
                "wbawp=true",
                f"spw={spw}",
                f"field={self.config['field']}",
                "pbcor=1",
                f"conjbeams={self.config['conjbeams']}",
                f"pblimit={self.config['pblimit']}",
                "reffreq=mean"
            ]
            desc = f"roadrunner {mode} SPW {spw}" + (" with model" if with_model else "")
            if not self._run_command(cmd, desc):
                return False
        return True

    def _dale(self, imtype: str) -> bool:
        tool = self._tool('dale')
        for spw in self.spws:
            # taylorpsf operates on the .psf file but writes PSF peak to .taylorwt
            file_ext = 'psf' if imtype == 'taylorpsf' else imtype
            cmd = [
                tool, "help=noprompt",
                f"imagename={self.basename}_spw{spw}.{file_ext}",
                f"imtype={imtype}",
                f"weightimage={self.basename}_spw{spw}.weight",
                f"sowimage={self.basename}_spw{spw}.{'sumwt' if imtype == 'taylorpsf' else 'taylorwt'}",
                "pblimit=0.0001",
                "computepb=1"
            ]
            if not self._run_command(cmd, f"dale {imtype} SPW {spw}"):
                return False
        return True

    def _normalize_weight_images(self) -> bool:
        """Normalize each SPW's weight image by taylorwt (the PSF peak for that SPW).
        taylorwt = max(weight_raw) / sumwt_chan, matching CASA's stored weight image
        which is weight_raw / sumwt_chan. This gives itsPBScaleFactor = sqrt(max(weight_raw)/taylorwt)
        per SPW in dale, reproducing CASA's frequency-dependent PB scale factor."""
        import re
        import pathlib
        casa_py = self.config['casa_python']
        for spw in self.spws:
            wt_path = f"{self.basename}_spw{spw}.weight"
            tw_path = f"{self.basename}_spw{spw}.taylorwt"
            snippet = (
                "from casatools import image as IA\n"
                "import re, pathlib, numpy as np\n"
                f"ia = IA()\n"
                f"ia.open('{tw_path}')\n"
                "tw = float(ia.getchunk().flat[0])\n"
                "ia.close()\n"
                f"ia.open('{wt_path}')\n"
                "wt = ia.getchunk()\n"
                "wt /= tw\n"
                "ia.putchunk(wt)\n"
                "ia.close()\n"
                f"p = pathlib.Path('{wt_path}/table.info')\n"
                "txt = p.read_text()\n"
                "txt = re.sub(r'SubType\\s*=.*', 'SubType = weight normalized', txt)\n"
                "p.write_text(txt)\n"
                f"print(f'SPW {spw}: weight normalized by taylorwt={{tw:.2f}}, peak now {{float(wt.max()):.6f}}')\n"
            )
            try:
                import subprocess as sp
                result = sp.run([casa_py, '-c', snippet],
                                capture_output=True, text=True, check=True)
                logger.info(result.stdout.strip())
            except Exception as e:
                logger.error(f"Failed to normalize weight image for SPW {spw}: {e}")
                return False
        return True

    def _hummbee(self, mode: str = "deconvolve") -> bool:
        tool = self._tool('hummbee')
        cmd = [
            tool, "help=noprompt",
            f"imagename={self.basename}",
            "modelimagename=",
            "deconvolver=mtmfs",
            "scales=",
            "largestscale=-1",
            "fusedthreshold=0",
            f"nterms={self.config['nterms']}",
            f"gain={self.config['gain']}",
            "nsigma=0",
            f"threshold={getattr(self, 'threshold', 0.0)}",
            f"cycleniter={self.config['cycleniter']}",
            f"cyclefactor={self.config['cyclefactor']}",
            f"mask={self.config.get('mask', '')}",
            "specmode=mfs",
            "pbcor=0",
            f"mode={mode}"
        ]
        ok = self._run_command(cmd, f"hummbee {mode}")
        if ok and mode == "deconvolve":
            self._log_minor_cycle_summary()
        return ok

    # SDAlgorithmBase::deconvolve emits e.g.:
    #   [Field0_libra] iters=0->379 [379], model=0.302863->0.385085,
    #   peakres=0.00750883->0.00121081, Reached cyclethreshold.
    # There is no literal "MinorCycleSummary" string anywhere in hummbee's
    # source (checked 2026-08-12) -- the old regex never matched.
    MINOR_RE = re.compile(r'(iters=.*)')

    def _log_minor_cycle_summary(self):
        """Surface hummbee's own per-invocation minor-cycle report into the
        driver log, so one line per major cycle carries iterdone/peakresidual.
        """
        # casacore LogIO writes to stderr, not stdout, so scan both streams.
        m = None
        combined = getattr(self, 'last_stdout', '') + "\n" + getattr(self, 'last_stderr', '')
        for line in combined.splitlines():
            hit = self.MINOR_RE.search(line)
            if hit:
                m = hit.group(1).strip()
        if m:
            logger.info(f"Minor cycle: {m}")
            self.minor_cycle_records.append(m)
        else:
            logger.warning("hummbee emitted no iters=... minor-cycle line "
                           "(check hummbee stderr for errors)")

    # -------- taylor mode wrappers (one method per CLI mode) --------

    def _taylor_compute_avgpb(self) -> bool:
        cmd = [
            self._tool('taylor'), "help=noprompt",
            f"cubeImage={self._csv(self._spw_images('residual'))}",
            f"pbimage={self._csv(self._spw_images('pb'))}",
            "taylorImages=",
            f"sumwtImage={self._csv(self._spw_images('taylorwt'))}",
            "overwrite=1",
            "mode=computeavgpb",
            f"avgpbname={self.config['avgpbname']}",
            f"minfreqpbname={self.config['minfreqpbname']}",
            # taylor's CLI accepts only {mean, median} (taylor_cl_interface.cc
            # clSetOptions); an unrecognized value silently falls back to the
            # default, so say what actually runs.
            "avgpbmode=mean"
        ]
        return self._run_command(cmd, "taylor computeavgpb")


    def _taylor_cube2taylor(self, imtype: str, prefix: Optional[str] = None) -> bool:
        prefix = prefix or self.basename
        # Pass per-SPW PBs and append avgPB last so:
        #   - cubeToTaylorSum indexes pbnames[idx] for the per-channel premultiply
        #   - it uses pbnames.back() = avgPB for the pblimit-thresholded divide
        # The N+1 contract is validated up front, so a short list now throws.
        pb_list = self._spw_images('pb') + [self.config['avgpbname']]
        # taylorwt (PSF peak per SPW) is the stable Hessian weight — never clobbered
        # by roadrunner residual passes. Use it as sumwtImage for all imtypes so the
        # Hessian (psf/sumwt Taylor terms) and residual RHS stay consistently weighted.
        # For imtype=sumwt the cubeImage is also taylorwt (the Hessian diagonal itself
        # is built from PSF peaks, not raw gridding sumwt).
        taylorwts = self._spw_images('taylorwt')
        cube_images = taylorwts if imtype == 'sumwt' else self._spw_images(imtype)
        cmd = [
            self._tool('taylor'), "help=noprompt",
            f"cubeImage={self._csv(cube_images)}",
            f"taylorImages={prefix}.{imtype}",
            f"pbimage={self._csv(pb_list)}",
            f"sumwtImage={self._csv(taylorwts)}",
            "overwrite=1",
            "mode=cube2taylor",
            f"reffreq={self.config['reffreq']}",
            f"nTerms={self.config['nterms']}",
            # Way-In avgPB divide: pblimit > 0 takes the branch at
            # the pblimit branch of cubeToTaylorSum's normalization, turning
            # the Sault-weighted sum S*avgPB^2 into
            # the flat-noise RHS S*avgPB that CASA's mtmfs is also given.
            # pblimit=0 takes the plain /wtsum branch instead
            # (and drops that branch's pblimit masking with it).
            f"pblimit={self.config['pblimit'] if self.divide_wayin else 0}",
            f"imtype={imtype}"
        ]
        return self._run_command(cmd, f"taylor cube2taylor ({imtype})")

    def _taylor_taylor2cube(self, prefix: Optional[str] = None) -> bool:
        prefix = prefix or self.basename
        # Way-Out avgPB divide, via taylorToCube's trailing
        # `if (!pbnames.empty())` -> applyPB("divide", pbnames[0]).
        # hummbee returns a model at the same scale as the residual it was
        # given, i.e. flat noise (S*avgPB). roadrunner's awp degridder applies
        # PB(nu) itself, so it must be handed TRUE SKY. Skipping this divide
        # (the historical pbimage="") makes it subtract S*avgPB*PB instead of
        # S*PB: measured 0.455 of the peak removed per major cycle where 0.865
        # is expected, and 0.865*avgPB = 0.425.
        taylor_images = [f"{prefix}.model.tt{i}" for i in range(self.config['nterms'])]
        cmd = [
            self._tool('taylor'), "help=noprompt",
            f"cubeImage={self._csv(self._spw_images('model'))}",
            f"taylorImages={self._csv(taylor_images)}",
            f"pbimage={self.config['avgpbname'] if self.divide_wayout else ''}",
            f"sumwtImage={self._csv(self._spw_images('sumwt'))}",
            "overwrite=1",
            "mode=taylor2cube",
            f"reffreq={self.config['reffreq']}",
            f"nTerms={self.config['nterms']}",
            "pblimit=0",
            "imtype=model"
        ]
        return self._run_command(cmd, "taylor taylor2cube")

    def _taylor_remove_freqdep_pb(self) -> bool:
        """Way-In flattening: replace each per-SPW residual (flatnoise = S*PB_chan)
        with S*avgPB in place, via `taylor mode=removefreqdepPB` (avgPB*cube/PB_chan).
        This is required so the PB premultiply inside cube2taylor acts as a per-channel
        sensitivity WEIGHT rather than squaring the beam. Matches CASA
        SynthesisUtilMethods::removeFreqDepPB, which CASA calls on the residual cube
        before cubeToTaylorSum. Residuals are regenerated by dale each major cycle,
        so the in-place edit does not accumulate."""
        cmd = [
            self._tool('taylor'), "help=noprompt",
            f"cubeImage={self._csv(self._spw_images('residual'))}",
            f"pbimage={self._csv(self._spw_images('pb'))}",
            f"avgpbname={self.config['avgpbname']}",
            "taylorImages=",
            "sumwtImage=",
            "overwrite=1",
            "mode=removefreqdepPB",
            f"pblimit={self.config['pblimit']}"
        ]
        return self._run_command(cmd, "taylor removefreqdepPB (residual)")


    def stage_imaging(self, with_model: bool = False) -> bool:
        logger.info("Stage 1: Per-channel imaging")
        if with_model:
            # Subsequent major cycles: only residual needed; dale uses SoW_residual.
            if not self._roadrunner("residual", with_model=True):
                return False
            if not self._dale("residual"):
                return False
        else:
            # Initial setup: psf -> weight -> taylorpsf -> residual -> dale.
            # Weight is raw (not pre-normalized). Dale.cc line 188 divides weight by SoW
            # (taylorwt) internally: normWt = weight / taylorwt, so
            # itsPBScaleFactor = sqrt(max(weight_raw)/taylorwt), matching CASA.
            if not self._roadrunner("psf"):
                return False
            if not self._roadrunner("weight"):
                return False
            if not self._dale("taylorpsf"):
                return False
            if not self._roadrunner("residual"):
                return False
            if not self._dale("residual"):
                return False
        self._log_state("Per-channel images", ImageState.GRIDDING)
        return True

    def stage_residual_to_taylor(self) -> bool:
        """Way In for residual: removeFreqDepPB (flatten S*PB_chan -> S*avgPB),
        then cube2taylor. cube2taylor multiplies by pb_chan internally and divides
        by avgPB at the end. The removeFreqDepPB step is required so the pb_chan
        premultiply acts as a per-channel sensitivity WEIGHT; without it the residual
        carries an extra PB_chan/avgPB factor (the per-SPW frequency slope that
        corrupted tt1/alpha). Matches CASA cubeToTaylorSum fed by removeFreqDepPB."""
        logger.info("Stage 2: Residual cube -> Taylor")
        self._log_state("Before removeFreqDepPB", ImageState.GRIDDING)
        if not self._taylor_remove_freqdep_pb():
            return False
        self._log_state("Before cube2taylor", ImageState.GRIDDING)
        if not self._taylor_cube2taylor("residual"):
            return False
        self._log_state("After cube2taylor", ImageState.TAYLOR)
        return True

    def stage_psf_pb_to_taylor(self) -> bool:
        """Way In for PSF / PB / sumwt support images. NO flatsky, NO
        removeFreqDepPB - cube2taylor is fed flatnoise PSF/PB/sumwt directly,
        matching CASA SynthesisUtilMethods.cc cubeToTaylorSum."""
        logger.info("Stage 2b: PSF/PB/sumwt support cubes -> Taylor")
        for imtype in ["psf", "pb", "sumwt"]:
            if not self._taylor_cube2taylor(imtype):
                return False
        return True

    def stage_deconvolve(self) -> bool:
        logger.info("Stage 3: Deconvolution")
        if not self._hummbee("deconvolve"):
            return False
        self._log_state("Model Taylor terms", ImageState.TAYLOR)
        return True

    def stage_taylor_to_cube(self) -> bool:
        """Way Out part 1: taylor2cube divides out avgPB internally."""
        logger.info("Stage 4: Taylor -> Cube")
        if not self._taylor_taylor2cube():
            return False
        self._log_state("Model cube", ImageState.MODEL_OVER_AVGPB)
        return True

    def _strip_normalized_subtype(self, image_paths: List[str]) -> None:
        """Strip 'normalized' from table.info SubType so dale isNormalized() does not
        skip normalization. taylor2cube and applyPB inherit the residual SubType
        ('residual normalized') from the source image, which causes dale model to exit
        early without writing divmodel in every cycle after the first."""
        for img in image_paths:
            info = Path(img) / "table.info"
            if info.exists():
                txt = info.read_text()
                out = []
                for line in txt.splitlines():
                    if line.startswith("SubType"):
                        line = (line.replace(" normalized", "")
                                    .replace("normalized", "")
                                    .rstrip())
                        if line.endswith("="):
                            line = "SubType = model"
                    out.append(line)
                info.write_text("\n".join(out) + "\n")

    def stage_prepare_model(self) -> bool:
        """Way Out part 2: NONE. taylor2cube already produced the true-sky
        per-SPW model (tt0 + tt1*w). roadrunner's awp degridder applies PB(nu)
        during predict, so the true-sky model is exactly what it must be fed.
        The previous applyPB(divide avgPB) + applyPB(multiply PB) + dale divmodel
        chain produced Sintr/PB, which over-subtracts by 1/PB in the major cycle.
        We only strip the 'normalized' SubType so roadrunner reads the model."""
        logger.info("Stage 5: model is true-sky (tt0+tt1*w); roadrunner applies PB on degrid")
        model_cubes = self._spw_images('model')
        self._strip_normalized_subtype(model_cubes)
        self._log_state("True-sky per-SPW model", ImageState.MODEL_OVER_AVGPB)
        return True

    # -------- pipeline control --------

    def _source_pixels(self) -> List[List[int]]:
        """Source pixel list from config: 'source_pixels' ([[x,y],...]) or the
        legacy single 'source_pixel' ([x,y]). Empty list if neither is set."""
        if 'source_pixels' in self.config:
            return self.config['source_pixels']
        if 'source_pixel' in self.config:
            return [self.config['source_pixel']]
        return []

    def _setup_mask(self):
        pixels = self._source_pixels()
        if not pixels:
            logger.info("No source_pixel(s) in config; skipping mask")
            return
        lines = ["#CRTFv0"] + [f"circle[[{sx}pix, {sy}pix], 0.5pix]"
                               for sx, sy in pixels]
        with open("source.mask", 'w') as f:
            f.write("\n".join(lines) + "\n")
        logger.info(f"Created source mask at pixels {pixels}")

    def _create_model_images(self):
        for spw in self.spws:
            src = Path(f"{self.basename}_spw{spw}.residual")
            dst = Path(f"{self.basename}_spw{spw}.model")
            if src.exists():
                import shutil
                if dst.exists():
                    shutil.rmtree(dst)
                shutil.copytree(src, dst)
                # Strip the "normalized" SubType marker so dale's isNormalized
                # guard at dale.cc:276 does NOT skip the model normalization.
                # table.info is plain text: lines like 'SubType = residual normalized'.
                info = dst / "table.info"
                if info.exists():
                    txt = info.read_text()
                    new = []
                    for line in txt.splitlines():
                        if line.startswith("SubType"):
                            line = (line.replace(" normalized", "")
                                        .replace("normalized", "")
                                        .rstrip())
                            if line.endswith("="):
                                line = "SubType = model"
                        new.append(line)
                    info.write_text("\n".join(new) + "\n")

    def _check_convergence(self) -> bool:
        try:
            current_flux = self._get_model_flux()
            if len(self.model_flux_history) > 0:
                prev_flux = self.model_flux_history[-1]
                change = abs(current_flux - prev_flux) / abs(prev_flux) if prev_flux != 0 else 1.0
                logger.info(f"Model flux: {current_flux:.6f} Jy (change: {change:.2%})")
                if change < self.config['convergence_factor']:
                    logger.info("Converged!")
                    return True
            self.model_flux_history.append(current_flux)
            return False
        except Exception as e:
            logger.warning(f"Could not check convergence: {e}")
            return False

    def _get_model_flux(self) -> float:
        """Read tt0 (and tt1) at the source pixel from <basename>.model.tt{i}
        via libra's own utilities2py.getchunk(path) (the PagedImage-backed
        overload; axis order x, y, pol, chan like casatools). Done in a
        subprocess because casatools and utilities2py must not be imported
        into the same process, and this driver may run under either."""
        reader = self.config['casa_python']
        libdir = self.config.get('libra_pylib',
                                 str(self.bin_path.parent / 'lib'))
        pixels = self._source_pixels()
        tt0 = f"{self.basename}.model.tt0"
        tt1 = f"{self.basename}.model.tt1"
        if not pixels or not Path(tt0).exists():
            return float('nan')
        pix_arg = ';'.join(f"{sx},{sy}" for sx, sy in pixels)
        snippet = (
            "import sys, utilities2py\n"
            "a0 = utilities2py.getchunk(sys.argv[1])\n"
            "try:\n"
            "    a1 = utilities2py.getchunk(sys.argv[2])\n"
            "except Exception:\n"
            "    a1 = None\n"
            "for tok in sys.argv[3].split(';'):\n"
            "    x, y = (int(v) for v in tok.split(','))\n"
            "    v1 = float(a1[x,y,0,0]) if a1 is not None else float('nan')\n"
            "    print(float(a0[x,y,0,0]), v1)\n"
        )
        env = {**os.environ,
               'PYTHONPATH': libdir, 'LD_LIBRARY_PATH': libdir}
        try:
            res = subprocess.run([reader, '-c', snippet, tt0, tt1, pix_arg],
                                 env=env,
                                 capture_output=True, text=True, check=True)
            total_tt0 = 0.0
            for (sx, sy), line in zip(pixels, res.stdout.strip().splitlines()):
                tt0_v, tt1_v = (float(x) for x in line.split())
                alpha = tt1_v / tt0_v if tt0_v != 0 else float('nan')
                logger.info(f"Source pixel ({sx},{sy}): tt0={tt0_v:.5f} Jy, "
                            f"tt1/tt0={alpha:+.4f}")
                total_tt0 += tt0_v
            return total_tt0
        except (subprocess.CalledProcessError, ValueError) as e:
            err = getattr(e, 'stderr', str(e))
            logger.warning(f"Flux readout failed: {str(err)[:300]}")
            return float('nan')

    def initial_setup(self) -> bool:
        logger.info("=" * 60)
        logger.info("INITIAL SETUP (fixed)")
        logger.info("=" * 60)
        self._setup_mask()
        if not self._build_cfcaches():
            return False
        if not self.stage_imaging(with_model=False):
            return False
        logger.info("Computing average PB...")
        if not self._taylor_compute_avgpb():
            return False
        if not self.stage_psf_pb_to_taylor():
            return False
        if not self.stage_residual_to_taylor():
            return False
        if getattr(self, 'stop_before_hummbee', False):
            logger.info("=" * 60)
            logger.info("STOPPING BEFORE HUMMBEE (--stop-before-hummbee)")
            logger.info(f"Inspect {self.basename}.{{residual,psf,pb,sumwt}}.tt* now.")
            logger.info("=" * 60)
            return True
        if not self.stage_deconvolve():
            return False
        if getattr(self, 'stop_after_deconvolve', False):
            logger.info("=" * 60)
            logger.info("STOPPING AFTER DECONVOLVE (--stop-after-deconvolve)")
            logger.info(f"Inspect {self.basename}.model.tt* now.")
            logger.info("=" * 60)
            return True
        self._create_model_images()
        if not self.stage_taylor_to_cube():
            return False
        if getattr(self, 'stop_after_taylor2cube', False):
            logger.info("=" * 60)
            logger.info("STOPPING AFTER TAYLOR2CUBE (--stop-after-taylor2cube)")
            logger.info(f"{self.basename}_spw*.model = tt0 + tt1*w  (bare polynomial, no PB applied)")
            logger.info("=" * 60)
            return True
        if not self.stage_prepare_model():
            return False
        if getattr(self, 'stop_after_initial_setup', False):
            logger.info("=" * 60)
            logger.info("STOPPING AFTER INITIAL SETUP (--stop-after-initial-setup)")
            logger.info(f"{self.basename}_spw*.model = (tt0+tt1*w)*PB_spw/avgPB after 1 deconvolve")
            logger.info("=" * 60)
            return True
        logger.info("Initial setup complete")
        return True

    def major_cycle(self) -> bool:
        logger.info("=" * 60)
        logger.info(f"MAJOR CYCLE {self.iteration}")
        logger.info("=" * 60)
        if self.iteration > 0:
            if not self.stage_imaging(with_model=True):
                return False
            if getattr(self, 'stop_after_residual_update', False):
                logger.info("=" * 60)
                logger.info("STOPPING AFTER RESIDUAL UPDATE (--stop-after-residual-update)")
                logger.info(f"{self.basename}_spw*.residual = updated residual after model subtraction")
                logger.info("=" * 60)
                self.converged = True
                return True
            if not self.stage_residual_to_taylor():
                return False
        if not self.stage_deconvolve():
            return False
        if self._check_convergence():
            self.converged = True
            return True
        if not self.stage_taylor_to_cube():
            return False
        if not self.stage_prepare_model():
            return False
        return True

    def final_restore(self) -> bool:
        logger.info("=" * 60)
        logger.info("FINAL RESTORATION")
        logger.info("=" * 60)
        if not self.stage_imaging(with_model=True):
            return False
        if not self.stage_residual_to_taylor():
            return False
        if not self._hummbee("restore"):
            return False
        logger.info("Restoration complete")
        return True

    def run(self, max_iterations: int = 10, threshold: float = 0.0,
            stop_before_hummbee: bool = False, stop_after_deconvolve: bool = False,
            stop_after_taylor2cube: bool = False, stop_after_initial_setup: bool = False,
            stop_after_residual_update: bool = False) -> bool:
        self.threshold = threshold
        self.stop_before_hummbee = stop_before_hummbee
        self.stop_after_deconvolve = stop_after_deconvolve
        self.stop_after_taylor2cube = stop_after_taylor2cube
        self.stop_after_initial_setup = stop_after_initial_setup
        self.stop_after_residual_update = stop_after_residual_update
        logger.info("=" * 60)
        logger.info("TAYLOR PIPELINE (FIXED) - MTMFS VIA CUBE")
        logger.info("=" * 60)
        logger.info(f"Max iterations: {max_iterations}, threshold: {threshold}")
        logger.info(f"SPWs: {self.spws}, nTerms: {self.config['nterms']}")
        logger.info(f"norm_mode: {self.norm_mode} "
                    f"(avgPB divide -- Way In: {self.divide_wayin}, "
                    f"Way Out: {self.divide_wayout})")
        logger.info("Way In : removeFreqDepPB -> cube2taylor (per-SPW PB list)")
        logger.info("Way Out: taylor2cube -> applyPB(per_spw_pb, multiply) -> dale")
        logger.info("=" * 60)
        if not self.initial_setup():
            logger.error("Initial setup failed")
            return False
        if self.stop_before_hummbee or self.stop_after_deconvolve or self.stop_after_taylor2cube or self.stop_after_initial_setup:
            return True
        for i in range(max_iterations):
            self.iteration = i + 1
            if not self.major_cycle():
                logger.error(f"Major cycle {self.iteration} failed")
                return False
            if self.converged:
                logger.info(f"Converged after {self.iteration} iterations")
                break
        if getattr(self, 'stop_after_residual_update', False):
            return True
        logger.info(f"Completed {self.iteration} iterations")
        if not self.final_restore():
            logger.error("Final restoration failed")
            return False
        logger.info("=" * 60)
        logger.info("PIPELINE COMPLETED SUCCESSFULLY")
        logger.info("=" * 60)
        return True


def main():
    parser = argparse.ArgumentParser(description='Taylor Pipeline (FIXED) for Multi-SPW MTMFS Imaging')
    parser.add_argument('--config', type=str, help='Configuration JSON file')
    parser.add_argument('--iterations', type=int, default=10, help='Maximum iterations')
    parser.add_argument('--threshold', type=float, default=0.0, help='Cleaning threshold (Jy)')
    parser.add_argument('--debug', action='store_true', help='Enable debug logging')
    parser.add_argument('--stop-before-hummbee', action='store_true',
                        help='Run Way-In through cube2taylor and exit; leaves <basename>.{residual,psf,pb,sumwt}.tt* on disk for inspection.')
    parser.add_argument('--stop-after-deconvolve', action='store_true',
                        help='Run Way-In + hummbee and exit; leaves <basename>.model.tt* on disk for inspection.')
    parser.add_argument('--stop-after-taylor2cube', action='store_true',
                        help='Run Way-In + hummbee + taylor2cube and exit; leaves <basename>_spw*.model = tt0+tt1*w on disk (no PB applied).')
    parser.add_argument('--stop-after-initial-setup', action='store_true',
                        help='Run full initial setup (1 deconvolve + Way-Out) and exit; no major cycles or restore.')
    parser.add_argument('--stop-after-residual-update', action='store_true',
                        help='Run initial setup + first major cycle residual re-image and exit; <basename>_spw*.residual = updated residual.')
    args = parser.parse_args()
    if args.debug:
        logging.getLogger().setLevel(logging.DEBUG)
    pipeline = TaylorPipeline(args.config)
    success = pipeline.run(max_iterations=args.iterations, threshold=args.threshold,
                           stop_before_hummbee=args.stop_before_hummbee,
                           stop_after_deconvolve=args.stop_after_deconvolve,
                           stop_after_taylor2cube=args.stop_after_taylor2cube,
                           stop_after_initial_setup=args.stop_after_initial_setup,
                           stop_after_residual_update=args.stop_after_residual_update)
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
