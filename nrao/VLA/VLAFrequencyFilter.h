//# VLAFrequencyFilter.h: 
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

#ifndef NRAO_VLAFREQUENCYFILTER_H
#define NRAO_VLAFREQUENCYFILTER_H

#include <casa/aips.h>
#include <nrao/VLA/VLAFilter.h>

#include <casa/namespace.h>
namespace casacore{

class MVFrequency;
}

namespace casa { //# NAMESPACE CASA - BEGIN
} //# NAMESPACE CASA - END

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
// Note the start and stop frequencys are the values at the center if the
// integration. 
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

class VLAFrequencyFilter: public VLAFilter
{
public:
  // The default constructor creates a filter that does not filter
  // anything. ie., the passThru() function always returns true;
  VLAFrequencyFilter();

  // Creates a filter that filters all records except those where any of the
  // observed data are within the specified band. The frequency frame is
  // assumed to be topocentric.
  VLAFrequencyFilter(const casacore::MVFrequency& centreFrequency, 
		     const casacore::MVFrequency& bandwidth);
  
  // The copy constructor uses copy semantics.
  VLAFrequencyFilter(const VLAFrequencyFilter& other);

  // The destructor is trivial
  virtual ~VLAFrequencyFilter();

  // The assignment operator uses copy semantics.
  VLAFrequencyFilter& operator=(const VLAFrequencyFilter& other);

  // set the reference frequency.
  void refFrequency(const casacore::MVFrequency& refFrequency);

  // set the bandwidth.
  void bandwidth(const casacore::MVFrequency& bandwidth);

  // returns true if the supplied record contains any data that is between the
  // specified start and end frequencies.
  virtual casacore::Bool passThru(const VLALogicalRecord& record) const;

  // Return a pointer to a copy of the VLAFrequencyFilter object upcast to a
  // VLAFilter object. The class that uses this function is responsible for
  // deleting the pointer. This is used to implement a virtual copy
  // constructor.
  virtual VLAFilter* clone() const;

  // casacore::Function which checks the internal data of this class for correct
  // dimensionality and consistant values. Returns true if everything is fine
  // otherwise returns false.
  virtual casacore::Bool ok() const;

private:
  casacore::Double itsTopEdge;
  casacore::Double itsBottomEdge;
};
#endif
// Local Variables: 
// compile-command: "gmake OPTLIB=1 VLAFrequencyFilter"
// End: 
