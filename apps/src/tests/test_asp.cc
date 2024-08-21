#include <filesystem>
#include "Asp/asp.h"
#include "translation/casacore_asp.h"
#include "translation/casacore_asp_cube.h"
#include "translation/casacore_asp_mdspan.h"
#include "Asp_mdspan/asp_mdspan.h"

#include "gtest/gtest.h"

using namespace std;
using namespace std::filesystem;


namespace test{
  const path goldDir = current_path() / "gold_standard";

TEST(AspTest, AspFuncLevel) {
  string specmode="cube";
  float largestscale = -1;
  float fusedthreshold = 0;
  int nterms=2;
  float gain=0.1;
  float threshold=1e-4;
  float nsigma=1.5;
  int cycleniter=10;
  float cyclefactor=1.0;
  float psfwidth = 5.0;
  float nsigmathreshold = 1;

  size_t nx = 2;
  size_t ny = 5;

  vector<vector<float>> psfb =
    {
        {1,8,12,20,25},
        {5,9,13,24,26}
    };

  vector<vector<float>> modelb(nx,vector<float>(ny,0));
  vector<vector<float>> residualb = {
        {1,8,12,-20,-25},
        {5,9,-13,24,26}
    };
  vector<vector<float>> maskb = {
        {1,8,0,20,25},
        {5,9,13,24,0}
    };

  Asp<float>(modelb, psfb, residualb,
       maskb,
       nx, ny,
       psfwidth,
       largestscale, fusedthreshold,
       nterms,
       gain, 
       threshold, nsigmathreshold,
       nsigma,
       cycleniter, cyclefactor,
       specmode
       );

  EXPECT_EQ(psfb[1][0],5.0);
  EXPECT_EQ(residualb[0][3],-20.0);  
}

TEST(AspTest, AspMDSpanFuncLevel)
{
  constexpr int n_rows = 2;
  constexpr int n_cols = 5;
  int data_row_major[] = {
       1,   2,   3,  /*|*/  4,   5,  /* X | */
       6,   7,   8,  /*|*/  9,  10   /* X | */
  };

  auto m = col_major_mdspan<int>(data_row_major, n_rows, n_cols);
  Matrix<int> matrix(n_rows, n_cols, 0);

  mdspan2casamatrix<int>(m, matrix);
  EXPECT_EQ(matrix(1,0), 2);
  EXPECT_EQ(matrix(0,4), 9);

  int data_b[n_cols * n_rows];
  auto md2 = col_major_mdspan<int>(data_b, n_rows, n_cols);
  casamatrix2mdspan<int>(matrix, md2);
  #if MDSPAN_USE_BRACKET_OPERATOR
    EXPECT_EQ(md2[1, 0], 2);
  #else
    EXPECT_EQ(md2(1, 0), 2);
  #endif
}


TEST(AspTest, casacore_asp_mfs) {

  // Check that the return value is true
  string imageName = "unittest_hummbee_mfs_revE";
  string modelImageName = "unittest_hummbee_mfs_revE.image";
  string specmode="mfs";
  float largestscale = -1;
  float fusedthreshold = 0.007;
  int nterms=2;
  float gain=0.2;
  float threshold=2.6e-07;
  float nsigma=0.0;
  int cycleniter=3;
  float cyclefactor=1.0;
  //vector<string> mask;
  //mask.resize(3);
  //mask[0] = "box[[890pix,1478pix],[1908pix,2131pix]]";
  //mask[1] = "box[[1794pix,1828pix],[2225pix,2232pix]]";
  //mask[2] = "box[[2077pix,1989pix],[3270pix,2616pix]]";
  
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create dir 
  std::filesystem::create_directory(testDir);
  
  std::filesystem::copy(goldDir/"unittest_hummbee_mfs_revE.psf", testDir/"unittest_hummbee_mfs_revE.psf", copy_options::recursive);
  std::filesystem::copy(goldDir/"unittest_hummbee_mfs_revE.mask", testDir/"unittest_hummbee_mfs_revE.mask", copy_options::recursive);
  std::filesystem::copy(goldDir/"unittest_hummbee_mfs_revE.residual", testDir/"unittest_hummbee_mfs_revE.residual", copy_options::recursive);
  //Step into dir
  std::filesystem::current_path(testDir);

  //copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee_mfs_revE.psf", current_path()/"unittest_hummbee_mfs_revE.psf", copy_options::recursive);
  //copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee_mfs_revE.residual", current_path()/"unittest_hummbee_mfs_revE.residual", copy_options::recursive);
  //copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee_mfs_revE.mask", current_path()/"unittest_hummbee_mfs_revE.mask", copy_options::recursive);

  casacore_asp(imageName, modelImageName,
                 largestscale, fusedthreshold,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 specmode
                 );


  PagedImage<Float> modelimage("unittest_hummbee_mfs_revE.model");

  float tol = 0.1;
  float goldValLoc0 = 0.000332008;
  float goldValLoc1 = 0.000176319;
  EXPECT_NEAR(modelimage(IPosition(4,1072,1639,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(modelimage(IPosition(4,3072,2406,0,0)), goldValLoc1, tol);
  
  PagedImage<Float> resimage("unittest_hummbee_mfs_revE.residual");
  float resGoldValLoc = 9.44497;
  EXPECT_NEAR(resimage(IPosition(4,1072,1639,0,0)), resGoldValLoc, tol);

  /*remove_all(current_path()/"unittest_hummbee_mfs_revE.psf");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.residual");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.model");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.mask");*/

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}


TEST(AspTest, casacore_asp_mdspan_mfs) {

  // Check that the return value is true
  string imageName = "unittest_hummbee_mfs_revE";
  string modelImageName = "unittest_hummbee_mfs_revE.image";
  string specmode="mfs";
  float largestscale = -1;
  float fusedthreshold = 0.007;
  int nterms=2;
  float gain=0.2;
  float threshold=2.6e-07;
  float nsigma=0.0;
  int cycleniter=3;
  float cyclefactor=1.0;

  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
// Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create dir 
  std::filesystem::create_directory(testDir);

  std::filesystem::copy(goldDir/"unittest_hummbee_mfs_revE.psf",  testDir/"unittest_hummbee_mfs_revE.psf", copy_options::recursive);
  std::filesystem::copy(goldDir/"unittest_hummbee_mfs_revE.mask",  testDir/"unittest_hummbee_mfs_revE.mask", copy_options::recursive);
  std::filesystem::copy(goldDir/"unittest_hummbee_mfs_revE.residual",  testDir/"unittest_hummbee_mfs_revE.residual", copy_options::recursive);
  //Step into dir
  std::filesystem::current_path(testDir);

  casacore_asp_mdspan(imageName, modelImageName,
                 largestscale, fusedthreshold,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 specmode
                 );

  PagedImage<Float> modelimage("unittest_hummbee_mfs_revE.model");

  float tol = 0.1;
  float goldValLoc0 = 0.000332008;
  float goldValLoc1 = 0.000176319;
  EXPECT_NEAR(modelimage(IPosition(4,1072,1639,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(modelimage(IPosition(4,3072,2406,0,0)), goldValLoc1, tol);
  PagedImage<Float> resimage("unittest_hummbee_mfs_revE.residual");
  float resGoldValLoc = 9.44497;
  EXPECT_NEAR(resimage(IPosition(4,1072,1639,0,0)), resGoldValLoc, tol);

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}






TEST(AspTest, casacore_asp_cube) {
  string imageName = "unittest_hummbee";
  string modelImageName = "unittest_hummbee.image";
  string specmode="cube";
  float largestscale = -1;
  float fusedthreshold = 0;
  int nterms=2;
  float gain=0.1;
  float threshold=1e-4;
  float nsigma=1.5;
  int cycleniter=10;
  float cyclefactor=1.0;
  //vector<string> mask;
  //mask.resize(1);
  //mask[0] ="circle[[256pix,290pix],140pix]";

  /*copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.pb", current_path()/"unittest_hummbee.pb", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.psf", current_path()/"unittest_hummbee.psf", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.residual", current_path()/"unittest_hummbee.residual", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.mask", current_path()/"unittest_hummbee.mask", copy_options::recursive);*/
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create dir 
  std::filesystem::create_directory(testDir);
 
  std::filesystem::copy(goldDir/"unittest_hummbee.pb", testDir/"unittest_hummbee.pb", copy_options::recursive); 
  std::filesystem::copy(goldDir/"unittest_hummbee.psf", testDir/"unittest_hummbee.psf", copy_options::recursive);
  std::filesystem::copy(goldDir/"unittest_hummbee.mask", testDir/"unittest_hummbee.mask", copy_options::recursive);
  std::filesystem::copy(goldDir/"unittest_hummbee.residual", testDir/"unittest_hummbee.residual", copy_options::recursive);
  //Step into dir
  std::filesystem::current_path(testDir);


  casacore_asp_cube(imageName, modelImageName,
                 largestscale, fusedthreshold,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 specmode
                 );


  
  PagedImage<Float> modelimage("unittest_hummbee.model");

  float tol = 0.1;
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
  remove_all(current_path()/"unittest_hummbee.sumwt");
  remove_all(current_path()/"unittest_hummbee.model");
  remove_all(current_path()/"unittest_hummbee.mask");*/

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);

}

};
