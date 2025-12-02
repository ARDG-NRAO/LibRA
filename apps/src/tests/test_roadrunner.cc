#define LIBRA_USE_HPG
#include <RoadRunner/LibHPG.h>
#include <RoadRunner/roadrunner.h>
#include <tests/test_utils.h>

using namespace std;
using namespace std::filesystem;

namespace test{
  const path goldDir = current_path() / "gold_standard";


TEST(RoadrunnerTest, InitializeTest) {
  // Create a LibHPG object
  //
  // This is an RAII class. The Resources (HPG/Kokkos devices) will be
  // Acquired and Initialize upon creation.
  bool ret=false;
  {
    LibHPG lib_hpg;
  }
  // Test that LibHPG::global_initialized_kokkos_backends is available
  // outside the LibHPG scope
  cerr << "RoadrunnerTest::InitializeTest: Kokkos backends from LibHPG::global_initialized_kokkos_backends: ";
  for (auto b : LibHPG::global_initialized_kokkos_backends)
    cerr << KokkosInterop::kokkos_backend_to_str(b) << " ";
  cerr << endl;
  // Call the initialize() function -- THIS IS IN CORRECT USE OF AN RAII CLASS
  // bool ret = lib_hpg.initialize();

  // If the control got here, LibHPG was successfully initialized.
  ret=true;

  // Check that the return value is true
  EXPECT_TRUE(ret);
}

TEST(RoadrunnerTest, AppLevelSNRPSF_performance) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create dir AppLevelSNRPSF
  std::filesystem::create_directory(testDir);

  //copy over CYGTST.corespiral.ms from gold_standard to AppLevelSNRPSF
  std::filesystem::copy(goldDir/"CYGTST.corespiral.ms", testDir/"CYGTST.corespiral.ms", copy_options::recursive);
  //copy over 4k_nosquint.cfc from gold_standard to AppLevelSNRPSF
  std::filesystem::copy(goldDir/"4k_nosquint.cfc", testDir/"4k_nosquint.cfc", copy_options::recursive);
  //Step into AppLevelSNRPSF
  std::filesystem::current_path(testDir);

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


  RRReturnType record = Roadrunner(MSNBuf,imageName, modelImageName,dataColumnName,
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

  // flag if runtime is abnormal
  auto gold_runtime = 4.3; //gpuhost003
  try {
    auto runtime = record.at(CUMULATIVE_GRIDDING_ENGINE_TIME);
    // to catch if synthesis is NOT compiled with `-O2` which can cause 2x slowness
    EXPECT_LT(runtime, 2.0 * gold_runtime) << "Runtime is too long. Check if casacpp is compiled with -O2"; 
  }
  catch (const std::out_of_range&) {
        std::cout << "Key CUMULATIVE_GRIDDING_ENGINE_TIME not found" << std::endl;
  }

   if (!IsGpuHost003()) {
     std::cout << "[  INFO ] Skipping vis processing rate check on non-gpuhost003 machine.\n" << std::endl;
   } 
   else {
    // flag if Vis Processing rate is reduced by > 10%
    auto gold_rate = 240000; //V100 in gpuhost003 for this setup only
    try {
      auto rate = record.at(NVIS) / record.at(IMAGING_TIME);  // Vis/sec
      EXPECT_GT(rate, 0.9 * gold_rate) << "Vis Processing Rate is reduced by more than 10%. Check if any code change slows it down";
    }
    catch (const std::out_of_range&) {
        std::cout << "Key IMAGING_TIME or NVIS not found" << std::endl;
    }
   }

	
  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}

TEST(RoadrunnerTest, AppLevelWeight) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create dir AppLevelWeight
  std::filesystem::create_directory(testDir);
  //copy over CYGTST.corespiral.ms from gold_standard to AppLevelWeight
  std::filesystem::copy(goldDir/"CYGTST.corespiral.ms", testDir/"CYGTST.corespiral.ms", copy_options::recursive);
  //copy over 4k_nosquint.cfc from gold_standard to AppLevelWeight
  std::filesystem::copy(goldDir/"4k_nosquint.cfc", testDir/"4k_nosquint.cfc", copy_options::recursive);
  //Step into AppLevelWeight
  std::filesystem::current_path(testDir);

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

  string ftmName="awphpg";
  string fieldStr="", spwStr="*", uvDistStr="", dataColumnName="data";
  string modelImageName="",stokes="I";
  string refFreqStr="3.0e9";
  string rmode="none";

