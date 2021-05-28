//# VLAOnlineInput.cc: This class reads VLA archive files from a Disk
//# Copyright (C) 2001
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
//# $Id: VLAOnlineInput.cc,v 19.1.20.4 2006/02/17 23:46:40 wyoung Exp $

#include <nrao/VLA/VLAOnlineInput.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/IO/MemoryIO.h>
#include <casa/IO/ByteIO.h>
#include <nrao/VLA/nreal.h>
#include <casa/stdlib.h>


// this is an expediant, we should get it from the IERS table
#define LEAP_SECONDS 33
  
String VLAOnlineInput::getTodaysFile(int relDay){
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
  aDate << tmTime->tm_mon+1 << "-" << tmTime->tm_mday << ".dat";
  return(String(aDate.str()));
} 
  
// simple function see what today is
  
Int VLAOnlineInput::whatsToday(){
  time_t LeapSeconds = LEAP_SECONDS;
  time_t curTime, holdTime;          /* time vars */
  struct tm *tmTime;                 /* time structure */
  curTime = time(&holdTime) + LeapSeconds;
  tmTime = gmtime(&curTime);
  return tmTime->tm_yday;
}

// Here's the online version to read data from the VLA online data repository

VLAOnlineInput::VLAOnlineInput(String &onlineFlag, Int afiles) 
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
      int dum(-99);
      std::cerr << getTodaysFile() << std::endl;
      //olopen_(&dum, (char *)getTodaysFile().data(), 1);
      olopen_(&dum, const_cast<char*>("online"), 1);
      oladvf_(&dum, &afiles);      // Positions the online filler 
                                   // afile < 0 previous days upto 14
                                   // afile == 0, start of current day
				   // afile > 0, now
      onlineFill = true;
   } else {
      throw(AipsError("Invalid online specifier " + onlineFlag));
   }

}

VLAOnlineInput::~VLAOnlineInput() {
  // The StreamIO destructor closes the port.
}

Bool VLAOnlineInput::read() {
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

// Local Variables: 
// compile-command: "gmake VLADiskInput; cd test; gmake OPTLIB=1 tVLADiskInput"
// End: 
