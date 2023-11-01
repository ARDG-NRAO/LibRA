//# TJonesTable.h: TJones calibration table access and creation
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

#ifndef CALIBRATION_TJONESTABLE_H
#define CALIBRATION_TJONESTABLE_H

#include <synthesis/CalTables/SolvableVJTable.h>
#include <synthesis/CalTables/TJonesDesc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// TJonesPolyTable: TJonesPoly calibration table access/creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="TJonesPolyDesc">TJonesPolyDesc</linkto>
// </prerequisite>
//
// <etymology>
// From "G Jones matrix", "polynomial" and "table".
// </etymology>
//
// <synopsis>
// The TJonesPolyTable classes provide basic access to TJonesPoly 
// calibration tables. This includes the creation of calibration tables, 
// sorting and selection capabilities, and basic data access. This 
// class derives from TJonesTable.
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

class TJonesPolyTable : public TJonesTable
{
 public:
   // Default null constructor, and destructor
   TJonesPolyTable();
   virtual ~TJonesPolyTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   TJonesPolyTable (const casacore::String& tableName, 
		      casacore::Table::TableOption access = casacore::Table::Old);

 private:
   TJonesPolyDesc itsTJonesPolyDesc;
};

// <summary> 
// TJonesSplineTable: TJonesSpline calibration table access/creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="TJonesSplineDesc">TJonesSplineDesc</linkto>
// </prerequisite>
//
// <etymology>
// From "G Jones matrix", "spline polynomial" and "table".
// </etymology>
//
// <synopsis>
// The TJonesSplineTable classes provide basic access to TJonesSpline 
// calibration tables. This includes the creation of calibration tables, 
// sorting and selection capabilities, and basic data access. This 
// class derives from TJonesPolyTable.
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

class TJonesSplineTable : public TJonesPolyTable
{
 public:
   // Default null constructor, and destructor
   TJonesSplineTable();
   virtual ~TJonesSplineTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   TJonesSplineTable (const casacore::String& tableName, 
		      casacore::Table::TableOption access = casacore::Table::Old);

 private:
   TJonesSplineDesc itsTJonesSplineDesc;
};


} //# NAMESPACE CASA - END

#endif
   




