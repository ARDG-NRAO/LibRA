#include <filesystem>
#include <MSSplit/mssplit.h>
#include <gtest/gtest.h>

using namespace std;
using namespace std::filesystem;


namespace test{
TEST(MSSplitTest, UIFactory) {
  // The Factory Settings.
  int argc = 3;
  char arg0[] = "./mssplit";
  char arg1[] = "help=noprompt";
  char arg2[] = "ms=test.ms";
  char* argv[] = {arg0, arg1, arg2};

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

    UI(restartUI,argc, argv, interactive, MSNBuf,OutMSBuf, deepCopy,
        fieldStr,timeStr,spwStr,baselineStr,scanStr,arrayStr,
        uvdistStr,taqlStr,polnStr,stateObsModeStr,observationStr);

}

};
