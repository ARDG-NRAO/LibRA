// # Copyright (C) 2021,2026
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$

// GoogleTest coverage for the taylor::Taylor C++ class (apps/src/Taylor/taylor.h,
// taylor.cc), for the modes the production driver scripts/run_taylor_fixed.py
// actually invokes:
//
//   computeavgpb     -> Taylor::computeAvgPB
//   removefreqdepPB  -> Taylor::removeFreqDepPB
//   cube2taylor      -> Taylor::cubeToTaylorSum
//   taylor2cube      -> Taylor::taylorToCube
//   applyPB          -> Taylor::applyPB
//
// Configuration (reffreq, nterms, pblimit, imtype, overwrite) is constructor
// state, so each test builds a Taylor with the configuration under test and
// then passes only image names.  Methods return void and report failure by
// throwing AipsError.
//
// WHERE THE NUMBERS COME FROM
// ---------------------------
// Every constant below is measured, not invented. The geometry and the pixel
// values are those of gold_standard/refim_oneshiftpoint.mosaic.ms :
//
//   3 SPWs, 1 channel each, 300 MHz wide, centred 1.2 / 1.5 / 1.8 GHz
//     (read from the MS SPECTRAL_WINDOW subtable)
//   reference frequency 1.5 GHz  ->  Taylor weights w = -0.2, 0.0, +0.2
//   per-SPW PB at the source pixel (512,596): 0.637277 / 0.488637 / 0.347568
//   avgPB at the same pixel:                  0.491160
//   converged flat-noise model tt0 = 0.498110, tt1/tt0 = -0.5751
//
// WHAT THESE TESTS ASSERT, AND WHY IT IS NOT CIRCULAR
// ---------------------------------------------------
// A moment sum like cubeToTaylorSum is easy to test : recompute
// sum(flux*w^k*wt)/sum(wt) in the test and compare. That cannot catch a wrong
// formula, only a broken implementation of the wrong formula. So the assertions
// here are invariants and cross-checks that hold independently of how the code
// is written:
//
//   * mean of the three measured per-SPW PBs must equal the avgPB the pipeline
//     produced on disk (two independent computations of the same quantity)
//   * the Taylor weights are symmetric (-0.2, 0, +0.2), so a FLAT spectrum must
//     give tt1 == 0 exactly, whatever the weighting
//   * the transform is linear: scaling every input scales every output
//   * the pblimit>0 branch differs from the pblimit==0 branch by exactly one
//     factor of avgPB -- this is the Bug 5 regression guard (see the norm_mode
//     block in run_taylor_fixed.py)
//   * taylorToCube at the reference frequency (w == 0) must return tt0 exactly
//
// NOT COVERED HERE: applyFreqDepPB 

#include <filesystem>
#include <string>
#include <vector>
#include <cmath>

#include <Taylor/taylor.h>
#include <gtest/gtest.h>
#include <tests/test_utils.h>

#include <casacore/images/Images/PagedImage.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MFrequency.h>
#include <casacore/casa/Quanta/Quantum.h>

using namespace std;
using namespace std::filesystem;
using namespace casacore;
using namespace taylor;

namespace test {

const path goldDir = current_path() / "gold_standard";

// --- measured constants (see header comment for provenance) ----------------
static constexpr double FREQ_SPW0 = 1.2e9;
static constexpr double FREQ_SPW1 = 1.5e9;
static constexpr double FREQ_SPW2 = 1.8e9;
static constexpr double REF_FREQ  = 1.5e9;      // == FREQ_SPW1
static const string     REF_FREQ_STR = "1.5e9Hz";

// PB at source pixel (512,596), per SPW, and their mean.
static constexpr float PB_SPW0 = 0.637277f;
static constexpr float PB_SPW1 = 0.488637f;
static constexpr float PB_SPW2 = 0.347568f;
static constexpr float AVG_PB  = 0.491160f;     // pipeline's dirty_spw0.avgpb

// Per-SPW PSF peaks used as the Hessian weights (dirty_spw*.taylorwt).
static constexpr float TWT_SPW0 = 8423.99f;
static constexpr float TWT_SPW1 = 8389.94f;
static constexpr float TWT_SPW2 = 8231.98f;

// Converged source: S(nu) = S_ref * (nu/nu_ref)^alpha
static constexpr double S_REF  = 0.498110;      // flat-noise tt0 at 1.5 GHz
static constexpr double ALPHA  = -0.5751;

static double powerLaw(double freqHz) {
  return S_REF * std::pow(freqHz / REF_FREQ, ALPHA);
}

class TaylorAppTest : public ::testing::Test {
protected:
  static constexpr int IMAGE_SIZE = 32;    // algebra is per-pixel; size only
                                           // affects runtime.
  static constexpr double CELL_SIZE = 10.0; // arcsec, as the real run

