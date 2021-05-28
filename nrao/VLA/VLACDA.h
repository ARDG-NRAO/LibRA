//# VLACDA: 
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

#ifndef NRAO_VLACDA_H
#define NRAO_VLACDA_H

#include <casa/aips.h>
#include <casa/IO/ByteSource.h>
#include <casa/Containers/Block.h>
#include <nrao/VLA/VLABaselineRecord.h>

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

class VLACDA
{
public:
  // The default constructor creates a unusable object. Use the attach function
  // to turn an object constructed this way into a usable object.
  VLACDA();

  // Initialise this CDA from the given BytesSource (which must contain a VLA
  // logical record), and must be readable & seekable. The offset specifies
  // where the CDA starts in the logical record. The number of antennas and
  // channels in this CDA, as well as the size of a baseline record, 
  // must also be specified.
  VLACDA(casacore::ByteSource& record, casacore::uInt offset, casacore::uInt baselineSize, 
	 casacore::uInt nant, casacore::uInt nChan);

  // The copy constructor uses copy semantics
  VLACDA(const VLACDA& other);

  // The destructor is trivial
  ~VLACDA();

  // The assignment constructor uses copy semantics
  VLACDA& operator=(const VLACDA& other);
    
  // Initialise this CDA from the given BytesSource (which must contain a VLA
  // logical record), and must be readable & seekable. The offset specifies
  // where the CDA starts in the logical record. The number of antennas and
  // channels in this CDA, as well as the size of a baseline record, 
  // must also be specified.
  void attach(casacore::ByteSource& record, casacore::uInt offset, casacore::uInt baselineSize, 
	      casacore::uInt nant, casacore::uInt nChan);
  
  // Return true if the CDA contains valid data. This, and the copy functions,
  // will not throw an exception if the CDA does not contain data.
  casacore::Bool isValid() const;

  // return the specified baseline record corresponding to the specified
  // auto-correlation. The index must be less than the number of antennas in
  // this subarray otherwise an exception is thrown.
  const VLABaselineRecord& autoCorr(casacore::uInt which) const;

  // return the specified baseline record corresponding to the specified
  // cross-correlation. The index must be less than NANT*(NANT-1)/2 otherwise
  // an exception is thrown.
  const VLABaselineRecord& crossCorr(casacore::uInt which) const;

private: 
  //# delete all the auto-correlation baseline pointers
  void deleteACorr(casacore::uInt startIndex);
  //# delete all the cross-correlation baseline pointers
  void deleteXCorr(casacore::uInt startIndex);

  //# Contains a logical record. It is mutable because functions which do not
  //# changes its contents do change its "location"
  mutable casacore::ByteSource itsRecord;

  // the offset to the start of this CDA in the record. An offset of zero means
  // this CDA is not used in this record.
  casacore::uInt itsOffset;

  // A cached value of the size, in bytes, of a baseline record.
  casacore::uInt itsBaselineSize;

  // A cached value of the number of antennas that this CDU has data for.
  casacore::uInt itsNant;

  // A cached value of the number of channels that this CDU has data for.
  casacore::uInt itsNchan;

  //# A block containing containing "cached" baseline records for the
  //# autocorrelations. The pointers are null until the data is actually
  //# accessed.
  mutable casacore::PtrBlock<VLABaselineRecord*> itsACorr; 

  //# A block containing containing "cached" baseline records for the
  //# crosscorrelations. The pointers are null until the data is actually
  //# accessed.
  mutable casacore::PtrBlock<VLABaselineRecord*> itsXCorr; 
}; 
#endif
