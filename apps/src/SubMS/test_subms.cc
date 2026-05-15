#include <filesystem>
#include <SubMS/subms.h>
#include <gtest/gtest.h>

using namespace std;
using namespace std::filesystem;


namespace test{

TEST(SubMSTest, UIFactory) {
    // The Factory Settings.
  int argc = 4;
  char arg0[] = "./subms";
  char arg1[] = "help=noprompt";
  char arg2[] = "ms=testin.ms";
  char arg3[] = "outms=testout.ms";
  char* argv[] = {arg0, arg1, arg2, arg3};

  // The Factory Settings.
   string MSNBuf,OutMSBuf,WhichColStr="data",fieldStr="*",timeStr,spwStr="*",
    baselineStr,uvdistStr, taqlStr,scanStr,arrayStr,corrStr;

   MSNBuf="testin.ms";
   OutMSBuf="testout.ms";
   WhichColStr=fieldStr=timeStr=spwStr=baselineStr=uvdistStr=
        taqlStr=scanStr=corrStr=arrayStr="";
   WhichColStr="data"; fieldStr="*"; spwStr="*";
   int deepCopy=0;
   float integ=-1;
   Bool restartUI = false;
   Bool interactive = false;

   UI(restartUI,argc, argv, interactive, MSNBuf,OutMSBuf, WhichColStr, deepCopy,
         fieldStr,timeStr,spwStr,baselineStr,scanStr,arrayStr,uvdistStr,taqlStr,integ);
}

};
