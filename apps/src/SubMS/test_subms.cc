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