  bool WBAwp=true;
  bool doPointing=false;
  bool normalize=false;
  bool doPBCorr= true;


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
  float goldValLoc0 = 1255553.875;
  float goldValLoc1 = 580468.3125;
  EXPECT_NEAR(weight(IPosition(4,2000,2053,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(weight(IPosition(4,1379,1969,0,0)), goldValLoc1, tol);


  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());
  remove_all(testDir);

}



TEST(RoadrunnerTest, Interface_rmode_plus2) {
  // Test if the rmode is set correctly in Roadrunner()
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create dir AppLevelSNRPSF
  std::filesystem::create_directory(testDir);

  //copy over CYGTST.corespiral.ms from gold_standard to Interface_rmode
  std::filesystem::copy(goldDir/"CYGTST.corespiral.ms", testDir/"CYGTST.corespiral.ms", copy_options::recursive);
  //copy over 4k_nosquint.cfc from gold_standard to Interface_rmode
  std::filesystem::copy(goldDir/"4k_nosquint.cfc", testDir/"4k_nosquint.cfc", copy_options::recursive);
  //Step into test dir
  std::filesystem::current_path(testDir);


  // weighting=briggs robust=2 
	string MSNBuf="CYGTST.corespiral.ms";
   string cfCache="4k_nosquint.cfc";
   string imageName="BriggsRobust+2.weight";
   string cmplxGridName="";
   string phaseCenter="J2000 19h57m44.44s  040d35m46.3s";
   string weighting="briggs";
   string sowImageExt="sumwt";
   string imagingMode="weight";

  float cellSize=0.025;
  float robust=2;
  int NX=4000, nW=1;

  bool conjBeams=false;
  float pbLimit=0.01;
  bool doSPWDataIter=true;
  vector<float> posigdev = {0.0,0.0};

  string ftmName="awphpg";
  string fieldStr="", spwStr="*", uvDistStr="", dataColumnName="data";
  string modelImageName="",stokes="I";
  string refFreqStr="3.0e9";
  string rmode="";

  bool WBAwp=true;
  bool doPointing=false;
  bool normalize=false;
  bool doPBCorr= true;

  Roadrunner(MSNBuf,imageName, modelImageName,dataColumnName,
                 sowImageExt, cmplxGridName, NX, nW, cellSize,
                 stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
                 ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
                 doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
                 doSPWDataIter);

  // Check that the images are generated
  path p1("BriggsRobust+2.weight");
  path p2("BriggsRobust+2.sumwt");
  bool ans = exists(p1) && exists(p2);
   
  EXPECT_TRUE( ans );

  PagedImage<casacore::Float> Nimage("BriggsRobust+2.weight");
  float tol = 0.1;
  float goldValLoc0 = 1257491.37;

  EXPECT_NEAR(Nimage(IPosition(4,1994,1973,0,0)), goldValLoc0, tol);
  
  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}

TEST(RoadrunnerTest, Interface_rmode_minus2) {
  // Small delay to ensure proper cleanup
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // Test if the rmode is set correctly in Roadrunner()
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create dir AppLevelSNRPSF
  std::filesystem::create_directory(testDir);

  //copy over CYGTST.corespiral.ms from gold_standard to Interface_rmode
  std::filesystem::copy(goldDir/"CYGTST.corespiral.ms", testDir/"CYGTST.corespiral.ms", copy_options::recursive);
  //copy over 4k_nosquint.cfc from gold_standard to Interface_rmode
  std::filesystem::copy(goldDir/"4k_nosquint.cfc", testDir/"4k_nosquint.cfc", copy_options::recursive);
  //Step into test dir
  std::filesystem::current_path(testDir);


  // weighting=briggs robust=-2 
  string MSNBuf="CYGTST.corespiral.ms";
   string cfCache="4k_nosquint.cfc";
   string imageName="BriggsRobust-2.weight";
   string cmplxGridName="";
   string phaseCenter="J2000 19h57m44.44s  040d35m46.3s";
   string weighting="briggs";
   string sowImageExt="sumwt";
   string imagingMode="weight";

  float cellSize=0.025;
  float robust=-2;
  int NX=4000, nW=1;

  bool conjBeams=false;
  float pbLimit=0.01;
  bool doSPWDataIter=true;
  vector<float> posigdev = {0.0,0.0};

  string ftmName="awphpg";
  string fieldStr="", spwStr="*", uvDistStr="", dataColumnName="data";
  string modelImageName="",stokes="I";
  string refFreqStr="3.0e9";
  string rmode="";

  bool WBAwp=true;
  bool doPointing=false;
  bool normalize=false;
  bool doPBCorr= true;

  Roadrunner(MSNBuf,imageName, modelImageName,dataColumnName,
                 sowImageExt, cmplxGridName, NX, nW, cellSize,
                 stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
                 ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
                 doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
                 doSPWDataIter);

  // Check that the images are generated
  path p3("BriggsRobust-2.weight");
  path p4("BriggsRobust-2.sumwt");
  bool ans = exists(p3) && exists(p4);
   
  EXPECT_TRUE( ans );

  PagedImage<casacore::Float> Uimage("BriggsRobust-2.weight");
  float tol = 0.1;
  float goldValLoc1 = 46.539;
  EXPECT_NEAR(Uimage(IPosition(4,1994,1973,0,0)), goldValLoc1, tol);

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}

TEST(RoadrunnerTest, UIFactory) {
    // The Factory Settings.
  int argc = 1;
  const char* argv[] = {"./roadrunner"};


  string MSNBuf,ftmName="awphpg",
    cfCache, fieldStr="", spwStr="*", uvDistStr="", dataColumnName="data",
    imageName="test", modelImageName,cmplxGridName="", stokes="I",
    weighting="natural", sowImageExt,
    imagingMode="residual",rmode="norm";

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
  UI(restartUI, argc, (char **)argv, interactive,
     MSNBuf,imageName, modelImageName, dataColumnName,
     sowImageExt, cmplxGridName, NX, nW, cellSize,
     stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
     ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
     doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
     doSPWDataIter);
}


TEST(RoadrunnerTest, UIThrow) {
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

  try {
        UI(restartUI, argc, argv, interactive,
              MSNBuf,imageName, modelImageName, dataColumnName,
            sowImageExt, cmplxGridName, NX, nW, cellSize,
            stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
            ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
            doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
            doSPWDataIter);
        FAIL() << "Expected an exception to be thrown";
    }
    catch (const std::exception& e) {
        // This will catch any std::exception derived class
        std::cout << "Caught exception: " << typeid(e).name() << " - " << e.what() << std::endl;
        SUCCEED();
    }
    catch (...) {
        std::cout << "Caught unknown exception type" << std::endl;
        SUCCEED();
    }

}


};
