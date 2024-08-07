#include <filesystem>
#include "Acme/acme.h"
#include "gtest/gtest.h"

using namespace std;
using namespace std::filesystem;

namespace test{

TEST(AcmeTest, AppLevelPSF) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;
  string testdir = testDir.string();

  // create test dir 
  std::filesystem::create_directory(testDir);

  //copy over from gold_standard to test dir
  std::filesystem::copy(current_path()/"gold_standard/refim_point_wterm_vlad.psf", testdir + "/refim_point_wterm_vlad.psf", copy_options::recursive);
  std::filesystem::copy(current_path()/"gold_standard/refim_point_wterm_vlad.weight", testdir + "/refim_point_wterm_vlad.weight", copy_options::recursive);
  std::filesystem::copy(current_path()/"gold_standard/refim_point_wterm_vlad.sumwt", testdir + "/refim_point_wterm_vlad.sumwt", copy_options::recursive);

  //Step into test dir 
  std::filesystem::current_path(testDir);
  
  // note, workdir, psfcutoff and facets are actually not used un acme
  string imageName="refim_point_wterm_vlad", deconvolver="hogbom", normtype="flatnoise", workdir, imType="psf", mode="normalize";
  float pblimit=0.005, psfcutoff=0.35;
  int nterms=1, facets=1;
  vector<float> restoringbeam;
  vector<string> partImageNames;
  bool resetImages=false;
  bool computePB = true;

  acme_func(imageName, deconvolver, 
    normtype, workdir, mode, imType, pblimit, 
    nterms, facets, psfcutoff, restoringbeam, 
    partImageNames, resetImages, computePB);
  

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


TEST(AcmeTest, AppLevelResidual) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;
  string testdir = testDir.string();
  
  // create test dir 
  std::filesystem::create_directory(testDir);

  //copy over from gold_standard to test dir
  std::filesystem::copy(current_path()/"gold_standard/refim_point_wterm_vlad_step2.psf", testdir + "/refim_point_wterm_vlad_step2.psf", copy_options::recursive);
  std::filesystem::copy(current_path()/"gold_standard/refim_point_wterm_vlad_step2.weight", testdir + "/refim_point_wterm_vlad_step2.weight", copy_options::recursive);
  std::filesystem::copy(current_path()/"gold_standard/refim_point_wterm_vlad_step2.sumwt", testdir + "/refim_point_wterm_vlad_step2.sumwt", copy_options::recursive);
  std::filesystem::copy(current_path()/"gold_standard/refim_point_wterm_vlad_step2.pb", testdir + "/refim_point_wterm_vlad_step2.pb", copy_options::recursive);
  std::filesystem::copy(current_path()/"gold_standard/refim_point_wterm_vlad_step2.residual", testdir + "/refim_point_wterm_vlad_step2.residual", copy_options::recursive);
  
  //Step into test dir 
  std::filesystem::current_path(testDir);
  // note, workdir, psfcutoff and facets are actually not used un acme
  string imageName="refim_point_wterm_vlad_step2", deconvolver="hogbom", normtype="flatnoise", workdir, imType="residual", mode="normalize";
  float pblimit=0.005, psfcutoff=0.35;
  int nterms=1, facets=1;
  vector<float> restoringbeam;
  vector<string> partImageNames;
  bool resetImages=false;
  bool computePB = false;

  acme_func(imageName, deconvolver,
    normtype, workdir, mode, imType, pblimit,
    nterms, facets, psfcutoff, restoringbeam,
    partImageNames, resetImages, computePB);

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



// turned off this test until the cl interface is fixed.
// issue is even though `imageName` is set, the cl interface
// still thinks it's empty
/*TEST(AcmeTest, UIFactory) {
    // The Factory Settings.
  int argc = 1;
  char* argv[] = {"./acme"};

  string imageName="notEmpty", deconvolver="hogbom", normtype="flatnoise", workdir, imType="psf";
  float pblimit=0.2, psfcutoff=0.35;
  int nterms=1, facets=1;
  vector<float> restoringbeam;
  Bool computePB=False;
  Bool restartUI=False;;
  bool interactive = false;
  
  UI(restartUI, argc, argv, interactive, 
    imageName, 
    deconvolver, normtype, workdir, imType, 
    pblimit, nterms, facets, psfcutoff, 
    restoringbeam, computePB);
}*/



};
