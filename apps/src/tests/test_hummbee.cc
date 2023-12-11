#include <filesystem>
#include "Hummbee/hummbee_func.h"
#include "gtest/gtest.h"
#include <casacore/images/Images/PagedImage.h>

using namespace std;
using namespace std::filesystem;
using namespace casacore;


namespace test{
TEST(HummbeeTest, AppLevelCubeAsp) {
  // Check that the return value is true
  string imageName = "unittest_hummbee";
  string modelImageName = "unittest_hummbee.image";
  string deconvolver="asp", specmode="cube";
  vector<float> scales;
  float largestscale = -1;
  float fusedthreshold = 0;
  int nterms=2;
  float gain=0.1;
  float threshold=1e-4;
  float nsigma=1.5;
  int cycleniter=10;
  float cyclefactor=1.0;
  vector<string> mask;
  mask.resize(1);
  mask[0] ="circle[[256pix,290pix],140pix]";

  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.pb", current_path()/"unittest_hummbee.pb", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.psf", current_path()/"unittest_hummbee.psf", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.residual", current_path()/"unittest_hummbee.residual", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee.sumwt", current_path()/"unittest_hummbee.sumwt", copy_options::recursive);

  float PeakRes = Hummbee(imageName, modelImageName,
                 deconvolver,
                 scales,
                 largestscale, fusedthreshold,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 mask, specmode
                 );
	
  PagedImage<Float> myimage ("image.name");

  remove_all(current_path()/"unittest_hummbee.pb");
  remove_all(current_path()/"unittest_hummbee.psf");
  remove_all(current_path()/"unittest_hummbee.residual");
  remove_all(current_path()/"unittest_hummbee.sumwt");
  remove_all(current_path()/"unittest_hummbee.model");
  remove_all(current_path()/"unittest_hummbee.mask");

  float goldPeakRes = 4.98845;
  EXPECT_TRUE((PeakRes >= goldPeakRes*0.9) && (PeakRes <= goldPeakRes*1.1));

}

TEST(HummbeeTest,  AppLevelMfsAsp) {
   // Check that the return value is true
  string imageName = "unittest_hummbee_mfs_revE";
  string modelImageName = "unittest_hummbee_mfs_revE.image";
  string deconvolver="asp", specmode="mfs";
  vector<float> scales;
  float largestscale = -1;
  float fusedthreshold = 0.007;
  int nterms=2;
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

  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee_mfs_revE.psf", current_path()/"unittest_hummbee_mfs_revE.psf", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee_mfs_revE.residual", current_path()/"unittest_hummbee_mfs_revE.residual", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee_mfs_revE.sumwt", current_path()/"unittest_hummbee_mfs_revE.sumwt", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/unittest_hummbee_mfs_revE.weight", current_path()/"unittest_hummbee_mfs_revE.weight", copy_options::recursive);

  float PeakRes = Hummbee(imageName, modelImageName,
                 deconvolver,
                 scales,
                 largestscale, fusedthreshold,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 mask, specmode
                 );


  remove_all(current_path()/"unittest_hummbee_mfs_revE.psf");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.residual");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.sumwt");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.model");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.mask");
  remove_all(current_path()/"unittest_hummbee_mfs_revE.weight");

  float goldPeakRes = 26.0554;
  EXPECT_TRUE((PeakRes >= goldPeakRes*0.9) && (PeakRes <= goldPeakRes*1.1));
}

};
