//# VLAArchiveInput.cc: This class reads and reconstructs VLA archive records
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

#include <nrao/VLA/VLAArchiveInput.h>

// These parameters define the fundamental physical data record
// characteristics.  I'd be surprised if they ever have to be changed.
const uInt VLAArchiveInput::BlockSize = 2048;
const uInt VLAArchiveInput::HeaderSize = 4;
const uInt VLAArchiveInput::MaxBlocksPerPhysicalRecord = 13;

VLAArchiveInput::~VLAArchiveInput() {
}

ByteSource& VLAArchiveInput::logicalRecord() {
  return itsRecord;
}

const ByteSource& VLAArchiveInput::logicalRecord() const {
  return itsRecord;
}

Bool VLAArchiveInput::hasData() const {
  MemoryIO& nonconstmemio = const_cast<MemoryIO&>(itsMemIO);
  return nonconstmemio.length() != 0 ? true: false;
}

VLAArchiveInput::VLAArchiveInput()
  :itsMemIO(VLAArchiveInput::BlockSize, VLAArchiveInput::BlockSize),
   itsModComp(),
   itsCtrIO(&itsModComp, &itsMemIO, VLAArchiveInput::BlockSize, false),
   itsRecord(&itsCtrIO)
{
}
// Local Variables: 
// compile-command: "gmake VLAArchiveInput"
// End: 
