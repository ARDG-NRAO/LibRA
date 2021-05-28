//# VLASpectralLineRecord.cc:
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
//# $Id$

#include <nrao/VLA/VLASpectralLineRecord.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicMath/Math.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>

#include <casa/iomanip.h>

VLASpectralLineRecord::VLASpectralLineRecord()
  :VLABaselineRecord(),
   itsNchan(0)
{
}

VLASpectralLineRecord::
VLASpectralLineRecord(ByteSource& record, uInt offset, uInt nChan)
  :VLABaselineRecord(record, offset),
   itsNchan(nChan)
{
  DebugAssert(itsNchan != 0, AipsError);
}

VLASpectralLineRecord::~VLASpectralLineRecord() {
}

void VLASpectralLineRecord::
attach(ByteSource& record, uInt offset, uInt nChan) {
  VLABaselineRecord::attach(record, offset);
  DebugAssert(nChan != 0, AipsError);
  itsNchan = nChan;
}

Vector<Complex> VLASpectralLineRecord::data() const {
  const uInt trueChan = itsNchan-1;
  Vector<Complex> v(trueChan);
  data(v);
  return v;
}

void VLASpectralLineRecord::data(Array<Complex>& values) const {
  const uInt trueChan = itsNchan-1;
  if (values.nelements() == 0) {
    values.resize(IPosition(1,trueChan));
  } else {
    DebugAssert(values.nelements() == trueChan, AipsError);
  }

  const Float fscale = scale();
  const Int64 where = itsOffset + 2*((itsNchan+15)/16 + 4);
  itsRecord.seek(where);
  Bool isACopy;
  Complex* dataPtr = values.getStorage(isACopy);
  Complex* curDataPtr = dataPtr + trueChan;
  uInt n = 2*trueChan;
  Short *idata = new Short[n];
  itsRecord.read(n, idata);
#if defined(AIPS_DEBUG)
  Short max = 0;
#endif
  while (n > 0) {
    n--;
    curDataPtr--;
#if defined(AIPS_DEBUG)
    Short absData = abs(idata[n]);
    if (absData > max) max = absData;
#endif
    Float tmpval = Float(idata[n])/fscale;
    n--;
#if defined(AIPS_DEBUG)
    absData = abs(idata[n]);
    if (absData > max) max = absData;
#endif
    *curDataPtr = Complex (Float(idata[n])/fscale, tmpval);
  }
  DebugAssert(curDataPtr == dataPtr, AipsError);
  values.putStorage(dataPtr, isACopy);
#if defined(AIPS_DEBUG)
  AlwaysAssert(max >= 16384, AipsError);
#endif
  delete [] idata;
}

Vector<Bool> VLASpectralLineRecord::flags() const {
  const uInt trueChan = itsNchan-1;
  Vector<Bool> flags(trueChan, false);
//   Bool isACopy;
//   Bool* dataPtr = flags.getStorage(isACopy);
//   const Int64 where = itsOffset;
//   itsRecord.seek(where);
//   itsRecord.read(itsNchan, dataPtr);
//   flags.putStorage(dataPtr, isACopy);
// //   const uInt n = 2*((itsNchan+15)/16 + 2);
// //   uChar bytes[n];
// //   itsRecord.seek(itsOffset);
// //   itsRecord.read(n, bytes);
// //   cout << "Header bytes from a spectral line record. [" << setbase(16);
// //   for (uInt i = 0; i < n; i++) {
// //     cout << Int(bytes[i]) << ":";
// //   }
// //   cout << "]" << setbase(10) << endl;
  return flags;
}

uInt VLASpectralLineRecord::scale() const {
  const uInt offset = 2*((itsNchan+15)/16);
  return VLABaselineRecord::scale(offset);
}

uInt VLASpectralLineRecord::ant1() const {
  const uInt offset = 2*((itsNchan+15)/16);
  return VLABaselineRecord::ant1(offset + 2u);
}

uInt VLASpectralLineRecord::ant2() const {
  const uInt offset = 2*((itsNchan+15)/16);
  return VLABaselineRecord::ant2(offset + 2u);
}
  
VLABaselineRecord::Type VLASpectralLineRecord::type() const {
  return VLABaselineRecord::SPECTRALLINE;
}
// Local Variables: 
// compile-command: "gmake VLASpectralLineRecord"
// End: 
