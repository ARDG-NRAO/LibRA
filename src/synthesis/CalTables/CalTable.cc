//# CalTable.cc: Implementation of CalTable.h
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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

#include <synthesis/CalTables/CalTable.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <casacore/tables/Tables/TableRow.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/casa/Arrays.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <msvis/MSVis/MSCalEnums.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalTable::CalTable() : itsMainTable(NULL), itsDescTable(NULL), 
	itsHistoryTable(NULL), itsObservationTable(NULL)
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsMainTable         Table*           Ptr to cal_main Table object
//    itsDescTable         Table*           Ptr to cal_desc Table object
//    itsHistoryTable      Table*           Ptr to cal_history Table object
//    itsObservationTable  Table*           Ptr to observation Table object
//
};

//----------------------------------------------------------------------------

CalTable::~CalTable()
{
// Default desctructor
// Output to private data:
//    itsMainTable         Table*           Ptr to cal_main Table object
//    itsDescTable         Table*           Ptr to cal_desc Table object
//    itsHistoryTable      Table*           Ptr to cal_history Table object
//    itsObservationTable  Table*           Ptr to observation Table object
//
  if (itsMainTable) {
    delete (itsMainTable);
  };
  if (itsDescTable) {
    delete (itsDescTable);
  };
  if (itsHistoryTable) {
    delete (itsHistoryTable);
  };
  if (itsObservationTable) {
    delete (itsObservationTable);
  };
};

//----------------------------------------------------------------------------

CalTable::CalTable (const String& tableName, CalTableDesc& ctableDesc,
		    Table::TableOption access) : 
  itsMainTable(NULL), itsDescTable(NULL), itsHistoryTable(NULL),
	itsObservationTable(NULL)
{
// Construct from a cal table name, descriptor and access option.
// Used for creating new tables.
// Input:
//    tableName        const String&         Cal table name
//    ctableDesc       const CalTableDesc&   Cal table descriptor
//    access           Table::TableOption    Access mode
// Output to private data:
//    itsMainTable         Table*            Ptr to cal_main Table object
//    itsDescTable         Table*            Ptr to cal_desc Table object
//    itsHistoryTable      Table*            Ptr to cal_history Table object
//    itsObservationTable  Table*            Ptr to observation Table object
//
  createCalTable (tableName, ctableDesc, access);
};

//----------------------------------------------------------------------------

CalTable::CalTable (const String& tableName, Table::TableOption access) :
  itsMainTable(NULL), itsDescTable(NULL), itsHistoryTable(NULL),
	itsObservationTable(NULL)
{
// Construct from a cal table name and access option. Used for
// accessing existing tables.
// Input:
//    tableName        const String&         Cal table name
//    access           Table::TableOption    Access option
// Output to private data:
//    itsMainTable         Table*            Ptr to cal_main Table object
//    itsDescTable         Table*            Ptr to cal_desc Table object
//    itsHistoryTable      Table*            Ptr to cal_history Table object
//    itsObservationTable  Table*            Ptr to observation Table object
//
  openCalTable (tableName, access);
};

//----------------------------------------------------------------------------

CalTable::CalTable (const Table& table)
{
// Construct from an existing table object
// Input:
//    table            const Table&          Input table
// Output to private data:
//    itsMainTable         Table*            Ptr to cal_main Table object
//    itsDescTable         Table*            Ptr to cal_desc Table object
//    itsHistoryTable      Table*            Ptr to cal_history Table object
//    itsObservationTable  Table*            Ptr to observation Table object
//
  itsMainTable = new Table (table);
  itsDescTable = new Table 
    (itsMainTable->keywordSet().asTable (MSC::fieldName (MSC::CAL_DESC)));
  itsHistoryTable = new Table
    (itsMainTable->keywordSet().asTable (MSC::fieldName (MSC::CAL_HISTORY)));
  if (hasObsTable())
    itsObservationTable = new Table
      (itsMainTable->keywordSet().asTable (MSC::fieldName (MSC::OBSERVATION)));
  else
    itsObservationTable = NULL;
};

//----------------------------------------------------------------------------

CalTable::CalTable (const CalTable& other)
{
// Copy constructor
// Input:
//    other            const CalTable&       Existing CalTable object
// Output to private data:
//    itsMainTable         Table*            Ptr to cal_main Table object
//    itsDescTable         Table*            Ptr to cal_desc Table object
//    itsHistoryTable      Table*            Ptr to cal_history Table object
//    itsObservationTable  Table*            Ptr to observation Table object
//
  itsMainTable = new Table (*(other.itsMainTable));
  itsDescTable = new Table 
    (itsMainTable->keywordSet().asTable (MSC::fieldName (MSC::CAL_DESC)));
  itsHistoryTable = new Table
    (itsMainTable->keywordSet().asTable (MSC::fieldName (MSC::CAL_HISTORY)));
  if (hasObsTable())
    itsObservationTable = new Table
      (itsMainTable->keywordSet().asTable (MSC::fieldName (MSC::OBSERVATION)));
  else
    itsObservationTable = NULL;
};

