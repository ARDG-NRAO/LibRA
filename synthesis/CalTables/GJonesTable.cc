//# GJonesTable.cc: Implementation of GJonesTable.h
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#include <synthesis/CalTables/GJonesTable.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

GJonesPolyTable::GJonesPolyTable() : GJonesTable(), itsGJonesPolyDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsGJonesPolyDesc   GJonesPolyDesc     GJonesPoly table descriptor
//
};

//----------------------------------------------------------------------------

GJonesPolyTable::~GJonesPolyTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

GJonesPolyTable::GJonesPolyTable (const String& tableName, 
				  Table::TableOption access) :
  itsGJonesPolyDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName            const String&         Cal table name
//    access               Table::TableOption    Access option
// Output to private data:
//    itsGJonesPolyDesc    GJonesPolyDesc        GJonesPoly table descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New || access == Table::NewNoReplace ||
      access == Table::Scratch) {
    createCalTable (tableName, itsGJonesPolyDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------

GJonesSplineTable::GJonesSplineTable() : 
  GJonesPolyTable(), 
  itsGJonesSplineDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsGJonesSplineDesc   GJonesSplineDesc     GJonesSpline table descriptor
//
};

//----------------------------------------------------------------------------

GJonesSplineTable::~GJonesSplineTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

GJonesSplineTable::GJonesSplineTable (const String& tableName, 
				      Table::TableOption access) :
  itsGJonesSplineDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName            const String&         Cal table name
//    access               Table::TableOption    Access option
// Output to private data:
//    itsGJonesSplineDesc  GJonesSplineDesc      GJonesSpline table descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New || access == Table::NewNoReplace ||
      access == Table::Scratch) {
    createCalTable (tableName, itsGJonesSplineDesc, access);
    calMainAsTable().tableInfo().setSubType("GSPLINE");
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

