//# VLAFilter.h: 
//# Copyright (C) 1999,2000
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
//#
//# $Id$

#ifndef NRAO_VLAFILTER_H
#define NRAO_VLAFILTER_H

#include <casa/aips.h>

#include <casa/namespace.h>
class VLALogicalRecord;

// <summary>
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> some concept
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <templating arg=T>
//    <li>
//    <li>
// </templating>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="yyyy/mm/dd">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class VLAFilter
{
public:
  // A virtual destructor is necessary to ensure that the destructor of derived
  // classes is used.
  virtual ~VLAFilter();

  // Return a pointer to a copy of the derived object upcast to a
  // VLAFilter object. The class that uses this function is responsible
  // for deleting the pointer. This is used to implement a virtual copy
  // constructor.
  virtual VLAFilter* clone() const = 0;

  // returns true if the supplied record meets the filter criteria.
  virtual casacore::Bool passThru(const VLALogicalRecord& record) const = 0;

  // casacore::Function which checks the internal data of this class for correct
  // dimensionality and consistant values. Returns true if everything is fine
  // otherwise returns false.
  virtual casacore::Bool ok() const;

private:

};
#endif
