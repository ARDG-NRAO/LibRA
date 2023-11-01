//# VLASDA.cc:
//# Copyright (C) 1999,2000,2001,2003
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

#include <nrao/VLA/VLASDA.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>

VLASDA::VLASDA()
  :itsRecord(),
   itsOffset(0)
{
}

VLASDA::VLASDA(ByteSource& record, uInt offset)
  :itsRecord(record),
   itsOffset(offset)
{
  DebugAssert(record.isReadable(), AipsError);
  DebugAssert(record.isSeekable(), AipsError);
}

VLASDA::VLASDA(const VLASDA& other)
  :itsRecord(other.itsRecord),
   itsOffset(other.itsOffset)
{
}

VLASDA::~VLASDA() {
}

VLASDA& VLASDA::operator=(const VLASDA& other) {
  if (this != &other) {
    itsRecord = other.itsRecord;
    itsOffset = other.itsOffset;
  }
  return *this;
}

void VLASDA::attach(ByteSource& record, uInt offset) {
  itsRecord = record;
  itsOffset = offset;
  DebugAssert(record.isReadable(), AipsError);
  DebugAssert(record.isSeekable(), AipsError);
}

uInt VLASDA::trueChannels(VLAEnum::CDA cda) const {
  Int64 where = itsOffset + 2*(18);
  if (cda > VLAEnum::CDA1) where++;
  itsRecord.seek(where);
  uChar byte;
  itsRecord >> byte;
  uInt exponent;
  if ((cda == VLAEnum::CDA0) || (cda == VLAEnum::CDA2)) {
    exponent = (byte & 0xf0) >> 4;
  } else {
    exponent = (byte & 0x0f);
  }
  uInt nChan = 1;
  return nChan << exponent;
}

uInt VLASDA::nChannels(VLAEnum::CDA cda) const {
  uInt nChan = trueChannels(cda);
  if (nChan > 1) nChan--;
    // A special kludge because the first channel is the vector average of the
    // inner 3/4 of the remaining channels. I discard the first channel and
    // hence have to correct for this here.
  return nChan;
}

Double VLASDA::obsFrequency(VLAEnum::CDA cda) const {
  // Calculate the centre frequency using the edge frequency to allow for
  // channel offsets.
  Double freq = edgeFrequency(cda);
  freq += nChannels(cda)/2.0 * channelWidth(cda);
  return freq;
}

Double VLASDA::edgeFrequency(VLAEnum::CDA cda) const {

  uInt which=cda;
  // Trap full-pol sp-line modes correctly
  if (correlatorMode()==VLAEnum::PA) which=0;  // only the A (& C) freq
  if (correlatorMode()==VLAEnum::PB) which=1;  // only the B (& D) freq
  Int64 where = itsOffset + 2*(40+which*4) ;
  itsRecord.seek(where);
  Double edgeFreq;
  itsRecord >> edgeFreq;
  edgeFreq *= 1E9;

  where = itsOffset + 2*(56+which*4) ;
  itsRecord.seek(where);
  Double centreFreq;
  itsRecord >> centreFreq;
  centreFreq *= 1E9;
  if (edgeFreq > centreFreq) {//Looks like we are pointing to the upper edge
    edgeFreq -= correlatedBandwidth(cda);
  }
   
  // Now check if there is a channel offset.
  if (nChannels(cda) > 1) {
    where = itsOffset + 2*(162+which);
    itsRecord.seek(where);
    uShort offset;
    itsRecord >> offset;
    if (offset > 0) {
      edgeFreq += static_cast<Double>(offset)*channelWidth(cda);
    }
  }
  if (nChannels(cda) > 1) { 
    // A special kludge because the first channel is the vector average of the
    // inner 3/4 of the remaining channels. I discard the first channel and
    // hence have to correct for this here. The 0.5 is to get the edge of the
    // channel and not the centre of it.
    edgeFreq += 0.5 * channelWidth(cda);;
  }
  return edgeFreq;
}

Bool VLASDA::dopplerTracking(VLAEnum::CDA cda) const {
  Int64 where = itsOffset + 2*(153+uInt(cda))+1 ;
  itsRecord.seek(where);
  Char code;
  itsRecord >> code;
  if (code == 'F' || code == ' ') return false;
  return true;
}

Double VLASDA::restFrequency(VLAEnum::CDA cda) const {
  Double freq;
  Int64 where = itsOffset + 2*(137+uInt(cda)*4) ;
  itsRecord.seek(where);
  itsRecord >> freq;
  return freq*1E6;
}