//----------------------------------------------------------------------------

CalTable& CalTable::operator= (const CalTable& other)
{
// Assignment operator
// Input:
//    other            const CalTable&       RHS CalTable object
// Output to private data:
//    itsMainTable         Table*            Ptr to cal_main Table object
//    itsDescTable         Table*            Ptr to cal_desc Table object
//    itsHistoryTable      Table*            Ptr to cal_history Table object
//    itsObservationTable  Table*            Ptr to observation Table object
//
  Bool identity = (this == &other);
  if (itsMainTable && !identity) {
    *itsMainTable = *(other.itsMainTable);
  };
  if (itsDescTable && !identity) {
    *itsDescTable = *(other.itsDescTable);
  };
  if (itsHistoryTable && !identity) {
    *itsHistoryTable = *(other.itsHistoryTable);
  };
  if (itsObservationTable && !identity && other.hasObsTable()) {
    *itsObservationTable = *(other.itsObservationTable);
  };
  return *this;
};

//----------------------------------------------------------------------------

CalTable CalTable::sort (const Block <String>& columnNames, Sort::Order order,
			 Sort::Option option)
{
// Sort the calibration table on multiple columns
// Input:
//    columnNames      const Block <String>&    Sort columns
//    order            Sort::Order              Sort order
//    option           Sort::Option             Sort option
// Output:
//    sort             CalTable                 Sorted reference cal table
// Input from private data:
//    itsMainTable     Table*                   Underlying table object
// 
  // Sort the cal_main table directly
  Table result = itsMainTable->sort (columnNames, order, option);
  return CalTable (result);
};

//----------------------------------------------------------------------------

void CalTable::sort2 (const Block <String>& columnNames, Sort::Order order,
			 Sort::Option option)
{
// Sort the calibration table on multiple columns
// Input:
//    columnNames      const Block <String>&    Sort columns
//    order            Sort::Order              Sort order
//    option           Sort::Option             Sort option
// Output:
//    sort             CalTable                 Sorted reference cal table
// Input from private data:
//    itsMainTable     Table*                   Underlying table object
// 
  // Sort the cal_main table directly (IN PLACE!)
  Table *sorted = new Table(itsMainTable->sort (columnNames, order, option));
  delete itsMainTable;
  itsMainTable=sorted;

  return;
};

//----------------------------------------------------------------------------

CalTable CalTable::select (const String& calSelect)
{
// Apply selection to the calibration table
// Input:
//    calSelect        const String&            TAQL selection string
// Output:
//    select           CalTable                 Selected reference cal table
// Input from private data:
//    itsMainTable     Table*                   Underlying table object
//
  // Apply selection to the cal_main table directly
  Int len = calSelect.length();
  Int nspace = calSelect.freq (' ');
  if (calSelect.empty() || nspace==len) {
    return *this;
  } else {
    String parseString = "select from $1 where " + calSelect;
    Table result = tableCommand (parseString, *itsMainTable).table();
    return CalTable (result);
  };
};

//----------------------------------------------------------------------------

void CalTable::select2 (const String& calSelect)
{
// Apply selection to the calibration table
// Input:
//    calSelect        const String&            TAQL selection string
// Output:
//    select           CalTable                 Selected reference cal table
// Input from private data:
//    itsMainTable     Table*                   Underlying table object
//
  // Apply selection to the cal_main table directly
  Int len = calSelect.length();
  Int nspace = calSelect.freq (' ');
  if (!calSelect.empty() && nspace!=len) {
    String parseString = "select from $1 where " + calSelect;
    Table *selected = new Table(tableCommand (parseString, *itsMainTable).table());
    delete itsMainTable;
    itsMainTable=selected;
  };
  return;
};

//----------------------------------------------------------------------------

Int CalTable::nRowMain() const
{
// Return the number of rows in cal_main
// Output:
//    nRowMain         Int                   No of rows in cal_main
//
  return itsMainTable->nrow();
};

//----------------------------------------------------------------------------

Int CalTable::nRowDesc() const
{
// Return the number of rows in cal_desc
// Output:
//    nRowDesc         Int                   No of rows in cal_desc
//
  return itsDescTable->nrow();
};

//----------------------------------------------------------------------------

