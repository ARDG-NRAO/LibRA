//# SolvableVJTable.h: SolvableVisJones calibration table access and creation
//# Copyright (C) 1996,1997,1998,1999,2001,2003
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

#ifndef CALIBRATION_SOLVABLEVJTABLE_H
#define CALIBRATION_SOLVABLEVJTABLE_H

#include <synthesis/CalTables/TimeVarVJTable.h>
#include <synthesis/CalTables/SolvableVJDesc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// SolvableVisJonesTable: SolvableVisJones calibration table access/creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="SolvableVisJonesDesc">SolvableVisJonesDesc</linkto>
// </prerequisite>
//
// <etymology>
// From "solvable","visibility", "Jones matrix", and "table".
// </etymology>
//
// <synopsis>
// The SolvableVisJonesTable classes provide basic access to time-variable,
// solvable VisJones calibration tables. This includes the 
// creation of calibration tables, sorting and selection capabilities, 
// and basic data access. This class derives from TimeVarVisJonesTable.
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

class SolvableVisJonesTable : public TimeVarVisJonesTable
{
 public:
   // Default null constructor, and destructor
   SolvableVisJonesTable();
   virtual ~SolvableVisJonesTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   SolvableVisJonesTable (const casacore::String& tableName, 
			 casacore::Table::TableOption access = casacore::Table::Old);

   SolvableVisJonesTable (const casacore::String& tableName, 
			  const casacore::String& type,
			 casacore::Table::TableOption access = casacore::Table::Old);

 private:
   SolvableVisJonesDesc itsSolvableVisJonesDesc;
};

// <summary> 
// GJonesTable: GJones calibration table access and creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="GJonesDesc">GJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "gain", "Jones matrix", and "table".
// </etymology>
//
// <synopsis>
// The GJonesTable classes provide basic access to time-variable,
// solvable electronic gain calibration tables. This includes 
// the creation of calibration tables, sorting and selection 
// capabilities, and basic data access. This class derives from 
// SolvableVisJonesTable.
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

class GJonesTable : public SolvableVisJonesTable
{
 public:
   // Default null constructor, and destructor
   GJonesTable();
   virtual ~GJonesTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   GJonesTable (const casacore::String& tableName, 
		casacore::Table::TableOption access = casacore::Table::Old);

 private:
   GJonesDesc itsGJonesDesc;
};

// <summary> 
// GJonesDelayRateSBTable: GJonesDelayRateSB calibration table access and creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="GJonesDelayRateSBDesc">GJonesDelayRateSBDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "gain", "Jones matrix", and "table".
// </etymology>
//
// <synopsis>
// The GJonesDelayRateSBTable classes provide basic access to time-variable,
// solvable electronic gain calibration tables. This includes 
// the creation of calibration tables, sorting and selection 
// capabilities, and basic data access. This class derives from 
// SolvableVisJonesTable.
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

class GJonesDelayRateSBTable : public SolvableVisJonesTable
{
 public:
   // Default null constructor, and destructor
   GJonesDelayRateSBTable();
   virtual ~GJonesDelayRateSBTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   GJonesDelayRateSBTable (const casacore::String& tableName, 
			   casacore::Table::TableOption access = casacore::Table::Old);

 private:
   GJonesDelayRateSBDesc itsGJonesDelayRateSBDesc;
};

// <summary> 
// DJonesTable: DJones calibration table access and creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="DJonesDesc">DJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "DJones matrix", and "table".
// </etymology>
//
// <synopsis>
// The DJonesTable classes provide basic access to time-variable,
// solvable polarization response calibration tables. This 
// includes the creation of calibration tables, sorting and selection 
// capabilities, and basic data access. This class derives from 
// SolvableVisJonesTable.
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

class DJonesTable : public SolvableVisJonesTable
{
 public:
   // Default null constructor, and destructor
   DJonesTable();
   virtual ~DJonesTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   DJonesTable (const casacore::String& tableName, 
		casacore::Table::TableOption access = casacore::Table::Old);

 private:
   DJonesDesc itsDJonesDesc;
};

// <summary> 
// TJonesTable: TJones calibration table access and creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="TJonesDesc">TJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "TJones matrix", and "table".
// </etymology>
//
// <synopsis>
// The TJonesTable classes provide basic access to time-variable,
// solvable atmosphere calibration tables. This includes the 
// creation of calibration tables, sorting and selection 
// capabilities, and basic data access. This class derives from 
// SolvableVisJonesTable.
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

class TJonesTable : public SolvableVisJonesTable
{
 public:
   // Default null constructor, and destructor
   TJonesTable();
   virtual ~TJonesTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   TJonesTable (const casacore::String& tableName, 
		casacore::Table::TableOption access = casacore::Table::Old);

 private:
   TJonesDesc itsTJonesDesc;
};


} //# NAMESPACE CASA - END

#endif
   




