//# CalHistoryColumns.h: Calibration table cal_history column access
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef CALIBRATION_CALHISTORYCOLUMNS_H
#define CALIBRATION_CALHISTORYCOLUMNS_H

#include <casa/aips.h>
#include <measures/Measures/MFrequency.h>
#include <tables/Tables/TableColumn.h>
#include <tables/Tables/ScalarColumn.h>
#include <synthesis/CalTables/CalTable.h>
#include <msvis/MSVis/MSCalEnums.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// ROCalHistoryColumns: Read-only cal_history calibration table column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTable">CalTable</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only", "calibration history table" and "columns".
// </etymology>
//
// <synopsis>
// The ROCalHistoryColumns class allows read-only access to columns
// in the cal_history calibration sub-table. 
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-only access to cal_history calibration sub-table columns.
// </motivation>
//
// <todo asof="11/01/01">
// (i) Deal with non-standard columns.
// </todo>

class ROCalHistoryColumns 
{
 public:
  // Construct from a cal_history calibration sub-table
  ROCalHistoryColumns (const CalTable& calTable);

  // Default destructor
  virtual ~ROCalHistoryColumns() {};

  // Read-only column accessors
  const casacore::ScalarColumn<casacore::String>& calParms() const {return calParms_p;};
  const casacore::ScalarColumn<casacore::String>& calTables() const {return calTables_p;};
  const casacore::ScalarColumn<casacore::String>& calSelect() const {return calSelect_p;};
  const casacore::ScalarColumn<casacore::String>& calNotes() const {return calNotes_p;};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROCalHistoryColumns() {};

  // Return a CalTable cal_history subtable as a casacore::Table reference. 
  // Utilizes friendship relationship with class CalTable.
  const casacore::Table& calHistoryAsTable(const CalTable& calTable) 
    {return calTable.calHistoryAsTable();}

  // Attach a table column accessor
  void attach (const CalTable& calTable, casacore::TableColumn& tabCol, 
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);

 private:
  // Prohibit copy constructor and assignment operator 
  ROCalHistoryColumns (const ROCalHistoryColumns&);
  ROCalHistoryColumns& operator= (const ROCalHistoryColumns&);

  // Private column accessors
  casacore::ScalarColumn<casacore::String> calParms_p;
  casacore::ScalarColumn<casacore::String> calTables_p;
  casacore::ScalarColumn<casacore::String> calSelect_p;
  casacore::ScalarColumn<casacore::String> calNotes_p;
};

// <summary> 
// CalHistoryColumns: Read-write cal_history calibration table column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTable">CalTable</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration history table" and "columns".
// </etymology>
//
// <synopsis>
// The CalHistoryColumns class allows read-write access to columns
// in the cal_history calibration sub-table. 
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to cal_history calibration sub-table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class CalHistoryColumns 
{
 public:
  // Construct from a calibration table
  CalHistoryColumns (CalTable& calTable);

  // Default destructor
  virtual ~CalHistoryColumns() {};

  // Read-write column accessors
  casacore::ScalarColumn<casacore::String>& calParms() {return calParms_p;};
  casacore::ScalarColumn<casacore::String>& calTables() {return calTables_p;};
  casacore::ScalarColumn<casacore::String>& calSelect() {return calSelect_p;};
  casacore::ScalarColumn<casacore::String>& calNotes() {return calNotes_p;};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  CalHistoryColumns() {};

  // Return a CalTable cal_history subtable as a casacore::Table reference. 
  // Utilizes friendship relationship with class CalTable.
  casacore::Table& calHistoryAsTable(CalTable& calTable) 
    {return calTable.calHistoryAsTable();}

  // Attach a table column accessor
  void attach (CalTable& calTable, casacore::TableColumn& tabCol, 
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);

 private:
  // Prohibit copy constructor and assignment operator 
  CalHistoryColumns (const CalHistoryColumns&);
  CalHistoryColumns& operator= (const CalHistoryColumns&);

  // Private column accessors
  casacore::ScalarColumn<casacore::String> calParms_p;
  casacore::ScalarColumn<casacore::String> calTables_p;
  casacore::ScalarColumn<casacore::String> calSelect_p;
  casacore::ScalarColumn<casacore::String> calNotes_p;

};


} //# NAMESPACE CASA - END

#endif
   
  