Int CalTable::nRowHistory() const
{
// Return the number of rows in cal_history
// Output:
//    nRowHistory      Int                   No of rows in cal_history
//
  return itsHistoryTable->nrow();
};

//----------------------------------------------------------------------------

Int CalTable::nRowObservation() const
{
// Return the number of rows in observation
// Output:
//    nRowObservation      Int               No of rows in observation
//
  if (hasObsTable())
	  return itsObservationTable->nrow();
  else
	  return -1;
};

//----------------------------------------------------------------------------
Record CalTable::getRowMain (const Int& jrow)
{
// Get a row from cal_main
// Input:
//    jrow             const Int&            Row number
// Output:
//    getRowMain       Record                Row record
//
  ROTableRow trow (*itsMainTable);
  trow.get (jrow);
  return trow.record();
};

//----------------------------------------------------------------------------

Record CalTable::getRowDesc (const Int& jrow)
{
// Get a row from cal_desc
// Input:
//    jrow             const Int&            Row number
// Output:
//    getRowDesc       Record                Row record
//
  ROTableRow trow (*itsDescTable);
  trow.get (jrow);
  return trow.record();
};

//----------------------------------------------------------------------------

Record CalTable::getRowHistory (const Int& jrow)
{
// Get a row from cal_history
// Input:
//    jrow             const Int&            Row number
// Output:
//    getRowHistory    Record                Row record
//
  ROTableRow trow (*itsHistoryTable);
  trow.get (jrow);
  return trow.record();
};

//----------------------------------------------------------------------------

Record CalTable::getRowObservation (const Int& jrow)
{
// Get a row from observation
// Input:
//    jrow             const Int&            Row number
// Output:
//    getRowObservation    Record            Row record
//
  if (hasObsTable()) {
    ROTableRow trow (*itsObservationTable);
    trow.get (jrow);
    return trow.record();
  } else {
	return Record();
  }
};

//----------------------------------------------------------------------------
void CalTable::putRowMain (const Int& jrow, CalMainRecord& tableRec)
{
// Put a row to cal_main
// Input:
//    jrow             const Int&            Row number
//    tableRec         CalMainRecord&        Table record
//
  // Add rows as required
  Int nMaxRow = itsMainTable->nrow();
  Int nAdd = jrow - nMaxRow + 1;
  if (nAdd > 0) {
    itsMainTable->addRow (nAdd);
  };

  // Write the record
  TableRow trow (*itsMainTable);
  TableRecord trec = tableRec.record();
  trow.putMatchingFields (jrow, trec);
};

//----------------------------------------------------------------------------

void CalTable::putRowDesc (const Int& jrow, CalDescRecord& tableRec)
{
// Put a row to cal_desc
// Input:
//    jrow             const Int&            Row number
//    tableRec         CalDescRecord&        Table record
//
  // Add rows as required
  Int nMaxRow = itsDescTable->nrow();
  Int nAdd = jrow - nMaxRow + 1;
  if (nAdd > 0) {
    itsDescTable->addRow (nAdd);
  };

  // Write the record
  TableRow trow (*itsDescTable);
  TableRecord trec = tableRec.record();
  trow.putMatchingFields (jrow, trec);
};

//----------------------------------------------------------------------------

void CalTable::putRowHistory (const Int& jrow, CalHistoryRecord& tableRec)
{
// Put a row to cal_history
// Input:
//    jrow             const Int&            Row number
//    tableRec         CalHistoryRecord&     Table record
//
  // Add rows as required
  Int nMaxRow = itsHistoryTable->nrow();
  Int nAdd = jrow - nMaxRow + 1;
  if (nAdd > 0) {
    itsHistoryTable->addRow (nAdd);
  };
  TableRow trow (*itsHistoryTable);
  TableRecord trec = tableRec.record();
  trow.putMatchingFields (jrow, trec);
};

//----------------------------------------------------------------------------

Int CalTable::maxAntenna()
{
// Return the maximum antenna no. found in the calibration table
// Output:
//    maxAntenna          Int         Max. antenna no. found
//
  ScalarColumn <Int> antCol (*itsMainTable, MSC::fieldName (MSC::ANTENNA1));
  Vector <Int> antVal;
  antCol.getColumn (antVal);
  uInt i;
  Int maxAnt = 0;
  for (i = 0; i < antVal.nelements(); i++) {
    maxAnt = max (antVal(i), maxAnt);
  };

  return maxAnt;
};

//----------------------------------------------------------------------------

