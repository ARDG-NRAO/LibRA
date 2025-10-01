#include <RoadRunner/roadrunner.h>
#include <tests/test_utils.h>

using namespace std;
using namespace std::filesystem;

namespace test{
  const path goldDir = current_path() / "gold_standard";

  TEST(RoadrunnerTest, Interface_rmode_minus2) {
  // Small delay to ensure proper cleanup
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // Test if the rmode is set correctly in Roadrunner()
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create dir 
  std::filesystem::create_directory(testDir);

  //copy over CYGTST.corespiral.ms from gold_standard to Interface_rmode
  std::filesystem::copy(goldDir/"CYGTST.corespiral.ms", testDir/"CYGTST.corespiral.ms", copy_options::recursive);
  //copy over 4k_nosquint.cfc from gold_standard to Interface_rmode
  std::filesystem::copy(goldDir/"4k_nosquint.cfc", testDir/"4k_nosquint.cfc", copy_options::recursive);
  //Step into test dir
  std::filesystem::current_path(testDir);


  // weighting=briggs robust=-2
  string MSNBuf="CYGTST.corespiral.ms";
   string cfCache="4k_nosquint.cfc";
   string imageName="BriggsRobust-2.weight";
   string cmplxGridName="";
   string phaseCenter="J2000 19h57m44.44s  040d35m46.3s";
   string weighting="briggs";
   string sowImageExt="sumwt";
   string imagingMode="weight";

  float cellSize=0.025;
  float robust=-2;
  int NX=4000, nW=1;

  bool conjBeams=false;
  float pbLimit=0.01;
  bool doSPWDataIter=true;
  vector<float> posigdev = {0.0,0.0};

  string ftmName="awphpg";
  string fieldStr="", spwStr="*", uvDistStr="", dataColumnName="data";
  string modelImageName="",stokes="I";
  string refFreqStr="3.0e9";
  string rmode="";

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

  // Check that the images are generated
  path p3("BriggsRobust-2.weight");
  path p4("BriggsRobust-2.sumwt");
  bool ans = exists(p3) && exists(p4);

  EXPECT_TRUE( ans );

  PagedImage<casacore::Float> Uimage("BriggsRobust-2.weight");
  float tol = 0.1;
  float goldValLoc1 = 46.539;
  EXPECT_NEAR(Uimage(IPosition(4,1994,1973,0,0)), goldValLoc1, tol);

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}

}
