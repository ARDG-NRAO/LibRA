//# VLAADA.cc:
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

#include <iomanip>
#include <nrao/VLA/VLAADA.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Quanta/QC.h>
#include <casacore/casa/Quanta/Unit.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/BasicMath/Math.h>

Double ns2m;

VLAADA::VLAADA()
  :itsRecord(),
   itsOffset(0)
{
  ns2m = QC::c( ).getValue("m/ns");
  // cerr << QC::c( ) << endl;
  // cerr << C::c << endl;
  DebugAssert(ok(), AipsError);
}

VLAADA::VLAADA(ByteSource& record, uInt offset)
  :itsRecord(record),
   itsOffset(offset)
{
  DebugAssert(ok(), AipsError);
}

VLAADA::VLAADA(const VLAADA& other)
  :itsRecord(other.itsRecord),
   itsOffset(other.itsOffset)
{
  DebugAssert(ok(), AipsError);
}

VLAADA::~VLAADA()
{
  DebugAssert(ok(), AipsError);
}

VLAADA& VLAADA::operator=(const VLAADA& other) {
  if (this != &other) {
    itsRecord = other.itsRecord;
    itsOffset = other.itsOffset;
  }
  DebugAssert(ok(), AipsError);
  return *this;
}

void VLAADA::attach(ByteSource& record, uInt offset) {
  itsRecord = record;
  itsOffset = offset;
  DebugAssert(ok(), AipsError);
}

uInt VLAADA::antId() const {
  DebugAssert(ok(), AipsError);
  const Int64 where = itsOffset + 2*(0);
  itsRecord.seek(where);
  uChar id;
  itsRecord >> id;
  return id;
}

String VLAADA::antName(Bool newStyle) const {
  DebugAssert(ok(), AipsError);
  String antid=String::toString(antId());
  if(antId() < 10)
    antid=String("0")+antid;
  if(newStyle){
    Int64 where = itsOffset + 2*(1);
    itsRecord.seek(where);

    Short bits;
    itsRecord >> bits;
    // Bit 9 == fiber == evla or pie town  (0x0040 = 0000.0000.0100.0000)
    Bool fiber = (bits & 0x0040);

    if (fiber)
      if (antId()==29)
	// pie town is just an old VLA antenna in this context
	antid=String("VA")+antid;
      else
	// evla
	antid=String("EA")+antid;
	
    else
      antid=String("VA")+antid;
  
  }
  return antid;
}

Float VLAADA::frontEndTemp(VLAEnum::IF which) const {
  DebugAssert(ok(), AipsError);
  Int64 where = itsOffset+2*(48);
  switch (which) {
  case VLAEnum::IFA:
    break;
  case VLAEnum::IFB:
    where += 4;
    break;
  case VLAEnum::IFC:
    where += 8;
    break;
  case VLAEnum::IFD:
    where += 12;
    break;
  default:
    throw(AipsError("VLAADA::Unknown IF"));
  }
  itsRecord.seek(where);
  Float temp;
  itsRecord >> temp;
  return temp;
}

Double VLAADA::u() const {
  DebugAssert(ok(), AipsError);
  const Int64 where = itsOffset+2*(28);
  itsRecord.seek(where);
  Double u;
  itsRecord >> u;
  // cerr << u << " " << ns2m << " ";
  return u*ns2m;
}

Double VLAADA::v() const {
  DebugAssert(ok(), AipsError);
  const Int64 where = itsOffset+2*(30);
  itsRecord.seek(where);
  Double v;
  itsRecord >> v;
  return v*ns2m;
}
Double VLAADA::w() const {
  DebugAssert(ok(), AipsError);
  const Int64 where = itsOffset+2*(32);
  itsRecord.seek(where);
  Double w;
  itsRecord >> w;
  return w*ns2m;
}

Double VLAADA::bx() const {
  DebugAssert(ok(), AipsError);
  const Int64 where = itsOffset+2*(34);
  itsRecord.seek(where);
  Double bx;
  itsRecord >> bx;
  return bx*ns2m;
}

Double VLAADA::by() const {
  DebugAssert(ok(), AipsError);
  const Int64 where = itsOffset+2*(38);
  itsRecord.seek(where);
  Double by;
  itsRecord >> by;
  return by*ns2m;
}

Double VLAADA::bz() const {
  DebugAssert(ok(), AipsError);
  const Int64 where = itsOffset+2*(42);
  itsRecord.seek(where);
  Double bz;
  itsRecord >> bz;
  return bz*ns2m;
}

Vector<Double> VLAADA::pos() const {
  DebugAssert(ok(), AipsError);
  Vector<Double> pos(3);
  pos(0) = bx();
  pos(1) = by();
  pos(2) = bz();
  return pos;
}

