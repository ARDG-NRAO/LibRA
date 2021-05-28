//# VLABaselineRecord.cc:  this defines VLABaselineRecord.cc
//# Copyright (C) 1997,1998,1999,2001
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
//# $Id$

#include <nrao/VLA/VLABaselineRecord.h>
#include <casa/Arrays/Vector.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>
#include <casa/BasicSL/String.h>
#include <casa/iomanip.h>

VLABaselineRecord::VLABaselineRecord()
  :itsRecord(),
   itsOffset(0)
{
}

VLABaselineRecord::VLABaselineRecord(ByteSource& record, uInt offset)
  :itsRecord(record),
   itsOffset(offset)
{
  DebugAssert(record.isReadable(), AipsError);
  DebugAssert(record.isSeekable(), AipsError);
  DebugAssert(offset != 0, AipsError);
}

VLABaselineRecord::~VLABaselineRecord() {
}

void VLABaselineRecord::attach(ByteSource& record, uInt offset) {
  DebugAssert(record.isReadable(), AipsError);
  DebugAssert(record.isSeekable(), AipsError);
  itsRecord = record;
  DebugAssert(offset != 0, AipsError);
  itsOffset = offset;
}

Vector<Complex> VLABaselineRecord::data() const {
  Vector<Complex> v;
  data(v);
  return v;
}

String VLABaselineRecord::name(VLABaselineRecord::Type typeEnum) {
  switch (typeEnum) {
  case VLABaselineRecord::CONTINUUM: 
    return "continuum";
  case VLABaselineRecord::SPECTRALLINE:
    return "spectral line";
  default:
    return "unknown";
  };
}

VLABaselineRecord::Type VLABaselineRecord::type(const String& typeName) {
  String canonicalCase(typeName);
  canonicalCase.downcase();
  VLABaselineRecord::Type t;
  for (uInt i = 0; i < NUMBER_TYPES; i++) {
    t = (VLABaselineRecord::Type) i;
    if (canonicalCase.matches(VLABaselineRecord::name(t))) {
      return t;
    }
  }
  return VLABaselineRecord::UNKNOWN_TYPE;
}

uInt VLABaselineRecord::scale(uInt headerOffset) const {
  const Int64 where = itsOffset + headerOffset + 1;
  itsRecord.seek(where);
  Char exponent;
  itsRecord >> exponent;
  // The exponent can sometimes be negative (after April 9 1997) - Another
  // undocumented feature.
  //  DebugAssert((exponent & 0x1F) == exponent, AipsError);
  //  exponent &= 0x1F;
  // cout << "exponent: " << setbase(10) << Int(exponent) << endl;
  // DONT ASK WHERE THE 8 COMES FROM ITS **UNDOCUMENTED** EXCEPT IN THE SOURCE
  // CODE FOR THE AIPS TASK FILLM!!!
  uInt scale = 1 << (exponent+8);
  DebugAssert(scale != 0, AipsError);
  return scale;
}

uInt VLABaselineRecord::ant1(uInt headerOffset) const {
  const Int64 where = itsOffset + headerOffset;
  itsRecord.seek(where);
  uChar byte[2]; // read as two bytes to defeat byte swapping
  itsRecord.read(2, byte);
  byte[0] &= 0xe0;
  byte[0] >>= 5;
  byte[1] &= 0x03;
  byte[1] <<= 2;
  uInt ant1 = byte[1] | byte[0];
  DebugAssert(ant1 < 32, AipsError);
  return ant1;
}

uInt VLABaselineRecord::ant2(uInt headerOffset) const {
  const Int64 where = itsOffset + headerOffset + 1;
  itsRecord.seek(where);
  uChar byte;
  itsRecord >> byte;
  uInt ant2 = byte & 0x1f;
  DebugAssert(ant2 < 32, AipsError);
  return ant2;
}
  
// Local Variables: 
// compile-command: "gmake VLABaselineRecord"
// End: 
