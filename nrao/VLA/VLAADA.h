//# VLAADA: 
//# Copyright (C) 1999,2000,2001
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

#ifndef NRAO_VLAADA_H
#define NRAO_VLAADA_H

#include <casa/aips.h>
#include <casa/IO/ByteSource.h>
#include <measures/Measures/Stokes.h>
#include <nrao/VLA/VLAEnum.h>

#include <casa/namespace.h>

namespace casa { //# NAMESPACE CASA - BEGIN
} //# NAMESPACE CASA - END


// <summary>
// Interprets the data in an antenna data area of a VLA logical record.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> The VLA archive format as described in VLA computer memo 188
// </prerequisite>
//
// <etymology> The antenna data is, in memo 188, frequently shortened to the
// acronym ADA. As this is specific to the VLA the name VLAADA seemed
// appropriate.
// </etymology>
//
// <synopsis> 
// This class contains functions which access and interpret the data in an
// antenna data area of a VLA logical record. Only data within one antenna data
// are is accessable with this class, and multiple instances of this class are
// needed to simultaneously look at the data in the other antenna data areas
// in a VLA logical record. 
//
// The functions in this class access the specified data in the antenna data
// area, and may convert the units and data types so something more suitable
// for general use. They also interpret the data and provide some derived
// quantities eg., the padName function.
// </synopsis>
//
// <example>
// This code fragment shows how to initialise a casacore::Block of objects of this class
// and then use these objects to determine which antenna is on which pad. It
// is assumed that the Bytesource object contains a VLA Logical record.
// <srcblock>
//   casacore::ByteSource VLARecord(...);
//   casacore::Block<VLAADA> adaBlock;
//   VLARCA rca(VLARecord);
//   const casacore::uInt nant = rca.nAntennas();
//   adaBlock.resize(nant);
//   for (casacore::uInt a = 0; a < nant; a++) {
//     adaBlock[a].attach(VLARecord, rca.ADAOffset(a));
//     cout << "Antenna " << adaBlock[a].antName() 
// 	 << " is on pad " << adaBlock[a].padName() << endl;
//   }
// </srcblock>
// </example>
//
// <motivation>
// This class was needed as part of the VLA filler application. 
// </motivation>
//
// <thrown>
// When compiled in debug mode this class does a lot of consistancy
// checking. If it detects anomolies it will throw an exception (casacore::AipsError).
// </thrown>
//
// <todo asof="1999/08/11">
//   <li> The member functions should probably be rounded out.
// </todo>

class VLAADA
{
public:
  // The default constructor creates a unusable object. You need to use the
  // assignment operator or the attach function to create a usable object.
  VLAADA();

  // Construct this object to read data from the specified VLA logical
  // record. The antenna data area that will be used begins at the specified
  // number of bytes from the beginning of the record.
  VLAADA(casacore::ByteSource& record, casacore::uInt offset);

  // The destructor is trivial.
  ~VLAADA();

  // The copy constructor uses reference semantics.
  VLAADA(const VLAADA& other);

  // The assignment constructor uses reference semantics.
  VLAADA& operator=(const VLAADA& other);
    
  // Re-initialise this object so that it now reads data from the given VLA
  // logical record.  The antenna data area that will be used begins at the
  // specified number of bytes from the beginning of the record.
  void attach(casacore::ByteSource& record, casacore::uInt offset);
  
  // return the antenna name. This is a string version of the antenna id n is a
  // one or two digit number that contains the antenna identifier (see below).
  casacore::String antName(casacore::Bool newStyle=true) const;

  // returns the antenna Id. This is a number which stays with the "steel" and
  // gets moved to new locations. It is not the pad number. It is between 1
  // and 28 inclusive.
  casacore::uInt antId() const;

  // Front end Temp...gives a clue if antenna is EVLA
  casacore::Float frontEndTemp(VLAEnum::IF which) const;
  // return the pad name that this antenna is currently located on. This is
  // obtained by knowing where the pads are and looking at the bx position of
  // the antenna (the bx position in the logical record has to be within half a
  // meter of the value given in an table). Pad names are of the form "Nn" or
  // "En" or "Wn" where n is a one or two digit number that has an approximate
  // correspondence with the distance of the pad from the centre of the array.
  casacore::String padName() const;

  // return the u coordinate in meters of this antenna at the centre of the
  // integration. 
  casacore::Double u() const;

  // return the v coordinate in meters of this antenna at the centre of the
  // integration. 
  casacore::Double v() const;

  // return the w coordinate in meters of this antenna at the centre of the
  // integration. 
  casacore::Double w() const;

  // return the Bx position in meters of this antenna, from the centre of the
  // Array.
  casacore::Double bx() const;

  // return the By position in meters of this antenna, from the centre of the
  // Array.
  casacore::Double by() const;

  // return the Bz position in meters of this antenna, from the centre of the
  // Array.
  casacore::Double bz() const;

  // return the bx, by, bz positions in meters of this antenna, from the centre
  // of the Array.
  casacore::Vector<casacore::Double> pos() const;

  // return the IF status. The status indicates how bad the data probably is
  // for the specified if and the current antenna. Values of zero indicate the
  // IF is OK, 1 is a warning, 2 means not so good, and bigger numbers, up to
  // 15 mean the IF is successively worse.
  casacore::uInt ifStatus(VLAEnum::IF which) const;

  // returns the nominal sensitivity of the specified IF. These numbers are
  // multiplied by the raw correlaton co-efficients to produce the numbers in
  // the correlation data area.
  casacore::Float nominalSensitivity(VLAEnum::IF which) const;

  // return the IF polarisation. Normally at the VLA IF's A & B measure
  // right-hand-circular polarisation, and IF's C & D measure
  // left-hand-circular polarisation. But in special circumstances this can be
  // swapped. This function returns casacore::Stokes::RCircular or casacore::Stokes::LCircular.
  casacore::Stokes::StokesTypes ifPol(VLAEnum::IF which) const;

  // Has the nominal sensitivity amplitude scaling been applied?
  //  (auto-true prior to revision 25 [exclusive])
  casacore::Bool nomSensApplied(VLAEnum::IF which, const casacore::uInt rev) const;

  // Identifies which array an antennn belongs to: VLA, EVLA, or VLBA
  casacore::String arrayName() const;

  // casacore::Function which checks the internal data of this class for consistant
  // values. Returns true if everything is fine otherwise returns false.
  casacore::Bool ok() const;

private:
  //# Contains a VLA logical record
  mutable casacore::ByteSource itsRecord;
  //# the offset to the start of this ADA in the record. An offset of zero
  //# means this ADA is not used in this record.
  casacore::uInt itsOffset;
};
#endif
