//# EPointJonesMCol.h: SolvableVisJones cal_main table column access
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

#ifndef CALIBRATION_EPOINTMCOL_H
#define CALIBRATION_EPOINTMCOL_H

#include <synthesis/CalTables/SolvableVJMCol.h>
#include <synthesis/CalTables/SolvableVJTable.h>
#include <synthesis/CalTables/EPointTable.h>

namespace casa {
class ROEPointMCol : public ROSolvableVisJonesMCol
{
 public:
  // Construct from a calibration table
  ROEPointMCol (const EPointTable& epTable);

  // Default destructor
  virtual ~ROEPointMCol() {};

  // Read-only column accessors

  const casacore::ArrayColumn<casacore::Float>& pointingOffset() const {return pointingOffset_p;}

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROEPointMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROEPointMCol (const ROEPointMCol&);
  ROEPointMCol& operator= (const ROEPointMCol&);

  // Private column accessors
  casacore::ArrayColumn<casacore::Float> pointingOffset_p;
};

// <summary> 
// EPointJonesMCol: RW SolvableVisJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "solvable visibility Jones","cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The EPointJonesMCol class allows read-write access to columns in the
// SolvableVisJones main calibration table. Specializations for individual
// solvable Jones matrix types (e.g. GJones) are provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-write access to SVJ calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class EPointMCol : public SolvableVisJonesMCol
{
 public:
  // Construct from a calibration table
  EPointMCol (EPointTable& epTable);

  // Default destructor
  virtual ~EPointMCol() {};

  // Read-write column accessors
  casacore::ArrayColumn<casacore::Float>& pointingOffset() {return pointingOffset_p;};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  EPointMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  EPointMCol (const EPointMCol&);
  EPointMCol& operator= (const EPointMCol&);

  // Private column accessors
  casacore::ArrayColumn<casacore::Float> pointingOffset_p;
};
}
#endif
