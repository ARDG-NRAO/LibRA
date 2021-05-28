//# GJonesTable.h: GJones calibration table access and creation
//# Copyright (C) 1996,1997,1998,1999,2001,2002,2003
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

#ifndef CALIBRATION_GJONESTABLE_H
#define CALIBRATION_GJONESTABLE_H

#include <synthesis/CalTables/SolvableVJTable.h>
#include <synthesis/CalTables/GJonesDesc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// GJonesPolyTable: GJonesPoly calibration table access/creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="GJonesPolyDesc">GJonesPolyDesc</linkto>
// </prerequisite>
//
// <etymology>
// From "G Jones matrix", "polynomial" and "table".
// </etymology>
//
// <synopsis>
// The GJonesPolyTable classes provide basic access to GJonesPoly 
// calibration tables. This includes the creation of calibration tables, 
// sorting and selection capabilities, and basic data access. This 
// class derives from GJonesTable.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// This class is used by other calibration table accessors and iterators.
// </motivation>
//
// <todo asof="02/02/01">
// (i) 
// </todo>

class GJonesPolyTable : public GJonesTable
{
 public:
   // Default null constructor, and destructor
   GJonesPolyTable();
   virtual ~GJonesPolyTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   GJonesPolyTable (const casacore::String& tableName, 
		      casacore::Table::TableOption access = casacore::Table::Old);

 private:
   GJonesPolyDesc itsGJonesPolyDesc;
};

// <summary> 
// GJonesSplineTable: GJonesSpline calibration table access/creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="GJonesSplineDesc">GJonesSplineDesc</linkto>
// </prerequisite>
//
// <etymology>
// From "G Jones matrix", "spline polynomial" and "table".
// </etymology>
//
// <synopsis>
// The GJonesSplineTable classes provide basic access to GJonesSpline 
// calibration tables. This includes the creation of calibration tables, 
// sorting and selection capabilities, and basic data access. This 
// class derives from GJonesPolyTable.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// This class is used by other calibration table accessors and iterators.
// </motivation>
//
// <todo asof="02/02/01">
// (i) 
// </todo>

class GJonesSplineTable : public GJonesPolyTable
{
 public:
   // Default null constructor, and destructor
   GJonesSplineTable();
   virtual ~GJonesSplineTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   GJonesSplineTable (const casacore::String& tableName, 
		      casacore::Table::TableOption access = casacore::Table::Old);

 private:
   GJonesSplineDesc itsGJonesSplineDesc;
};


} //# NAMESPACE CASA - END

#endif
   




