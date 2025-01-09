#include <filesystem>
#include "Dale/dale.h"
#include "gtest/gtest.h"

using namespace std;
using namespace std::filesystem;
using namespace Dale;

namespace test{
  const path goldDir = current_path() / "gold_standard";

TEST(DaleTest, AppLevelPSF) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;
  string testdir = testDir.string();

  // create test dir 
  std::filesystem::create_directory(testDir);

  //copy over from gold_standard to test dir
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad.psf", testDir/"refim_point_wterm_vlad.psf", copy_options::recursive);
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad.weight", testDir/"refim_point_wterm_vlad.weight", copy_options::recursive);
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad.sumwt", testDir/"refim_point_wterm_vlad.sumwt", copy_options::recursive);

  //Step into test dir 
  std::filesystem::current_path(testDir);
  
  // note, workdir, psfcutoff and facets are actually not used un acme
  string imageName="refim_point_wterm_vlad", normtype="flatnoise", imType="psf";
  string wtImageName, sowImageName;
  float pblimit=0.005, psfcutoff=0.35;
  bool computePB = true;
  bool normalize_weight = true;

  Dale::dale(imageName, 
	     wtImageName, sowImageName,
	     normtype, imType, pblimit, 
	     psfcutoff,
	     computePB, normalize_weight);
  

  // Check that the .psf is generated
  path p1("refim_point_wterm_vlad.psf");
  path p2("refim_point_wterm_vlad.pb");
  bool ans = exists(p1) && exists(p2) ;
   
  EXPECT_TRUE( ans );

  float tol = 0.05;
  float goldValLoc0 = 1;
  float goldValLoc1 = -0.0070736;

  PagedImage<Float> psfimage("refim_point_wterm_vlad.psf");
  EXPECT_NEAR(psfimage(IPosition(4,1024,1024,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(psfimage(IPosition(4,1050,1050,0,0)), goldValLoc1, tol);

  PagedImage<Float> pbimage("refim_point_wterm_vlad.pb");
  EXPECT_NEAR(pbimage(IPosition(4,1024,1024,0,0)), goldValLoc0, tol);

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}


TEST(DaleTest, AppLevelResidual) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;
  string testdir = testDir.string();
  
  // create test dir 
  std::filesystem::create_directory(testDir);

  //copy over from gold_standard to test dir
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad_step2.psf", testDir/"refim_point_wterm_vlad_step2.psf", copy_options::recursive);
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad_step2.weight", testDir/"refim_point_wterm_vlad_step2.weight", copy_options::recursive);
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad_step2.sumwt", testDir/"refim_point_wterm_vlad_step2.sumwt", copy_options::recursive);
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad_step2.pb", testDir/"refim_point_wterm_vlad_step2.pb", copy_options::recursive);
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad_step2.residual", testDir/"refim_point_wterm_vlad_step2.residual", copy_options::recursive);
  
  //Step into test dir 
  std::filesystem::current_path(testDir);
  // note, workdir, psfcutoff and facets are actually not used un acme
  string imageName="refim_point_wterm_vlad_step2",
    normtype="flatnoise", imType="residual";
  string wtImageName, sowImageName;
  float pblimit=0.005, psfcutoff=0.35;
  bool computePB = false;
  bool normalize_weight = false;

  Dale::dale(imageName, 
	     wtImageName, sowImageName,
	     normtype, imType, pblimit,
	     psfcutoff, 
	     computePB, normalize_weight);

  float tol = 0.05;
  float goldValLoc0 = 0.9942138;
  float goldValLoc1 = 0.007225;

  PagedImage<Float> resimage("refim_point_wterm_vlad_step2.residual");
  EXPECT_NEAR(resimage(IPosition(4,1158,1384,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(resimage(IPosition(4,1050,1050,0,0)), goldValLoc1, tol);

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}

TEST(DaleTest, AppLevelModel) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;
  string testdir = testDir.string();
  
  // create test dir 
  std::filesystem::create_directory(testDir);

  //copy over from gold_standard to test dir
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad_step3.model", testDir/"refim_point_wterm_vlad_step3.model", copy_options::recursive);
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad_step3.weight", testDir/"refim_point_wterm_vlad_step3.weight", copy_options::recursive);
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad_step3.sumwt", testDir/"refim_point_wterm_vlad_step3.sumwt", copy_options::recursive);
    
  //Step into test dir 
  std::filesystem::current_path(testDir);
  // note, workdir, psfcutoff and facets are actually not used un acme
  string imageName="refim_point_wterm_vlad_step3",
    normtype="flatnoise", imType="model";
  string wtImageName, sowImageName;
  
  float pblimit=0.005, psfcutoff=0.35;
  bool computePB = false;
  bool normalize_weight = false;

  Dale::dale(imageName, 
	     wtImageName, sowImageName,
	     normtype, imType, pblimit,
	     psfcutoff, 
	     computePB, normalize_weight);

  float tol = 0.01;
  float goldValLoc0 = 1.52205098;
  
  PagedImage<Float> modimage("refim_point_wterm_vlad_step3.divmodel");
  EXPECT_NEAR(modimage(IPosition(4,1158,1384,0,0)), goldValLoc0, tol);
  
  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}


TEST(DaleTest, UIFactory) {
    // The Factory Settings.
  int argc = 1;
  char* argv[] = {"./dale"};

  string imageName="notEmpty", normtype="flatnoise", imType="psf";
  float pblimit=0.2, psfcutoff=0.35;
  bool computePB=false;
  bool normalize_weight=false;
  bool restartUI=false;;
  bool interactive = false;
  
  UI(restartUI, argc, argv, interactive, 
    imageName, normtype, imType, 
    pblimit, psfcutoff, 
    computePB, normalize_weight);
}


};
