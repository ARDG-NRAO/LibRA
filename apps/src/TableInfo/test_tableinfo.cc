#include <filesystem>
#include <TableInfo/tableinfo.h>
#include <gtest/gtest.h>

using namespace std;
using namespace std::filesystem;


namespace test{
TEST(TableInfoTest, UIFactory) {
    // The Factory Settings.
  int argc = 3;
  char arg0[] = "./tableinfo";
  char arg1[] = "help=noprompt";
  char arg2[] = "table=test.ms";
  char* argv[] = {arg0, arg1, arg2};

  // The Factory Settings.
      bool restartUI = false;
      bool verbose = 0;
      string MSNBuf,OutBuf;
      MSNBuf="test.ms";
      OutBuf="";
      bool interactive = false;
      
      UI(restartUI,argc, argv, interactive, MSNBuf,OutBuf,verbose);
}

};
