#include <filesystem>
#include "Restore/restore.h"
#include "translation/casacore_restore.h"

#include "gtest/gtest.h"

using namespace std;
using namespace std::filesystem;


namespace test{


TEST(RestoreTest, RestoreFuncLevel)
{
  constexpr int n_rows = 2;
  constexpr int n_cols = 5;
  float data_row_major_model[] = {
       1,   2,   3,  /*|*/  4,   5,  /* X | */
       6,   7,   8,  /*|*/  9,  10   /* X | */
  };
  auto model = col_major_mdspan<float>(data_row_major_model, n_rows, n_cols);

  float data_row_major_res[] = {
       1,   -1,   1,  /*|*/  -1,   1,  /* X | */
       -1,   1,   -1,  /*|*/  1,  -1   /* X | */
  };
  auto residual = col_major_mdspan<float>(data_row_major_res, n_rows, n_cols);
  
  auto emptydata1 = std::make_unique<float[]>(n_rows * n_cols);
  auto image = col_major_mdspan<float>(emptydata1.get(), n_rows, n_cols);

  auto emptydata3 = std::make_unique<float[]>(n_rows * n_cols);
  auto pb = col_major_mdspan<float>(emptydata3.get(), n_rows, n_cols);

  auto emptydata4 = std::make_unique<float[]>(n_rows * n_cols);
  auto image_pbcor = col_major_mdspan<float>(emptydata4.get(), n_rows, n_cols);

  double refi = 1.0;
  double refj = 1.0;
  double inci = 0.5;
  double incj = 0.5;
  double majaxis = 2.0;
  double minaxis = 1.0;
  double pa = 0;
  /*int nSubChans = 1; 
  int chanid = 0;*/
  bool pbcor = false;

  Restore<float>(model, residual, image,
      n_rows, n_cols, 
      refi, refj, inci, incj,
      majaxis, minaxis, pa,
      /*nSubChans, chanid,*/
      pbcor, pb, image_pbcor);

  double tol = 0.01;
  float goldValLoc1 = 19.6208;
  float goldValLoc2 = 28.8573;

  #if MDSPAN_USE_BRACKET_OPERATOR
    EXPECT_NEAR(image[1, 0], goldValLoc1, tol);
    EXPECT_NEAR(image[2, 0], goldValLoc2, tol);
  #else
    EXPECT_NEAR(image(1, 0), goldValLoc1, tol);
    EXPECT_NEAR(image(2, 0), goldValLoc2, tol);
  #endif
   
}


TEST(RestoreTest, casacore_restore) {

  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
  // Create a unique directory for this test case
  path testDir = current_path() / testName;
  string testdir = testDir.string();

  // create dir 
  std::filesystem::create_directory(testDir);

  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.psf", testdir+"/unittest_hummbee_mfs_revE_restore.psf", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.residual", testdir+"/unittest_hummbee_mfs_revE_restore.residual", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.sumwt", testdir+"/unittest_hummbee_mfs_revE_restore.sumwt", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.weight", testdir+"/unittest_hummbee_mfs_revE_restore.weight", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.model", testdir+"/unittest_hummbee_mfs_revE_restore.model", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore_gold.image", testdir+"/unittest_hummbee_mfs_revE_restore_gold.image", copy_options::recursive);
  
  // Set the current working directory to the test directory
  current_path(testDir);

  string imageName = "unittest_hummbee_mfs_revE_restore";
  bool doPBCorr = false;
  casacore_restore(imageName, doPBCorr);

  // Check that the .image is generated
  path p1("unittest_hummbee_mfs_revE_restore.image");
  EXPECT_TRUE(exists(p1));

  PagedImage<Float> image("unittest_hummbee_mfs_revE_restore.image");
  PagedImage<Float> goldimage("unittest_hummbee_mfs_revE_restore_gold.image");
  // check the restored image is the same as the output of HTCSynthesisImager.makeFinalImages()
  float tol = 0.05;
  EXPECT_NEAR(image(IPosition(4,1072,1639,0,0)), goldimage(IPosition(4,1072,1639,0,0)), tol);
  EXPECT_NEAR(image(IPosition(4,3072,2406,0,0)), goldimage(IPosition(4,3072,2406,0,0)), tol);

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}


