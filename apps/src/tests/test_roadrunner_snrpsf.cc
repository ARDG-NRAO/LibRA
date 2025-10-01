//#include <filesystem>
#include <RoadRunner/roadrunner.h>
//#include <gtest/gtest.h>
#include <tests/test_utils.h>

using namespace std;
using namespace std::filesystem;

namespace test{
  const path goldDir = current_path() / "gold_standard";
 
  TEST(RoadrunnerTest, AppLevelSNRPSF_performance) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create dir AppLevelSNRPSF
  std::filesystem::create_directory(testDir);

  //copy over CYGTST.corespiral.ms from gold_standard to AppLevelSNRPSF
  std::filesystem::copy(goldDir/"CYGTST.corespiral.ms", testDir/"CYGTST.corespiral.ms", copy_options::recursive);
  //copy over 4k_nosquint.cfc from gold_standard to AppLevelSNRPSF
  std::filesystem::copy(goldDir/"4k_nosquint.cfc", testDir/"4k_nosquint.cfc", copy_options::recursive);
  //Step into AppLevelSNRPSF
  std::filesystem::current_path(testDir);

   string MSNBuf="CYGTST.corespiral.ms";
   string cfCache="4k_nosquint.cfc";
   string imageName="htclean_gpu_newpsf.psf";
   string cmplxGridName="htclean_gpu_newpsf_gridv";
   string phaseCenter="J2000 19h57m44.44s  040d35m46.3s";
   string weighting="natural";
   string sowImageExt="sumwt";
   string imagingMode="snrpsf";

  float cellSize=0.025;
  float robust=-0.75;
  int NX=4000, nW=1;

  bool conjBeams=false;
  float pbLimit=0.01;
  bool doSPWDataIter=true;
  vector<float> posigdev = {0.0,0.0};

  string ftmName="awphpg";
  string fieldStr="", spwStr="*", uvDistStr="", dataColumnName="data";
  string modelImageName="",stokes="I";
  string refFreqStr="3.0e9";
  string rmode="none";

  bool WBAwp=true;
  bool doPointing=false;
  bool normalize=false;
  bool doPBCorr= true;


  RRReturnType record = Roadrunner(MSNBuf,imageName, modelImageName,dataColumnName,
                 sowImageExt, cmplxGridName, NX, nW, cellSize,
                 stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
                 ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
                 doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
                 doSPWDataIter);

  // Check that the .psf is generated
  path p1("htclean_gpu_newpsf.psf");
  path p2("htclean_gpu_newpsf.sumwt");
  path p3("htclean_gpu_newpsf_gridv.vis");
  bool ans = exists(p1) && exists(p2) && exists(p3);

  EXPECT_TRUE( ans );

  PagedImage<Float> psfimage("htclean_gpu_newpsf.psf");
  float tol = 0.1;
  float goldValLoc0 = 44163072;
  float goldValLoc1 = 6067482.5;

  EXPECT_NEAR(psfimage(IPosition(4,2000,2000,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(psfimage(IPosition(4,1885,1885,0,0)), goldValLoc1, tol);

  // flag if runtime is abnormal
  auto gold_runtime = 4.3; //gpuhost003
  try {
    auto runtime = record.at(CUMULATIVE_GRIDDING_ENGINE_TIME);
    // to catch if synthesis is NOT compiled with `-O2` which can cause 2x slowness
    EXPECT_LT(runtime, 2.0 * gold_runtime) << "Runtime is too long. Check if casacpp is compiled with -O2";
  }
  catch (const std::out_of_range&) {
        std::cout << "Key CUMULATIVE_GRIDDING_ENGINE_TIME not found" << std::endl;
  }

   if (!IsGpuHost003()) {
     std::cout << "[  INFO ] Skipping vis processing rate check on non-gpuhost003 machine.\n" << std::endl;
   }
   else {
    // flag if Vis Processing rate is reduced by > 10%
    auto gold_rate = 240000; //V100 in gpuhost003 for this setup only
    try {
      auto rate = record.at(NVIS) / record.at(IMAGING_TIME);  // Vis/sec
      EXPECT_GT(rate, 0.9 * gold_rate) << "Vis Processing Rate is reduced by more than 10%. Check if any code change slows it down";
    }
    catch (const std::out_of_range&) {
        std::cout << "Key IMAGING_TIME or NVIS not found" << std::endl;
    }
   }


  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}
}