Int CalTable::numberTimeSlots (const Double& fracError)
{
// Return the no. of unique time slots found in the calibration table
// Input:
//    fracError        const Double&   Fractional error allowed in comparison
// Output:
//    numberTimeSlots  Int             No. of unique time stamps found
//    
  ScalarColumn <Double> timeCol (*itsMainTable, MSC::fieldName (MSC::TIME));
  ScalarColumn <Double> intervalCol (*itsMainTable, 
				     MSC::fieldName (MSC::INTERVAL));
  Vector <Double> timeVal;
  Vector <Double> intervalVal;
  timeCol.getColumn (timeVal);
  intervalCol.getColumn (intervalVal);
  
  // Index of matches
  Vector <Int> match;
  Int n = timeVal.nelements();
  match.resize (n);
  match = 0;
  Int i, j;
  Bool found;

  // Loop through all time stamps
  for (i = 0; i < (n-1); i++) {
    found = false;
    for (j = i+1; j < n; j++) {
      if (abs (timeVal(i) - timeVal(j)) < fracError * intervalVal(i) &&
	  abs (intervalVal(i) - intervalVal(j)) < fracError * intervalVal(i)) {
	found = true;
      };
    };
    if (!found) {
      match(i) = 1;
    };
  };
  if (n > 0) {
    match(n-1) = 1;
  };

  // Sum the number of matches
  Int sum = 0;
  for (i = 0; i < n; i++) {
    sum = sum + match(i);
  };

  return sum;
};

//----------------------------------------------------------------------------

// Return the number of rows per CalDescId
void CalTable::rowsPerCalDescId(Vector<Int>& rowspercdi) {

  Int nCDI=nRowDesc();

  rowspercdi.resize(nCDI);

  ScalarColumn<Int> cdidcol(*itsMainTable,"CAL_DESC_ID");
  Vector<Int> cdi;
  cdidcol.getColumn(cdi);

  for (Int icdi=0;icdi<nCDI;++icdi)
    rowspercdi(icdi)=ntrue(cdi==icdi);

}


//----------------------------------------------------------------------------

void CalTable::createCalTable (const String& tableName, 
			       CalTableDesc& ctableDesc,
			       Table::TableOption access)
{
// Create a new cal table
// Input:
//    tableName        const String&         Cal table name
//    ctableDesc       const CalTableDesc&   Cal table descriptor
//    access           Table::TableOption    Access mode
// Output to private data:
//    itsMainTable     Table*                Ptr to cal_main Table object
//    itsDescTable     Table*                Ptr to cal_desc Table object
//    itsHistoryTable  Table*                Ptr to cal_history Table object
//
  // Construct the table descriptor, including the sub-table structure
  // for cal_desc and cal_history. Add subtables to CAL_DESC and
  // CAL_HISTORY keywords.
  SetupNewTable calMainSetup (tableName, ctableDesc.calMainDesc(), access);
  itsMainTable = new Table (calMainSetup);
  SetupNewTable calDescSetup (tableName + "/CAL_DESC", 
			      ctableDesc.calDescDesc(), access);
  itsDescTable = new Table (calDescSetup);
  itsMainTable->rwKeywordSet().defineTable (MSC::fieldName (MSC::CAL_DESC),
					    *itsDescTable);
  SetupNewTable calHistorySetup (tableName + "/CAL_HISTORY",
				 ctableDesc.calHistoryDesc(), access);
  itsHistoryTable = new Table (calHistorySetup);
  itsMainTable->rwKeywordSet().defineTable (MSC::fieldName (MSC::CAL_HISTORY),
					    *itsHistoryTable);
  // Set table type and sub-type
  String reqdType = TableInfo::type (TableInfo::ME_CALIBRATION);
  itsMainTable->tableInfo().setType (reqdType);
  itsMainTable->tableInfo().setSubType (itsMainTable->tableDesc().getType());
};

//----------------------------------------------------------------------------

void CalTable::openCalTable (const String& tableName, 
			     Table::TableOption access)
{
// Open an existing cal table
// Input:
//    tableName        const String&         Cal table name
//    access           Table::TableOption    Access option
// Output to private data:
//    itsMainTable         Table*            Ptr to cal_main Table object
//    itsDescTable         Table*            Ptr to cal_desc Table object
//    itsHistoryTable      Table*            Ptr to cal_history Table object
//    itsObservationTable  Table*            Ptr to cal_history Table object
//
  itsMainTable = new Table (tableName, access);
  itsDescTable = new Table (itsMainTable->keywordSet().asTable
    (MSC::fieldName(MSC::CAL_DESC)));
  itsHistoryTable = new Table (itsMainTable->keywordSet().asTable
    (MSC::fieldName(MSC::CAL_HISTORY)));
  if (hasObsTable())
    itsObservationTable = new Table (itsMainTable->keywordSet().asTable
      (MSC::fieldName(MSC::OBSERVATION)));
  else
    itsObservationTable = NULL;
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

