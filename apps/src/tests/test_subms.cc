#include <filesystem>
#include <SubMS/subms.h>
#include <gtest/gtest.h>

using namespace std;
using namespace std::filesystem;


namespace test{

TEST(SubMSTest, UIFactory) {
    // The Factory Settings.
  int argc = 1;
  const char* argv[] = {"./subms"};

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

   UI(restartUI,argc, (char **)argv, interactive, MSNBuf,OutMSBuf, WhichColStr, deepCopy,
         fieldStr,timeStr,spwStr,baselineStr,scanStr,arrayStr,uvdistStr,taqlStr,integ);
}

};
