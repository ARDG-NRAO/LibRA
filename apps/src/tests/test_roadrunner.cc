#include <filesystem>
#include "RoadRunner/roadrunner.h"
#include "gtest/gtest.h"

using namespace std;
using namespace std::filesystem;

namespace test{
TEST(RoadrunnerTest, InitializeTest) {
  // Create a LibHPG object
  LibHPG lib_hpg;

  // Call the initialize() function
  bool ret = lib_hpg.initialize();

  // Check that the return value is true
  EXPECT_TRUE(ret);
}

TEST(RoadrunnerTest, AppLevelSNRPSF) {
   string MSNBuf="CYGTST.corespiral.ms";
   string cfCache="4k_nosquint.cfc";
   string imageName="htclean_gpu_newpsf.psf";
   string cmplxGridName="htclean_gpu_newpsf_gridv";
   string phaseCenter="J2000 19h57m44.44s  040d35m46.3s";
   string weighting="natural";
   string sowImageExt="sumwt";
   string imagingMode="snrpsf";

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

  copy(current_path()/"../../../../../apps/src/tests/gold_standard/CYGTST.corespiral.ms", current_path()/"CYGTST.corespiral.ms", copy_options::recursive);
  copy(current_path()/"../../../../../apps/src/tests/gold_standard/4k_nosquint.cfc", current_path()/"4k_nosquint.cfc", copy_options::recursive);

  Roadrunner(MSNBuf,imageName, modelImageName,dataColumnName,
                 sowImageExt, cmplxGridName, NX, nW, cellSize,
                 stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
                 ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
                 doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
                 doSPWDataIter);

  // Check that the .psf is generated
  path p1("htclean_gpu_newpsf.psf");
  path p2("htclean_gpu_newpsf.sumwt");
  path p3("htclean_gpu_newpsf_gridv.vis");
  bool ans = exists(p1) && exists(p2) && exists(p3);
   
  EXPECT_TRUE( ans );

  PagedImage<Float> psfimage("htclean_gpu_newpsf.psf");
  float tol = 0.1;
  float goldValLoc0 = 44163072;
  float goldValLoc1 = 6067482.5;

  EXPECT_NEAR(psfimage(IPosition(4,2000,2000,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(psfimage(IPosition(4,1885,1885,0,0)), goldValLoc1, tol);

  remove_all(current_path()/"CYGTST.corespiral.ms");
  remove_all(current_path()/"4k_nosquint.cfc");
  remove_all(current_path()/"htclean_gpu_newpsf.psf");
  remove_all(current_path()/"htclean_gpu_newpsf_gridv.vis/");
  remove_all(current_path()/"htclean_gpu_newpsf.sumwt");
}

TEST(RoadrunnerTest, AppLevelWeight) {
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

  string ftmName="awproject";
  string fieldStr="", spwStr="*", uvDistStr="", dataColumnName="data";
  string modelImageName="",stokes="I";
  string refFreqStr="3.0e9";
  string rmode="none";

  bool WBAwp=true;
  bool doPointing=false;
  bool normalize=false;
  bool doPBCorr= true;

  copy(current_path()/"../../../../../apps/src/tests/gold_standard/CYGTST.corespiral.ms", current_path()/"CYGTST.corespiral.ms", copy_options::recursive);
  copy(current_path()/"../../../../../apps/src/tests/gold_standard/4k_nosquint.cfc", current_path()/"4k_nosquint.cfc", copy_options::recursive);

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
  float goldValLoc0 = -277710.1875;
  float goldValLoc1 = 470668.9375;
  EXPECT_NEAR(weight(IPosition(4,2000,2053,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(weight(IPosition(4,1379,1969,0,0)), goldValLoc1, tol);

   remove_all(current_path()/"CYGTST.corespiral.ms");
   remove_all(current_path()/"4k_nosquint.cfc");
   remove_all(current_path()/"htclean_gpu_newpsf.weight");
   remove_all(current_path()/"htclean_gpu_newpsf.sumwt");

}

TEST(RoadrunnerTest, UIFactory) {
    // The Factory Settings.
  int argc = 1;
  char* argv[] = {"./roadrunner"};


  string MSNBuf,ftmName="awphpg",
    cfCache, fieldStr="", spwStr="*", uvDistStr="", dataColumnName="data",
    imageName, modelImageName,cmplxGridName="", stokes="I",
    weighting="natural", sowImageExt,
    imagingMode="residual",rmode="none";

  string phaseCenter = "J2000 19h57m44.44s  040d35m46.3s";

  string refFreqStr="3.0e9"; 
  float cellSize=0.025;
  float robust=0.0;
  int NX=4000, nW=1;
  bool WBAwp=true;
  bool restartUI=false;
  bool doPointing=false;
  bool normalize=false;
  bool doPBCorr= true;
  bool conjBeams= true;
  float pbLimit=1e-3;
  bool doSPWDataIter=false;
  vector<float> posigdev = {300.0,300.0};
  bool interactive = false;
  cfCache="test";
  UI(restartUI, argc, argv, interactive,
     MSNBuf,imageName, modelImageName, dataColumnName,
     sowImageExt, cmplxGridName, NX, nW, cellSize,
     stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
     ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
     doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
     doSPWDataIter);
}


/*TEST(RoadrunnerTest, UIThrow) {
  int argc = 1;
  char* argv[] = {"./roadrunner"};

  string MSNBuf,ftmName="awphpg",
    cfCache, fieldStr="", spwStr="*", uvDistStr="", dataColumnName="data",
    imageName, modelImageName,cmplxGridName="",phaseCenter, stokes="I",
    refFreqStr="3.0e9", weighting="natural", sowImageExt,
    imagingMode="residual",rmode="none";

  float cellSize=0;
  float robust=0.0;
  int NX=0, nW=1;
  bool WBAwp=true;
  bool restartUI=false;
  bool doPointing=false;
  bool normalize=false;
  bool doPBCorr= true;
  bool conjBeams= true;
  float pbLimit=1e-3;
  bool doSPWDataIter=false;
  vector<float> posigdev = {300.0,300.0};
  bool interactive = false;

  
           UI(restartUI, argc, argv, interactive,
              MSNBuf,imageName, modelImageName, dataColumnName,
            sowImageExt, cmplxGridName, NX, nW, cellSize,
            stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
            ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
            doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
            doSPWDataIter);

  if (HasFatalFailure()) return;

}*/


};
