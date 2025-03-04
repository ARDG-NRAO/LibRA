#include <filesystem>
#include <MSSplit/mssplit.h>
#include <gtest/gtest.h>

using namespace std;
using namespace std::filesystem;


namespace test{
TEST(MSSplitTest, UIFactory) {
  // The Factory Settings.
  int argc = 1;
  const char* argv[] = {"./mssplit"};

  // The Factory Settings.
  string MSNBuf,OutMSBuf,fieldStr,timeStr,spwStr,baselineStr,
    uvdistStr,taqlStr,scanStr,arrayStr, polnStr,stateObsModeStr,
    observationStr;
    MSNBuf="test.ms";
    OutMSBuf=fieldStr=timeStr=spwStr=baselineStr=
          uvdistStr=taqlStr=scanStr=arrayStr=polnStr=stateObsModeStr=observationStr="";
    Bool deepCopy=0;
    
    fieldStr=spwStr="";
	  Bool restartUI = false;
    Bool interactive = false;

    UI(restartUI,argc, (char **)argv, interactive, MSNBuf,OutMSBuf, deepCopy,
        fieldStr,timeStr,spwStr,baselineStr,scanStr,arrayStr,
        uvdistStr,taqlStr,polnStr,stateObsModeStr,observationStr);

}

};
