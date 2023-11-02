//# TimeVarVJTable.cc: Implementation of TimeVarVJTable.h
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

#include <synthesis/CalTables/TimeVarVJTable.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

TimeVarVisJonesTable::TimeVarVisJonesTable() : VisJonesTable(), 
  itsTimeVarVisJonesDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsTimeVarVisJonesDesc   TimeVarVisJonesDesc  TimeVarVisJones table 
//                                                  descriptor
//
};

//----------------------------------------------------------------------------

TimeVarVisJonesTable::~TimeVarVisJonesTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

TimeVarVisJonesTable::TimeVarVisJonesTable (const String& tableName, 
					    Table::TableOption access) :
  itsTimeVarVisJonesDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName               const String&         Cal table name
//    access                  Table::TableOption    Access option
// Output to private data:
//    itsTimeVarVisJonesDesc  TimeVarVisJonesDesc   TimeVarVisJones table 
//                                                  descriptor
  // Is this a new or existing calibration table ?
  if (access == Table::New && access == Table::NewNoReplace &&
      access == Table::Scratch) {
    createCalTable (tableName, itsTimeVarVisJonesDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------

PJonesTable::PJonesTable() : TimeVarVisJonesTable(), itsPJonesDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsPJonesDesc      PJonesDesc         PJones table descriptor
//
};

//----------------------------------------------------------------------------

PJonesTable::~PJonesTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

PJonesTable::PJonesTable (const String& tableName, 
			  Table::TableOption access) :
  itsPJonesDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName          const String&         Cal table name
//    access             Table::TableOption    Access option
// Output to private data:
//    itsPJonesDesc      PJonesDesc            PJones table descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New && access == Table::NewNoReplace &&
      access == Table::Scratch) {
    createCalTable (tableName, itsPJonesDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------

CJonesTable::CJonesTable() : TimeVarVisJonesTable(), itsCJonesDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsCJonesDesc      CJonesDesc           CJones table descriptor
//
};

//----------------------------------------------------------------------------

CJonesTable::~CJonesTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

CJonesTable::CJonesTable (const String& tableName, 
			  Table::TableOption access) :
  itsCJonesDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName           const String&         Cal table name
//    access              Table::TableOption    Access option
// Output to private data:
//    itsCJonesDesc       CJonesDesc            CJones table descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New && access == Table::NewNoReplace &&
      access == Table::Scratch) {
    createCalTable (tableName, itsCJonesDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------






} //# NAMESPACE CASA - END

