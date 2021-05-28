//# VLADiskInput.cc: This class reads VLA archive files from a Disk
//# Copyright (C) 1999,2000,2001,2002,2003
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
//# $Id: VLADiskInput.cc,v 19.2.20.3 2006/10/09 20:08:34 wyoung Exp $

#include <nrao/VLA/VLADiskInput.h>
#include <nrao/VLA/nreal.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/IO/MemoryIO.h>
#include <casa/IO/ByteIO.h>
#include <casa/OS/RegularFile.h>
#include <casa/iostream.h>
#include <arpa/inet.h>
#include <casa/stdlib.h>
#include <unistd.h>

#ifndef MAX_LOGICAL_RECORD_SIZE
#define MAX_LOGICAL_RECORD_SIZE 850000u
#endif

// this is an expediant, we should get it from the IERS table
#define LEAP_SECONDS 33

String VLADiskInput::getTodaysFile(int relDay){
  int LeapSeconds = LEAP_SECONDS;
  time_t curTime, holdTime;          /* time vars */
  struct tm *tmTime;                 /* time structure */
  ostringstream  aDate;
  curTime = time(&holdTime) + LeapSeconds + relDay*86400;
  tmTime = gmtime(&curTime);
  oldDay = tmTime->tm_yday;

// Format up the online vla file

  aDate << visDir << "/vla";
  aDate.width(4);
  aDate.fill('0');
  aDate << tmTime->tm_year+1900 << "-";
  aDate.width(2);
  aDate << tmTime->tm_mon+1 << "-" << tmTime->tm_mday;
  aDate << ".dat";
  return(String(aDate.str()));
}

// simple function see what today is

Int VLADiskInput::whatsToday(){
  time_t LeapSeconds = LEAP_SECONDS;
  time_t curTime, holdTime;          /* time vars */
  struct tm *tmTime;                 /* time structure */
  curTime = time(&holdTime) + LeapSeconds;
  tmTime = gmtime(&curTime);
  return tmTime->tm_yday;
}

// As we go over IAT midnight the VLA data repository file
// changes its name so we have to attach to the new day's file.

void VLADiskInput::reattachCurrent(){
   // OK file may not have been created yet so lets wait a bit
   RegularFile nrtFile(getTodaysFile(0));
   while(!nrtFile.exists()){
      sleep(5);
   }
   //std::cerr << "Attached to new file" << std::endl;
   itsFile = new RegularFileIO(nrtFile);
}

// Here's the online version to read data from the VLA online data repository

VLADiskInput::VLADiskInput(const String& onlineFlag) 
  :VLAArchiveInput()
{
   // Need to check for :-x to set the relative day.
   if(onlineFlag == "online"){
      previousDay = 0;
      char *dataDir = getenv("VISDATADIR");
      if(!dataDir){
          dataDir = strdup("/home/vis-serv-mirror/vladata");
      }
      visDir = String(dataDir);
      Path nrtFile = Path(getTodaysFile());
      itsFile = new RegularFileIO(RegularFile(nrtFile));
      onlineFill = true;
      itsFile->seek(0, ByteIO::End);
   } else {
      throw(AipsError("Invalid online specifier " + onlineFlag));
   }
}

VLADiskInput::VLADiskInput(const Path& fileName) 
  :VLAArchiveInput(),
   itsFile(new RegularFileIO(fileName)), onlineFill(false)
{
	attachFile(fileName.absoluteName().c_str());
}

VLADiskInput::~VLADiskInput() {
  // The RegularFileIO destructor closes the disk file.
  delete itsFile;
  detachFile();
}

