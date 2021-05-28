//# VLARCA: 
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

#ifndef NRAO_VLARCA_H
#define NRAO_VLARCA_H

#include <casa/aips.h>
#include <casa/IO/ByteSource.h>

#include <casa/namespace.h>
//# Forward Declarations

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

class VLARCA
{
public:
  // The default constructor creates an unusable object
  VLARCA();

  // Initialise this RCA from the given BytesSource (which must contain a VLA
  // logical record), and must be readable & seekable.
  VLARCA(casacore::ByteSource& record);

  // The copy constructor uses copy semantics
  VLARCA(const VLARCA& other);

  // The destructor is trivial;
  ~VLARCA();

  // The assignment constructor uses copy semantics
  VLARCA& operator=(const VLARCA& other);
    
  // attach to the specified casacore::ByteSource 
  void attach(casacore::ByteSource& record);
  
  // return the length of this logical record
  casacore::uInt length() const;
  
  // return the revision of this logical record
  casacore::uInt revision() const;
  
  // return the position (in bytes) of the start of the SDA, from the
  // beginning of the logical record.
  casacore::uInt SDAOffset() const;
  
  // return the position (in bytes) of the start of the specified ADA, from the
  // beginning of the logical record.
  casacore::uInt ADAOffset(casacore::uInt which) const;
  
  // return the number of antennas in the sub-array that this logical record
  // contains data for.
  casacore::uInt nAntennas() const;
  
  // return the position (in bytes) of the start of the specified CDA, from the
  // beginning of the logical record.
  casacore::uInt CDAOffset(casacore::uInt which) const;
  
  // return the number of bytes in the header of a baseline record in the
  // specified CDA
  casacore::uInt CDAHeaderBytes(casacore::uInt which) const;

  // return the number of bytes in the baseline record, including the header in
  // the specified CDA
  casacore::uInt CDABaselineBytes(casacore::uInt which) const;

  // return the time in Modified Julian Days that is the reference point for
  // the times given in this record. 
  casacore::uInt obsDay() const;

private:
  //# Contains a logical record
  mutable casacore::ByteSource itsRecord;
};
#endif


