//# VisJonesMCol.h: VisJones cal_main table column access
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

#ifndef CALIBRATION_VISJONESMCOL_H
#define CALIBRATION_VISJONESMCOL_H

#include <synthesis/CalTables/CalMainColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// ROVisJonesMCol: Read-only VisJones cal_main calibration table column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only", "visibility Jones", "cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The ROVisJonesMCol class allows read-only access to columns in the
// VisJones main calibration table. Specializations for baseline-based,
// time-variable and solvable VisJones calibration table record types
// are provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class ROVisJonesMCol : public ROCalMainColumns
{
 public:
  // Construct from a calibration table
  ROVisJonesMCol (const CalTable& calTable);

  // Default destructor
  virtual ~ROVisJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROVisJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROVisJonesMCol (const ROVisJonesMCol&);
  ROVisJonesMCol& operator= (const ROVisJonesMCol&);
};

// <summary> 
// VisJonesMCol: Read-write VisJones cal_main calibration table column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "visibility Jones", "calibration main table" and "columns".
// </etymology>
//
// <synopsis>
// The VisJonesMCol class allows read-write access to columns in the
// VisJones main calibration table. Specializations for baseline-based,
// time-variable and solvable VisJones calibration table record types
// are provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class VisJonesMCol : public CalMainColumns
{
 public:
  // Construct from a calibration table
  VisJonesMCol (CalTable& calTable);

  // Default destructor
  virtual ~VisJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  VisJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  VisJonesMCol (const VisJonesMCol&);
  VisJonesMCol& operator= (const VisJonesMCol&);

};


} //# NAMESPACE CASA - END

#endif
   
  



