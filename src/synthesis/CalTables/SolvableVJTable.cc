//# SolvableVJTable.cc: Implementation of SolvableVJTable.h
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

#include <synthesis/CalTables/SolvableVJTable.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

SolvableVisJonesTable::SolvableVisJonesTable() : TimeVarVisJonesTable(),
						 itsSolvableVisJonesDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsSolvableVisJonesDesc   SolvableVisJonesDesc  SolvableVisJones table 
//                                                    descriptor
//
};

//----------------------------------------------------------------------------

SolvableVisJonesTable::~SolvableVisJonesTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

SolvableVisJonesTable::SolvableVisJonesTable (const String& tableName, 
					      Table::TableOption access) :
  itsSolvableVisJonesDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName                const String&         Cal table name
//    access                   Table::TableOption    Access option
// Output to private data:
//    itsSolvableVisJonesDesc  SolvableVisJonesDesc  SolvableVisJones table 
//                                                   descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New || access == Table::NewNoReplace ||
      access == Table::Scratch) {
    createCalTable (tableName, itsSolvableVisJonesDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------

SolvableVisJonesTable::SolvableVisJonesTable (const String& tableName, 
					      const String& type,
					      Table::TableOption access) :
  itsSolvableVisJonesDesc(type)
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName                const String&         Cal table name
//    access                   Table::TableOption    Access option
// Output to private data:
//    itsSolvableVisJonesDesc  SolvableVisJonesDesc  SolvableVisJones table 
//                                                   descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New || access == Table::NewNoReplace ||
      access == Table::Scratch) {
    createCalTable (tableName, itsSolvableVisJonesDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};


//----------------------------------------------------------------------------

GJonesTable::GJonesTable() : SolvableVisJonesTable(), itsGJonesDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsGJonesDesc      GJonesDesc         GJones table descriptor
//
};

//----------------------------------------------------------------------------

GJonesTable::~GJonesTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

GJonesTable::GJonesTable (const String& tableName, 
			  Table::TableOption access) :
  itsGJonesDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName          const String&         Cal table name
//    access             Table::TableOption    Access option
// Output to private data:
//    itsGJonesDesc      GJonesDesc            GJones table descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New && access == Table::NewNoReplace &&
      access == Table::Scratch) {
    createCalTable (tableName, itsGJonesDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------

GJonesDelayRateSBTable::GJonesDelayRateSBTable() : SolvableVisJonesTable(),
						   itsGJonesDelayRateSBDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsGJonesDelayRateSBDesc   GJonesDelayRateSBDesc   GJonesDelayRateSB 
//                                                       table descriptor
//
};

//----------------------------------------------------------------------------

GJonesDelayRateSBTable::~GJonesDelayRateSBTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

GJonesDelayRateSBTable::GJonesDelayRateSBTable (const String& tableName, 
						Table::TableOption access) :
  itsGJonesDelayRateSBDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName                  const String&          Cal table name
//    access                     Table::TableOption     Access option
// Output to private data:
//    itsGJonesDelayRateSBDesc   GJonesDelayRateSBDesc  GJonesDelayRateSB 
//                                                      table descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New && access == Table::NewNoReplace &&
      access == Table::Scratch) {
    createCalTable (tableName, itsGJonesDelayRateSBDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------

DJonesTable::DJonesTable() : SolvableVisJonesTable(), itsDJonesDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsDJonesDesc      DJonesDesc         DJones table descriptor
//
};

//----------------------------------------------------------------------------

DJonesTable::~DJonesTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

DJonesTable::DJonesTable (const String& tableName, 
			  Table::TableOption access) :
  itsDJonesDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName          const String&         Cal table name
//    access             Table::TableOption    Access option
// Output to private data:
//    itsDJonesDesc      DJonesDesc            DJones table descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New && access == Table::NewNoReplace &&
      access == Table::Scratch) {
    createCalTable (tableName, itsDJonesDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------

TJonesTable::TJonesTable() : SolvableVisJonesTable(), itsTJonesDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsTJonesDesc      TJonesDesc         TJones table descriptor
//
};

//----------------------------------------------------------------------------

TJonesTable::~TJonesTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

TJonesTable::TJonesTable (const String& tableName, 
			  Table::TableOption access) :
  itsTJonesDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName          const String&         Cal table name
//    access             Table::TableOption    Access option
// Output to private data:
//    itsTJonesDesc      TJonesDesc            TJones table descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New && access == Table::NewNoReplace &&
      access == Table::Scratch) {
    createCalTable (tableName, itsTJonesDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

