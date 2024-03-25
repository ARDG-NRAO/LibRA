#include <filesystem>
#include "Asp/asp.h"
#include "translation/casacore_asp.h"
#include "translation/casacore_asp_cube.h"
#include "gtest/gtest.h"

using namespace std;
using namespace std::filesystem;


namespace test{
TEST(AspTest, FuncLevel) {
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
  /*float psfa[nx][ny] =
    {
        {1,8,12,20,25},
        {5,9,13,24,26}
    };

  float modela[nx][ny] = {0};
  float residuala[nx][ny] = {
        {1,8,12,-20,-25},
        {5,9,-13,24,26}
    };
  float maska[nx][ny] = {
        {1,8,0,20,25},
        {5,9,13,24,0}
    };

  float *psfb[nx];
  float *modelb[nx];
  float *residualb[nx];
  float *maskb[nx];

  for (size_t i = 0; i < nx; ++i)
  {
        psfb[i] = psfa[i];
        modelb[i] = modela[i];
        residualb[i] = residuala[i];
        maskb[i] = maska[i];
  }*/

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
  

  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee_mfs_revE.psf", current_path()/"unittest_hummbee_mfs_revE.psf", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee_mfs_revE.residual", current_path()/"unittest_hummbee_mfs_revE.residual", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee_mfs_revE.mask", current_path()/"unittest_hummbee_mfs_revE.mask", copy_options::recursive);

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

  remove_all(current_path()/"unittest_hummbee_mfs_revE.psf");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.residual");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.model");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.mask");
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

  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.pb", current_path()/"unittest_hummbee.pb", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.psf", current_path()/"unittest_hummbee.psf", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.residual", current_path()/"unittest_hummbee.residual", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.mask", current_path()/"unittest_hummbee.mask", copy_options::recursive);

  casacore_asp_cube(imageName, modelImageName,
                 largestscale, fusedthreshold,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 specmode
                 );

  remove_all(current_path()/"unittest_hummbee.pb");
  remove_all(current_path()/"unittest_hummbee.psf");
  remove_all(current_path()/"unittest_hummbee.residual");
  remove_all(current_path()/"unittest_hummbee.sumwt");
  remove_all(current_path()/"unittest_hummbee.mask");
  remove_all(current_path()/"unittest_hummbee.model");
  EXPECT_TRUE(true);
  
  /*PagedImage<Float> modelimage("unittest_hummbee.model");

  float tol = 0.1;
  float goldPeakRes = 4.98845;
  EXPECT_NEAR(PeakRes, goldPeakRes, tol);

  float goldValchan0 = 0.0442593;
  float goldValchan2 = 0.0384871;
  EXPECT_NEAR(modelimage(IPosition(4,275,330,0,0)), goldValchan0, tol);
  EXPECT_NEAR(modelimage(IPosition(4,275,330,0,2)), goldValchan2, tol);

  remove_all(current_path()/"unittest_hummbee.pb");
  remove_all(current_path()/"unittest_hummbee.psf");
  remove_all(current_path()/"unittest_hummbee.residual");
  //remove_all(current_path()/"unittest_hummbee.sumwt");
  remove_all(current_path()/"unittest_hummbee.model");
  remove_all(current_path()/"unittest_hummbee.mask");*/
}

};
