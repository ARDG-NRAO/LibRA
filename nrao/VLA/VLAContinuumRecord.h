//# VLAContinuumRecord.h: 
//# Copyright (C) 1999
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

#ifndef NRAO_VLACONTINUUMRECORD_H
#define NRAO_VLACONTINUUMRECORD_H

#include <casa/aips.h>
#include <nrao/VLA/VLABaselineRecord.h>
#include <casa/IO/ByteSource.h>
//# The following should be a forward declaration. But our casacore::Complex & DComplex
//# classes are a typedef hence this does not work. Replace the following with
//# forward declarations when casacore::Complex and casacore::DComplex are no longer typedefs.
#include <casa/BasicSL/Complex.h>

#include <casa/namespace.h>

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

class VLAContinuumRecord: public VLABaselineRecord
{
public:
  // The default constructor creates a unusable object. Use the attach function
  // to turn an object constructed this way into a usable object.
  VLAContinuumRecord();

  // Initialise this ContinuumRecord from the given record with the
  // specified offset.
  VLAContinuumRecord(casacore::ByteSource& record, casacore::uInt offset);

  // The destructor is trivial
  virtual ~VLAContinuumRecord();

  // Initialise this VLAContinuumRecord from the given BytesSource (which
  // must contain a VLA logical record), and must be readable & seekable. The
  // offset specifies where the record starts.
  void attach(casacore::ByteSource& record, casacore::uInt offset);

  // returns the correlations for this baseline. This will be a casacore::Vector with
  // four elements containing the AA, CC, AC & CA (or BB, DD, BD, DB)
  // correlations (ie all four polarisations). The returned casacore::Vector has been
  // scaled.
  virtual casacore::Vector<casacore::Complex> data() const;

  // Returns the correlations for this baseline in the supplied Array. The
  // supplied casacore::Array may have any number of dimensions, but all except one must
  // have a length on 1. The axis that is not of length one must be the right
  // length to hold all the data contained in the record.  Alternatively the
  // casacore::Array may contain no elements in which case it will be resized to a Vector
  // of the correct length. The returned values have been scaled. 

  // The returned casacore::Array has four elements containing the AA, CC, AC & CA (or
  // BB, DD, BD, DB) correlations (ie all four polarisations).
  virtual void data(casacore::Array<casacore::Complex>& values) const;

  // returns the flags for the correlations for this baseline. The returned
  // casacore::Vector will always have a length of four.
  virtual casacore::Vector<casacore::Bool> flags() const;

  // returns the variance of the correlations for this baseline. The returned
  // casacore::Vector will always have a length of four.
  casacore::Vector<casacore::Float> variance() const;

  // returns the scaling factor used to convert the internal data to the actual
  // values.
  virtual casacore::uInt scale() const;

  // returns the antenna 1 index for this baseline.
  virtual casacore::uInt ant1() const;

  // returns the antenna 2 index for this baseline.
  virtual casacore::uInt ant2() const;
  
  // Return the type of this record (ie., CONTINUUM).
  virtual VLABaselineRecord::Type type() const;

private:
  //# The copy constructor is private and undefined.
  VLAContinuumRecord(const VLAContinuumRecord& other);

  //# The assignment constructor is private and undefined
  VLAContinuumRecord& operator=(const VLAContinuumRecord& other);

};
#endif