String VLAADA::padName() const {
  DebugAssert(ok(), AipsError);
  const Double x = bx();
  const uInt nPad = 74;
  static Block<Double> padX(nPad);
  static Block<String> padName(nPad);
  static Bool init = false;
  if (!init) { // These are the nominal bx positions of the pads. This table
    // was extracted from the AIPS task FILLM
    padName[0] = "W1";    padX[0] = 77.   ;
    padName[1] = "W2";    padX[1] = 49.   ;
    padName[2] = "W3";    padX[2] = 96.   ;
    padName[3] = "W4";    padX[3] = 156.  ;
    padName[4] = "W5";    padX[4] = 229.  ;
    padName[5] = "W6";    padX[5] = 312.  ;
    padName[6] = "W7";    padX[6] = 406.  ;
    padName[7] = "W8";    padX[7] = 510.  ;
    padName[8] = "W9";    padX[8] = 623.  ;
    padName[9] = "W10";   padX[9] = 747.  ;
    padName[10] = "W12"; padX[10] = 1021. ;
    padName[11] = "W14"; padX[11] = 1328. ;
    padName[12] = "W16"; padX[12] = 1667. ;
    padName[13] = "W18"; padX[13] = 2041. ;
    padName[14] = "W20"; padX[14] = 2446. ;
    padName[15] = "W24"; padX[15] = 3354. ;
    padName[16] = "W28"; padX[16] = 4391. ;
    padName[17] = "W32"; padX[17] = 5470. ;
    padName[18] = "W36"; padX[18] = 6671. ;
    padName[19] = "W40"; padX[19] = 7988. ;
    padName[20] = "W48"; padX[20] = 10926.;
    padName[21] = "W56"; padX[21] = 14206.;
    padName[22] = "W64"; padX[22] = 17843.;
    padName[23] = "W72"; padX[23] = 21803.;
    padName[24] = "E1";	 padX[24] = 151.  ;
    padName[25] = "E2";	 padX[25] = 38.   ;
    padName[26] = "E3";	 padX[26] = 73.   ;
    padName[27] = "E4";	 padX[27] = 119.  ;
    padName[28] = "E5";  padX[28] = 173.  ;
    padName[29] = "E6";	 padX[29] = 236.  ;
    padName[30] = "E7";	 padX[30] = 305.  ;
    padName[31] = "E8";	 padX[31] = 382.  ;
    padName[32] = "E9";	 padX[32] = 466.  ;
    padName[33] = "E10"; padX[33] = 558.  ;
    padName[34] = "E12"; padX[34] = 765.  ;
    padName[35] = "E14"; padX[35] = 1000. ;
    padName[36] = "E16"; padX[36] = 1257. ;
    padName[37] = "E18"; padX[37] = 1548. ;
    padName[38] = "E20"; padX[38] = 1868. ;
    padName[39] = "E24"; padX[39] = 2552. ;
    padName[40] = "E28"; padX[40] = 3331. ;
    padName[41] = "E32"; padX[41] = 4180. ;
    padName[42] = "E36"; padX[42] = 5119. ;
    padName[43] = "E40"; padX[43] = 6127. ;
    padName[44] = "E48"; padX[44] = 8325. ;
    padName[45] = "E56"; padX[45] = 10814.;
    padName[46] = "E64"; padX[46] = 13620.;
    padName[47] = "E72"; padX[47] = 16204.;
    padName[48] = "N1";	 padX[48] = 2.5    ;
    padName[49] = "N2";	 padX[49] = -100.  ;
    padName[50] = "N3";	 padX[50] = -175.  ;
    padName[51] = "N4";	 padX[51] = -250.  ;
    padName[52] = "N5";  padX[52] = -362.  ;
    padName[53] = "N6";  padX[53] = -495.  ;
    padName[54] = "N7";  padX[54] = -646.  ;
    padName[55] = "N8";  padX[55] = -813.  ;
    padName[56] = "N9";  padX[56] = -995.  ;
    padName[57] = "N10"; padX[57] = -1193. ;
    padName[58] = "N12"; padX[58] = -1632. ;
    padName[59] = "N14"; padX[59] = -2126. ;
    padName[60] = "N16"; padX[60] = -2673. ;
    padName[61] = "N18"; padX[61] = -3271. ;
    padName[62] = "N20"; padX[62] = -3917. ;
    padName[63] = "N24"; padX[63] = -5539. ;
    padName[64] = "N28"; padX[64] = -6976. ;
    padName[65] = "N32"; padX[65] = -8770. ;
    padName[66] = "N36"; padX[66] = -10733.;
    padName[67] = "N40"; padX[67] = -12858.;
    padName[68] = "N48"; padX[68] = -17583.;
    padName[69] = "N56"; padX[69] = -22919.;
    padName[70] = "N64"; padX[70] = -28827.;
    padName[71] = "N72"; padX[71] = -35283.;
    padName[72] = "MPD"; padX[72] = 1148.;
    padName[73] = "VPT"; padX[73] = -46201.; // Pie Town
    for (uInt i = 0; i < nPad; i++) {
      padX[i] *= ns2m;
    }
    init = true;
  }
  String name;
  uInt i = 0;
  String arrname=arrayName();
  while (name.length() == 0 && i < nPad) {
    if (nearAbs(x, padX[i], 0.5)) { // This is nearly the largest slop
      // allowable as W1 is at 77*(0.3m/ns) and E3 is at 73.*(0.3m/ns)

      name = arrayName();
      name += padName[i];
    }
    i++;
  }
  if (name.length() == 0) {
    name = "UNKNOWN";
  }
  return name;
}

