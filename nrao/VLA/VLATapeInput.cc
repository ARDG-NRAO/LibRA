//# VLAArchiveInput.cc: This class reads and reconstructs VLA archive records
//# Copyright (C) 1995,1999,2000,2001,2002
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

#include <nrao/VLA/VLATapeInput.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/IO/MemoryIO.h>

const uInt VLATapeInput::ReadSize = VLAArchiveInput::BlockSize *
                                   VLAArchiveInput::MaxBlocksPerPhysicalRecord;

VLATapeInput::VLATapeInput(const Path& device, uInt whichFile) 
  :VLAArchiveInput(),
   itsTape(device),
   itsFiles(1,whichFile),
   itsCurFile(-1),
   itsBuffer(VLATapeInput::ReadSize)
{
  itsTape.setVariableBlockSize();
  if (!nextFile()) {
    throw(AipsError("VLATapeInput:: problem positioning the tape to file " + 
		    String::toString(whichFile)));
  }
}

VLATapeInput::VLATapeInput(const Path& device, const Block<uInt>& whichFiles) 
  :VLAArchiveInput(),
   itsTape(device),
   itsFiles(whichFiles),
   itsCurFile(-1),
   itsBuffer(VLATapeInput::ReadSize)
{
  itsTape.setVariableBlockSize();
  const uInt nFiles = itsFiles.nelements();
  if (nFiles == 0) {
    throw(AipsError("VLATapeInput:: no tape file specified"));
  }
  uInt lastFile = 0;
  for (uInt i = 0; i < nFiles; i++) {
    uInt& thisFile = itsFiles[i];
    if ((thisFile == 0) && (lastFile != 0)) {
      thisFile = lastFile + 1;
    }
    if ((thisFile != 0) && (thisFile <= lastFile)) {
      throw(AipsError
	    ("VLATapeInput:: badly ordered or incorrect file numbers"));
    }
    lastFile = thisFile;
  }
  if (!nextFile()) {
    throw(AipsError("VLATapeInput:: problem positioning the tape to file " + 
		    String::toString(whichFiles[0])));
  }
}

VLATapeInput::~VLATapeInput() {
}

Bool VLATapeInput::read() {
  while (nextRecord() == false && nextFile() == true);
  return hasData();
}

// Find an initial record. An initial record MUST have the first 2-byte integer
// as 1 and the next 2-byte integer as a number greater than zero. If we have
// not found an initial record after searching 5MBytes worth of data then just
// give up.
Bool VLATapeInput::findFirstRecord(Short& m) {
  const uInt maxBytesToSearch = 5*1024*1024;
  Short n = 0 ;
  m = 0;
  uInt bytesSearched = 0;
  // Search for the correct sequence number or give up after a while.
  uInt bytesRead = 0;
  while (!(n == 1 && m > 0) && (bytesSearched <= maxBytesToSearch)) {
    bytesSearched += bytesRead;
    bytesRead = VLATapeInput::ReadSize;
    if (fillBuffer(bytesRead) == false) return false;
// copy just enough bytes into the MemoryIO object to find out what the
// sequence numbers are
    itsRecord.seek(0);
    itsRecord.write(VLAArchiveInput::HeaderSize, itsBuffer.storage());
    itsRecord.seek(0);
    itsRecord >> n;
    itsRecord >> m;
  }
  if (bytesSearched > maxBytesToSearch) {
    itsMemIO.clear();
    return false;
  }
  // OK so we have found the beginning of the first record. Copy the rest of
  // the buffer into the record.
  uInt bytesToCopy;
  if (m != 1) { // Need to copy VLATapeInput::ReadSize bytes
    bytesToCopy = VLATapeInput::ReadSize - VLAArchiveInput::HeaderSize;
  } else {// If m=1 we probably can copy less bytes than the amount of
    // data read. To work this out we need to find the logical record size.
    itsRecord.seek(0);
    itsRecord.write(4, itsBuffer.storage() + VLAArchiveInput::HeaderSize);
    itsRecord.seek(0);
    Int logicalRecordSize;
    itsRecord >> logicalRecordSize;
    bytesToCopy = logicalRecordSize * 2;
  }
  itsRecord.seek(0);
  itsRecord.write(bytesToCopy, 
		  itsBuffer.storage() + VLAArchiveInput::HeaderSize);
  DebugAssert(n == 1, AipsError);
  DebugAssert(m > 0, AipsError);
  return true;
}

