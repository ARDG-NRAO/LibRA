//#include <filesystem>
#include <RoadRunner/roadrunner.h>
//#include <gtest/gtest.h>
#include <tests/test_utils.h>

using namespace std;
using namespace std::filesystem;

namespace test{
  const path goldDir = current_path() / "gold_standard";

  TEST(RoadrunnerTest, AppLevelWeight) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create dir AppLevelWeight
  std::filesystem::create_directory(testDir);
  //copy over CYGTST.corespiral.ms from gold_standard to AppLevelWeight
  std::filesystem::copy(goldDir/"CYGTST.corespiral.ms", testDir/"CYGTST.corespiral.ms", copy_options::recursive);
  //copy over 4k_nosquint.cfc from gold_standard to AppLevelWeight
  std::filesystem::copy(goldDir/"4k_nosquint.cfc", testDir/"4k_nosquint.cfc", copy_options::recursive);
  //Step into AppLevelWeight
  std::filesystem::current_path(testDir);

   string MSNBuf="CYGTST.corespiral.ms";
   string cfCache="4k_nosquint.cfc";
   string imageName="htclean_gpu_newpsf.weight";
   string cmplxGridName="htclean_gpu_newpsf_gridv";
   string phaseCenter="J2000 19h57m44.44s  040d35m46.3s";
   string weighting="natural";
   string sowImageExt="sumwt";
   string imagingMode="weight";

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


  Roadrunner(MSNBuf,imageName, modelImageName,dataColumnName,
                 sowImageExt, cmplxGridName, NX, nW, cellSize,
                 stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
                 ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
                 doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
                 doSPWDataIter);

  // Check that the weight files are generated
  path p1("htclean_gpu_newpsf.weight");
  path p2("htclean_gpu_newpsf.sumwt");
  bool ans = exists(p1) && exists(p2);
  EXPECT_TRUE( ans );
   
  PagedImage<Float> weight("htclean_gpu_newpsf.weight");
  float tol = 0.1;
  float goldValLoc0 = 1255553.875;
  float goldValLoc1 = 580468.3125;
  EXPECT_NEAR(weight(IPosition(4,2000,2053,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(weight(IPosition(4,1379,1969,0,0)), goldValLoc1, tol);


  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());
  remove_all(testDir);

}


}
