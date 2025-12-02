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

#ifndef MSSPLIT_H
#define MSSPLIT_H

#include <casacore/casa/aips.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/ms/MSSel/MSSelectionError.h>
#include <casacore/ms/MSSel/MSSelectionTools.h>
#include <casacore/ms/MSSel/MSSelectableTable.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/PlainTable.h>
#include <string>
using namespace std;
using namespace casacore;
//
//-------------------------------------------------------------------------
//
/**
 * @fn void showTableCache()
 * @brief Displays the current state of the table cache.
 */
void showTableCache();

//
//-------------------------------------------------------------------------
//
/**
 * @fn void printBaselineList(const casacore::MeasurementSet& ms)
 * @brief Prints the list of baselines in the given MeasurementSet.
 * @param ms The MeasurementSet to print baselines from.
 */
void printBaselineList(Matrix<Int> list, ostream& os);

//
//-------------------------------------------------------------------------
//
/**
 * @fn void printInfo(const casacore::MeasurementSet& ms)
 * @brief Prints information about the given MeasurementSet.
 * @param ms The MeasurementSet to print information about.
 */

void printInfo(casacore::MSSelection& msSelection);

//
//-------------------------------------------------------------------------
//
/**
 * @fn void MSSplit_func(const std::string& MSName, const std::string& subMSName, const std::string& fieldStr, const std::string& spwStr, const std::string& uvDistStr)
 * @brief Splits a MeasurementSet into a subset.
 * @param MSName The name of the original MeasurementSet.
 * @param subMSName The name of the subset MeasurementSet to be created.
 * @param fieldStr The field string to select specific fields.
 * @param spwStr The spectral window string to select specific spectral windows.
 * @param uvDistStr The UV distance string to select specific UV distances.
 */
void MSSplit_func(const string& MSNBuf="", const string& OutMSBuf="", const bool& deepCopy=0, const string& fieldStr="",
		 const string& timeStr="", const string &spwStr="", const string& baselineStr="",
		 const string& uvdistStr="", const string& taqlStr="", const string& scanStr="",
		 const string& arrayStr="", const string& polnStr="", const string& stateObsModeStr="",
		 const string& observationStr="");



/**
 * @brief This function is responsible for handling user input for the MSSplit application.
 * 
 * @param restart A boolean value indicating whether the application is being restarted.
 * @param argc An integer value indicating the number of command line arguments.
 * @param argv A pointer to a character array containing the command line arguments.
 * @param interactive A boolean parameter indicating whether to run in the interactive mode
 * @param MSNBuf A reference to a string variable to store the input table name.
 * @param OutMSBuf A reference to a string variable to store the output table name.
 * @param deepCopy A boolean reference to indicate whether to perform a deep copy.
 * @param fieldStr A reference to a string variable to store the field selection.
 * @param timeStr A reference to a string variable to store the time selection.
 * @param spwStr A reference to a string variable to store the spectral window selection.
 * @param baselineStr A reference to a string variable to store the baseline selection.
 * @param scanStr A reference to a string variable to store the scan selection.
 * @param arrayStr A reference to a string variable to store the array selection.
 * @param uvdistStr A reference to a string variable to store the uvdist selection.
 * @param taqlStr A reference to a string variable to store the TAQL selection.
 * @param polnStr A reference to a string variable to store the polarization selection.
 * @param stateObsModeStr A reference to a string variable to store the state observation mode selection.
 * @param observationStr A reference to a string variable to store the observation selection.
 * 
 * @throws AipsError if the input table name is not set.
 */
void UI(Bool restart, int argc, char **argv, bool interactive, 
	string& MSNBuf, string& OutMSBuf, bool& deepCopy,
	string& fieldStr, string& timeStr, string& spwStr, string& baselineStr,
	string& scanStr, string& arrayStr, string& uvdistStr,string& taqlStr, string& polnStr,
	string& stateObsModeStr, string& observationStr);

#endif
