//# VLARCA.cc:
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

#include <nrao/VLA/VLARCA.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>

VLARCA::VLARCA()
  :itsRecord()
{
}

VLARCA::VLARCA(ByteSource& record)
  :itsRecord(record)
{
  DebugAssert(record.isReadable(), AipsError);
  DebugAssert(record.isSeekable(), AipsError);
}

VLARCA::VLARCA(const VLARCA& other)
  :itsRecord(other.itsRecord)
{
}

VLARCA::~VLARCA() {
}

VLARCA& VLARCA::operator=(const VLARCA& other) {
  if (this != &other) {
    itsRecord = other.itsRecord;
  }
  return *this;
}

uInt VLARCA::length() const {
  const Int64 where = 0;
  itsRecord.seek(where);
  Int length;
  itsRecord >> length;
  DebugAssert(length > 0, AipsError);
  return 2*length; // The length is returned in bytes NOT words
}

uInt VLARCA::revision() const {
  const Int64 where = 2*3;
  itsRecord.seek(where);
  Short rev;
  itsRecord >> rev;
  DebugAssert(rev > 0, AipsError);
  return rev; 
}

void VLARCA::attach(ByteSource& record) {
  itsRecord = record;
  DebugAssert(record.isReadable(), AipsError);
  DebugAssert(record.isSeekable(), AipsError);
}

uInt VLARCA::SDAOffset() const {
  const Int64 where = 2*(12);
  itsRecord.seek(where);
  uInt offset;
  itsRecord >> offset;
  return offset*2; // The offset is in bytes NOT words
}

uInt VLARCA::ADAOffset(uInt which) const {
  DebugAssert(which < nAntennas(), AipsError);
  Int64 where = 2*(14);
  itsRecord.seek(where);
  uInt offset;
  itsRecord >> offset;
  if (which != 0) {
    uShort ADASize;
    itsRecord >> ADASize;
    offset += which*ADASize;
  }
  return offset*2; // The offset is in bytes NOT words
}

uInt VLARCA::nAntennas() const {
  const Int64 where = 2*17;
  itsRecord.seek(where);
  uShort nant;
  itsRecord >> nant;
  return nant;
}

uInt VLARCA::CDAOffset(uInt which) const {
  AlwaysAssert(which < 4, AipsError);
  const Int64 where = 2*(which*4+18);
  itsRecord.seek(where);
  uInt offset;
  itsRecord >> offset;
  return offset*2; // The offset is in bytes NOT words
}

uInt VLARCA::CDAHeaderBytes(uInt which) const {
  AlwaysAssert(which < 4, AipsError);
  const Int64 where = 2*(which*4+20);
  itsRecord.seek(where);
  Short headerBytes;
  itsRecord >> headerBytes;
  return headerBytes*2; // The length is returned in bytes NOT words
}

uInt VLARCA::CDABaselineBytes(uInt which) const {
  AlwaysAssert(which < 4, AipsError);
  const Int64 where = 2*(which*4+21);
  itsRecord.seek(where);
  Short baselineBytes;
  itsRecord >> baselineBytes;
  return baselineBytes*2; // The length is returned in bytes NOT words
}

uInt VLARCA::obsDay() const {
  const Int64 where = 2*(4);
  itsRecord.seek(where);
  uInt date;
  itsRecord >> date;
  return date;
}

// Local Variables: 
// compile-command: "gmake OPTLIB=1 VLARCA; cd test; gmake tVLARCA"
// End: 
