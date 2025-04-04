#include <filesystem>
#include <Chip/chip.h>
#include <gtest/gtest.h>

using namespace std;
using namespace std::filesystem;
using namespace Chip;

namespace test{
  const path goldDir = current_path() / "gold_standard";

TEST(ChipTest, AppLevelPSF) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create test dir 
  std::filesystem::create_directory(testDir);

  //copy over from gold_standard to test dir
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad.psf", testDir/"refim_point_wterm_vlad.n1.psf", copy_options::recursive);
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad.psf", testDir/"refim_point_wterm_vlad.n2.psf", copy_options::recursive);

  //Step into test dir 
  std::filesystem::current_path(testDir);
  
  std::vector<std::string> imageName={"refim_point_wterm_vlad.n1.psf","refim_point_wterm_vlad.n2.psf"};
  string outputimage="refim_point_wterm_vlad.psf";
  bool overwrite=false;
  bool resetoutput=false;
  string stats="none";

  chip(imageName, outputimage, overwrite, resetoutput, stats);

  // Check that the ouput image is generated
  path p1("refim_point_wterm_vlad.psf");
  bool ans = exists(p1) ;
   
  EXPECT_TRUE( ans );

  float tol = 0.05;
  float goldValLoc0 = 2 * 30185.982421875;
  float goldValLoc1 = 2 * -213.523565259375;

  PagedImage<Float> psfimage("refim_point_wterm_vlad.psf");
  EXPECT_NEAR(psfimage(IPosition(4,1024,1024,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(psfimage(IPosition(4,1050,1050,0,0)), goldValLoc1, tol);

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}

TEST(ChipTest, UIFactory) {
    // The Factory Settings.
  int argc = 1;
  const char* argv[] = {"./chip"};

  std::vector<std::string> imageName={"notEmpty"};
  string outputImage="notEmpty", stats="none";
  bool overWrite=false, resetOutputImage=false;
  bool restartUI=false, interactive=false;

  
  UI(restartUI, argc, (char **)argv, interactive, 
    imageName, outputImage, overWrite,
    resetOutputImage, stats);

};

};