/*
Bool VLADiskInput::read() {
  // Clear the internal buffers and reset the flags as we will try to read some
  // more data.
  itsMemIO.clear();
  // Find an initial record. 
  Short n = 1, m;
  uInt curRecSize(0);
  Int bytesToRead(0);
  Int thisReadSize(0);
  uChar *recordPtr(0);
  if (findFirstRecord(m) == false) if(!onlineFill)return false;
  // We have the first physical record in Memory. Now decode how long this
  // logical record is.
  itsRecord.seek(0);
  Int logicalRecordSize;
  if(this->bytesRead() < this->totalBytes()){
     try {
        itsRecord >> logicalRecordSize;
        logicalRecordSize *= 2;
        curRecSize = itsMemIO.length();
        bytesToRead = logicalRecordSize - curRecSize;
  // make the buffer a bit bigger than necessary as we may need to read up to
  // VLAArchiveInput::BlockSize - 1 extra bytes (to pad out the block).
	// cerr << "Uno Setting buffer size: " << logicalRecordSize << " " << VLAArchiveInput::BlockSize << endl;
	if(logicalRecordSize > MAX_LOGICAL_RECORD_SIZE)
		return false;
        recordPtr = itsMemIO.setBuffer(logicalRecordSize + 
				     VLAArchiveInput::BlockSize-1);
     } catch (AipsError x) {
	     std::cerr << "end of file???" << std::endl;
	    std::cerr << x.getMesg() << std::endl;
     }
// Sanity check here.
  }
  cerr << "A File position is " << itsFile->seek(0, ByteIO::Current) << " " << endl;
  while(this->bytesRead() >= this->totalBytes()){
    //std::cerr << "Waiting..." << std::endl;
    if(!onlineFill){
        cerr << "VLADiskInput::read end of file encountered." << endl;
        itsMemIO.clear();
        return false;
    }
        // I'm online so check to see if the day changed and we need
        // to move to the next day's file otherwise sleep and see if
        // we've got some more data.
    sleep(10);
    itsMemIO.clear();
    itsRecord.seek(0);
    Int curDay = whatsToday();
    if(oldDay != curDay){
        reattachCurrent();
        oldDay += 1;
    }
    //std::cerr << this->bytesRead() << " " << this->totalBytes() << std::endl;
    if(this->bytesRead() < this->totalBytes()){
       itsFile->seek(Int64(-4), ByteIO::Current);
       if(findFirstRecord(m)){
          itsRecord >> logicalRecordSize;
        // A new record has arrived, march on!
          logicalRecordSize *= 2;
          curRecSize = itsMemIO.length();
          bytesToRead = logicalRecordSize - curRecSize;
	// cerr << "Dos Setting buffer size: " << logicalRecordSize << " " << VLAArchiveInput::BlockSize << endl;
	if(logicalRecordSize > MAX_LOGICAL_RECORD_SIZE)
		return false;
          recordPtr = itsMemIO.setBuffer(logicalRecordSize + 
				  VLAArchiveInput::BlockSize-1);
       }
    }
  }
  
  cerr << "B File position is " << itsFile->seek(0, ByteIO::Current) << " " << endl;
  while (bytesToRead > 0) {
    thisReadSize = VLAArchiveInput::HeaderSize;
    DebugAssert(static_cast<Int64>(curRecSize + VLAArchiveInput::HeaderSize) <=
                itsMemIO.length(), AipsError);
    Int bytesRead =
      itsFile->read(VLAArchiveInput::HeaderSize, recordPtr+curRecSize, false);
    if (bytesRead < static_cast<Int>(VLAArchiveInput::HeaderSize)) {
      itsMemIO.clear();
      return false;
    }
    // Check the sequence numbers
    {
      itsRecord.seek(Int(curRecSize));
      Short newn, newm;
      itsRecord >> newn;
      itsRecord >> newm;
      if (newm != m || ++n != newn) {
 	itsMemIO.clear();
 	return false;
      }
    }
    // The sequence numbers are OK so read the rest of the data
    if (bytesToRead < 
	static_cast<Int>(VLAArchiveInput::BlockSize*
			 VLAArchiveInput::MaxBlocksPerPhysicalRecord)) {
      thisReadSize = (bytesToRead-1)/VLAArchiveInput::BlockSize + 1;
      thisReadSize *= VLAArchiveInput::BlockSize;
    } else {
      thisReadSize = VLAArchiveInput::BlockSize *
	VLAArchiveInput::MaxBlocksPerPhysicalRecord;
    }
    thisReadSize -= VLAArchiveInput::HeaderSize;
    DebugAssert(static_cast<Int64>(curRecSize+thisReadSize)<=itsMemIO.length(),
		AipsError);
    bytesRead = itsFile->read(thisReadSize, recordPtr+curRecSize, false);
    if (bytesRead < thisReadSize) {
      itsMemIO.clear();
      return false;
    }
    curRecSize += thisReadSize;
    bytesToRead -= thisReadSize;
    cerr << "File position is " << itsFile->seek(0, ByteIO::Current) << " " << endl;
  }
  cerr << "C File position is " << itsFile->seek(0, ByteIO::Current) << " " << endl;
  itsMemIO.setUsed(logicalRecordSize);
  itsRecord.seek(0);
  return true;
}
*/


Bool VLADiskInput::read() {
   Bool rstatus(true);
   itsMemIO.clear();
   Long logicalRecordSize(MAX_LOGICAL_RECORD_SIZE);
   Char* recordPtr = (Char *)itsMemIO.setBuffer(logicalRecordSize);
   logicalRecordSize = readVLALogRec(recordPtr);
   itsMemIO.setUsed(logicalRecordSize);
   itsRecord.seek(0);
   if(!logicalRecordSize)
      rstatus = false;
   return rstatus;
}