  path testDir;

  void SetUp() override {
    string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    testDir = current_path() / (string("TaylorAppTest_") + testName);
    std::filesystem::create_directory(testDir);
    current_path(testDir);
  }

  void TearDown() override {
    current_path(testDir.parent_path());
    remove_directory(testDir);
  }

  // Configuration-only Taylor, for the methods that do not need a transform
  // set up (applyPB, removeFreqDepPB, computeAvgPB).
  static Taylor withPblimit(float pblimit) {
    return Taylor("", 0, pblimit, RESIDUAL, true);
  }

  static Taylor forTransform(int nterms, float pblimit, ImageType imtype) {
    return Taylor(REF_FREQ_STR, nterms, pblimit, imtype, true);
  }

  CoordinateSystem makeCoordSystem(double freqHz) {
    CoordinateSystem coord;
    Vector<Double> refCoord(2, 0.0);
    Matrix<Double> xform(2, 2, 0.0);
    xform.diagonal() = 1.0;
    DirectionCoordinate dirCoord(MDirection::J2000, Projection(Projection::SIN),
                                 refCoord(0), refCoord(1),
                                 -CELL_SIZE / 3600.0, CELL_SIZE / 3600.0,
                                 xform, IMAGE_SIZE / 2.0, IMAGE_SIZE / 2.0);
    coord.addCoordinate(dirCoord);
    Vector<Int> stokes(1, Stokes::I);
    coord.addCoordinate(StokesCoordinate(stokes));
    Vector<Double> freqs(1, freqHz);
    coord.addCoordinate(SpectralCoordinate(MFrequency::TOPO, freqs));
    return coord;
  }

  void createConstantImage(const string &name, double freqHz, float value) {
    IPosition shape(4, IMAGE_SIZE, IMAGE_SIZE, 1, 1);
    PagedImage<float> image(shape, makeCoordSystem(freqHz), name);
    image.put(Array<float>(shape, value));
    image.flush();
    image.unlock();
  }

  float centrePixel(const string &name) {
    PagedImage<float> im(name);
    return im(IPosition(4, IMAGE_SIZE / 2, IMAGE_SIZE / 2, 0, 0));
  }

  // The three per-SPW PB images at their measured values.
  vector<string> makeMeasuredPBs(const string &prefix) {
    vector<string> names = {prefix + "0", prefix + "1", prefix + "2"};
    createConstantImage(names[0], FREQ_SPW0, PB_SPW0);
    createConstantImage(names[1], FREQ_SPW1, PB_SPW1);
    createConstantImage(names[2], FREQ_SPW2, PB_SPW2);
    return names;
  }

