//# CalIterBase.h: Base class for calibration table iterators
//# Copyright (C) 1996,1997,1998,2001,2002,2003
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

#ifndef CALIBRATION_CALITERBASE_H
#define CALIBRATION_CALITERBASE_H

#include <casa/aips.h>
#include <synthesis/CalTables/CalTable.h>


namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CalIterBase: Base class for calibration table iterators
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTable">CalTable</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration table iterator" and "base class"
// </etymology>
//
// <synopsis>
// The CalIterBase class is an abstract base class defining
// the common fundamental interface for different types of
// calibration table iterators. Specializations include
// sequential and indexed iterators.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Provide an abstract base class for all calibration table iterators
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class CalIterBase 
{
 public:
  // Default destructor
  virtual ~CalIterBase() {};

  // Current iteration sub-table
  virtual CalTable& subTable() = 0;

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  CalIterBase() {};

 private:

};


} //# NAMESPACE CASA - END

#endif
   
  



