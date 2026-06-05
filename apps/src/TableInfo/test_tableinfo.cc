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