Double VLASDA::radialVelocity(VLAEnum::CDA cda) const {
  Int64 where = itsOffset + 2*(121+uInt(cda)*4) ;
  itsRecord.seek(where);
  Double rv;
  itsRecord >> rv;
  return rv*1E3;
}

MFrequency::Types VLASDA::restFrame(VLAEnum::CDA cda) const {
  Int64 where = itsOffset + 2*(153+uInt(cda)) ;
  itsRecord.seek(where);
  Char code;
  itsRecord >> code;
  switch (code) {
  case 'G':
    return MFrequency::GEO;
  case 'B':
    return MFrequency::BARY;
  case 'L':
    return MFrequency::LSRK;
  case 'T':
  default:
    // Assume TOPO when not specified
    return MFrequency::TOPO;
  }
  // I have to return something! This should be suitably meaningless.
  return  MFrequency::N_Types;
}

MDoppler::Types VLASDA::dopplerDefn(VLAEnum::CDA cda) const {
  Int64 where = itsOffset + 2*(153+uInt(cda))+1 ;
  itsRecord.seek(where);
  Char code;
  itsRecord >> code;
  switch (code) {
  case 'Z':
    return MDoppler::OPTICAL;
  case 'V':
    return MDoppler::RADIO;
  }
  // I have to return something! This should be suitably meaningless.
  return  MDoppler::N_Types;
}

Double VLASDA::channelWidth(VLAEnum::CDA cda) const {
  if (trueChannels(cda) == 1) {
    return correlatedBandwidth(cda);
  } // As word 166 is only valid is spectral line mode!
  const Int64 where = itsOffset + 2*(166+uInt(cda));
  itsRecord.seek(where);
  Short exponent;
  itsRecord >> exponent;
  const Int factor = 1 << exponent;
  return 50.0/Double(factor)*1E6;
}

Double VLASDA::correlatedBandwidth(VLAEnum::CDA cda) const {
  Int64 where = itsOffset + 2*(100) ;
  if (cda > VLAEnum::CDA1) where++;
  itsRecord.seek(where);
  uChar byte;
  itsRecord >> byte;
  Int code;
  if ((cda == VLAEnum::CDA0) || (VLAEnum::CDA2)) {
    code = (byte & 0xf0) >> 4;
  } else {
    code = (byte & 0x0f);
  }
  if (trueChannels(cda) > 1) {
    if (code == 7) return 25.0/32.0 * 1E6;
    if (code == 9) return 25.0/128.0 * 1E6;
    Int factor = 1 << code;
    return 50.0/static_cast<Double>(factor) * 1E6;
  } else {// continuum
    if (code == 7) return 25.0/128.0 * 1E6;
    if (code == 8) return 70.0 * 1E6;
    if (code == 9) return doubleInf();
    Int factor = 1 << code;
    return 50.0/static_cast<Double>(factor) * 1E6;
  }
}

Double VLASDA::filterBandwidth(VLAEnum::CDA cda) const {
  Int64 where = itsOffset + 2*(101) ;
  if (cda > VLAEnum::CDA1) where++;
  itsRecord.seek(where);
  uChar byte;
  itsRecord >> byte;
  Int code;
  if ((cda == VLAEnum::CDA0) || (VLAEnum::CDA2)) {
    code = (byte & 0xf0) >> 4;
  } else {
    code = (byte & 0x0f);
  }
  if (code == 4) {
    Float retVal;
    setNaN(retVal);
    return retVal;
  }
  if (code == 3) return 1E200; // Should translate into Inf
  Int factor = 1 << code;
  return 50.0/Double(factor)*1E6;
}

VLAEnum::CorrMode VLASDA::correlatorMode() const {
  const Int64 where = itsOffset + 2*(157) ;
  itsRecord.seek(where);
  Char modeChars[4];
  itsRecord.read(4, modeChars);
  uInt strLen = 4;
  if (modeChars[3] == ' ')  strLen--;
  if (strLen == 3 && modeChars[2] == ' ')  strLen--;
  if (strLen == 2 && modeChars[1] == ' ')  strLen--;
  return VLAEnum::corrMode(String(modeChars, strLen));
}