TEST(RestoreTest, casacore_restore_pbcor) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
// Create a unique directory for this test case
  path testDir = current_path() / testName;
  string testdir = testDir.string();

  // create dir 
  std::filesystem::create_directory(testDir);

  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.psf", testdir+"/unittest_hummbee_mfs_revE_restore.psf", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.residual", testdir+"/unittest_hummbee_mfs_revE_restore.residual", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.sumwt", testdir+"/unittest_hummbee_mfs_revE_restore.sumwt", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.weight", testdir+"/unittest_hummbee_mfs_revE_restore.weight", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.model", testdir+"/unittest_hummbee_mfs_revE_restore.model", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore_gold.image", testdir+"/unittest_hummbee_mfs_revE_restore_gold.image", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.pb", testdir+"/unittest_hummbee_mfs_revE_restore.pb", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore_gold.image.pbcor", testdir+"/unittest_hummbee_mfs_revE_restore_gold.image.pbcor", copy_options::recursive);

  // Set the current working directory to the test directory
  current_path(testDir);

  string imageName = "unittest_hummbee_mfs_revE_restore";
  bool doPBCorr = true;
  casacore_restore(imageName, doPBCorr);

  // Check that the .image .image.pbcor are generated
  path p2("unittest_hummbee_mfs_revE_restore.image");
  path p3("unittest_hummbee_mfs_revE_restore.image.pbcor");
  EXPECT_TRUE(exists(p2) && exists(p3));

  PagedImage<Float> pbcor("unittest_hummbee_mfs_revE_restore.image.pbcor");
  PagedImage<Float> goldpbcor("unittest_hummbee_mfs_revE_restore_gold.image.pbcor");
  // check the restored image.pbcor is the same as the output of HTCSynthesisImager.makeFinalImages()
  //std::cout << "pbcor " << pbcor(IPosition(4,1072,1639,0,0)) << " , gold pbcor " << goldpbcor(IPosition(4,1072,1639,0,0)) << std::endl;
  float tol = 0.05;
  EXPECT_NEAR(pbcor(IPosition(4,1072,1639,0,0)), goldpbcor(IPosition(4,1072,1639,0,0)), tol);
  EXPECT_NEAR(pbcor(IPosition(4,3072,2406,0,0)), goldpbcor(IPosition(4,3072,2406,0,0)), tol);

  // Set the current working directory back to the parent dir
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}


TEST(RestoreTest, casacore_restore_psf) {

  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
  // Create a unique directory for this test case
  path testDir = current_path() / testName;
  string testdir = testDir.string();

  // create dir 
  std::filesystem::create_directory(testDir);

  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.psf", testdir+"/unittest_hummbee_mfs_revE_restore.psf", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.residual", testdir+"/unittest_hummbee_mfs_revE_restore.residual", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.sumwt", testdir+"/unittest_hummbee_mfs_revE_restore.sumwt", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.weight", testdir+"/unittest_hummbee_mfs_revE_restore.weight", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore.model", testdir+"/unittest_hummbee_mfs_revE_restore.model", copy_options::recursive);
  copy(current_path()/"gold_standard/unittest_hummbee_mfs_revE_restore_gold.image", testdir+"/unittest_hummbee_mfs_revE_restore_gold.image", copy_options::recursive);
  
  // Set the current working directory to the test directory
  current_path(testDir);

  string imageName = "unittest_hummbee_mfs_revE_restore";
  bool doPBCorr = false;
  casacore_restore_psf(imageName, doPBCorr);

  // Check that the .image is generated
  path p1("unittest_hummbee_mfs_revE_restore.image");
  EXPECT_TRUE(exists(p1));

  PagedImage<Float> image("unittest_hummbee_mfs_revE_restore.image");
  PagedImage<Float> goldimage("unittest_hummbee_mfs_revE_restore_gold.image");
  // check the restored image is the same as the output of HTCSynthesisImager.makeFinalImages()
  float tol = 0.05;
  EXPECT_NEAR(image(IPosition(4,1072,1639,0,0)), goldimage(IPosition(4,1072,1639,0,0)), tol);
  EXPECT_NEAR(image(IPosition(4,3072,2406,0,0)), goldimage(IPosition(4,3072,2406,0,0)), tol);

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}



};
