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

#ifndef SUBMS_H
#define SUBMS_H

#include <casacore/casa/aips.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/ms/MSSel/MSSelectionError.h>
#include <casacore/ms/MSSel/MSSelectionTools.h>
#include <msvis/MSVis/SubMS.h>

using namespace std;
using namespace casa;
using namespace casacore;
//
//-------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------
//
/**
 * @fn void SubMS_func(const std::string& MSName, const std::string& subMSName, const std::string& fieldStr, const std::string& spwStr, const std::string& uvDistStr)
 * @brief Creates a subset of a MeasurementSet.
 * @param MSName The name of the original MeasurementSet.
 * @param subMSName The name of the subset MeasurementSet to be created.
 * @param fieldStr The field string to select specific fields.
 * @param spwStr The spectral window string to select specific spectral windows.
 * @param uvDistStr The UV distance string to select specific UV distances.
 */
void SubMS_func(const string& MSNBuf="", const string& OutMSBuf="",
		const string& WhichColStr="data", const bool& deepCopy=false,
		const string& fieldStr="*",const string& timeStr="",
		const string& spwStr="*", const string& baselineStr="",
		const string& scanStr="", const string& arrayStr="",
		const string& uvdistStr="",const string& taqlStr="",
		const float integ=-1);


/**
 * @brief This function provides a user interface for the command line interface (CLI) of the subms program.
 * 
 * @param restart A boolean flag indicating whether to restart the CLI or not.
 * @param argc The number of command line arguments.
 * @param argv An array of command line arguments.
 * @param interactive  A boolean parameter indicating whether to run in the interactive mode
 * @param MSNBuf A string reference to store the input MS name.
 * @param OutMSBuf A string reference to store the output MS name.
 * @param WhichColStr A string reference to store the data column name.
 * @param deepCopy An integer reference to store the deep copy flag.
 * @param fieldStr A string reference to store the field name.
 * @param timeStr A string reference to store the time range.
 * @param spwStr A string reference to store the spectral window name.
 * @param baselineStr A string reference to store the baseline name.
 * @param scanStr A string reference to store the scan name.
 * @param arrayStr A string reference to store the array name.
 * @param uvdistStr A string reference to store the uv distance range.
 * @param taqlStr A string reference to store the TAQL query.
 * @param integ A float reference to store the integration time.
 */
void UI(bool restart, int argc, char **argv, bool interactive, string& MSNBuf, string& OutMSBuf, 
	string& WhichColStr,int& deepCopy,string& fieldStr, string& timeStr, 
	string& spwStr, string& baselineStr,string& scanStr, string& arrayStr, 
	string& uvdistStr,string& taqlStr,float& integ);


#endif