uInt VLASDA::npol(VLAEnum::CDA cda) const {
  const VLAEnum::CorrMode mode = correlatorMode();
  switch (mode) {
  case VLAEnum::CONTINUUM:
    if ((cda == VLAEnum::CDA0) || (cda == VLAEnum::CDA1)) return 4;
    break;
  case VLAEnum::A:
    if (cda == VLAEnum::CDA0) return 1;
    break;
  case VLAEnum::B:
    if (cda == VLAEnum::CDA1) return 1;
    break;
  case VLAEnum::C:
    if (cda == VLAEnum::CDA2) return 1;
    break;
  case VLAEnum::D:
    if (cda == VLAEnum::CDA3) return 1;
    break;
  case VLAEnum::AB:
    if ((cda == VLAEnum::CDA0) || (cda == VLAEnum::CDA1)) return 1;
    break;
  case VLAEnum::AC:
    if ((cda == VLAEnum::CDA0) || (cda == VLAEnum::CDA2)) return 1;
    break;
  case VLAEnum::AD:
    if ((cda == VLAEnum::CDA0) || (cda == VLAEnum::CDA3)) return 1;
    break;
  case VLAEnum::BC:
    if ((cda == VLAEnum::CDA1) || (cda == VLAEnum::CDA2)) return 1;
    break;
  case VLAEnum::BD:
    if ((cda == VLAEnum::CDA1) || (cda == VLAEnum::CDA3)) return 1;
    break;
  case VLAEnum::CD:
    if ((cda == VLAEnum::CDA2) || (cda == VLAEnum::CDA3)) return 1;
    break;
  case VLAEnum::ABCD:
  case VLAEnum::PA:
  case VLAEnum::PB:
    return 1;
  default:
    return 0;
  }
  return 0;
}

Matrix<VLAEnum::IF> VLASDA::ifUsage(VLAEnum::CDA cda) const {
  const VLAEnum::CorrMode mode = correlatorMode();
  switch (mode) {
  case VLAEnum::CONTINUUM:
    if (cda == VLAEnum::CDA0) {
      Matrix<VLAEnum::IF> retVal(2, 4);
      retVal(0,0) = VLAEnum::IFA; retVal(1,0) = VLAEnum::IFA;
      retVal(0,1) = VLAEnum::IFC; retVal(1,1) = VLAEnum::IFC;
      retVal(0,2) = VLAEnum::IFA; retVal(1,2) = VLAEnum::IFC;
      retVal(0,3) = VLAEnum::IFC; retVal(1,3) = VLAEnum::IFA;
      return retVal;
    } else if (cda == VLAEnum::CDA1) {
      Matrix<VLAEnum::IF> retVal(2, 4);
      retVal(0,0) = VLAEnum::IFB; retVal(1,0) = VLAEnum::IFB;
      retVal(0,1) = VLAEnum::IFD; retVal(1,1) = VLAEnum::IFD;
      retVal(0,2) = VLAEnum::IFB; retVal(1,2) = VLAEnum::IFD;
      retVal(0,3) = VLAEnum::IFD; retVal(1,3) = VLAEnum::IFB;
      return retVal;
    }
    break;
  case VLAEnum::A:
    if (cda == VLAEnum::CDA0) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFA);
      return retVal;
    }
    break;
  case VLAEnum::B:
    if (cda == VLAEnum::CDA1) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFB);
      return retVal;
    }
    break;
  case VLAEnum::C:
    if (cda == VLAEnum::CDA2) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFC);
      return retVal;
    }
    break;
  case VLAEnum::D:
    if (cda == VLAEnum::CDA3) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFD);
      return retVal;
    }
    break;
  case VLAEnum::AB:
    if (cda == VLAEnum::CDA0) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFA);
      return retVal;
    } else if (cda == VLAEnum::CDA1) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFB);
      return retVal;
    }
    break;
  case VLAEnum::AC:
    if (cda == VLAEnum::CDA0) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFA);
      return retVal;
    } else if (cda == VLAEnum::CDA2) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFC);
      return retVal;
    }
    break;
  case VLAEnum::AD:
    if (cda == VLAEnum::CDA0) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFA);
      return retVal;
    } else if (cda == VLAEnum::CDA3) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFD);
      return retVal;
    }
    break;
  case VLAEnum::BC:
    if (cda == VLAEnum::CDA1) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFB);
      return retVal;
    } else if (cda == VLAEnum::CDA2) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFC);
      return retVal;
    }
    break;
  case VLAEnum::BD:
    if (cda == VLAEnum::CDA1) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFB);
      return retVal;
    } else if (cda == VLAEnum::CDA3) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFD);
      return retVal;
    }
    break;
  case VLAEnum::CD:
    if (cda == VLAEnum::CDA2) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFC);
      return retVal;
    } else if (cda == VLAEnum::CDA3) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFD);
      return retVal;
    }
    break;
  case VLAEnum::ABCD:
    if (cda == VLAEnum::CDA0) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFA);
      return retVal;
    } else if (cda == VLAEnum::CDA1) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFB);
      return retVal;
    } else if (cda == VLAEnum::CDA2) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFC);
      return retVal;
    } else if (cda == VLAEnum::CDA3) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFD);
      return retVal;
    }
    break;
  case VLAEnum::PA:
    if (cda == VLAEnum::CDA0) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFA);
      return retVal;
    } else if (cda == VLAEnum::CDA1) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFC);
      return retVal;
    } else if (cda == VLAEnum::CDA2) {
      Matrix<VLAEnum::IF> retVal(2, 1);
      retVal(0,0) = VLAEnum::IFA;
      retVal(1,0) = VLAEnum::IFC;
      return retVal;
    } else if (cda == VLAEnum::CDA3) {
      Matrix<VLAEnum::IF> retVal(2, 1);
      retVal(0,0) = VLAEnum::IFC;
      retVal(1,0) = VLAEnum::IFA;
      return retVal;
    }
    break;
  case VLAEnum::PB:
    if (cda == VLAEnum::CDA0) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFB);
      return retVal;
    } else if (cda == VLAEnum::CDA1) {
      Matrix<VLAEnum::IF> retVal(2, 1, VLAEnum::IFD);
      return retVal;
    } else if (cda == VLAEnum::CDA2) {
      Matrix<VLAEnum::IF> retVal(2, 1);
      retVal(0,0) = VLAEnum::IFB;
      retVal(1,0) = VLAEnum::IFD;
      return retVal;
    } else if (cda == VLAEnum::CDA3) {
      Matrix<VLAEnum::IF> retVal(2, 1);
      retVal(0,0) = VLAEnum::IFD;
      retVal(1,0) = VLAEnum::IFB;
      return retVal;
    }
    break;
  default:
    return Matrix<VLAEnum::IF>(2,1);
  }
  return Matrix<VLAEnum::IF>(2,1);
}

