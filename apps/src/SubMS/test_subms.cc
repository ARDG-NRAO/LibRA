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
#include <SubMS/subms.h>
#include <gtest/gtest.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSMainColumns.h>
#include <casacore/ms/MeasurementSets/MSDataDescColumns.h>
#include <casacore/ms/MeasurementSets/MSPolColumns.h>

using namespace std;
using namespace std::filesystem;
using namespace casacore;

namespace test{

// Real MS shared by the SubMS_func tests below: 2 fields, 3 spws, RR/LL.
static const string kGoldMS = string(GOLD_STANDARD_DIR) + "/refim_oneshiftpoint.mosaic.ms";

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
    baselineStr,uvdistStr, taqlStr,scanStr,arrayStr,corrStr,combineStr,intentStr,obsStr;

   MSNBuf="testin.ms";
   OutMSBuf="testout.ms";
   WhichColStr=fieldStr=timeStr=spwStr=baselineStr=uvdistStr=
        taqlStr=scanStr=corrStr=arrayStr=combineStr=intentStr=obsStr="";
   WhichColStr="data"; fieldStr="*"; spwStr="*";
   int deepCopy=0;
   int chanStep=1;
   float integ=-1;
   Bool restartUI = false;
   Bool interactive = false;

   UI(restartUI,argc, argv, interactive, MSNBuf,OutMSBuf, WhichColStr, deepCopy,
         fieldStr,timeStr,spwStr,baselineStr,scanStr,arrayStr,uvdistStr,taqlStr,integ,
         chanStep,combineStr,corrStr,intentStr,obsStr);
}

// Mirrors split's ctor -> setmsselect -> selectTime -> makeSubMS contract:
// selecting field=0, spw=0 must restrict every output row to that field/spw.
TEST(SubMSTest, FieldAndSpwSelectionMatchesSplit) {
  string outms = "test_subms_field0_spw0.ms";
  remove_all(outms);

  SubMS_func(kGoldMS, outms, "data", false, "0", "", "0", "", "", "", "", "", -1, 1, "");

  ASSERT_TRUE(exists(outms));
  MeasurementSet ms(outms);
  ASSERT_GT(ms.nrow(), 0u);

  ScalarColumn<Int> fieldIdCol(ms, MS::columnName(MS::FIELD_ID));
  ScalarColumn<Int> ddIdCol(ms, MS::columnName(MS::DATA_DESC_ID));
  MSDataDescColumns ddCols(ms.dataDescription());
  for (rownr_t row = 0; row < ms.nrow(); ++row) {
    EXPECT_EQ(fieldIdCol(row), 0);
    EXPECT_EQ(ddCols.spectralWindowId()(ddIdCol(row)), 0);
  }

  remove_all(outms);
}

// Exercises the params added to bring SubMS up to split's parameter set.
TEST(SubMSTest, SelectionParamsWireThrough) {
  // chanStep < 1 is rejected before the MS is even opened for split.
  EXPECT_THROW(
    SubMS_func(kGoldMS, "test_subms_badstep.ms", "data", false, "*", "", "*", "",
               "", "", "", "", -1, 0, ""),
    AipsError);

  // correlation="RR" must reduce the output POLARIZATION table to 1 correlation.
  string corrOut = "test_subms_corr.ms";
  remove_all(corrOut);
  SubMS_func(kGoldMS, corrOut, "data", false, "*", "", "*", "", "", "", "", "",
             -1, 1, "", "RR");
  ASSERT_TRUE(exists(corrOut));
  {
    MeasurementSet ms(corrOut);
    MSPolarizationColumns polCols(ms.polarization());
    EXPECT_EQ(polCols.numCorr()(0), 1);
  }
  remove_all(corrOut);

  // combine/intent/observation/subarray must pass through without error.
  string passOut = "test_subms_passthrough.ms";
  remove_all(passOut);
  SubMS_func(kGoldMS, passOut, "data", false, "*", "", "*", "", "", "", "", "",
             -1, 1, "scan", "", "*OBSERVE_TARGET*", "0");
  ASSERT_TRUE(exists(passOut));
  EXPECT_GT(MeasurementSet(passOut).nrow(), 0u);
  remove_all(passOut);
}

};
