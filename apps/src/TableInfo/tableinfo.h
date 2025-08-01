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

#ifndef TABLEINFO_H
#define TABLEINFO_H

#include <casacore/casa/aips.h>
#include <casacore/ms/MSOper/MSSummary.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/StreamLogSink.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/images/Images/ImageOpener.h>
#include <casacore/images/Images/ImageSummary.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/lattices/Lattices/PagedArray.h>
#include <fstream>

using namespace std;
using namespace casacore;

//
//-------------------------------------------------------------------------
//
/**
 * @fn void TableInfo_func(const std::string& tableName)
 * @brief Prints information about a given table.
 * @param tableName The name of the table to print information about.
 */
void TableInfo_func(const string& MSNBuf="",
		    const string& OutBuf="",
		    const bool& verbose=false);


/**
 * @brief This function retrieves command-line arguments using the cl library and sets the input and output file names.
 * 
 * @param restart A boolean flag indicating whether the function is being called for the first time or being restarted.
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @param interactive  A boolean parameter indicating whether to run in the interactive mode
 * @param MSNBuf A string reference to store the input table name.
 * @param OutBuf A string reference to store the output file name.
 * @param verbose A boolean reference to store the verbosity flag.
 * 
 * @throws AipsError if the input table name is not set.
 */
void UI(Bool restart, int argc, char **argv, bool interactive,
	string& MSNBuf, string& OutBuf,
	bool& verbose);

#endif
