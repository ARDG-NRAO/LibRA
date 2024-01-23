#include <filesystem>
#include "Coyote/coyote.h"
#include "gtest/gtest.h"

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
    imageNamePrefix="",mode="dryrun";
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
     cfCache, imageNamePrefix, WBAwp,
     psTerm, aTerm, mType, pa, dpa,
     fieldStr, spwStr, phaseCenter, conjBeams,
     cfBufferSize, cfOversampling,
     cfList,
     mode);

}





};