uInt VLASDA::electronicPath(VLAEnum::CDA cda) const {
  const VLAEnum::CorrMode mode = correlatorMode();
  if (mode == VLAEnum::PA) return 0;
  if (mode == VLAEnum::PB) return 1;
  if ((cda == VLAEnum::CDA0) || (cda == VLAEnum::CDA2)) return 0;
  return 1;
}

uInt VLASDA::subArray() const {
  const Int64 where = itsOffset;
  itsRecord.seek(where);
  Short id;
  itsRecord >> id;
  DebugAssert(id >= 0, AipsError);
  return id;
}

String VLASDA::arrayConfig() const {
  const Int64 where = itsOffset + 2*(10);
  itsRecord.seek(where);
  Char c[2];
  itsRecord.read(2,c);
  uInt len = 2;
  if (c[1] == ' ') len = 1;
  return String(c, len);
}

Vector<Double> VLASDA::sourceDir() const {
  Vector<Double> pos(2);
  const Int64 where = itsOffset + 2*(24);
  itsRecord.seek(where);
  Bool isACopy;
  Double* dPtr = pos.getStorage(isACopy);
  itsRecord.read(2,dPtr);
  pos.putStorage(dPtr, isACopy);
  return pos;
}

String VLASDA::sourceName() const {
  Int64 where = itsOffset + 2*(1) ;
  itsRecord.seek(where);
  Int len = 16;
  Char *c = new Char[len];
  itsRecord.read(len, c);
  while (len > 0 && c[--len] == ' ');
  String name(c, len+1);
  delete [] c;
  return name;
}

Int VLASDA::sourceQual() const {
  Int64 where = itsOffset + 2*(9) ;
  itsRecord.seek(where);
  Short qual;
  itsRecord >> qual;
  return qual;
}

Double VLASDA::intTime() const {
  Int64 where = itsOffset + 2*(19) ;
  itsRecord.seek(where);
  Short interrupts;
  itsRecord >> interrupts;
  return Double(interrupts)/19.2;
}