// Find an initial record. An initial record MUST have the first 2-byte integer
// as 1 and the next 2-byte integer as a number greater than zero. If we have
// not found an initial record after searching 5MBytes worth of data then just
// give up.
//
//This is nuts for VLA Disk files and needs to be redone as we can calculate
//the number of physical records and avoid all this piecemeal reading of
//the file to find where the records start.
//wky 2-23-08
Bool VLADiskInput::findFirstRecord(Short& m) {
  const uInt maxBytesToSearch = 5*1024*1024;
  Short n = 0 ;
  m = 0;
  cerr << "AA File position is " << itsFile->seek(0, ByteIO::Current) << " " << endl;
  {
    uInt bytesSearched = 0;
    // Search for the correct sequence number or give up after a while.
    Bool maybe(false);
    uInt logicalRecordSize;
    uChar* recordPtr = itsMemIO.setBuffer(VLAArchiveInput::HeaderSize);
    while (!maybe && (bytesSearched <= maxBytesToSearch)) {
      //std::cerr << n << " " << m << std::endl;
      if (bytesSearched > 0) {
	if(itsFile->seek(Int64(VLAArchiveInput::BlockSize -
			   VLAArchiveInput::HeaderSize),
		     ByteIO::Current) <= 0)
		return false;
      }
      bytesSearched += VLAArchiveInput::BlockSize;
      // std::cerr << bytesSearched << std::endl;

      // std::cerr << this->bytesRead() << " " << this->totalBytes() << std::endl;
      Int bytesRead = 
	itsFile->read(VLAArchiveInput::HeaderSize, recordPtr, false);
      cerr << "BB File position is " << itsFile->seek(0, ByteIO::Current) << " " << endl;
      if (bytesRead < static_cast<Int>(VLAArchiveInput::HeaderSize)) {
	itsMemIO.clear();
	return false;
      }
      // Find out what the sequence numbers are.
      itsRecord.seek(0);
      itsRecord >> n;
      itsRecord >> m;
      // OK we need to make sure that Logical record size is OK too before accepting n and m.
       if(n == 1 && m > 0 && m < 40){
	itsFile->read(4, recordPtr, false);
	itsRecord.seek(0);
	itsRecord >> logicalRecordSize;
	itsFile->seek(Int64(-4), ByteIO::Current);
	// itsRecord.seek(-4);
	//`cerr << "Logical Record Size is: " << logicalRecordSize << endl;
	if(logicalRecordSize > MAX_LOGICAL_RECORD_SIZE)
		maybe = false;
	else
		maybe = true;
       }
    }
    //cerr << n << " " << m << endl;
    if (bytesSearched > maxBytesToSearch) {
      itsMemIO.clear();
      return false;
    }
  }
  // OK so we have found the beginning of the first physical record. Now read
  // the data into the logical record.
  uInt offset = 0;
  Int bytesToCopy = 
    VLAArchiveInput::MaxBlocksPerPhysicalRecord * VLAArchiveInput::BlockSize -
    VLAArchiveInput::HeaderSize;
  if (m == 1) { // If m=1 we may need to copy less than the maximum number of
    // blocks per physical record. To work this out we need to read and parse
    // the first block.
    itsRecord.seek(0);
    const Int bytesToRead =
      VLAArchiveInput::BlockSize - VLAArchiveInput::HeaderSize;
    uChar* recordPtr = itsMemIO.setBuffer(bytesToRead);
    // cerr << "Bytes to read " << bytesToRead << endl;
    const Int bytesRead = itsFile->read(bytesToRead, recordPtr, false);
    cerr << "CC File position is " << itsFile->seek(0, ByteIO::Current) << " " << endl;
    if (bytesRead < bytesToRead) {
      itsMemIO.clear();
      return false;
    }
    itsRecord.seek(0);
    Int logicalRecordSize;
    itsRecord >> logicalRecordSize;
    logicalRecordSize *= 2;
    bytesToCopy =
      ((logicalRecordSize - bytesToRead)/VLAArchiveInput::BlockSize + 1) * 
      VLAArchiveInput::BlockSize;
    offset = bytesToRead;
  }
  if (bytesToCopy > 0) {
    uChar* recordPtr = itsMemIO.setBuffer(bytesToCopy+offset);
    // cerr << "Bytes to copy " << bytesToCopy << endl;
    Int bytesRead = itsFile->read(bytesToCopy, recordPtr+offset, false);
    cerr << "DD File position is " << itsFile->seek(0, ByteIO::Current) << " " << endl;
    if (bytesRead < bytesToCopy) {
      itsMemIO.clear();
      return false;
    }
  }
  DebugAssert(n == 1, AipsError);
  DebugAssert(m > 0, AipsError);
  return true;
}

uInt VLADiskInput::bytesRead() {
  return itsFile->seek(0, ByteIO::Current);
}

uInt VLADiskInput::totalBytes() {
  return itsFile->length();
}
  
// Local Variables: 
// compile-command: "gmake VLADiskInput; cd test; gmake OPTLIB=1 tVLADiskInput"
// End: 
