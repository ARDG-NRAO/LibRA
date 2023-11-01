//# TJonesTable.cc: Implementation of TJonesTable.h
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

#include <synthesis/CalTables/TJonesTable.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

TJonesPolyTable::TJonesPolyTable() : TJonesTable(), itsTJonesPolyDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsTJonesPolyDesc   TJonesPolyDesc     TJonesPoly table descriptor
//
};

//----------------------------------------------------------------------------

TJonesPolyTable::~TJonesPolyTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

TJonesPolyTable::TJonesPolyTable (const String& tableName, 
				  Table::TableOption access) :
  itsTJonesPolyDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName            const String&         Cal table name
//    access               Table::TableOption    Access option
// Output to private data:
//    itsTJonesPolyDesc    TJonesPolyDesc        TJonesPoly table descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New || access == Table::NewNoReplace ||
      access == Table::Scratch) {
    createCalTable (tableName, itsTJonesPolyDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------

TJonesSplineTable::TJonesSplineTable() : 
  TJonesPolyTable(), 
  itsTJonesSplineDesc()
{
// Default null constructor for calibration table; do nothing for now
// Output to private data:
//    itsTJonesSplineDesc   TJonesSplineDesc     TJonesSpline table descriptor
//
};

//----------------------------------------------------------------------------

TJonesSplineTable::~TJonesSplineTable()
{
// Default destructor
//
};

//----------------------------------------------------------------------------

TJonesSplineTable::TJonesSplineTable (const String& tableName, 
				      Table::TableOption access) :
  itsTJonesSplineDesc()
{
// Construct from a time-variable VisJones cal table name and access option. 
// Used for creating new tables or opening an existing table.
// Input:
//    tableName            const String&         Cal table name
//    access               Table::TableOption    Access option
// Output to private data:
//    itsTJonesSplineDesc  TJonesSplineDesc      TJonesSpline table descriptor
//
  // Is this a new or existing calibration table ?
  if (access == Table::New || access == Table::NewNoReplace ||
      access == Table::Scratch) {
    createCalTable (tableName, itsTJonesSplineDesc, access);
  } else {
    openCalTable (tableName, access);
  };
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

