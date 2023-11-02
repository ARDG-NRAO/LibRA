//# VLATimeFilter.cc:
//# Copyright (C) 1999,2000
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

#include <nrao/VLA/VLATimeFilter.h>
#include <nrao/VLA/VLALogicalRecord.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/measures/Measures/MeasConvert.h>

VLATimeFilter::VLATimeFilter()
  :itsStart(0.0),
   itsStop(DBL_MAX/2.0)
{
  DebugAssert(ok(), AipsError);
}

VLATimeFilter::VLATimeFilter(const MVEpoch& startTime, const MVEpoch& stopTime)
  :itsStart(startTime),
   itsStop(stopTime)
{
  DebugAssert(ok(), AipsError);
}

VLATimeFilter::VLATimeFilter(const VLATimeFilter& other) 
  :VLAFilter(),
   itsStart(other.itsStart),
   itsStop(other.itsStop)
{
  DebugAssert(ok(), AipsError);
}

VLATimeFilter::~VLATimeFilter() {
  DebugAssert(ok(), AipsError);
}

VLATimeFilter& VLATimeFilter::operator=(const VLATimeFilter& other) {
  if (this != &other) {
    itsStart = other.itsStart;
    itsStop = other.itsStop;
  }
  DebugAssert(ok(), AipsError);
  return *this;
}

void VLATimeFilter::startTime(const MVEpoch& startTime) {
  itsStart = startTime;
  DebugAssert(ok(), AipsError);
}

void VLATimeFilter::stopTime(const MVEpoch& stopTime) {
  itsStop = stopTime;
  DebugAssert(ok(), AipsError);
}

//#include <casa/Quanta/MVTime.h>
Bool VLATimeFilter::passThru(const VLALogicalRecord& record) const {
  const Double recordTime = record.RCA().obsDay() + 
    Quantum<Double>(record.SDA().obsTime(), "s").getValue("d");

//   cerr << "This record time: " << MVTime(recordTime).string()
//        << "\trequired time range:" << MVTime(itsStart.get()).string()
//        << " - " << MVTime(itsStop.get()).string();
  if (recordTime > itsStart.get() && recordTime < itsStop.get()) {
//     cerr << " match" << endl;
    return true;
  } else {
//     cerr << " NO match" << endl;
    return false;
  }
}

VLAFilter* VLATimeFilter::clone() const {
  DebugAssert(ok(), AipsError);
  VLATimeFilter* tmpPtr = new VLATimeFilter(*this);
  AlwaysAssert(tmpPtr != 0, AipsError);
  return tmpPtr;
}

Bool VLATimeFilter::ok() const {
  // The LogIO class is only constructed if an error is detected for
  // performance reasons. Both function static and file static variables
  // where considered and rejected for this purpose.
  if (!VLAFilter::ok()) return false; 
  if (itsStart.get() > itsStop.get()) {
    LogIO logErr(LogOrigin("VLATimeFilter", "ok()"));
    logErr << LogIO::SEVERE 
	   << "The start time cannot be after the stop time"
           << LogIO::POST;
    return false;
  }
  return true;
}

// Local Variables: 
// compile-command: "gmake VLATimeFilter"
// End: 
