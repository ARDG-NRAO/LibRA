//# test_coyote.cc: Regression test in GTest framework for the Coyote()
//# Copyright (C) 2024
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

//
//================================================================================
//
#include <filesystem>
#include "Coyote/coyote.h"
#include "gtest/gtest.h"
#include <casacore/images/Images/PagedImage.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
using namespace std::filesystem;

using namespace std;
//using namespace fs;

namespace test{

  // Global absolute location of the test dir
  std::string testDir=string(fs::current_path())+"/CoyoteDryRun";
  //
  //-----------------------------------------------------------------------------------------
  //
    string msName = "refim_point_wterm_vlad.ms";
    string sourceTestMS="gold_standard/"+msName;
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
  //
  //-----------------------------------------------------------------------------------------
  //
  void cleanUp(const std::string& base_path)
  {
    // for (auto& path : std::filesystem::directory_iterator(base_path))
    //   {
    // 	std::filesystem::remove_all(path);
    //   }
    fs::remove_all(base_path);
  }
  //
  //-----------------------------------------------------------------------------------------
  //

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
    std::string targetTestMS=testDir+"/"+msName;

    cleanUp(test::testDir);

    //create directory CoyoteDryRun
    fs::create_directory(test::testDir);
    //copy over refim_point_wterm_vlad.ms from gold_standard to CoyoteDryRun
    std::filesystem::copy(sourceTestMS, targetTestMS , copy_options::recursive);
    //change directory to CoyoteDryRun
    fs::current_path(test::testDir);


    Coyote(msName, telescopeName, NX, cellSize, stokes, refFreqStr, nW, cfCacheName,
           WBAwp, aTerm, psTerm, mType, pa, dpa, fieldStr, spwStr, phaseCenter,
           conjBeams, cfBufferSize, cfOversampling, cfList, mode);

    // Add assertions here to verify the behavior of the Coyote function in dryrun mode produces the required images.
    for (int j = 0; j < 16; j++) {
      for (int i = 0; i < 2; i++) {
        std::string filePath = "use_awp.cf/CFS_0_0_CF_0_" + std::to_string(j) + "_" + std::to_string(i) + ".im";
        ASSERT_TRUE(fs::exists(filePath)) << "File does not exist"; 
      }
    }

    //cleanUp(test::testDir);
    //    fs::remove_all(test::testDir);

  }

  TEST(CoyoteTest, MiscInfo) {
    //change directory to CoyoteDryRun
    fs::current_path(test::testDir);

    // Get MiscInfo
    string dryrunMiscInfo = "use_awp.cf/CFS_0_0_CF_0_0_0.im/miscInfo.rec";

    // Below is the expected miscInfo
    //
    // Xsupport: Int 3072
    // Ysupport: Int 3072
    // Sampling: Float 20
    // ParallacticAngle: Double 0
    // MuellerElement: Int 0
    // WValue: Double 0
    // WIncr: Double 0.0436332
    // Name: String "CFS_0_0_CF_0_0_0.im"
    // ConjFreq: Double 1e+09
    // ConjPoln: Int 8
    // TelescopeName: String "EVLA"
    // BandName: String "LBand"
    // Diameter: Float 25
    // OpCode: Bool 0
    // IsFilled: Bool 0
    // aTermOn: Bool 1
    // psTermOn: Bool 0
    // wTermOn: Bool 1
    // conjBeams: Bool 1
    
    // Add assertions here to verify the behavior of the Coyote function in dryrun mode produces the required misc info.
    double WIncr=0.043633231299858244;
    bool isFilled=false;
    
    auto verifyMiscInfo = [&WIncr,&isFilled](const string& name)
    {
      casacore::Record miscInfo = SynthesisUtils::readRecord(casacore::String(name));
      double  dVal;
      int iVal;
      float fVal;
      bool bVal;
      casacore::String sVal;
      miscInfo.get("Xsupport",iVal);         ASSERT_TRUE(iVal == 3072);
      miscInfo.get("Ysupport",iVal);         ASSERT_TRUE(iVal == 3072);
      miscInfo.get("Sampling",fVal);         ASSERT_TRUE(fVal == 20);
      miscInfo.get("ParallacticAngle",dVal); EXPECT_DOUBLE_EQ(dVal, 0.0);
      miscInfo.get("MuellerElement",iVal);   ASSERT_TRUE(iVal == 0);
      miscInfo.get("WIncr",dVal);            EXPECT_DOUBLE_EQ(dVal, WIncr);
      miscInfo.get("Name",sVal);             ASSERT_TRUE(sVal == "CFS_0_0_CF_0_0_0.im");
      miscInfo.get("ConjFreq",dVal);         EXPECT_DOUBLE_EQ(dVal,1e9);
      miscInfo.get("ConjPoln",iVal);         ASSERT_TRUE(iVal == 8);
      miscInfo.get("TelescopeName",sVal);    ASSERT_TRUE(sVal == "EVLA");
      miscInfo.get("BandName",sVal);         ASSERT_TRUE(sVal == "LBand");
      miscInfo.get("Diameter", fVal);        ASSERT_TRUE(fVal == 25.0);
      miscInfo.get("OpCode",bVal);           ASSERT_FALSE(bVal);
      miscInfo.get("IsFilled",bVal);         ASSERT_TRUE(bVal == isFilled);
      miscInfo.get("aTermOn",bVal);          ASSERT_TRUE(bVal);
      miscInfo.get("psTermOn",bVal);         ASSERT_FALSE(bVal);
      miscInfo.get("wTermOn",bVal);          ASSERT_TRUE(bVal);
      miscInfo.get("conjBeams",bVal);        ASSERT_TRUE(bVal);
    };

    verifyMiscInfo(dryrunMiscInfo);

    // Add assertions here to verify the behavior of the Coyote
    // function in dryrun mode produces the required values in
    // cgrid_csys.rec and iminfo.rec.
  }

  TEST(CoyoteTest, CoyoteFillCF) {

    // char *CASAPATH = std::getenv("CASAPATH");
    // if (CASAPATH == nullptr)
    //   {
    //   //      throw(AipsError("CASAPATH for CoyoteFillCF test not defined"));
    //   cout  << "[INFO] CASAPATH for CoyoteFillCF test not defined" << endl;
    //   }
    // else
      {
	fs::current_path(test::testDir);

	std::vector<int> wList={0,5,10},
	  polList={0,1};
	std::vector<std::string> cfList;

	for (auto iw : wList) {
	  for (auto ip : polList) {
	    std::stringstream filePath;
	    filePath << "CFS_0_0_CF_0_" << iw << "_" << ip << ".im";
	    cfList.push_back(filePath.str());
	  }
	}

	//for(auto cf : cfList) cerr << cf << " "; cerr << endl;

	// string mode_l="fillcf";
	// Coyote(msName, telescopeName, NX, cellSize, stokes, refFreqStr, nW, cfCacheName,
	//        WBAwp, aTerm, psTerm, mType, pa, dpa, fieldStr, spwStr, phaseCenter,
	//        conjBeams, cfBufferSize, cfOversampling, cfList, mode_l);

	// Add assertions here to verify the behavior of the Coyote function in fillcf mode
	//    fs::remove_all(test::testDir);
      }
  }

};
