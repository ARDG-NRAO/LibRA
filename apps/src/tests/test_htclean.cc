#include <filesystem>
#include "gtest/gtest.h"

#include <cstdlib>
#include <cstdio>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include "Hummbee/hummbee.h"

using namespace std;
using namespace std::filesystem;
using namespace casacore;

namespace test{
  const path goldDir = current_path() / "gold_standard";

// Function to run the shell script
std::string run_shell_script(const std::string& imagename) {
    // Ensure that the script is executable
    system("chmod +x ./libra_htclean.sh");
    system("chmod +x ./runapp.sh");

    std::string command = "./libra_htclean.sh -i " + imagename + " -n 3 -p libra_htclean.def -L ../../../../install/bin/ -l LOGS";
    char buffer[128];
    std::string result = "";
    FILE* fp = popen(command.c_str(), "r");
    if (fp == nullptr) {
        return "Error running script";
    }
    while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
        result += buffer;
    }
    fclose(fp);
    return result;
}



TEST(HtcleanTest, Loops) {
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
  // copy the input .def
  std::filesystem::copy(goldDir/"libra_htclean.def", testDir/"libra_htclean.def", copy_options::recursive);  
  // copy htclean scripts
  std::filesystem::copy(goldDir/"../../../../frameworks/htclean/libra_htclean.sh", testDir/"libra_htclean.sh", copy_options::recursive);
  std::filesystem::copy(goldDir/"../../../../frameworks/htclean/runapp.sh", testDir/"runapp.sh", copy_options::recursive);
  //Step into test dir
  std::filesystem::current_path(testDir);

  

  string imagename = "cygA"; 
  // Run the script
  string output = run_shell_script(imagename);

  // Check that the images are generated
  path p1("cygA.psf");
  path p2("cygA.model");
  path p3("cygA.image");
  bool ans = exists(p1) && exists(p2) && exists(p3);
   
  EXPECT_TRUE( ans );

  PagedImage<casacore::Float> psfimage("cygA.model");
  float tol = 0.1;
  float goldValLoc0 = 0.095916;
  float goldValLoc1 = 0.0876324;

  EXPECT_NEAR(psfimage(IPosition(4,2937,2352,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(psfimage(IPosition(4,1076,1785,0,0)), goldValLoc1, tol);



  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}




};
