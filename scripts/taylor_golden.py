#!/usr/bin/env python3
"""Behavior pin for the `taylor` application.

The Taylor pipeline is validated against CASA to sub-1% in spectral index on
refim_oneshiftpoint.mosaic.ms.  A silent arithmetic regression in taylor.cc is
expensive and hard to detect, so this script records what the app outputs and
lets a later version prove it did not move.

It drives the parafeed CLI, not the C++ API, for two reasons: the CLI is the
stable contract (it is what scripts/run_taylor_fixed.py uses), and a C++ test
program cannot compile against both the old and the new API.

Inputs are synthetic images built to hit every branch.  Bit-for-bit agreement
on images designed to exercise the branches is a stronger statement than
equality on one real dataset, and it needs no MS, no cfcache and no CASAPATH.

Usage:
    taylor_golden.py --record baseline.json [--workdir DIR] [--taylor PATH]
    taylor_golden.py --compare baseline.json [--workdir DIR] [--taylor PATH]

Requires a python with casatools on the path.  On this machine that is
/var/home/pjaganna/Software/casa_sim/.pixi/envs/default/bin/python3
"""

import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys

import numpy as np

try:
    from casatools import image as _image
    from casatools import coordsys as _coordsys_tool
except ImportError:
    sys.exit("casatools not importable; run this with the casa python "
             "(see the module docstring)")

# Fixture geometry.  Small enough to be fast, large enough that the PB has a
# real above/below pblimit structure and an off-axis zero region.
NX = NY = 32
GHZ = 1.0e9

# The three-SPW set straddles reffreq 1.5 GHz, so computeWeights gives
# -0.2, 0.0, +0.2.  A flat spectrum then drives tt1 to zero ONLY when the
# per-SPW sumwt values are equal: the accumulation is sumwt-weighted, so
# unequal weights leave a residual tt1 even for a flat source.  Measured on
# the default falling-sumwt fixtures: tt1 absmax 0.0498 against tt0 max 1.0.
SPW3_FREQS = [1.2 * GHZ, 1.5 * GHZ, 1.8 * GHZ]
# Four SPWs so computeAvgPB's median path takes its even-count branch.
SPW4_FREQS = [1.2 * GHZ, 1.4 * GHZ, 1.6 * GHZ, 1.8 * GHZ]

REFFREQ_IN = "1.5GHz"        # inside the band
REFFREQ_OUT = "3.0GHz"       # outside it, exercises checkRefFreqRange


# --------------------------------------------------------------------------
# fixture construction
# --------------------------------------------------------------------------

def _coordsys(freq_hz):
    """Canonical (direction, stokes, spectral) coordinate system.

    The direction coordinate spans two pixel axes, so the spectral *coordinate*
    index is 2 while the spectral *pixel axis* is 3.  That mismatch is the bug
    this pipeline has been bitten by; the fixtures deliberately use the layout
    where the two differ.
    """
    cs = _coordsys_tool().newcoordsys(direction=True, stokes="I", spectral=True)
    # axis order comes out (Right Ascension, Declination, Stokes, Frequency)
    cs.setreferencevalue(value=[freq_hz], type="spectral")
    cs.setincrement(value=0.1 * freq_hz, type="spectral")
    cs.setreferencepixel(value=[NX / 2.0, NY / 2.0], type="direction")
    return cs


def _write(path, arr, freq_hz):
    """Create a PagedImage at `path` holding `arr` at frequency `freq_hz`."""
    if os.path.exists(path):
        shutil.rmtree(path)
    ia = _image()
    cs = _coordsys(freq_hz)
    ia.fromarray(outfile=path, pixels=arr.astype(np.float32),
                 csys=cs.torecord(), overwrite=True)
    ia.close()
    ia.done()
    cs.done()


def _gaussian_pb(freq_hz, ref_freq=1.5 * GHZ):
    """A PB that narrows with frequency, with a hard zero outside the support.

    Gives pixels above pblimit, pixels below it, and an exactly-zero region so
    the divide guards are actually exercised.
    """
    y, x = np.mgrid[0:NY, 0:NX]
    r2 = (x - NX / 2.0) ** 2 + (y - NY / 2.0) ** 2
    width = 90.0 * (ref_freq / freq_hz) ** 2
    pb = np.exp(-r2 / width)
    pb[r2 > (0.45 * NX) ** 2] = 0.0
    return pb.T.reshape(NX, NY, 1, 1)


