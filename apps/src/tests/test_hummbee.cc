// #include <filesystem>
#include <Hummbee/hummbee.h>
#include <gtest/gtest.h>
#include <tests/test_utils.h>
using namespace std;
using namespace std::filesystem;


namespace test{
  const path goldDir = current_path() / "gold_standard";

  // Some basic functions to perform operations that we will use over and over

  // bool directoryExists(const std::string& path) {
    // std::filesystem::path dir(path);
    // return std::filesystem::is_directory(dir);
  // }
  // bool create_directory(const std::filesystem::path& path) {
    // return std::filesystem::create_directory(path);
  // }
  // bool remove_directory(const std::filesystem::path& path) {
    // return std::filesystem::remove_all(path);
  // }

TEST(HummbeeTest, CheckGoldStandardDirectory) {
    EXPECT_TRUE(directoryExists(goldDir));
  }

TEST(HummbeeTest, AppLevelCubeAsp) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create test dir 
  std::filesystem::create_directory(testDir);

    copy(goldDir/"unittest_hummbee.pb", testDir/"unittest_hummbee.pb", copy_options::recursive);
    copy(goldDir/"unittest_hummbee.psf", testDir/"unittest_hummbee.psf", copy_options::recursive);
    copy(goldDir/"unittest_hummbee.residual", testDir/"unittest_hummbee.residual", copy_options::recursive);
    copy(goldDir/"unittest_hummbee.sumwt", testDir/"unittest_hummbee.sumwt", copy_options::recursive);


  // Set the current working directory to the test directory
  current_path(testDir);

  // Check that the return value is true
  string imageName = "unittest_hummbee";
  string modelImageName = "unittest_hummbee.image";
  string deconvolver="asp", specmode="cube";
  vector<float> scales;
  float largestscale = -1;
  float fusedthreshold = 0;
  vector<float> waveletscales;
  vector<float> waveletamps;
  bool wavelettrigger; 
  bool mfasp = false;
  float autothreshold;
  int automaxiter;
  float autogain;
  float hogbomgain;
  bool autohogbom;
  float autotrigger;
  float autopower; 
  int autoxmask; 
  int autoymask; 
  float lbfgsepsf=0.001; 
  float lbfgsepsx=0.001; 
  float lbfgsepsg=0.001; 
  int lbfgsmaxit=5;
  int nterms=1;
  float gain=0.1;
  float threshold=1e-4;
  float nsigma=1.5;
  int cycleniter=10;
  float cyclefactor=1.0;
  vector<string> mask;
  mask.resize(1);
  mask[0] ="circle[[256pix,290pix],140pix]";
  bool doPBCorr = false;
  string imagingMode="deconvolve";

  float PeakRes = Hummbee(imageName, modelImageName,
                 deconvolver,
                 scales,
                 largestscale, fusedthreshold,
                 waveletscales, waveletamps, wavelettrigger, mfasp,
                 autothreshold, automaxiter, autogain, hogbomgain, autohogbom, autotrigger, autopower, autoxmask, autoymask, 
                 lbfgsepsf, lbfgsepsx, lbfgsepsg, lbfgsmaxit,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 mask, specmode,
                 doPBCorr,
                 imagingMode
                 );
	
  PagedImage<Float> modelimage("unittest_hummbee.model");

  float tol = 0.1;
  float goldPeakRes = 4.98845;
  EXPECT_NEAR(PeakRes, goldPeakRes, tol);

  float goldValchan0 = 0.0442593;
  float goldValchan2 = 0.0384871;
  EXPECT_NEAR(modelimage(IPosition(4,275,330,0,0)), goldValchan0, tol);
  EXPECT_NEAR(modelimage(IPosition(4,275,330,0,2)), goldValchan2, tol);

  PagedImage<Float> resimage("unittest_hummbee.residual");
  float resGoldValchan0 = 4.726145;
  float resGoldValchan2 = 1.797757;
  EXPECT_NEAR(resimage(IPosition(4,275,330,0,0)), resGoldValchan0, tol);
  EXPECT_NEAR(resimage(IPosition(4,275,330,0,2)), resGoldValchan2, tol);

