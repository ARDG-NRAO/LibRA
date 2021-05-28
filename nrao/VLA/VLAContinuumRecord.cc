//# VLAContinuumRecord.cc:
//# Copyright (C) 1999,2001
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

#include <nrao/VLA/VLAContinuumRecord.h>
#include <casa/Arrays/Vector.h>
#include <casa/IO/ByteIO.h>
#include <casa/BasicMath/Math.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>

//#include <casa/iomanip.h>

VLAContinuumRecord::VLAContinuumRecord()
  :VLABaselineRecord()
{
}

VLAContinuumRecord::VLAContinuumRecord(ByteSource& record, uInt offset)
  :VLABaselineRecord(record, offset)
{
}

VLAContinuumRecord::~VLAContinuumRecord() {
}

void VLAContinuumRecord::attach(ByteSource& record, uInt offset) {
  VLABaselineRecord::attach(record, offset);
}

Vector<Complex> VLAContinuumRecord::data() const {
  Vector<Complex> v(4);
  data(v);
  return v;
}

void VLAContinuumRecord::data(Array<Complex>& values) const {
  // Note this function slows down lots if the data is not contigious.
  if (values.nelements() == 0) {
    values.resize(IPosition(1,4));
  } else {
    DebugAssert(values.nelements() == 4, AipsError);
  }

  const Float fscale = scale();
  const Int64 where = itsOffset + 4;
  itsRecord.seek(where);
  Bool isACopy;
  Complex* dataPtr = values.getStorage(isACopy);
  Complex* curDataPtr = dataPtr;
  Short idata[2];
#if defined(AIPS_DEBUG)
  Short max = 0;
#endif
  for (uInt i = 0; i < 4; i++, curDataPtr++) {
    itsRecord.read(2, idata);
#if defined(AIPS_DEBUG)
    Short absData = abs(idata[0]);
    if (absData > max) max = absData;
    absData = abs(idata[1]);
    if (absData > max) max = absData;
#endif
    *curDataPtr = Complex (Float(idata[0])/fscale, Float(idata[1])/fscale);
    itsRecord.seek(2, ByteIO::Current); // skip over the variance;
  }
  values.putStorage(dataPtr, isACopy);
#if defined(AIPS_DEBUG)
  AlwaysAssert(max >= 16384, AipsError);
#endif
}

Vector<Bool> VLAContinuumRecord::flags() const {
  Vector<Bool> flags(4);
  Bool isACopy;
  Bool* dataPtr = flags.getStorage(isACopy);
  const Int64 where = itsOffset + 2;
  itsRecord.seek(where);
  itsRecord.read(4, dataPtr);
  flags.putStorage(dataPtr, isACopy);

//   const uInt n = 4;
//   uChar bytes[n];
//   itsRecord.seek(itsOffset);
//   itsRecord.read(n, bytes);
  //  cout << "Header bytes from a continuum record. [" << setbase(16);
//   for (uInt i = 0; i < n; i++) {
//     cout << Int(bytes[i]) << ":";
//   }
//   cout << "]" << setbase(10) << endl;
  return flags;
}

Vector<Float> VLAContinuumRecord::variance() const {
  const Float fscale = scale();
  const Int64 where = itsOffset + 8;
  itsRecord.seek(where);
  Vector<Float> var(4);
  uShort ivar;
  for (uInt i = 0; i < 4; i++) {
    itsRecord >> ivar;
    var(i) = Float(ivar)/fscale;
    itsRecord.seek(4, ByteIO::Current); // skip over the data;
  }
  return var;
}

uInt VLAContinuumRecord::scale() const {
  return VLABaselineRecord::scale(0u);
}

uInt VLAContinuumRecord::ant1() const {
  return VLABaselineRecord::ant1(2u);
}

uInt VLAContinuumRecord::ant2() const {
  return VLABaselineRecord::ant2(2u);
}
  
VLABaselineRecord::Type VLAContinuumRecord::type() const {
  return VLABaselineRecord::CONTINUUM;
}

// Local Variables: 
// compile-command: "gmake VLAContinuumRecord"
// End: 