  vector<string> makeTaylorWts(const string &prefix) {
    vector<string> names = {prefix + "0", prefix + "1", prefix + "2"};
    createConstantImage(names[0], FREQ_SPW0, TWT_SPW0);
    createConstantImage(names[1], FREQ_SPW1, TWT_SPW1);
    createConstantImage(names[2], FREQ_SPW2, TWT_SPW2);
    return names;
  }
};

// ---------------------------------------------------------------------------
// computeAvgPB
// ---------------------------------------------------------------------------

// Cross-check: the mean of the three measured per-SPW PBs must reproduce the
// avgPB the production pipeline wrote to disk (0.491160). Two independent
// computations of the same physical quantity, so this is not circular.
TEST_F(TaylorAppTest, ComputeAvgPBReproducesPipelineAvgPB) {
  vector<string> pbnames = makeMeasuredPBs("pb.spw");

  Taylor t = withPblimit(0.0f);
  ASSERT_NO_THROW(t.computeAvgPB(pbnames, "avgpb", "minfreqpb", "mean"));
  ASSERT_TRUE(directoryExists("avgpb"));

  EXPECT_NEAR(centrePixel("avgpb"), AVG_PB, 1e-5f);

  // minfreq PB is the lowest-frequency SPW's beam, i.e. the widest one.
  ASSERT_TRUE(directoryExists("minfreqpb"));
  EXPECT_NEAR(centrePixel("minfreqpb"), PB_SPW0, 1e-5f);
}

// ---------------------------------------------------------------------------
// removeFreqDepPB:  S(nu)*PB(nu)  ->  S(nu)*avgPB
// ---------------------------------------------------------------------------

// The Way-In flattening step. Its defining property is that the frequency
// dependence of the BEAM is removed while the frequency dependence of the SKY
// is left untouched: output/input == avgPB/PB(nu), independent of the sky.
TEST_F(TaylorAppTest, RemoveFreqDepPBReplacesPerSpwBeamWithAvgPB) {
  vector<string> pbnames = makeMeasuredPBs("pb.spw");
  createConstantImage("avgpb", REF_FREQ, AVG_PB);

  // Gridded residual as dale leaves it: S(nu) * PB(nu).
  const double freqs[3] = {FREQ_SPW0, FREQ_SPW1, FREQ_SPW2};
  const float pbs[3] = {PB_SPW0, PB_SPW1, PB_SPW2};
  vector<string> cubenames = {"res.spw0", "res.spw1", "res.spw2"};
  for (int i = 0; i < 3; ++i)
    createConstantImage(cubenames[i], freqs[i], powerLaw(freqs[i]) * pbs[i]);

  Taylor t = withPblimit(0.1f);
  ASSERT_NO_THROW(t.removeFreqDepPB("avgpb", cubenames, pbnames));

  // Expected: S(nu) * avgPB -- the sky spectrum survives, the beam is now flat.
  for (int i = 0; i < 3; ++i)
    EXPECT_NEAR(centrePixel(cubenames[i]), powerLaw(freqs[i]) * AVG_PB, 1e-5f)
        << "SPW " << i;
}

// Pixels below pblimit must be blanked, not divided by a small number. Uses a
// PB of 0.05 against the production pblimit of 0.1.
TEST_F(TaylorAppTest, RemoveFreqDepPBBlanksBelowPblimit) {
  createConstantImage("pb.low", FREQ_SPW0, 0.05f);
  createConstantImage("avgpb", REF_FREQ, AVG_PB);
  createConstantImage("res.low", FREQ_SPW0, 1.0f);

  vector<string> cubenames = {"res.low"}, pbnames = {"pb.low"};
  Taylor t = withPblimit(0.1f);
  ASSERT_NO_THROW(t.removeFreqDepPB("avgpb", cubenames, pbnames));

  EXPECT_NEAR(centrePixel("res.low"), 0.0f, 1e-6f);
}

// ---------------------------------------------------------------------------
// applyPB
// ---------------------------------------------------------------------------

// multiply then divide by the same beam is the identity. Implementation
// independent.
TEST_F(TaylorAppTest, ApplyPBMultiplyThenDivideIsIdentity) {
  const float sky = static_cast<float>(S_REF);
  createConstantImage("cube.rt", REF_FREQ, sky);
  createConstantImage("pb.rt", REF_FREQ, AVG_PB);
  vector<string> cubenames = {"cube.rt"};

  Taylor t = withPblimit(0.0f);
  ASSERT_NO_THROW(t.applyPB("pb.rt", cubenames, "multiply"));
  EXPECT_NEAR(centrePixel("cube.rt"), sky * AVG_PB, 1e-5f);

  ASSERT_NO_THROW(t.applyPB("pb.rt", cubenames, "divide"));
  EXPECT_NEAR(centrePixel("cube.rt"), sky, 1e-4f);
}

TEST_F(TaylorAppTest, ApplyPBDivideBlanksBelowPblimit) {
  createConstantImage("cube.guard", REF_FREQ, 1.0f);
  createConstantImage("pb.guard", REF_FREQ, 0.05f);
  vector<string> cubenames = {"cube.guard"};

  Taylor t = withPblimit(0.1f);
  ASSERT_NO_THROW(t.applyPB("pb.guard", cubenames, "divide"));
  EXPECT_NEAR(centrePixel("cube.guard"), 0.0f, 1e-6f);
}

// An invalid direction must be rejected rather than quietly doing one of them.
TEST_F(TaylorAppTest, ApplyPBRejectsUnknownMode) {
  createConstantImage("cube.mode", REF_FREQ, 1.0f);
  createConstantImage("pb.mode", REF_FREQ, 1.0f);
  Taylor t = withPblimit(0.0f);
  EXPECT_THROW(t.applyPB("pb.mode", {"cube.mode"}, "scale"), AipsError);
}

// ---------------------------------------------------------------------------
// cubeToTaylorSum
//
// NOTE ON WHAT tt1/tt0 IS HERE: cubeToTaylorSum forms the RIGHT-HAND SIDE of
// the normal equations (a PB- and weight-weighted Taylor moment), not a
// spectral index. Recovering alpha needs the PSF-Taylor Hessian solve that
// happens downstream in hummbee -- on the real data the RHS ratio is about
// -0.10 while the deconvolved alpha is -0.575. So none of these tests assert
// tt1/tt0 == alpha; they assert invariants that hold whatever the weighting.
// ---------------------------------------------------------------------------

// The Taylor weights for this MS are w = (nu-nu_ref)/nu_ref = -0.2, 0, +0.2,
// which are symmetric about the reference frequency. A source with NO spectral
// structure therefore has to produce a first moment of exactly zero, for any
// choice of per-SPW weighting, because the weights cancel in pairs. A wrong
// reference frequency or an asymmetric weighting breaks this immediately.

TEST_F(TaylorAppTest, CubeToTaylorSumFlatSpectrumGivesZeroFirstMoment) {
  const float flat = 1.0f;
  const double freqs[3] = {FREQ_SPW0, FREQ_SPW1, FREQ_SPW2};

  vector<string> cubenames = {"flat.spw0", "flat.spw1", "flat.spw2"};
  for (int i = 0; i < 3; ++i) createConstantImage(cubenames[i], freqs[i], flat);

  // Equal per-SPW PB and equal weights, so only the w symmetry is under test.
  vector<string> pbnames = {"pbf.spw0", "pbf.spw1", "pbf.spw2", "pbf.avg"};
  for (int i = 0; i < 3; ++i) createConstantImage(pbnames[i], freqs[i], 1.0f);
  createConstantImage(pbnames[3], REF_FREQ, 1.0f);

  vector<string> wts = {"wtf.spw0", "wtf.spw1", "wtf.spw2"};
  for (int i = 0; i < 3; ++i) createConstantImage(wts[i], freqs[i], 1.0f);

  vector<string> mtnames = {"flat.taylor"};
  Taylor t = forTransform(2, 0.0f, RESIDUAL);
  ASSERT_NO_THROW(t.cubeToTaylorSum(cubenames, mtnames, pbnames, wts));

  EXPECT_NEAR(centrePixel("flat.taylor.tt0"), flat, 1e-5f);
  EXPECT_NEAR(centrePixel("flat.taylor.tt1"), 0.0f, 1e-6f)
      << "w = -0.2, 0, +0.2 are symmetric about the reference frequency, so a "
         "flat spectrum must have zero first moment";
}

// A falling spectrum must give a negative first moment, a rising one positive,
// and the two must be antisymmetric for equal-and-opposite alpha. Sign and
// symmetry only -- no formula is reproduced.
TEST_F(TaylorAppTest, CubeToTaylorSumFirstMomentTracksSpectralSlope) {
  const double freqs[3] = {FREQ_SPW0, FREQ_SPW1, FREQ_SPW2};
  vector<string> pbnames = {"pbs.spw0", "pbs.spw1", "pbs.spw2", "pbs.avg"};
  for (int i = 0; i < 3; ++i) createConstantImage(pbnames[i], freqs[i], 1.0f);
  createConstantImage(pbnames[3], REF_FREQ, 1.0f);
  vector<string> wts = {"wts.spw0", "wts.spw1", "wts.spw2"};
  for (int i = 0; i < 3; ++i) createConstantImage(wts[i], freqs[i], 1.0f);

  auto firstMoment = [&](double alpha, const string &tag) {
    vector<string> cubenames = {tag + ".spw0", tag + ".spw1", tag + ".spw2"};
    for (int i = 0; i < 3; ++i)
      createConstantImage(cubenames[i], freqs[i],
                          static_cast<float>(std::pow(freqs[i] / REF_FREQ, alpha)));
    vector<string> mtnames = {tag + ".taylor"};
    Taylor t = forTransform(2, 0.0f, RESIDUAL);
    EXPECT_NO_THROW(t.cubeToTaylorSum(cubenames, mtnames, pbnames, wts));
    return centrePixel(tag + ".taylor.tt1");
  };

  float falling = firstMoment(ALPHA, "fall");    // alpha < 0
  float rising  = firstMoment(-ALPHA, "rise");   // alpha > 0

  EXPECT_LT(falling, 0.0f);
  EXPECT_GT(rising, 0.0f);

  // A power law is NOT antisymmetric under alpha -> -alpha, so |falling| and
  // |rising| do not match exactly. With w = -0.2, 0, +0.2 the first moment is
  // proportional to sum_c w_c * (1+w_c)^alpha, giving
  //     alpha = -0.5751:  (-0.2)(0.8^-0.5751) + (0.2)(1.2^-0.5751) = -0.047294
  //     alpha = +0.5751:  (-0.2)(0.8^+0.5751) + (0.2)(1.2^+0.5751) = +0.046198
  // i.e. a predicted asymmetry ratio of 1.0237. Asserting that ratio (rather
  // than assuming 1.0) tests that the Taylor weights and the reference
  // frequency are what we think they are: a wrong nu_ref shifts the w values
  // and changes this ratio well outside the tolerance below.
  const double predictedRatio =
      (-0.2 * std::pow(0.8, ALPHA) + 0.2 * std::pow(1.2, ALPHA)) /
      (-0.2 * std::pow(0.8, -ALPHA) + 0.2 * std::pow(1.2, -ALPHA));
  EXPECT_NEAR(falling / rising, predictedRatio, 0.005);
}

// Linearity: doubling every input flux doubles every Taylor term. Holds for any
// weighting, so it cannot be satisfied by a wrong formula that happens to match
// a hand-computed value.
TEST_F(TaylorAppTest, CubeToTaylorSumIsLinearInInputFlux) {
  const double freqs[3] = {FREQ_SPW0, FREQ_SPW1, FREQ_SPW2};
  vector<string> pbnames = {"pbl.spw0", "pbl.spw1", "pbl.spw2", "pbl.avg"};
  createConstantImage(pbnames[0], FREQ_SPW0, PB_SPW0);
  createConstantImage(pbnames[1], FREQ_SPW1, PB_SPW1);
  createConstantImage(pbnames[2], FREQ_SPW2, PB_SPW2);
  createConstantImage(pbnames[3], REF_FREQ, AVG_PB);
  vector<string> wts = makeTaylorWts("wtl.spw");

  auto run = [&](double scale, const string &tag) {
    vector<string> cubenames = {tag + ".spw0", tag + ".spw1", tag + ".spw2"};
    for (int i = 0; i < 3; ++i)
      createConstantImage(cubenames[i], freqs[i],
                          static_cast<float>(scale * powerLaw(freqs[i]) * AVG_PB));
    vector<string> mtnames = {tag + ".taylor"};
    Taylor t = forTransform(2, 0.1f, RESIDUAL);
    EXPECT_NO_THROW(t.cubeToTaylorSum(cubenames, mtnames, pbnames, wts));
    return pair<float, float>(centrePixel(tag + ".taylor.tt0"),
                              centrePixel(tag + ".taylor.tt1"));
  };

  auto one = run(1.0, "lin1");
  auto two = run(2.0, "lin2");

  EXPECT_NEAR(two.first, 2.0f * one.first, 1e-4f * std::abs(two.first));
  EXPECT_NEAR(two.second, 2.0f * one.second, 1e-4f * std::abs(two.second));
}

// REGRESSION GUARD for Bug 5 (the Way-In/Way-Out avgPB divide, fixed 2026-08-06;
// see the norm_mode block in scripts/run_taylor_fixed.py).
//
// The avgPB divide is taken for PSF and RESIDUAL only, and only when
// pblimit > 0. The whole norm_mode design rests on that being the ONLY
// difference between the two branches, so the ratio of the two outputs must be
// exactly avgPB. If someone changes what that branch does, this fails.
TEST_F(TaylorAppTest, CubeToTaylorSumPblimitBranchDividesByAvgPBExactly) {
  const double freqs[3] = {FREQ_SPW0, FREQ_SPW1, FREQ_SPW2};
  vector<string> pbnames = {"pbb.spw0", "pbb.spw1", "pbb.spw2", "pbb.avg"};
  createConstantImage(pbnames[0], FREQ_SPW0, PB_SPW0);
  createConstantImage(pbnames[1], FREQ_SPW1, PB_SPW1);
  createConstantImage(pbnames[2], FREQ_SPW2, PB_SPW2);
  createConstantImage(pbnames[3], REF_FREQ, AVG_PB);
  vector<string> wts = makeTaylorWts("wtb.spw");

  auto run = [&](float pblimit, const string &tag) {
    vector<string> cubenames = {tag + ".spw0", tag + ".spw1", tag + ".spw2"};
    for (int i = 0; i < 3; ++i)
      createConstantImage(cubenames[i], freqs[i],
                          static_cast<float>(powerLaw(freqs[i]) * AVG_PB));
    vector<string> mtnames = {tag + ".taylor"};
    Taylor t = forTransform(2, pblimit, RESIDUAL);
    EXPECT_NO_THROW(t.cubeToTaylorSum(cubenames, mtnames, pbnames, wts));
    return centrePixel(tag + ".taylor.tt0");
  };

  float withDivide = run(0.1f, "gpb");   // production pblimit -> divides by avgPB
  float noDivide   = run(0.0f, "gnopb"); // plain /wtsum

  ASSERT_GT(std::abs(noDivide), 1e-8f);
  EXPECT_NEAR(noDivide / withDivide, AVG_PB, 1e-4f)
      << "the pblimit>0 branch must differ from the pblimit==0 branch by exactly "
         "one factor of avgPB -- this is what norm_mode's Way-In switch relies on";
}

// PSF and SUMWT fill the upper triangle of the Hessian, which needs 2n-1 terms.
// Everything else gets n.
TEST_F(TaylorAppTest, CubeToTaylorSumTermCountFollowsImageType) {
  const double freqs[3] = {FREQ_SPW0, FREQ_SPW1, FREQ_SPW2};
  vector<string> pbnames = {"pbn.spw0", "pbn.spw1", "pbn.spw2", "pbn.avg"};
  for (int i = 0; i < 3; ++i) createConstantImage(pbnames[i], freqs[i], 1.0f);
  createConstantImage(pbnames[3], REF_FREQ, 1.0f);
  vector<string> wts = {"wtn.spw0", "wtn.spw1", "wtn.spw2"};
  for (int i = 0; i < 3; ++i) createConstantImage(wts[i], freqs[i], 1.0f);
  vector<string> cubenames = {"cn.spw0", "cn.spw1", "cn.spw2"};
  for (int i = 0; i < 3; ++i) createConstantImage(cubenames[i], freqs[i], 1.0f);

  Taylor psf = forTransform(2, 0.0f, PSF);
  ASSERT_NO_THROW(psf.cubeToTaylorSum(cubenames, {"psfterms"}, pbnames, wts));
  EXPECT_TRUE(directoryExists("psfterms.tt2"));   // 2*2-1 == 3 terms
  EXPECT_FALSE(directoryExists("psfterms.tt3"));

  Taylor res = forTransform(2, 0.0f, RESIDUAL);
  ASSERT_NO_THROW(res.cubeToTaylorSum(cubenames, {"resterms"}, pbnames, wts));
  EXPECT_TRUE(directoryExists("resterms.tt1"));
  EXPECT_FALSE(directoryExists("resterms.tt2"));
}

// SUMWT is not premultiplied by the PB, matching CASA's `imtype < 2` in
// SynthesisUtilMethods::cubeToTaylorSum. Multiplying a [1,1,1,1] sumwt image by
// a full size PB used to make the expression take the PB's shape, so copyData
// asserted and the terms were left as the zeros initializeTaylorImage wrote.
TEST_F(TaylorAppTest, CubeToTaylorSumSumwtIgnoresPerSpwPB) {
  const double freqs[3] = {FREQ_SPW0, FREQ_SPW1, FREQ_SPW2};

  // Per-SPW PBs deliberately far from 1, so a premultiply would be obvious.
  vector<string> pbnames = makeMeasuredPBs("pbw.spw");
  pbnames.push_back("pbw.avg");
  createConstantImage(pbnames[3], REF_FREQ, AVG_PB);

  // Constant sumwt of 1 in every SPW: the weighted mean of a constant is that
  // constant, so tt0 must come back as 1 and not as 1 * PB.
  vector<string> cubenames = {"sw.spw0", "sw.spw1", "sw.spw2"};
  for (int i = 0; i < 3; ++i) createConstantImage(cubenames[i], freqs[i], 1.0f);

  Taylor t = forTransform(2, 0.0f, SUMWT);
  ASSERT_NO_THROW(t.cubeToTaylorSum(cubenames, {"swterms"}, pbnames, {}));

  EXPECT_NEAR(centrePixel("swterms.tt0"), 1.0f, 1e-5f)
      << "SUMWT must not be premultiplied by the per-SPW PB";
  EXPECT_NEAR(centrePixel("swterms.tt1"), 0.0f, 1e-6f);
}

// ---------------------------------------------------------------------------
// taylorToCube, on the real wasp_jet Taylor-domain images from gold_standard.
// ---------------------------------------------------------------------------

// At the reference frequency w == 0, so the Taylor series collapses to tt0
// exactly, regardless of the higher terms. Uses real (not synthetic) tt images.
TEST_F(TaylorAppTest, TaylorToCubeAtReferenceFrequencyReturnsTT0) {
  ASSERT_TRUE(directoryExists(goldDir));
  for (const string &tt : {"tt0", "tt1", "tt2"})
    copy(goldDir / ("wasp_jet.residual." + tt),
         testDir / ("wasp_jet.residual." + tt), copy_options::recursive);

  IPosition ttShape;
  CoordinateSystem ttCoord;
  {
    PagedImage<float> tt0("wasp_jet.residual.tt0");
    ttShape = tt0.shape();
    ttCoord = tt0.coordinates();
  }

  auto chanCoord = [&](double freqHz) {
    CoordinateSystem c = ttCoord;
    int idx = c.findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate spec = c.spectralCoordinate(idx);
    spec.setReferencePixel(Vector<Double>(1, 0.0));
    spec.setReferenceValue(Vector<Double>(1, freqHz));
    c.replaceCoordinate(spec, idx);
    return c;
  };

  // Three output channels at the real SPW frequencies.
  const double freqs[3] = {FREQ_SPW0, FREQ_SPW1, FREQ_SPW2};
  vector<string> cubenames = {"out.spw0", "out.spw1", "out.spw2"};
  for (int i = 0; i < 3; ++i) {
    PagedImage<float> c(ttShape, chanCoord(freqs[i]), cubenames[i]);
    c.put(Array<float>(ttShape, 0.0f));
    c.flush();
    c.unlock();
  }

  vector<string> mtnames = {"wasp_jet.residual.tt0", "wasp_jet.residual.tt1",
                            "wasp_jet.residual.tt2"};
  vector<string> pbnames;      // empty -> taylorToCube skips its avgPB divide

  Taylor t = forTransform(3, 0.1f, MODEL);
  ASSERT_NO_THROW(t.taylorToCube(mtnames, cubenames, pbnames));

  IPosition centre(4, ttShape[0] / 2, ttShape[1] / 2, 0, 0);
  const float tt0 = PagedImage<float>("wasp_jet.residual.tt0")(centre);
  const float tt1 = PagedImage<float>("wasp_jet.residual.tt1")(centre);
  const float tt2 = PagedImage<float>("wasp_jet.residual.tt2")(centre);

  // w == 0 at the reference frequency: higher terms drop out entirely.
  EXPECT_NEAR(PagedImage<float>("out.spw1")(centre), tt0, 1e-5f)
      << "at nu == nu_ref the Taylor series must collapse to tt0";

  // The two outer channels sit at w = -0.2 and +0.2. Their MEAN removes the odd
  // (tt1) term and leaves tt0 + tt2*w^2, which is a statement about the series,
  // not about the implementation.
  const float lo = PagedImage<float>("out.spw0")(centre);
  const float hi = PagedImage<float>("out.spw2")(centre);
  const double w = (FREQ_SPW2 - REF_FREQ) / REF_FREQ;  // +0.2
  EXPECT_NEAR(0.5f * (lo + hi), tt0 + tt2 * w * w, 1e-4f);
  // and their DIFFERENCE isolates the odd term.
  EXPECT_NEAR(0.5f * (hi - lo), tt1 * w, 1e-4f);
}

// ---------------------------------------------------------------------------
// computeAvgPB, median mode
// ---------------------------------------------------------------------------

// Odd number of inputs: the median is the middle order statistic, which for the
// three measured per-SPW PBs is SPW1's 0.488637 (0.347568 < 0.488637 < 0.637277).
// This branch previously indexed 4D images with a 1D IPosition and threw.
TEST_F(TaylorAppTest, ComputeAvgPBMedianOddCountPicksMiddleValue) {
  vector<string> pbnames = makeMeasuredPBs("pbm.spw");

  Taylor t = withPblimit(0.0f);
  ASSERT_NO_THROW(t.computeAvgPB(pbnames, "medpb", "minfreqpb", "median"));
  ASSERT_TRUE(directoryExists("medpb"));

  EXPECT_NEAR(centrePixel("medpb"), PB_SPW1, 1e-6f);
  // The median is NOT the mean here; if it were, this branch would be
  // indistinguishable from avgpbmode="mean".
  EXPECT_GT(std::abs(centrePixel("medpb") - AVG_PB), 1e-4f);
}

// Even number of inputs: the median is the mean of the two central values.
TEST_F(TaylorAppTest, ComputeAvgPBMedianEvenCountAveragesCentralPair) {
  vector<string> pbnames = {"pbe.spw0", "pbe.spw1", "pbe.spw2", "pbe.spw3"};
  createConstantImage(pbnames[0], FREQ_SPW0, 1.0f);
  createConstantImage(pbnames[1], FREQ_SPW1, 2.0f);
  createConstantImage(pbnames[2], FREQ_SPW2, 4.0f);
  createConstantImage(pbnames[3], FREQ_SPW2, 100.0f);  // outlier

  Taylor t = withPblimit(0.0f);
  ASSERT_NO_THROW(t.computeAvgPB(pbnames, "medpbe", "minfreqpbe", "median"));

  // Central pair is (2, 4) -> 3. The mean would be 26.75, so the outlier is
  // what distinguishes a correct median from a mean.
  EXPECT_NEAR(centrePixel("medpbe"), 3.0f, 1e-6f);
}

TEST_F(TaylorAppTest, ComputeAvgPBRejectsUnknownMode) {
  vector<string> pbnames = makeMeasuredPBs("pbu.spw");
  Taylor t = withPblimit(0.0f);
  EXPECT_THROW(t.computeAvgPB(pbnames, "avgu", "", "mode"), AipsError);
}

// ---------------------------------------------------------------------------
// pbnames contract
// ---------------------------------------------------------------------------

// cubeToTaylorSum requires one per-SPW PB per cube image plus avgPB appended
// last (run_taylor_fixed.py:_taylor_cube2taylor builds exactly that). Passing
// N PBs for N cubes used to silently reuse pbnames[0] for the last SPW, giving
// an image gridded against the wrong PB with exit code 0 and no ERROR line.
// It must now be loud.
TEST_F(TaylorAppTest, CubeToTaylorSumRejectsPbListWithoutAvgPB) {
  const double freqs[3] = {FREQ_SPW0, FREQ_SPW1, FREQ_SPW2};
  vector<string> cubenames = {"nc.spw0", "nc.spw1", "nc.spw2"};
  vector<string> pbnames   = {"np.spw0", "np.spw1", "np.spw2"};  // no avgPB
  for (int i = 0; i < 3; ++i) {
    createConstantImage(cubenames[i], freqs[i], 1.0f);
    createConstantImage(pbnames[i], freqs[i], 1.0f);
  }
  vector<string> wts = makeTaylorWts("nw.spw");
  vector<string> mtnames = {"nc.taylor"};

  Taylor t = forTransform(2, 0.1f, RESIDUAL);
  EXPECT_THROW(t.cubeToTaylorSum(cubenames, mtnames, pbnames, wts), AipsError);
}

// ---------------------------------------------------------------------------
// Argument guards. Each of these used to be an unchecked index into an empty or
// short vector, i.e. undefined behaviour rather than a diagnosable error.
// ---------------------------------------------------------------------------

TEST_F(TaylorAppTest, ArgumentGuardsRejectMalformedInput) {
  const double freqs[3] = {FREQ_SPW0, FREQ_SPW1, FREQ_SPW2};
  vector<string> cubenames = {"gc.spw0", "gc.spw1", "gc.spw2"};
  for (int i = 0; i < 3; ++i) createConstantImage(cubenames[i], freqs[i], 1.0f);
  vector<string> pbnames = makeMeasuredPBs("gp.spw");
  pbnames.push_back("gp.avg");
  createConstantImage(pbnames[3], REF_FREQ, AVG_PB);
  vector<string> wts = makeTaylorWts("gw.spw");
  vector<string> mtnames = {"gc.taylor"};

  Taylor t = forTransform(2, 0.1f, RESIDUAL);

  // cubeToTaylorSum: no cube images, no output base name, and one sumwt short.
  EXPECT_THROW(t.cubeToTaylorSum({}, mtnames, {"gp.avg"}, {}), AipsError);
  EXPECT_THROW(t.cubeToTaylorSum(cubenames, {}, pbnames, wts), AipsError);
  EXPECT_THROW(t.cubeToTaylorSum(cubenames, mtnames, pbnames,
                                 {wts[0], wts[1]}),  // one short
               AipsError);

  // nterms < 1 is now a configuration error, caught before any indexing.
  Taylor zeroTerms = forTransform(0, 0.1f, RESIDUAL);
  EXPECT_THROW(zeroTerms.cubeToTaylorSum(cubenames, mtnames, pbnames, wts), AipsError);

  // taylorToCube: a single base name cannot serve nterms > 1. This used to read
  // past the end of mtnames.
  EXPECT_THROW(t.taylorToCube(mtnames, cubenames, {}), AipsError);
  Taylor oneTerm = forTransform(1, 0.1f, MODEL);
  EXPECT_THROW(oneTerm.taylorToCube(mtnames, {}, {}), AipsError);

  // computeAvgPB with no inputs, and computeAlpha with too few output names.
  EXPECT_THROW(t.computeAvgPB({}, "avg", "", "mean"), AipsError);
  EXPECT_THROW(t.computeAlpha(mtnames, {}), AipsError);
  Taylor threeTerms = forTransform(3, 0.1f, RESIDUAL);
  EXPECT_THROW(threeTerms.computeAlpha(mtnames, {"a"}), AipsError);
}

} // namespace test
