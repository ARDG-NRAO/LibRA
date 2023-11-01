//# VLAFilterSet.h: 
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

#ifndef NRAO_VLAFILTERSET_H
#define NRAO_VLAFILTERSET_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Containers/Block.h>

#include <casacore/casa/namespace.h>
class VLALogicalRecord;
class VLAFilter;

// <summary></summary>

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

class VLAFilterSet
{
public:
  // The default constructor creates a filter set that does not contan any
  // filters. Using the passThru function without any filters always returns
  // true;
  VLAFilterSet();
  
  // The copy constructor uses copy semantics.
  VLAFilterSet(const VLAFilterSet& other);
  
  // The destructor is trivial
  virtual ~VLAFilterSet();
  
  // The assignment operator uses copy semantics.
  VLAFilterSet& operator=(const VLAFilterSet& other);
  
  // Adds the specified filter to the set.
  void addFilter(const VLAFilter& filter);
  
  // Adds the specified filter from the set. Throws an exception if which is
  // not less than the value returned by nelements().
  void removeFilter(const casacore::uInt which);

  // Returns the number of filters in the set.
  casacore::uInt nelements() const;

  // Returns a reference to the specified filter
  const VLAFilter& filter(const casacore::uInt which) const;

  // returns true if the supplied record meets the filter criteria for all the
  // filters that are contained within in this filter set. Also returns true if
  // there are no filters in the set.
  virtual casacore::Bool passThru(const VLALogicalRecord& record) const;
  
  // casacore::Function which checks the internal data of this class for correct
  // dimensionality and consistant values. Returns true if everything is fine
  // otherwise returns false.
  virtual casacore::Bool ok() const;
  
private:
  //# deletes all the filters in the current filter set.
  void deleteAllFilters();
  //# copies all the filters from the specified set to the current one.
  void copyFilters(const VLAFilterSet& other);

  casacore::PtrBlock<VLAFilter*> itsFilters;
};

inline casacore::uInt VLAFilterSet::nelements() const {
  return itsFilters.nelements();
}

#endif