  /*remove_all(current_path()/"unittest_hummbee.pb");
  remove_all(current_path()/"unittest_hummbee.psf");
  remove_all(current_path()/"unittest_hummbee.residual");
  //remove_all(current_path()/"unittest_hummbee.sumwt");
  remove_all(current_path()/"unittest_hummbee.model");
  remove_all(current_path()/"unittest_hummbee.mask");*/
 
  // Set the current working directory back to the parent dir
  current_path(testDir.parent_path());

  remove_directory(testDir);
}

TEST(HummbeeTest,  AppLevelMfsAsp) {

  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create test dir 
  std::filesystem::create_directory(testDir);

    copy(goldDir/"unittest_hummbee_mfs_revE.psf", testDir/"unittest_hummbee_mfs_revE.psf", copy_options::recursive);
    copy(goldDir/"unittest_hummbee_mfs_revE.residual", testDir/"unittest_hummbee_mfs_revE.residual", copy_options::recursive);
    copy(goldDir/"unittest_hummbee_mfs_revE.sumwt", testDir/"unittest_hummbee_mfs_revE.sumwt", copy_options::recursive);
    copy(goldDir/"unittest_hummbee_mfs_revE.weight", testDir/"unittest_hummbee_mfs_revE.weight", copy_options::recursive);


  // Set the current working directory to the test directory
  current_path(testDir);

   // Check that the return value is true
  string imageName = "unittest_hummbee_mfs_revE";
  string modelImageName = "unittest_hummbee_mfs_revE.image";
  string deconvolver="asp", specmode="mfs";
  vector<float> scales;
  float largestscale = -1;
  float fusedthreshold = 0.007;
  vector<float> waveletscales;
  vector<float> waveletamps;
  bool wavelettrigger; 
  bool mfasp = false;
  float autothreshold;
  int automaxiter;
  float autogain;
  float hogbomgain;
  bool autohogbom;
  float autotrigger;
  float autopower; 
  int autoxmask; 
  int autoymask; 
  float lbfgsepsf=0.001; 
  float lbfgsepsx=0.001; 
  float lbfgsepsg=0.001; 
  int lbfgsmaxit=5;
  int nterms=1;
  float gain=0.2;
  float threshold=2.6e-07;
  float nsigma=0.0;
  int cycleniter=3;
  float cyclefactor=1.0;
  vector<string> mask;
  mask.resize(3);
  mask[0] = "box[[890pix,1478pix],[1908pix,2131pix]]";
  mask[1] = "box[[1794pix,1828pix],[2225pix,2232pix]]";
  mask[2] = "box[[2077pix,1989pix],[3270pix,2616pix]]";
  bool doPBCorr = false;
  string imagingMode="deconvolve";

  float PeakRes = Hummbee(imageName, modelImageName,
                 deconvolver,
                 scales,
                 largestscale, fusedthreshold,
                 waveletscales, waveletamps, wavelettrigger, mfasp,
                 autothreshold, automaxiter, autogain, hogbomgain, autohogbom, autotrigger, autopower, autoxmask, autoymask, 
                 lbfgsepsf, lbfgsepsx, lbfgsepsg, lbfgsmaxit,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 mask, specmode,
                 doPBCorr,
                 imagingMode
                 );

  PagedImage<Float> modelimage("unittest_hummbee_mfs_revE.model");

  float tol = 0.1;
  float goldPeakRes = 26.0554;
  EXPECT_NEAR(PeakRes, goldPeakRes, tol);

  float goldValLoc0 = 0.000332008;
  float goldValLoc1 = 0.000176319;
  EXPECT_NEAR(modelimage(IPosition(4,1072,1639,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(modelimage(IPosition(4,3072,2406,0,0)), goldValLoc1, tol);

  /*remove_all(current_path()/"unittest_hummbee_mfs_revE.psf");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.residual");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.model");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.mask");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.weight");*/

  // Set the current working directory back to the parent dir
  current_path(testDir.parent_path());

  remove_directory(testDir);

}


TEST(HummbeeTest,  AppLevelWAsp) {

  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create test dir 
  std::filesystem::create_directory(testDir);

  copy(goldDir/"wasp_jet.psf.tt0", testDir/"wasp_jet.psf.tt0", copy_options::recursive);
  copy(goldDir/"wasp_jet.psf.tt1", testDir/"wasp_jet.psf.tt1", copy_options::recursive);
  copy(goldDir/"wasp_jet.psf.tt2", testDir/"wasp_jet.psf.tt2", copy_options::recursive);
  copy(goldDir/"wasp_jet.psf.tt3", testDir/"wasp_jet.psf.tt3", copy_options::recursive);
  copy(goldDir/"wasp_jet.psf.tt4", testDir/"wasp_jet.psf.tt4", copy_options::recursive);
  copy(goldDir/"wasp_jet.residual.tt0", testDir/"wasp_jet.residual.tt0", copy_options::recursive);
  copy(goldDir/"wasp_jet.residual.tt1", testDir/"wasp_jet.residual.tt1", copy_options::recursive);
  copy(goldDir/"wasp_jet.residual.tt2", testDir/"wasp_jet.residual.tt2", copy_options::recursive);
  copy(goldDir/"wasp_jet.sumwt.tt0", testDir/"wasp_jet.sumwt.tt0", copy_options::recursive);
  copy(goldDir/"wasp_jet.sumwt.tt1", testDir/"wasp_jet.sumwt.tt1", copy_options::recursive);
  copy(goldDir/"wasp_jet.sumwt.tt2", testDir/"wasp_jet.sumwt.tt2", copy_options::recursive);
  copy(goldDir/"wasp_jet.sumwt.tt3", testDir/"wasp_jet.sumwt.tt3", copy_options::recursive);
  copy(goldDir/"wasp_jet.sumwt.tt4", testDir/"wasp_jet.sumwt.tt4", copy_options::recursive);


  // Set the current working directory to the test directory
  current_path(testDir);

   // Check that the return value is true
  string imageName = "wasp_jet";
  string modelImageName = "wasp_jet.image";
  string deconvolver="asp", specmode="mfs";
  vector<float> scales;
  float largestscale = 5;
  float fusedthreshold = 0.05;
  vector<float> waveletscales;
  vector<float> waveletamps;
  bool wavelettrigger; 
  bool mfasp = false;
  float autothreshold;
  int automaxiter;
  float autogain;
  float hogbomgain;
  bool autohogbom;
  float autotrigger;
  float autopower; 
  int autoxmask; 
  int autoymask; 
  float lbfgsepsf=0.001; 
  float lbfgsepsx=0.001; 
  float lbfgsepsg=0.001; 
  int lbfgsmaxit=5;
  int nterms=3;
  float gain=0.6;
  float threshold=0.2;
  float nsigma=0.0;
  int cycleniter=3;
  float cyclefactor=1.0;
  bool doPBCorr = false;
  vector<string> mask;
  string imagingMode="deconvolve";

  float PeakRes = Hummbee(imageName, modelImageName,
                 deconvolver,
                 scales,
                 largestscale, fusedthreshold,
                 waveletscales, waveletamps, wavelettrigger, mfasp,
                 autothreshold, automaxiter, autogain, hogbomgain, autohogbom, autotrigger, autopower, autoxmask, autoymask, 
                 lbfgsepsf, lbfgsepsx, lbfgsepsg, lbfgsmaxit,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 mask, specmode,
                 doPBCorr,
                 imagingMode
                 );

  PagedImage<Float> modelimage("wasp_jet.model.tt0");

  float tol = 0.05;
  float goldPeakRes = 2.43175;
  EXPECT_NEAR(PeakRes, goldPeakRes, tol);

  float goldValLoc0 = 0.000633522;
  EXPECT_NEAR(modelimage(IPosition(4,254,331,0,0)), goldValLoc0, tol);
  

  // Set the current working directory back to the parent dir
  current_path(testDir.parent_path());

  remove_directory(testDir);
}


TEST(HummbeeTest,  AppLevelMfsRestore) {

  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create test dir 
  std::filesystem::create_directory(testDir);

    copy(goldDir/"unittest_hummbee_mfs_revE_restore.psf", testDir/"unittest_hummbee_mfs_revE_restore.psf", copy_options::recursive);
    copy(goldDir/"unittest_hummbee_mfs_revE_restore.residual", testDir/"unittest_hummbee_mfs_revE_restore.residual", copy_options::recursive);
    copy(goldDir/"unittest_hummbee_mfs_revE_restore.sumwt", testDir/"unittest_hummbee_mfs_revE_restore.sumwt", copy_options::recursive);
    copy(goldDir/"unittest_hummbee_mfs_revE_restore.weight", testDir/"unittest_hummbee_mfs_revE_restore.weight", copy_options::recursive);
    copy(goldDir/"unittest_hummbee_mfs_revE_restore.model", testDir/"unittest_hummbee_mfs_revE_restore.model", copy_options::recursive);
    copy(goldDir/"unittest_hummbee_mfs_revE_restore_gold.image", testDir/"unittest_hummbee_mfs_revE_restore_gold.image", copy_options::recursive);
    copy(goldDir/"unittest_hummbee_mfs_revE_restore.pb", testDir/"unittest_hummbee_mfs_revE_restore.pb", copy_options::recursive);
    copy(goldDir/"unittest_hummbee_mfs_revE_restore_gold.image.pbcor", testDir/"unittest_hummbee_mfs_revE_restore_gold.image.pbcor", copy_options::recursive);

  // Set the current working directory to the test directory
  current_path(testDir);

   // Check that the return value is true
  string imageName = "unittest_hummbee_mfs_revE_restore";
  string modelImageName = "unittest_hummbee_mfs_revE_restore.image";
  string deconvolver="asp", specmode="mfs";
  vector<float> scales;
  float largestscale = -1;
  float fusedthreshold = 0.007;
  vector<float> waveletscales;
  vector<float> waveletamps;
  bool wavelettrigger; 
  bool mfasp = false;
  float autothreshold;
  int automaxiter;
  float autogain;
  float hogbomgain;
  bool autohogbom;
  float autotrigger;
  float autopower; 
  int autoxmask; 
  int autoymask; 
  float lbfgsepsf=0.001; 
  float lbfgsepsx=0.001; 
  float lbfgsepsg=0.001; 
  int lbfgsmaxit=5;
  int nterms=1;
  float gain=0.2;
  float threshold=2.6e-07;
  float nsigma=0.0;
  int cycleniter=3;
  float cyclefactor=1.0;
  vector<string> mask;
  mask.resize(3);
  mask[0] = "box[[890pix,1478pix],[1908pix,2131pix]]";
  mask[1] = "box[[1794pix,1828pix],[2225pix,2232pix]]";
  mask[2] = "box[[2077pix,1989pix],[3270pix,2616pix]]";
  bool doPBCorr = false;
  string imagingMode="restore";


  float PeakRes = Hummbee(imageName, modelImageName,
                 deconvolver,
                 scales,
                 largestscale, fusedthreshold,
                 waveletscales, waveletamps, wavelettrigger, mfasp,
                 autothreshold, automaxiter, autogain, hogbomgain, autohogbom, autotrigger, autopower, autoxmask, autoymask, 
                 lbfgsepsf, lbfgsepsx, lbfgsepsg, lbfgsmaxit,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 mask, specmode,
                 doPBCorr,
                 imagingMode
                 );

  // Check that the .image is generated
  path p1("unittest_hummbee_mfs_revE_restore.image");
  EXPECT_TRUE(exists(p1));

  PagedImage<Float> image("unittest_hummbee_mfs_revE_restore.image");
  PagedImage<Float> goldimage("unittest_hummbee_mfs_revE_restore_gold.image");
  // check the restored image is the same as the output of HTCSynthesisImager.makeFinalImages()
  float tol = 0.1;
  EXPECT_NEAR(image(IPosition(4,1072,1639,0,0)), goldimage(IPosition(4,1072,1639,0,0)), tol);
  EXPECT_NEAR(image(IPosition(4,3072,2406,0,0)), goldimage(IPosition(4,3072,2406,0,0)), tol);

  ////// test for restore with pbcor //////
  remove_all(current_path()/"unittest_hummbee_mfs_revE_restore.image");
  doPBCorr = true;

  PeakRes = Hummbee(imageName, modelImageName,
                 deconvolver,
                 scales,
                 largestscale, fusedthreshold,
                 waveletscales, waveletamps, wavelettrigger, mfasp,
                 autothreshold, automaxiter, autogain, hogbomgain, autohogbom, autotrigger, autopower, autoxmask, autoymask, 
                 lbfgsepsf, lbfgsepsx, lbfgsepsg, lbfgsmaxit,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 mask, specmode,
                 doPBCorr,
                 imagingMode
                 );

  // Check that the .image .image.pbcor are generated
  path p2("unittest_hummbee_mfs_revE_restore.image");
  path p3("unittest_hummbee_mfs_revE_restore.image.pbcor");
  EXPECT_TRUE(exists(p2) && exists(p3));

  PagedImage<Float> pbcor("unittest_hummbee_mfs_revE_restore.image.pbcor");
  PagedImage<Float> goldpbcor("unittest_hummbee_mfs_revE_restore_gold.image.pbcor");
  // check the restored image.pbcor is the same as the output of HTCSynthesisImager.makeFinalImages()
  //std::cout << "pbcor " << pbcor(IPosition(4,1072,1639,0,0)) << " , gold pbcor " << goldpbcor(IPosition(4,1072,1639,0,0)) << std::endl;
  EXPECT_NEAR(pbcor(IPosition(4,1072,1639,0,0)), goldpbcor(IPosition(4,1072,1639,0,0)), tol);
  EXPECT_NEAR(pbcor(IPosition(4,3072,2406,0,0)), goldpbcor(IPosition(4,3072,2406,0,0)), tol);

  // Set the current working directory back to the parent dir
  current_path(testDir.parent_path());

  remove_directory(testDir);

}


TEST(HummbeeTest, UIFactory) {
    // The Factory Settings.
  int argc = 1;
  const char* argv[] = {"./hummbee"};

  // The Factory Settings.
  string imageName, modelImageName, deconvolver="hogbom", specmode="mfs";
  bool restartUI=false;

  vector<float> scales;
  float largestscale = -1;
  float fusedthreshold = 0;
  vector<float> waveletscales;
  vector<float> waveletamps;
  bool wavelettrigger; 
  bool mfasp = false;
  float autothreshold;
  int automaxiter;
  float autogain;
  float hogbomgain;
  bool autohogbom;
  float autotrigger;
  float autopower; 
  int autoxmask; 
  int autoymask; 
  float lbfgsepsf=0.001; 
  float lbfgsepsx=0.001; 
  float lbfgsepsg=0.001; 
  int lbfgsmaxit=5;
  int nterms=1;
  float gain=0.1; 
  float threshold=0.0;
  float nsigma=0.0;
  int cycleniter=-1;
  float cyclefactor=1.0;
  vector<string> mask; 
  bool interactive = false;
  bool doPBCorr = false;
  string imagingMode;

  UI(restartUI, argc, (char **)argv, interactive,
    imageName, modelImageName, 
    deconvolver,
    scales,
    largestscale, fusedthreshold,
    waveletscales, waveletamps, wavelettrigger, mfasp,
    autothreshold, automaxiter, autogain, hogbomgain, autohogbom, autotrigger, autopower, autoxmask, autoymask, 
    lbfgsepsf, lbfgsepsx, lbfgsepsg, lbfgsmaxit,
    nterms,
    gain, threshold,
    nsigma,
    cycleniter, cyclefactor,
    mask, specmode,
    doPBCorr,
    imagingMode);
}

};
