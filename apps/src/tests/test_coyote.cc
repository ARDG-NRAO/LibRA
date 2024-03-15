#include <filesystem>
#include "Coyote/coyote.h"
#include "gtest/gtest.h"
#include <casacore/images/Images/PagedImage.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>

using namespace std;
using namespace std::filesystem;

namespace test{




TEST(CoyoteTest, UIFactory) {
    // The Factory Settings.
  int argc = 1;
  char* argv[] = {"./coyote"};

  // The Factory Settings.
  string MSNBuf="", cfCache="test", fieldStr="", spwStr="*",
    imageName,cmplxGridName="",phaseCenter, stokes="I",
    refFreqStr="3.0e9", telescopeName="EVLA", mType="diagonal",
    mode="dryrun";
  std::vector<std::string> cfList;
  
  float cellSize=0.025;//refFreq=3e09, freqBW=3e9;
  int NX=1400, nW=1, cfBufferSize=1, cfOversampling=20;
  bool WBAwp=true;
  bool restartUI=false;
  bool doPointing=false;
  bool normalize=false;
  bool conjBeams= true;
  bool psTerm = false;
  bool aTerm = true;
  float pa=-200.0, // Get PA from the MS
    dpa=360.0; // Don't rotate CFs for PA
  
  bool interactive = false;

  UI(restartUI, argc, argv, interactive, 
     MSNBuf,
     telescopeName,
     NX, cellSize, stokes, refFreqStr, nW,
     cfCache, WBAwp,
     psTerm, aTerm, mType, pa, dpa,
     fieldStr, spwStr, phaseCenter, conjBeams,
     cfBufferSize, cfOversampling,
     cfList,
     mode);

}

TEST(CoyoteTest, CoyoteDryRun) {
  //create directory CoyoteDryRun
  std::filesystem::create_directory("CoyoteDryRun");
  //copy over refim_point_wterm_vlad.ms from gold_standard to CoyoteDryRun
  std::filesystem::copy("gold_standard/refim_point_wterm_vlad.ms", "CoyoteDryRun/refim_point_wterm_vlad.ms", copy_options::recursive);
  //change directory to CoyoteDryRun
  std::filesystem::current_path("CoyoteDryRun");

  string MSNBuf = "refim_point_wterm_vlad.ms";
  string telescopeName = "EVLA";
  int NX = 2048;
  float cellSize = 10.0;
  string stokes = "I";
  string refFreqStr = "1.0e9";
  int nW = 16;
  string cfCacheName = "use_awp.cf";
  bool WBAwp = true;
  bool aTerm = true;
  bool psTerm = false;
  string mType = "diagonal";
  float pa = 0.0;
  float dpa = 360.0;
  string fieldStr = "";
  string spwStr = "*";
  string phaseCenter = "19:59:28.5 40.43.21.5 J2000";
  bool conjBeams = true;
  int cfBufferSize = 3072;
  int cfOversampling = 20;
  std::vector<std::string> cfList;
  string mode = "dryrun";

  Coyote(MSNBuf, telescopeName, NX, cellSize, stokes, refFreqStr, nW, cfCacheName,
         WBAwp, aTerm, psTerm, mType, pa, dpa, fieldStr, spwStr, phaseCenter,
         conjBeams, cfBufferSize, cfOversampling, cfList, mode);

  // Add assertions here to verify the behavior of the Coyote function in dryrun mode produces the required images.
  for (int j = 0; j < 16; j++) {
    for (int i = 0; i < 2; i++) {
      std::string filePath = "use_awp.cf/CFS_0_0_CF_0_" + std::to_string(j) + "_" + std::to_string(i) + ".im";
      ASSERT_TRUE(std::filesystem::exists(filePath)) << "File does not exist"; 
    }
  }

}

TEST(CoyoteTest, MiscInfo) {
  //open PagedImage and get MiscInfo
  // casacore::PagedImage<casacore::Complex> image("use_awp.cf/CFS_0_0_CF_0_0_0.im");
  // casacore::TableRecord miscInfo = image.miscInfo();
  // std::cout << "MiscInfo: " << miscInfo << std::endl;
  // Add assertions here to verify the behavior of the Coyote function in dryrun mode produces the required images.
  // ASSERT_TRUE(miscInfo.contains("Coyote"));
  // ASSERT_TRUE(miscInfo.contains("CFS_0_0_CF_0_0_0"));
  // ASSERT_TRUE(miscInfo.contains("EVLA"));
  // ASSERT_TRUE(miscInfo.contains("1.0e9"));
  // ASSERT_TRUE(miscInfo.contains("I"));
  // ASSERT_TRUE(miscInfo.contains("diagonal"));
  // ASSERT_TRUE(miscInfo.contains("19:59:28.5"));
}

TEST(CoyoteTest, CoyoteFillCF) {
  string MSNBuf = "refim_point_wterm_vlad.ms";
  string telescopeName = "EVLA";
  int NX = 2048;
  float cellSize = 10.0;
  string stokes = "I";
  string refFreqStr = "1.0e9";
  int nW = 16;
  string cfCacheName = "use_awp.cf";
  bool WBAwp = true;
  bool aTerm = true;
  bool psTerm = false;
  string mType = "diagonal";
  float pa = 0.0;
  float dpa = 360.0;
  string fieldStr = "";
  string spwStr = "*";
  string phaseCenter = "19:59:28.5 40.43.21.5 J2000";
  bool conjBeams = true;
  int cfBufferSize = 3072;
  int cfOversampling = 20;
  string mode = "fillcf";


  std::vector<std::string> cfList;
  for (int j = 0; j < 16; j++) {
    for (int i = 0; i < 2; i++) {
      std::string filePath = "CFS_0_0_CF_0_" + std::to_string(j) + "_" + std::to_string(i) + ".im";
      cfList.push_back(filePath);
    }
  }
  // Coyote(MSNBuf, telescopeName, NX, cellSize, stokes, refFreqStr, nW, cfCacheName,
        //  WBAwp, aTerm, psTerm, mType, pa, dpa, fieldStr, spwStr, phaseCenter,
        //  conjBeams, cfBufferSize, cfOversampling, cfList, mode);

  // Add assertions here to verify the behavior of the Coyote function in fillcf mode
}

};
