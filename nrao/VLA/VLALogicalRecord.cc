//# VLALogicalRecord.cc: This class reads and reconstructs VLA archive records
//# Copyright (C) 1995,1999,2000
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

#include <nrao/VLA/VLALogicalRecord.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>

VLALogicalRecord::VLALogicalRecord() 
  :itsRecordPtr( ),
   itsRCA(),
   itsSDA(),
   itsADA(0),
   itsCDA(4)
{
}

VLALogicalRecord::VLALogicalRecord(VLAArchiveInput* input) 
  :itsRecordPtr(input),
   itsRCA(),
   itsSDA(),
   itsADA(0),
   itsCDA(4)
{
}

VLALogicalRecord::VLALogicalRecord(const VLALogicalRecord& other) 
  :itsRecordPtr(other.itsRecordPtr),
   itsRCA(other.itsRCA),
   itsSDA(other.itsSDA),
   itsADA(other.itsADA),
   itsCDA(other.itsCDA)
{
}

VLALogicalRecord::~VLALogicalRecord() {
}

VLALogicalRecord& VLALogicalRecord::operator=(const VLALogicalRecord& other) {
  if (this != &other) {
    itsRecordPtr = other.itsRecordPtr;
    itsRCA = other.itsRCA;
    itsSDA = other.itsSDA;
    itsADA = other.itsADA;
    itsCDA = other.itsCDA;
  }
  return *this;
}

ByteSource& VLALogicalRecord::logicalRecord() {
  DebugAssert(isValid(), AipsError);
  return itsRecordPtr->logicalRecord();
}

Bool VLALogicalRecord::read() {
  DebugAssert(isValid(), AipsError);
  if (itsRecordPtr->read() == false) return false;
  ByteSource& itsRecord = itsRecordPtr->logicalRecord();
  itsRCA.attach(itsRecord);
  itsSDA.attach(itsRecord, itsRCA.SDAOffset());
  const uInt nant = itsRCA.nAntennas();
  itsADA.resize(nant, false, false);
  for (uInt i = 0; i < nant; i++) {
    itsADA[i].attach(itsRecord, itsRCA.ADAOffset(i));
  }
  for (uInt i = 0; i < 4; i++) {
    const uInt nchan = itsSDA.trueChannels(VLAEnum::CDA(i));
    const uInt size = itsRCA.CDABaselineBytes(i);
    itsCDA[i].attach(itsRecord, itsRCA.CDAOffset(i), size, nant, nchan);
  }
  return true;
}

Bool VLALogicalRecord::isValid() const {
  return !itsRecordPtr.null();
}

const VLARCA& VLALogicalRecord::RCA() const {
  DebugAssert(isValid(), AipsError);
  return itsRCA;
}

const VLASDA& VLALogicalRecord::SDA() const {
  DebugAssert(isValid(), AipsError);
  return itsSDA;
}

const VLACDA& VLALogicalRecord::CDA(uInt which) const {
  DebugAssert(isValid(), AipsError);
  DebugAssert(which < 4, AipsError);
  return itsCDA[which];
}

const VLAADA& VLALogicalRecord::ADA(uInt which) const {
  DebugAssert(isValid(), AipsError);
  DebugAssert(which < itsRCA.nAntennas(), AipsError);
  return itsADA[which];
}


Vector<Stokes::StokesTypes> 
VLALogicalRecord::polarisations(VLAEnum::CDA cda, uInt ant1, uInt ant2) const {
  const Matrix<VLAEnum::IF> ifs = itsSDA.ifUsage(cda);
  const uInt nPol = ifs.ncolumn();
  Vector<Stokes::StokesTypes> retVal(nPol);
  for (uInt p = 0; p < nPol; p++) {
    const Stokes::StokesTypes if1= itsADA[ant1].ifPol(ifs(0,p));
    const Stokes::StokesTypes if2= itsADA[ant2].ifPol(ifs(1,p));
    if ((if1 == Stokes::RCircular) && (if2 == Stokes::RCircular)) {
      retVal(p) = Stokes::RR;
    } else if ((if1 == Stokes::LCircular) && (if2 == Stokes::LCircular)) {
      retVal(p) = Stokes::LL;
    } else if ((if1 == Stokes::RCircular) && (if2 == Stokes::LCircular)) {
      retVal(p) = Stokes::RL;
    } else if ((if1 == Stokes::LCircular) && (if2 == Stokes::RCircular)) {
      retVal(p) = Stokes::LR;
    }
  }
  return retVal;
}

// Local Variables: 
// compile-command: "gmake VLALogicalRecord; cd test; gmake OPTLIB=1 tVLALogicalRecord"
// End:
