// Integration-level GoogleTest coverage for the Taylor::* call SEQUENCE used by
// the production pipeline driver scripts/run_taylor_fixed.py. Unlike
// apps/src/Taylor/test_taylor.cc (which exercises each Taylor:: method in
// isolation), this file composes the same ordered stages the driver runs, so a
// regression in the *composition* (wrong stage order, a reintroduced flatsky
// step, a non-empty pbimage handed to taylor2cube) shows up here even if each
// individual method still passes its own unit test.
//
// Mirrors (see run_taylor_fixed.py docstring, Bugs 1-4, for why):
//
//   Way In  (stage_psf_pb_to_taylor):
//     cube2taylor(psf|pb|sumwt)            -- NO removeFreqDepPB, NO flatsky
//   Way In  (stage_residual_to_taylor):
//     removeFreqDepPB(residual)  ->  cube2taylor(residual)
//   Way Out (stage_taylor_to_cube + stage_prepare_model):
//     taylor2cube(pbimage="")              -- true-sky model, no PB re-applied
//
// Hummbee deconvolution itself is not run here (that needs a real major-cycle
// loop); the model Taylor terms are stood in for by copying the residual
// Taylor terms, same as the algebra of a single CLEAN component at the source
// pixel.
//
// cubeToTaylorSum computes a raw weighted Taylor-MOMENT sum, not a
// least-squares fit. It only exactly recovers known input coefficients when the
// reference frequency coincides with a channel (w0 == 0); with more channels
// spread symmetrically around reffreq the recovered ratio is systematically
// biased (this is the real, already-documented ~8% alpha bias, not something
// this test suite should paper over). So these tests use a 2-channel, w0==0
// setup, extended with the removeFreqDepPB Way-In step and the pbimage=""
// Way-Out step, and check against the closed-form value the formula actually
// produces -- not against the injected spectral index.
//
// Configuration is constructor state on Taylor, so each stage below builds the
// object with the configuration the driver passes for that stage.

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

class TaylorPipelineTest : public ::testing::Test {
protected:
  static constexpr int IMAGE_SIZE = 32;    // small synthetic images -- keeps the
                                            // test fast; algebra is size-independent.
  static constexpr double CELL_SIZE = 1.0; // arcsec

  // Two SPWs, reffreq == freq0 so w0 == 0 (see file header on why this makes
  // cubeToTaylorSum's moment sum an exact closed form).
  static constexpr double FREQ0_HZ = 1.0e9;
  static constexpr double FREQ1_HZ = 1.5e9;
  static constexpr float PB_LIMIT = 0.1f;
  static constexpr double TOLERANCE = 1e-4;

  path testDir;

  static string refFreqStr() { return std::to_string(FREQ0_HZ) + "Hz"; }

  // The driver's configuration for each stage.
  static Taylor avgPBStage() { return Taylor("", 0, PB_LIMIT, RESIDUAL, true); }
  static Taylor flattenStage() { return Taylor("", 0, PB_LIMIT, RESIDUAL, true); }
  static Taylor wayInStage(ImageType imtype) {
    return Taylor(refFreqStr(), 2, PB_LIMIT, imtype, true);
  }
  static Taylor wayOutStage() {
    return Taylor(refFreqStr(), 2, PB_LIMIT, MODEL, true);
  }

  void SetUp() override {
    string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    testDir = current_path() / (string("TaylorPipelineTest_") + testName);
    std::filesystem::create_directory(testDir);
    current_path(testDir);
  }

  void TearDown() override {
    current_path(testDir.parent_path());
    remove_directory(testDir);
  }

  CoordinateSystem makeCoordSystem(double freqHz) {
    CoordinateSystem coord;

    Vector<Double> refPixel(2);
    refPixel(0) = IMAGE_SIZE / 2.0;
    refPixel(1) = IMAGE_SIZE / 2.0;
    Vector<Double> refCoord(2, 0.0);
    Vector<Double> increment(2);
    increment(0) = -CELL_SIZE / 3600.0;
    increment(1) = CELL_SIZE / 3600.0;
    Matrix<Double> xform(2, 2, 0.0);
    xform.diagonal() = 1.0;

    DirectionCoordinate dirCoord(MDirection::J2000, Projection(Projection::SIN),
                                 refCoord(0), refCoord(1), increment(0), increment(1),
                                 xform, refPixel(0), refPixel(1));
    coord.addCoordinate(dirCoord);

    Vector<Int> stokes(1, Stokes::I);
    StokesCoordinate stokesCoord(stokes);
    coord.addCoordinate(stokesCoord);

    Vector<Double> freqs(1, freqHz);
    SpectralCoordinate specCoord(MFrequency::TOPO, freqs);
    coord.addCoordinate(specCoord);

    return coord;
  }

  void createConstantImage(const string &filename, double freqHz, float value) {
    IPosition shape(4, IMAGE_SIZE, IMAGE_SIZE, 1, 1);
    CoordinateSystem coord = makeCoordSystem(freqHz);
    PagedImage<float> image(shape, coord, filename);
    Array<float> data(shape, value);
    image.put(data);
    image.flush();
    image.unlock();
  }

  double centerPixel(const string &imgPath) {
    PagedImage<float> img(imgPath);
    return img(IPosition(4, IMAGE_SIZE / 2, IMAGE_SIZE / 2, 0, 0));
  }
};

// ---------------------------------------------------------------------------
// stage_psf_pb_to_taylor: cube2taylor fed straight from gridding, no
// removeFreqDepPB, no flatsky.
// ---------------------------------------------------------------------------

TEST_F(TaylorPipelineTest, WayInPbToTaylorSkipsFlatteningSteps) {
  createConstantImage("pb.spw0", FREQ0_HZ, 0.9f);
  createConstantImage("pb.spw1", FREQ1_HZ, 0.8f);
  createConstantImage("taylorwt.spw0", FREQ0_HZ, 1.0f);
  createConstantImage("taylorwt.spw1", FREQ1_HZ, 1.0f);

  vector<string> pbNames = {"pb.spw0", "pb.spw1"};
  vector<string> taylorwtNames = {"taylorwt.spw0", "taylorwt.spw1"};

  Taylor avg = avgPBStage();
  ASSERT_NO_THROW(avg.computeAvgPB(pbNames, "avgpb", "minfreqpb", "mean"));

  // Matches driver._taylor_cube2taylor("pb"): cubeImage=pb images, and
  // pbimage=pb_list = per-SPW pbs + [avgpb]. mtnames only uses element [0] as
  // the output base name, so pass a single base, not one name per term.
  vector<string> pbList = pbNames;
  pbList.push_back("avgpb");
  vector<string> pbTaylorBase = {"pb_taylor"};

  Taylor wayIn = wayInStage(PB);
  ASSERT_NO_THROW(wayIn.cubeToTaylorSum(pbNames, pbTaylorBase, pbList, taylorwtNames));

  // No flatsky/removeFreqDepPB call happened above -- if this test compiles and
  // passes it proves the PB Way-In path needs nothing beyond cube2taylor,
  // matching stage_psf_pb_to_taylor's docstring.
  ASSERT_TRUE(directoryExists("pb_taylor.tt0"));
  ASSERT_TRUE(directoryExists("pb_taylor.tt1"));
}

// ---------------------------------------------------------------------------
// stage_residual_to_taylor: removeFreqDepPB then cube2taylor. Bug 1 in the old
// driver used flatsky here instead; validate the fixed sequence reproduces the
// exact closed-form moment sum (see file header) once the avgPB flattening
// step is included.
// ---------------------------------------------------------------------------

TEST_F(TaylorPipelineTest, WayInResidualToTaylorMatchesClosedForm) {
  const float flux0 = 2.0f, flux1 = 5.0f;
  const float pb0 = 0.9f, pb1 = 0.8f;
  const float avgpbVal = (pb0 + pb1) / 2.0f;

  // Gridding state: residual = S(nu) * PB(nu), as stage_imaging would emit.
  createConstantImage("residual.spw0", FREQ0_HZ, flux0 * pb0);
  createConstantImage("residual.spw1", FREQ1_HZ, flux1 * pb1);
  createConstantImage("pb.spw0", FREQ0_HZ, pb0);
  createConstantImage("pb.spw1", FREQ1_HZ, pb1);
  createConstantImage("taylorwt.spw0", FREQ0_HZ, 1.0f);
  createConstantImage("taylorwt.spw1", FREQ1_HZ, 1.0f);

  vector<string> residualNames = {"residual.spw0", "residual.spw1"};
  vector<string> pbNames = {"pb.spw0", "pb.spw1"};
  vector<string> taylorwtNames = {"taylorwt.spw0", "taylorwt.spw1"};

  Taylor avg = avgPBStage();
  ASSERT_NO_THROW(avg.computeAvgPB(pbNames, "avgpb", "minfreqpb", "mean"));
  EXPECT_NEAR(centerPixel("avgpb"), avgpbVal, TOLERANCE);

  // Way In step 1: S(nu)*PB(nu) -> S(nu)*avgPB, matching driver's
  // _taylor_remove_freqdep_pb (mode=removefreqdepPB).
  Taylor flatten = flattenStage();
  ASSERT_NO_THROW(flatten.removeFreqDepPB("avgpb", residualNames, pbNames));
  EXPECT_NEAR(centerPixel("residual.spw0"), flux0 * avgpbVal, TOLERANCE);
  EXPECT_NEAR(centerPixel("residual.spw1"), flux1 * avgpbVal, TOLERANCE);

  // Way In step 2: cube2taylor, per-SPW PB + avgPB appended, matching
  // driver._taylor_cube2taylor's pb_list = per-SPW pb + [avgpbname].
  vector<string> pbList = pbNames;
  pbList.push_back("avgpb");
  vector<string> residualTaylorBase = {"residual_taylor"};

  Taylor wayIn = wayInStage(RESIDUAL);
  ASSERT_NO_THROW(wayIn.cubeToTaylorSum(residualNames, residualTaylorBase, pbList,
                                        taylorwtNames));

  // Closed form: premultiply by cube_masked * pb_chan * sumwt_chan, normalize
  // by wtsum, then (pblimit>0, imtype==RESIDUAL) divide by avgPB. With
  // cube_chan == flux_chan*avgPB (post removeFreqDepPB) this collapses to the
  // same flux0/flux1 weighted moment sum, scaled by pb_chan/avgPB instead of 1.
  double w1 = (FREQ1_HZ - FREQ0_HZ) / FREQ0_HZ;
  double wtsum = 1.0 + 1.0;
  double term0 = flux0 * avgpbVal * pb0, term1 = flux1 * avgpbVal * pb1;
  double expected_tt0 = (term0 + term1) / wtsum / avgpbVal;
  double expected_tt1 = (term0 * 0.0 + term1 * w1) / wtsum / avgpbVal;

  EXPECT_NEAR(centerPixel("residual_taylor.tt0"), expected_tt0, TOLERANCE);
  EXPECT_NEAR(centerPixel("residual_taylor.tt1"), expected_tt1, TOLERANCE);
}

// ---------------------------------------------------------------------------
// stage_taylor_to_cube + stage_prepare_model: taylor2cube with pbimage="" ->
// true-sky per-SPW model, no PB re-applied afterward. This is Bug 4: the old
// code did applyPB(divide avgPB) + applyPB(multiply PB) + dale divmodel, which
// over-subtracts by 1/PB in the major cycle. Validate the reconstructed cube
// equals the bare polynomial tt0 + tt1*w evaluated at each SPW's frequency,
// with no PB factor anywhere.
// ---------------------------------------------------------------------------

TEST_F(TaylorPipelineTest, WayOutTaylorToCubeYieldsTrueSkyModel) {
  const float tt0Val = 3.0f, tt1Val = -0.6f;
  createConstantImage("model.tt0", FREQ0_HZ, tt0Val);
  createConstantImage("model.tt1", FREQ0_HZ, tt1Val);

  // taylor2cube zeroes-and-refills its target cube images in place -- it does
  // not create them. The driver's _create_model_images() copies the per-SPW
  // residual onto the model path before calling taylor2cube; stand in for
  // that here with placeholders of the right shape/coords.
  createConstantImage("model_spw0", FREQ0_HZ, 0.0f);
  createConstantImage("model_spw1", FREQ1_HZ, 0.0f);

  vector<string> modelTaylorNames = {"model.tt0", "model.tt1"};
  vector<string> spwModelNames = {"model_spw0", "model_spw1"};
  vector<string> emptyPb; // pbimage="" in the driver

  // Matches driver._taylor_taylor2cube's pbimage="" -- the `if
  // (!pbnames.empty())` guard then skips the internal applyPB(divide)
  // entirely, so no PB factor is reintroduced.
  Taylor wayOut = wayOutStage();
  ASSERT_NO_THROW(wayOut.taylorToCube(modelTaylorNames, spwModelNames, emptyPb));

  double w0 = 0.0, w1 = (FREQ1_HZ - FREQ0_HZ) / FREQ0_HZ;
  EXPECT_NEAR(centerPixel("model_spw0"), tt0Val + tt1Val * w0, TOLERANCE);
  EXPECT_NEAR(centerPixel("model_spw1"), tt0Val + tt1Val * w1, TOLERANCE);
}

// ---------------------------------------------------------------------------
// Full Way-In -> [mock deconvolve] -> Way-Out round trip in one test, the
// composition run_taylor_fixed.py actually executes per major cycle
// (stage_residual_to_taylor -> stage_deconvolve -> stage_taylor_to_cube ->
// stage_prepare_model).
// ---------------------------------------------------------------------------

TEST_F(TaylorPipelineTest, FullWayInWayOutRoundtripMatchesClosedForm) {
  const float flux0 = 2.0f, flux1 = 5.0f;
  const float pb0 = 0.9f, pb1 = 0.8f;

  createConstantImage("residual.spw0", FREQ0_HZ, flux0 * pb0);
  createConstantImage("residual.spw1", FREQ1_HZ, flux1 * pb1);
  createConstantImage("pb.spw0", FREQ0_HZ, pb0);
  createConstantImage("pb.spw1", FREQ1_HZ, pb1);
  createConstantImage("taylorwt.spw0", FREQ0_HZ, 1.0f);
  createConstantImage("taylorwt.spw1", FREQ1_HZ, 1.0f);

  vector<string> residualNames = {"residual.spw0", "residual.spw1"};
  vector<string> pbNames = {"pb.spw0", "pb.spw1"};
  vector<string> taylorwtNames = {"taylorwt.spw0", "taylorwt.spw1"};

  Taylor avg = avgPBStage();
  ASSERT_NO_THROW(avg.computeAvgPB(pbNames, "avgpb", "minfreqpb", "mean"));
  Taylor flatten = flattenStage();
  ASSERT_NO_THROW(flatten.removeFreqDepPB("avgpb", residualNames, pbNames));

  vector<string> pbList = pbNames;
  pbList.push_back("avgpb");
  vector<string> residualTaylorBase = {"rt"};

  Taylor wayIn = wayInStage(RESIDUAL);
  ASSERT_NO_THROW(wayIn.cubeToTaylorSum(residualNames, residualTaylorBase, pbList,
                                        taylorwtNames));

  // Stand in for Hummbee deconvolve: the model Taylor terms it would emit are
  // algebraically the same coefficients as the residual Taylor terms it was
  // fed (single dominant component).
  double tt0 = centerPixel("rt.tt0"), tt1 = centerPixel("rt.tt1");

  vector<string> modelTaylorNames = {"rt.tt0", "rt.tt1"};
  vector<string> spwModelNames = {"model_spw0", "model_spw1"};
  createConstantImage("model_spw0", FREQ0_HZ, 0.0f);
  createConstantImage("model_spw1", FREQ1_HZ, 0.0f);
  vector<string> emptyPb;

  Taylor wayOut = wayOutStage();
  ASSERT_NO_THROW(wayOut.taylorToCube(modelTaylorNames, spwModelNames, emptyPb));

  double w0 = 0.0, w1 = (FREQ1_HZ - FREQ0_HZ) / FREQ0_HZ;
  EXPECT_NEAR(centerPixel("model_spw0"), tt0 + tt1 * w0, TOLERANCE)
      << "Full Way-In -> Way-Out round trip: SPW0 model should equal tt0+tt1*w0 "
         "with no PB contamination.";
  EXPECT_NEAR(centerPixel("model_spw1"), tt0 + tt1 * w1, TOLERANCE)
      << "Full Way-In -> Way-Out round trip: SPW1 model should equal tt0+tt1*w1 "
         "with no PB contamination.";
}

} // namespace test
