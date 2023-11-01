//# BJonesTable.h: BJones calibration table access and creation
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

#ifndef CALIBRATION_BJONESTABLE_H
#define CALIBRATION_BJONESTABLE_H

#include <synthesis/CalTables/SolvableVJTable.h>
#include <synthesis/CalTables/BJonesDesc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// BJonesTable: BJones calibration table access/creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="BJonesDesc">BJonesDesc</linkto>
// </prerequisite>
//
// <etymology>
// From "solvable","visibility", "Jones matrix", and "table".
// </etymology>
//
// <synopsis>
// The BJonesTable classes provide basic access to BJones calibration
// tables. This includes the creation of calibration tables, 
// sorting and selection capabilities, and basic data access. This 
// class derives from SolvableVisJonesTable.
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
// <todo asof="01/06/20">
// (i) 
// </todo>

class BJonesTable : public SolvableVisJonesTable
{
 public:
   // Default null constructor, and destructor
   BJonesTable();
   virtual ~BJonesTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   BJonesTable (const casacore::String& tableName, 
		casacore::Table::TableOption access = casacore::Table::Old);

 private:
   BJonesDesc itsBJonesDesc;
};


// <summary> 
// BJonesPolyTable: BJonesPoly calibration table access/creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="BJonesPolyDesc">BJonesPolyDesc</linkto>
// </prerequisite>
//
// <etymology>
// From "B Jones matrix", "polynomial" and "table".
// </etymology>
//
// <synopsis>
// The BJonesPolyTable classes provide basic access to BJonesPoly 
// calibration tables. This includes the creation of calibration tables, 
// sorting and selection capabilities, and basic data access. This 
// class derives from BJonesTable.
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
// <todo asof="01/06/20">
// (i) 
// </todo>

class BJonesPolyTable : public BJonesTable
{
 public:
   // Default null constructor, and destructor
   BJonesPolyTable();
   virtual ~BJonesPolyTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   BJonesPolyTable (const casacore::String& tableName, 
		    casacore::Table::TableOption access = casacore::Table::Old);

 private:
   BJonesPolyDesc itsBJonesPolyDesc;
};


} //# NAMESPACE CASA - END

#endif
   