Bool VLATapeInput::fillBuffer(uInt& bytesToRead) {
//   cerr << "  Trying to read " << bytesToRead << " bytes" << endl;
  DebugAssert(bytesToRead <= VLATapeInput::ReadSize, AipsError);
  DebugAssert(bytesToRead%VLAArchiveInput::BlockSize == 0, AipsError);
  const Int bytesRead = itsTape.read(bytesToRead, itsBuffer.storage(), false);
//   cerr << "  Bytes read: " << bytesRead 
//        << " Position: " << itsInputPtr->seek(0L, ByteIO::Current) 
//        << " Length: " << itsInputPtr->length() 
//        << endl;
  if ((bytesRead <= 0) || (bytesRead%VLAArchiveInput::BlockSize != 0)) {
    itsMemIO.clear();
    return false;
  }
  bytesToRead = bytesRead;
  return true;
}

Bool VLATapeInput::nextFile() {
  if (itsCurFile == static_cast<Int>(itsFiles.nelements()) - 1) return false;
  Int skip;
  if (itsCurFile < 0) {
    itsCurFile = 0;
    if (itsFiles[0] > 0) {
      itsTape.rewind();
      skip = itsFiles[0] - 1;
    } else {
      skip = 0;
    }
  } else {
    const Int curFile = itsFiles[itsCurFile];
    itsCurFile++;
    const Int nextFile = itsFiles[itsCurFile];
    if ((nextFile != 0) && (curFile == 0)) {
      itsTape.rewind();
      skip = nextFile - 1;
    } else {
      skip = nextFile - curFile - 1;
    }
  }
  DebugAssert(skip >= 0, AipsError);
  if (skip > 0) itsTape.skip(skip);
  return true;
}

Bool VLATapeInput::nextRecord() {
  // Clear the internal buffers and reset the flags as we will try to read some
  // more data.
  const Bool gotDataPrev = itsMemIO.length() > 0 ? true : false;
  itsMemIO.clear();
  // Find an initial record. 
  Short n = 1, m;
  if (findFirstRecord(m) == false) {
    if (gotDataPrev) return false; // End of file
    throw(AipsError("VLATapeInput::nextRecord - Cannot find the start of the "
		    "record.\nPossible reasons are:\n"
		    "* your tape is not in VLA archive format\n"
		    "* you are trying to read the tape label\n"
		    "* you are trying to read beyond the end of the tape\n"
		    "* you have a corrupted tape."));

  }
  uInt thisReadSize = itsMemIO.length();
  DebugAssert(thisReadSize >= VLAArchiveInput::HeaderSize, AipsError);
  // We have the first physical record in Memory. Now decode how long this
  // logical record is.
  itsRecord.seek(0);
  Int logicalRecordSize;
  itsRecord >> logicalRecordSize;
  logicalRecordSize *= 2;
  itsRecord.seek(0, ByteIO::End);
  Int bytesToRead = logicalRecordSize - thisReadSize;
//   cerr << "Still have " << bytesToRead << " bytes to read out of " 
//        << logicalRecordSize << " bytes in this record."
//        << " The buffer contains " << itsMemIO.length() << " bytes" 
//        << endl;
  while (bytesToRead > 0) {
    if (bytesToRead < static_cast<Int>(VLATapeInput::ReadSize)) {
      thisReadSize = (bytesToRead-1)/VLAArchiveInput::BlockSize + 1;
      thisReadSize *= VLAArchiveInput::BlockSize;
    } else {
      thisReadSize = VLATapeInput::ReadSize;
    }
    
    if (fillBuffer(thisReadSize) == false) return false;
    // Check the sequence numbers
    {
      itsRecord.write(VLAArchiveInput::HeaderSize, itsBuffer.storage());
      itsRecord.seek(-static_cast<Int64>(VLAArchiveInput::HeaderSize),
		     ByteIO::End);
      Short newn, newm;
      itsRecord >> newn;
      itsRecord >> newm;
//       cerr << "Sequence numbers: Found m = " << newm << " n = " << newn;
//       cerr << "   Expected m = " << m << " n = " << n+1 << endl;
      if (newm != m || ++n != newn) {
	itsMemIO.clear();
	return false;
      }
      itsRecord.seek(-static_cast<Int64>(VLAArchiveInput::HeaderSize), 
                     ByteIO::End);
    }
    // The sequence numbers are OK so write the rest of the data
    const uInt bytesToWrite = thisReadSize -4u;
    //const uInt bytesToWrite = thisReadSize < static_cast<uInt>(bytesToRead) ? 
                              //(thisReadSize - 4u): thisReadSize;

    itsRecord.write(bytesToWrite,
		    itsBuffer.storage() + VLAArchiveInput::HeaderSize);
    bytesToRead -= thisReadSize;
//     cerr << "Read a logical block. Still have " 
// 	 << bytesToRead << " bytes to read. The buffer contains " 
// 	 << itsMemIO.length() << " bytes" << endl;
  }
  itsRecord.seek(0);
  return true;
}
// Local Variables: 
// compile-command: "gmake VLATapeInput; cd test; gmake OPTLIB=1 tVLATapeInput"
// End: 
