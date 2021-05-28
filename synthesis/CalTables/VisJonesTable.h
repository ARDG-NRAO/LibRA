//# VisJonesTable.h: VisJones calibration table access and creation
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

#ifndef CALIBRATION_VISJONESTABLE_H
#define CALIBRATION_VISJONESTABLE_H

#include <synthesis/CalTables/CalTable.h>
#include <synthesis/CalTables/VisJonesDesc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// VisJonesTable: VisJones calibration table access and creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="VisJonesDesc">VisJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "visibility", "Jones matrix", and "table".
// </etymology>
//
// <synopsis>
// The VisJonesTable classes provide basic access to VisJones 
// calibration tables. This includes the creation of calibration 
// tables, sorting and selection capabilities, and basic data access. 
// Specializations for time-variable, solvable VisJones sub-types 
// are provided through inheritance. This class inherits from CalTable.
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

class VisJonesTable : public CalTable
{
 public:
   // Default null constructor, and destructor
   VisJonesTable();
   virtual ~VisJonesTable();

   // Construct from a specified table name and table access option. 
   // Used for creating new tables or opening existing tables.
   VisJonesTable (const casacore::String& tableName,
		  casacore::Table::TableOption access = casacore::Table::Old);

 private:
   VisJonesDesc itsVisJonesDesc;
 };


} //# NAMESPACE CASA - END

#endif
   




