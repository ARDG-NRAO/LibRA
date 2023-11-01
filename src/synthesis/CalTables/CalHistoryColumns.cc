//# CalHistoryColumns.cc: Implementation of CalHistoryColumns.h
//# Copyright (C) 1996,1997,1998,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
//----------------------------------------------------------------------------

#include <synthesis/CalTables/CalHistoryColumns.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

ROCalHistoryColumns::ROCalHistoryColumns (const CalTable& calTable) 
{
// Construct from a calibration table
// Input:
//    calTable         const CalTable&                Calibration table
// Output to private data:
//    calParms_p       ScalarColumn<String>&        Solver parameters
//    calTables_p      ScalarColumn<String>&        Associated cal. tables
//    calSelect_p      ScalarColumn<String>&        Cal. selection
//    calNotes_p       ScalarColumn<String>&        Cal. notes
//
  // Attach all the column accessors (including required and
  // optional columns)
  attach (calTable, calParms_p, MSC::CAL_PARMS);
  attach (calTable, calTables_p, MSC::CAL_TABLES);
  attach (calTable, calSelect_p, MSC::CAL_SELECT);
  attach (calTable, calNotes_p, MSC::CAL_NOTES);
};

//----------------------------------------------------------------------------

void ROCalHistoryColumns::attach (const CalTable& calTable, 
				  TableColumn& tabCol, 
				  MSCalEnums::colDef colEnum, 
				  const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const CalTable&      Calibration table
//    tabCol           TableColumn&       Table column accessor
//    colEnum          MSCalEnums::colDef   Column enum
//    optional         const Bool&          true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calHistoryAsTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (calHistoryAsTable(calTable), colName);
    };
  } else {
    tabCol.attach (calHistoryAsTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

CalHistoryColumns::CalHistoryColumns (CalTable& calTable) 
{
// Construct from a calibration table
// Input:
//    calTable         const CalTable&              Calibration table
// Output to private data:
//    calParms_p       ScalarColumn<String>&        Solver parameters
//    calTables_p      ScalarColumn<String>&        Associated cal. tables
//    calSelect_p      ScalarColumn<String>&        Cal. selection
//    calNotes_p       ScalarColumn<String>&        Cal. notes
//
  // Attach all the column accessors (including required and
  // optional columns)
  attach (calTable, calParms_p, MSC::CAL_PARMS);
  attach (calTable, calTables_p, MSC::CAL_TABLES);
  attach (calTable, calSelect_p, MSC::CAL_SELECT);
  attach (calTable, calNotes_p, MSC::CAL_NOTES);
};

//----------------------------------------------------------------------------

void CalHistoryColumns::attach (CalTable& calTable, 
				TableColumn& tabCol, 
				MSCalEnums::colDef colEnum, 
				const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         CalTable&            Calibration table
//    tabCol           TableColumn&       Table column accessor
//    colEnum          MSCalEnums::colDef   Column enum
//    optional         const Bool&          true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calHistoryAsTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (calHistoryAsTable(calTable), colName);
    };
  } else {
    tabCol.attach (calHistoryAsTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

