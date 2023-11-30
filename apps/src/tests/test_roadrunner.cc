#include <filesystem>
#include "RoadRunner/Roadrunner_func.h"
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

  string ftmName="awproject";
  string fieldStr="", spwStr="*", uvDistStr="", dataColumnName="data";
  string modelImageName="",stokes="I";
  string refFreqStr="3.0e9";
  string rmode="none";

  bool WBAwp=true;
  bool doPointing=false;
  bool normalize=false;
  bool doPBCorr= true;

  copy(current_path()/"../../../src/tests/gold_standard/CYGTST.corespiral.ms", current_path()/"CYGTST.corespiral.ms", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/4k_nosquint.cfc", current_path()/"4k_nosquint.cfc", copy_options::recursive);

  Roadrunner(MSNBuf,imageName, modelImageName,dataColumnName,
                 sowImageExt, cmplxGridName, NX, nW, cellSize,
                 stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
                 ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
                 doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
                 doSPWDataIter);

  // Check that the .psf is generated
  path p1("htclean_gpu_newpsf.psf");
  path p2("htclean_gpu_newpsf.sumwt");
  bool ans = exists(p1) && exists(p2);

   remove_all(current_path()/"CYGTST.corespiral.ms");
   remove_all(current_path()/"4k_nosquint.cfc");
   remove_all(current_path()/"htclean_gpu_newpsf.psf");
   remove_all(current_path()/"htclean_gpu_newpsf.sumwt");
   
   EXPECT_TRUE( ans );
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

  copy(current_path()/"../../../src/tests/gold_standard/CYGTST.corespiral.ms", current_path()/"CYGTST.corespiral.ms", copy_options::recursive);
  copy(current_path()/"../../../src/tests/gold_standard/4k_nosquint.cfc", current_path()/"4k_nosquint.cfc", copy_options::recursive);

  Roadrunner(MSNBuf,imageName, modelImageName,dataColumnName,
                 sowImageExt, cmplxGridName, NX, nW, cellSize,
                 stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
                 ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
                 doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
                 doSPWDataIter);

  // Check that the .psf is generated
  path p1("htclean_gpu_newpsf.weight");
  bool ans = exists(p1);

   remove_all(current_path()/"CYGTST.corespiral.ms");
   remove_all(current_path()/"4k_nosquint.cfc");
   remove_all(current_path()/"htclean_gpu_newpsf.weight");

   EXPECT_TRUE( ans );
}

};
