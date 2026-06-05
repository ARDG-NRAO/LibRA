// # Copyright (C) 2021
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$
#include <filesystem>
#include <Chip/chip.h>
#include <gtest/gtest.h>

using namespace std;
using namespace std::filesystem;
using namespace Chip;

namespace test{
  const path goldDir = current_path() / "gold_standard";

TEST(ChipTest, AppLevelPSF) {
  // Get the test name
  string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();

  // Create a unique directory for this test case
  path testDir = current_path() / testName;

  // create test dir 
  std::filesystem::create_directory(testDir);

  //copy over from gold_standard to test dir
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad.psf", testDir/"refim_point_wterm_vlad.n1.psf", copy_options::recursive);
  std::filesystem::copy(goldDir/"refim_point_wterm_vlad.psf", testDir/"refim_point_wterm_vlad.n2.psf", copy_options::recursive);

  //Step into test dir 
  std::filesystem::current_path(testDir);
  
  std::vector<std::string> imageName={"refim_point_wterm_vlad.n1.psf","refim_point_wterm_vlad.n2.psf"};
  string outputimage="refim_point_wterm_vlad.psf";
  bool overwrite=false;
  bool resetoutput=false;
  string stats="none";

  chip(imageName, outputimage, overwrite, resetoutput, stats);

  // Check that the ouput image is generated
  path p1("refim_point_wterm_vlad.psf");
  bool ans = exists(p1) ;
   
  EXPECT_TRUE( ans );

  float tol = 0.05;
  float goldValLoc0 = 2 * 30185.982421875;
  float goldValLoc1 = 2 * -213.523565259375;

  PagedImage<Float> psfimage("refim_point_wterm_vlad.psf");
  EXPECT_NEAR(psfimage(IPosition(4,1024,1024,0,0)), goldValLoc0, tol);
  EXPECT_NEAR(psfimage(IPosition(4,1050,1050,0,0)), goldValLoc1, tol);

  //move to parent directory
  std::filesystem::current_path(testDir.parent_path());

  remove_all(testDir);
}

TEST(ChipTest, UIFactory) {
    // The Factory Settings.
  int argc = 4;
  char arg0[] = "./chip";
  char arg1[] = "help=noprompt";
  char arg2[] = "imagename=test";
  char arg3[] = "outputimage=testout";
  char* argv[] = {arg0, arg1, arg2, arg3};

  std::vector<std::string> imageName={"notEmpty"};
  string outputImage="notEmpty", stats="none";
  bool overWrite=false, resetOutputImage=false;
  bool restartUI=false, interactive=false;

  
  UI(restartUI, argc, argv, interactive,
    imageName, outputImage, overWrite,
    resetOutputImage, stats);

};

};