uInt VLAADA::ifStatus(VLAEnum::IF which) const {
  DebugAssert(ok(), AipsError);
  Int64 where = itsOffset+2*(3);
  if (which == VLAEnum::IFC || which == VLAEnum::IFD) {
    where++;
  }
  itsRecord.seek(where);
  uChar status;
  itsRecord >> status;
  if (which == VLAEnum::IFA || which == VLAEnum::IFC) {
    status >>= 4;
  }
  status &= 0x0f;
  return status;
}

Float VLAADA::nominalSensitivity(VLAEnum::IF which) const {
  DebugAssert(ok(), AipsError);
  Int64 where = itsOffset+2*(4);
  switch (which) {
  case VLAEnum::IFA:
    break;
  case VLAEnum::IFB:
    where += 4;
    break;
  case VLAEnum::IFC:
    where += 8;
    break;
  case VLAEnum::IFD:
    where += 12;
    break;
  default:
    throw(AipsError("VLAADA::Unknown IF"));
  }
  itsRecord.seek(where);
  Float sensitivity;
  itsRecord >> sensitivity;
  return sensitivity;
}
  
Stokes::StokesTypes VLAADA::ifPol(VLAEnum::IF which) const {
  DebugAssert(ok(), AipsError);
  Int64 where = itsOffset+2*(1);
  itsRecord.seek(where);
  uChar bits;
  itsRecord >> bits;
  Bool swap = bits & 0x80;
  if (which == VLAEnum::IFA || which == VLAEnum::IFB) {
    if (swap) {
      return Stokes::LCircular;
    } else {
      return Stokes::RCircular;
    }
  } else {
    if (swap) {
      return Stokes::RCircular;
    } else {
      return Stokes::LCircular;
    }
  }
}

// Has nominal sensitivity been applied to amplitudes?
Bool VLAADA::nomSensApplied(VLAEnum::IF which,const uInt rev) const {
  DebugAssert(ok(), AipsError);

  // Prior (exclusive) to rev 25, nominal sensitivity ALWAYS applied
  if (rev < 25) return true;

  // Note that VLA Archive Data Format (May 1, 1996) doc has typo 
  // in section 2.3.  The ADA's IF control bits are in words 64-67
  //  (not 65-68).

  Int64 where = itsOffset + 2*(64+which);
  itsRecord.seek(where);
    
  Short bits;
  itsRecord >> bits;
  // Bit 2 == Tsys Corr On  (0x2000 = 0010.0000.0000.0000)
  return (bits & 0x2000);  

}

String VLAADA::arrayName() const {
  
  // For prepending to padName

  Int64 where = itsOffset + 2*(1);
  itsRecord.seek(where);
  
  Short bits;
  itsRecord >> bits;
  // Bit 9 == fiber (0x0040 = 0100.0000.0100.0000)
  Bool fiber = (bits & 0x0040);

  // VLBA and EVLA are connected by fiber
  if (fiber) {
    // It is a VLBA antenna (e.g. PT)
    if (antId()>28)
      //      return String("VLBA:");
      return String("VLA:_");
    else
      // evla
      return String("EVLA:");
  }
  // Otherwise this is just the old VLA
  //  (the underscore is so string length same as above, for AIPS)
  return String("VLA:_");
  
}


Bool VLAADA::ok() const {
  // The LogIO class is only constructed if an Error is detected for
  // performance reasons. Both function static and file static variables
  // where considered and rejected for this purpose.

  // This function fails (dumps core) if the itsRecord data member was
  // constructed using the default constructor. I need to add a isNull
  // function to the BaseSinkSource class to overcome this
  if (!itsRecord.isNull()) {
    if (!itsRecord.isReadable()) {
      LogIO logErr(LogOrigin("VLAADA", "ok()"));
      logErr << LogIO::SEVERE 
	     << "The VLA logical record is not readable"
	     << LogIO::POST;
      return false;
    }
    if (!itsRecord.isSeekable()) {
      LogIO logErr(LogOrigin("VLAADA", "ok()"));
      logErr << LogIO::SEVERE 
	     << "The VLA logical record is not seekable"
	     << LogIO::POST;
      return false;
    }
    if (itsOffset == 0) {
      LogIO logErr(LogOrigin("VLAADA", "ok()"));
      logErr << LogIO::SEVERE 
	     << "The antenna data area cannot have a zero offset"
	     << LogIO::POST;
      return false;
    }
  }
  return true;
}

// Local Variables: 
// compile-command: "gmake VLAADA"
// End: 