Double VLASDA::obsTime() const {
  Int64 where = itsOffset + 2*(72) ;
  itsRecord.seek(where);
  Double radians;
  itsRecord >> radians;
  return radians/C::_2pi * 60.0 * 60.0 * 24.0 - intTime()/2.0;
}

String VLASDA::obsId() const {
  Int64 where = itsOffset + 2*(11) ;
  itsRecord.seek(where);
  Int len = 6;
  Char *c = new Char[len];
  itsRecord.read(len, c);
  while (len > 0 && c[--len] == ' ');
  String name(c, len+1);
  delete [] c;
  return name;
}

String VLASDA::obsMode() const {
  const Int64 where = itsOffset + 2*(15);
  itsRecord.seek(where);
  Char c[2];
  itsRecord.read(2,c);
  uInt len = 2;
  return String(c, len);
}

String VLASDA::obsModeFull() const {
  String om=obsMode();
  if(om=="  ") {
    return String("Standard Observing");
  }
  else if(om=="D ") {
    return String("Delay center determination mode");
  }
  else if(om=="H ") {
    return String("Holography raster mode");
  }
  else if(om=="IR") {
    return String("Interferometer reference pointing mode");
  }
  else if(om=="IA") {
    return String("Interferometer pointing mode (IF A)");
  }
  else if(om=="IB") {
    return String("Interferometer pointing mode (IF B)");
  }
  else if(om=="IC") {
    return String("Interferometer pointing mode (IF C)");
  }
  else if(om=="ID") {
    return String("Interferometer pointing mode (IF D)");
  }
  else if(om=="JA") {
    return String("JPL mode (IF A)");
  }
  else if(om=="JB") {
    return String("JPL mode (IF B)");
  }
  else if(om=="JC") {
    return String("JPL mode (IF C)");
  }
  else if(om=="JD") {
    return String("JPL mode (IF D)");
  }
  else if(om=="PA") {
    return String("Single dish pointing mode (IF A)");
  }
  else if(om=="PB") {
    return String("Single dish pointing mode (IF B)");
  }
  else if(om=="PC") {
    return String("Single dish pointing mode (IF C)");
  }
  else if(om=="PD") {
    return String("Single dish pointing mode (IF D)");
  }
  else if(om=="S ") {
    return String("Solar observing configuration");
  }
  else if(om=="SP") {
    return String("Solar observing configuration (low accuracy empheris)");
  }
  else if(om=="TB") {
    return String("Test back-end and front-end");
  }
  else if(om=="TE") {
    return String("Tipping curve");
  }
  else if(om=="TF") {
    return String("Test front-end");
  }
  else if(om=="VA") {
    return String("Self-phasing mode for VLBI phased-array (IFs A and D)");
  }
  else if(om=="VB") {
    return String("Self-phasing mode for VLBI phased-array (IFs B and C)");
  }
  else if(om=="VL") {
    return String("Self-phasing mode for VLBI phased-array (IFs C and D)");
  }
  else if(om=="VR") {
    return String("Self-phasing mode for VLBI phased-array (IFs A and B)");
  }
  else if(om=="VS") {
    return String("Single dish VLBI");
  }
  else if(om=="VX") {
    return String("Applies last phase update from source line using VA mode");
  }
  return String("Unknown mode: ") + om;
}

String VLASDA::calCode() const {
  Int64 where = itsOffset + 2*(16) ;
  itsRecord.seek(where);
  Char c;
  itsRecord >> c;
  return String(c);
}

MDirection::Types VLASDA::epoch() const {
  const Int64 where = itsOffset + 2*(161) ;
  itsRecord.seek(where);
  Short year;
  itsRecord >> year;
  if (year == 2000) {
    return MDirection::J2000;
  } else if (year == 1950) {
    // the 1979.9 epoch B1950 direction
    return MDirection::B1950_VLA;
  } else if (year == -1) {
    return MDirection::APP;
  }
  // year is likely 0, N_Types can't be used as is but this signals the problem
  return MDirection::N_Types;
}

Bool VLASDA::smoothed() const {
  Int64 where = itsOffset + 2*(159) ;
  itsRecord.seek(where);
  Char c;
  itsRecord >> c;
  if (c == 'H') return true;
  itsRecord >> c;
  if (c == 'H') return true;
  return false;
}

// Local Variables: 
// compile-command: "gmake VLASDA; cd test; gmake OPTLIB=1 tVLASDA"
// End: 