def _point_source(alpha, freq_hz, ref_freq=1.5 * GHZ, with_nan=False):
    """A flat or power-law point source, optionally with NaN pixels.

    alpha == 0 is the flat case (see SPW3_FREQS on when tt1 goes to zero).
    The NaN block
    exercises the isNaN masking branch in the accumulation.
    """
    arr = np.zeros((NX, NY, 1, 1), dtype=np.float32)
    arr[NX // 2, NY // 2, 0, 0] = (freq_hz / ref_freq) ** alpha
    arr[NX // 4, NY // 4, 0, 0] = 0.5 * (freq_hz / ref_freq) ** alpha
    if with_nan:
        arr[0:2, 0:2, 0, 0] = np.nan
    return arr


def _psf(freq_hz):
    y, x = np.mgrid[0:NY, 0:NX]
    r2 = (x - NX / 2.0) ** 2 + (y - NY / 2.0) ** 2
    return np.exp(-r2 / 4.0).T.reshape(NX, NY, 1, 1)


def _sumwt(value):
    return np.full((1, 1, 1, 1), value, dtype=np.float32)


def build_fixtures(root, freqs, alpha=0.0, with_nan=False, tag="spw",
                   equal_sumwt=False):
    """Write one full per-SPW image set and return the name lists.

    Returns a dict of lists in the order the CLI expects them, plus the
    prefixes used for Taylor outputs.
    """
    os.makedirs(root, exist_ok=True)
    names = {"residual": [], "psf": [], "pb": [], "sumwt": [], "model": []}
    for i, f in enumerate(freqs):
        base = os.path.join(root, "%s%d" % (tag, i))
        _write(base + ".residual", _point_source(alpha, f, with_nan=with_nan), f)
        _write(base + ".psf", _psf(f), f)
        _write(base + ".pb", _gaussian_pb(f), f)
        # By default the weights fall with frequency, as real per-SPW
        # sensitivity does.  equal_sumwt gives every SPW the same weight, which
        # is what makes the symmetric-band tt1 == 0 invariant hold; see the
        # cube2taylor_residual_flat_equalwt case.
        _write(base + ".sumwt",
               _sumwt(1000.0 if equal_sumwt else 1000.0 * (1.5 * GHZ / f)), f)
        _write(base + ".model", _point_source(alpha, f), f)
        for k in names:
            names[k].append(base + "." + k)
    return names


# --------------------------------------------------------------------------
# hashing
# --------------------------------------------------------------------------

def hash_image(path):
    """md5 of an image's pixels and mask.

    NaN is canonicalized to a single bit pattern before hashing: IEEE permits
    many NaN payloads and we care that a pixel is undefined, not which NaN it
    happens to be.  The mask is hashed separately because the PB branch of
    cubeToTaylorSum writes one and a lost mask is a real regression.
    """
    ia = _image()
    ia.open(path)
    try:
        pix = np.asarray(ia.getchunk(), dtype=np.float32)
        try:
            msk = np.asarray(ia.getchunk(getmask=True))
        except Exception:
            msk = np.ones(pix.shape, dtype=bool)
    finally:
        ia.close()
        ia.done()

    pix = np.where(np.isnan(pix), np.float32(np.nan), pix)
    canon = pix.copy()
    canon[np.isnan(canon)] = np.float32(-9.87654321e30)  # sentinel, not a NaN
    h = hashlib.md5()
    h.update(np.ascontiguousarray(canon).tobytes())
    h.update(np.ascontiguousarray(msk.astype(np.uint8)).tobytes())
    return h.hexdigest()


def hash_tree(root):
    """Hash every image directory directly under `root`.

    Hashing the whole case directory rather than a declared output list is
    deliberate: several modes (applyPB, removeFreqDepPB) edit their inputs in
    place, and an unintended write to an input is exactly the kind of
    regression this pin exists to catch.
    """
    out = {}
    for name in sorted(os.listdir(root)):
        p = os.path.join(root, name)
        if not os.path.isdir(p) or not os.path.exists(os.path.join(p, "table.dat")):
            continue
        try:
            out[name] = hash_image(p)
        except Exception as e:
            out[name] = "UNREADABLE: %s" % e
    return out


# --------------------------------------------------------------------------
# CLI driving
# --------------------------------------------------------------------------

def csv(items):
    return ",".join(items)


def run_taylor(taylor_bin, cwd, **kw):
    """Invoke the taylor CLI once.

    Returns (returncode, threw, tail).

    `threw` is not redundant with `returncode`.  main() in
    taylor_cl_interface.cc catches AipsError, prints "###AipsError:" to stderr,
    sets restartUI, and then falls off the end of main returning 0 -- in
    non-interactive mode RestartUI expands to nothing.  So a thrown error and
    a clean run are indistinguishable by exit code, and scanning stderr is the
    only reliable signal the pin has.
    """
    cmd = [taylor_bin, "help=noprompt"]
    for k, v in kw.items():
        cmd.append("%s=%s" % (k, v))
    p = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True)
    both = p.stdout + p.stderr
    threw = "###AipsError" in both
    tail = both.strip().splitlines()
    return p.returncode, threw, "\n".join(tail[-3:])


class Recorder:
    """Runs one case in its own directory and records the resulting hashes."""

    def __init__(self, taylor_bin, workdir):
        self.taylor = taylor_bin
        self.workdir = workdir
        self.results = {}

    def case(self, name):
        d = os.path.join(self.workdir, name)
        if os.path.exists(d):
            shutil.rmtree(d)
        os.makedirs(d)
        return d

    def record(self, name, d, rc, threw, tail):
        entry = {"returncode": rc, "threw": threw, "images": hash_tree(d)}
        if rc != 0 or threw:
            # Strip the absolute workdir so the baseline is machine independent.
            entry["tail"] = tail.replace(self.workdir, "<workdir>")
        self.results[name] = entry
        status = "ok" if (rc == 0 and not threw) else \
                 ("threw" if rc == 0 else "rc=%d" % rc)
        print("  %-40s %-6s (%d images)" % (name, status, len(entry["images"])))


# --------------------------------------------------------------------------
# the cases
# --------------------------------------------------------------------------

def run_all_cases(rec):
    """Every CLI mode, over inputs chosen to reach both sides of each branch."""

    # ---- computeavgpb: mean, median odd count, median even count ----------
    for tag, freqs, mode in (("mean_3spw", SPW3_FREQS, "mean"),
                             ("median_3spw", SPW3_FREQS, "median"),
                             ("median_4spw", SPW4_FREQS, "median")):
        d = rec.case("computeavgpb_" + tag)
        n = build_fixtures(d, freqs)
        rc, threw, tail = run_taylor(rec.taylor, d,
                              pbimage=csv(n["pb"]),
                              avgpbname="avgpb",
                              minfreqpbname="minfreqpb",
                              avgpbmode=mode,
                              taylorImages="", cubeImage="", sumwtImage="",
                              overwrite=1, mode="computeavgpb")
        rec.record("computeavgpb_" + tag, d, rc, threw, tail)

    # ---- removefreqdepPB / applyfreqdepPB --------------------------------
    # Both edit the residuals in place, so each gets a fresh fixture set.
    for mode, pblimit in (("removefreqdepPB", 0.1), ("removefreqdepPB", 0.0),
                          ("applyfreqdepPB", 0.1), ("applyfreqdepPB", 0.0)):
        name = "%s_pblimit%g" % (mode, pblimit)
        d = rec.case(name)
        n = build_fixtures(d, SPW3_FREQS)
        _make_avgpb(rec, d, n)
        rc, threw, tail = run_taylor(rec.taylor, d,
                              cubeImage=csv(n["residual"]),
                              pbimage=csv(n["pb"]),
                              avgpbname="avgpb",
                              taylorImages="", sumwtImage="",
                              overwrite=1, mode=mode, pblimit=pblimit)
        rec.record(name, d, rc, threw, tail)

    # ---- applyPB multiply / divide ---------------------------------------
    # The direction used to ride on the imtype field and now has its own
    # `pbmode` parameter.  The baseline rows were recorded from the old binary
    # driven with imtype; comparing them against the new binary driven with
    # pbmode is still valid, because the pin hashes output pixels rather than
    # command lines, and the arithmetic is what is under test.
    for direction, pblimit in (("multiply", 0.0), ("divide", 0.0), ("divide", 0.1)):
        name = "applyPB_%s_pblimit%g" % (direction, pblimit)
        d = rec.case(name)
        n = build_fixtures(d, SPW3_FREQS)
        rc, threw, tail = run_taylor(rec.taylor, d,
                              pbimage=n["pb"][0],
                              cubeImage=csv(n["residual"]),
                              taylorImages="", sumwtImage="",
                              overwrite=1, mode="applyPB",
                              pbmode=direction, pblimit=pblimit)
        rec.record(name, d, rc, threw, tail)

    # ---- cube2taylor: 4 imtypes x 2 pblimit branches x nterms 2 and 3 ----
    for nterms in (2, 3):
        for imtype in ("residual", "psf", "pb", "sumwt"):
            for pblimit in (0.1, 0.0):
                name = "cube2taylor_%s_n%d_pblimit%g" % (imtype, nterms, pblimit)
                d = rec.case(name)
                n = build_fixtures(d, SPW3_FREQS)
                _make_avgpb(rec, d, n)
                # The pbnames contract: one per-SPW PB per cube, in the same
                # order, with avgpb appended last.
                pbs = n["pb"] + [os.path.join(d, "avgpb")]
                rc, threw, tail = run_taylor(rec.taylor, d,
                                      cubeImage=csv(n[imtype]),
                                      taylorImages="out",
                                      pbimage=csv(pbs),
                                      sumwtImage=csv(n["sumwt"]),
                                      overwrite=1, mode="cube2taylor",
                                      reffreq=REFFREQ_IN, nTerms=nterms,
                                      pblimit=pblimit, imtype=imtype)
                rec.record(name, d, rc, threw, tail)

    # A flat spectrum with reffreq at band centre must drive tt1 to zero, and
    # a reffreq outside the band must still be recorded so the warn-vs-throw
    # decision on checkRefFreqRange is visible in the diff when it changes.
    for label, reffreq, alpha, nan, eqw in (
            ("flat_refin", REFFREQ_IN, 0.0, False, False),
            ("flat_equalwt", REFFREQ_IN, 0.0, False, True),
            ("alpha_refin", REFFREQ_IN, -0.7, False, False),
            ("flat_refout", REFFREQ_OUT, 0.0, False, False),
            ("nan_refin", REFFREQ_IN, -0.7, True, False)):
        name = "cube2taylor_residual_" + label
        d = rec.case(name)
        n = build_fixtures(d, SPW3_FREQS, alpha=alpha, with_nan=nan,
                           equal_sumwt=eqw)
        _make_avgpb(rec, d, n)
        pbs = n["pb"] + [os.path.join(d, "avgpb")]
        rc, threw, tail = run_taylor(rec.taylor, d,
                              cubeImage=csv(n["residual"]),
                              taylorImages="out",
                              pbimage=csv(pbs),
                              sumwtImage=csv(n["sumwt"]),
                              overwrite=1, mode="cube2taylor",
                              reffreq=reffreq, nTerms=2, pblimit=0.1,
                              imtype="residual")
        rec.record(name, d, rc, threw, tail)

    # ---- taylor2cube, with and without the Way-Out avgPB divide ----------
    for label, use_pb in (("withpb", True), ("nopb", False)):
        name = "taylor2cube_" + label
        d = rec.case(name)
        n = build_fixtures(d, SPW3_FREQS)
        _make_avgpb(rec, d, n)
        pbs = n["pb"] + [os.path.join(d, "avgpb")]
        rc, threw, tail = run_taylor(rec.taylor, d,
                              cubeImage=csv(n["model"]),
                              taylorImages="out",
                              pbimage=csv(pbs),
                              sumwtImage=csv(n["sumwt"]),
                              overwrite=1, mode="cube2taylor",
                              reffreq=REFFREQ_IN, nTerms=2, pblimit=0.0,
                              imtype="model")
        if rc != 0 or threw:
            rec.record(name, d, rc, threw, tail)
            continue
        rc, threw, tail = run_taylor(rec.taylor, d,
                              taylorImages=csv(["out.tt0", "out.tt1"]),
                              cubeImage=csv(n["model"]),
                              pbimage=(os.path.join(d, "avgpb") if use_pb else ""),
                              sumwtImage=csv(n["sumwt"]),
                              overwrite=1, mode="taylor2cube",
                              reffreq=REFFREQ_IN, nTerms=2, pblimit=0,
                              imtype="model")
        rec.record(name, d, rc, threw, tail)

    # ---- computealpha -----------------------------------------------------
    for nterms in (2, 3):
        name = "computealpha_n%d" % nterms
        d = rec.case(name)
        n = build_fixtures(d, SPW3_FREQS, alpha=-0.7)
        _make_avgpb(rec, d, n)
        pbs = n["pb"] + [os.path.join(d, "avgpb")]
        rc, threw, tail = run_taylor(rec.taylor, d,
                              cubeImage=csv(n["residual"]),
                              taylorImages="out",
                              pbimage=csv(pbs),
                              sumwtImage=csv(n["sumwt"]),
                              overwrite=1, mode="cube2taylor",
                              reffreq=REFFREQ_IN, nTerms=nterms, pblimit=0.0,
                              imtype="residual")
        if rc != 0 or threw:
            rec.record(name, d, rc, threw, tail)
            continue
        tts = ["out.tt%d" % i for i in range(nterms)]
        alphas = ["alpha", "beta"] if nterms > 2 else ["alpha"]
        rc, threw, tail = run_taylor(rec.taylor, d,
                              taylorImages=csv(tts),
                              cubeImage=csv(alphas),
                              pbimage="", sumwtImage="",
                              overwrite=1, mode="computealpha",
                              nTerms=nterms)
        rec.record(name, d, rc, threw, tail)

    # ---- malformed inputs -------------------------------------------------
    # These are the cases the rewrite is expected to CHANGE: today they are
    # undefined behavior or a silently wrong answer, afterwards they throw.
    # Recorded anyway, so the change is visible and provably confined here.
    _malformed_cases(rec)


def _malformed_cases(rec):
    # pbnames one short: the old silent fallback reused pbnames[0] for the
    # trailing SPW instead of failing.
    d = rec.case("malformed_short_pbnames")
    n = build_fixtures(d, SPW3_FREQS)
    _make_avgpb(rec, d, n)
    rc, threw, tail = run_taylor(rec.taylor, d,
                          cubeImage=csv(n["residual"]),
                          taylorImages="out",
                          pbimage=csv(n["pb"][:2] + [os.path.join(d, "avgpb")]),
                          sumwtImage=csv(n["sumwt"]),
                          overwrite=1, mode="cube2taylor",
                          reffreq=REFFREQ_IN, nTerms=2, pblimit=0.1,
                          imtype="residual")
    rec.record("malformed_short_pbnames", d, rc, threw, tail)

    # pbnames with no avgpb appended: back() is then a per-SPW PB.
    d = rec.case("malformed_no_avgpb_appended")
    n = build_fixtures(d, SPW3_FREQS)
    rc, threw, tail = run_taylor(rec.taylor, d,
                          cubeImage=csv(n["residual"]),
                          taylorImages="out",
                          pbimage=csv(n["pb"]),
                          sumwtImage=csv(n["sumwt"]),
                          overwrite=1, mode="cube2taylor",
                          reffreq=REFFREQ_IN, nTerms=2, pblimit=0.1,
                          imtype="residual")
    rec.record("malformed_no_avgpb_appended", d, rc, threw, tail)

    # nterms below 1.
    d = rec.case("malformed_nterms_zero")
    n = build_fixtures(d, SPW3_FREQS)
    _make_avgpb(rec, d, n)
    rc, threw, tail = run_taylor(rec.taylor, d,
                          cubeImage=csv(n["residual"]),
                          taylorImages="out",
                          pbimage=csv(n["pb"] + [os.path.join(d, "avgpb")]),
                          sumwtImage=csv(n["sumwt"]),
                          overwrite=1, mode="cube2taylor",
                          reffreq=REFFREQ_IN, nTerms=0, pblimit=0.1,
                          imtype="residual")
    rec.record("malformed_nterms_zero", d, rc, threw, tail)

    # empty cube list.
    d = rec.case("malformed_empty_cubes")
    build_fixtures(d, SPW3_FREQS)
    rc, threw, tail = run_taylor(rec.taylor, d,
                          cubeImage="", taylorImages="out", pbimage="",
                          sumwtImage="", overwrite=1, mode="cube2taylor",
                          reffreq=REFFREQ_IN, nTerms=2, pblimit=0.1,
                          imtype="residual")
    rec.record("malformed_empty_cubes", d, rc, threw, tail)

    # zero sumwt everywhere: every term's wtsum is 0, so the term is undefined.
    d = rec.case("malformed_zero_sumwt")
    n = build_fixtures(d, SPW3_FREQS)
    for s in n["sumwt"]:
        _write(s, _sumwt(0.0), 1.5 * GHZ)
    _make_avgpb(rec, d, n)
    rc, threw, tail = run_taylor(rec.taylor, d,
                          cubeImage=csv(n["residual"]),
                          taylorImages="out",
                          pbimage=csv(n["pb"] + [os.path.join(d, "avgpb")]),
                          sumwtImage=csv(n["sumwt"]),
                          overwrite=1, mode="cube2taylor",
                          reffreq=REFFREQ_IN, nTerms=2, pblimit=0.0,
                          imtype="residual")
    rec.record("malformed_zero_sumwt", d, rc, threw, tail)


def _make_avgpb(rec, d, names):
    """Produce the avgpb a case needs, via the app itself."""
    rc, threw, tail = run_taylor(rec.taylor, d,
                          pbimage=csv(names["pb"]),
                          avgpbname="avgpb",
                          minfreqpbname="",
                          avgpbmode="mean",
                          taylorImages="", cubeImage="", sumwtImage="",
                          overwrite=1, mode="computeavgpb")
    if rc != 0 or threw:
        raise RuntimeError("computeavgpb failed while preparing %s:\n%s" % (d, tail))


# --------------------------------------------------------------------------
# compare
# --------------------------------------------------------------------------

def compare(baseline, current):
    """Report every difference.  Returns the number of differing cases."""
    ndiff = 0
    for name in sorted(set(baseline) | set(current)):
        if name not in baseline:
            print("NEW CASE      %s" % name)
            ndiff += 1
            continue
        if name not in current:
            print("MISSING CASE  %s" % name)
            ndiff += 1
            continue
        b, c = baseline[name], current[name]
        problems = []
        if b["returncode"] != c["returncode"]:
            problems.append("returncode %s -> %s" % (b["returncode"], c["returncode"]))
        if b.get("threw") != c.get("threw"):
            problems.append("threw %s -> %s" % (b.get("threw"), c.get("threw")))
        bi, ci = b["images"], c["images"]
        for img in sorted(set(bi) | set(ci)):
            if img not in bi:
                problems.append("new image %s" % img)
            elif img not in ci:
                problems.append("image gone %s" % img)
            elif bi[img] != ci[img]:
                # The hash covers pixels and mask together, so a difference
                # here is one, the other, or both.
                problems.append("pixels or mask differ %s" % img)
        if problems:
            ndiff += 1
            print("DIFF  %s" % name)
            for p in problems:
                print("        %s" % p)
    return ndiff


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument("--record", metavar="OUT.json")
    g.add_argument("--compare", metavar="BASELINE.json")
    ap.add_argument("--workdir", default="taylor_golden_work",
                    help="scratch directory for fixtures and outputs")
    ap.add_argument("--taylor", default=None,
                    help="path to the taylor binary "
                         "(default: install/bin/taylor beside the repo root)")
    ap.add_argument("--keep", action="store_true",
                    help="keep the work directory after the run")
    args = ap.parse_args()

    taylor_bin = args.taylor
    if taylor_bin is None:
        here = os.path.dirname(os.path.abspath(__file__))
        taylor_bin = os.path.join(os.path.dirname(here), "install", "bin", "taylor")
    taylor_bin = os.path.abspath(taylor_bin)
    if not os.path.exists(taylor_bin):
        sys.exit("taylor binary not found: %s" % taylor_bin)

    workdir = os.path.abspath(args.workdir)
    if os.path.exists(workdir):
        shutil.rmtree(workdir)
    os.makedirs(workdir)

    print("taylor  : %s" % taylor_bin)
    print("workdir : %s" % workdir)
    rec = Recorder(taylor_bin, workdir)
    run_all_cases(rec)

    if args.record:
        with open(args.record, "w") as f:
            json.dump(rec.results, f, indent=2, sort_keys=True)
        print("\nrecorded %d cases to %s" % (len(rec.results), args.record))
        rc = 0
    else:
        with open(args.compare) as f:
            baseline = json.load(f)
        print("")
        ndiff = compare(baseline, rec.results)
        if ndiff == 0:
            print("no differences across %d cases" % len(rec.results))
            rc = 0
        else:
            print("\n%d of %d cases differ" % (ndiff, len(rec.results)))
            rc = 1

    if not args.keep:
        shutil.rmtree(workdir)
    return rc


if __name__ == "__main__":
    sys.exit(main())
