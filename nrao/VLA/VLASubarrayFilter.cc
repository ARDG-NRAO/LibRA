//# VLASubarrayFilter.cc:
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

#include <nrao/VLA/VLASubarrayFilter.h>
#include <nrao/VLA/VLALogicalRecord.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Logging/LogIO.h>
#include <casa/Utilities/Assert.h>

VLASubarrayFilter::VLASubarrayFilter()
  :itsArray(0)
{
  DebugAssert(ok(), AipsError);
}

VLASubarrayFilter::VLASubarrayFilter(const uInt subArray) 
  :itsArray(subArray)
{
  DebugAssert(ok(), AipsError);
}

VLASubarrayFilter::VLASubarrayFilter(const VLASubarrayFilter& other) 
  :VLAFilter(),
  itsArray(other.itsArray)
{
  DebugAssert(ok(), AipsError);
}

VLASubarrayFilter::~VLASubarrayFilter() {
  DebugAssert(ok(), AipsError);
}

VLASubarrayFilter& VLASubarrayFilter::
operator=(const VLASubarrayFilter& other) {
  if (this != &other) {
    itsArray = other.itsArray;
  }
  DebugAssert(ok(), AipsError);
  return *this;
}

Bool VLASubarrayFilter::passThru(const VLALogicalRecord& record) const {
//   cerr << "This record subarray id: |" << record.SDA().subArray()
//        << "| \trequired subarray id: |" << itsArray;
  if (itsArray == 0 || 
      itsArray == record.SDA().subArray()) {
//     cerr << " match" << endl;
    return true;
  } else {
//     cerr << " NO match" << endl;
    return false;
  }
}

VLAFilter* VLASubarrayFilter::clone() const {
  DebugAssert(ok(), AipsError);
  VLASubarrayFilter* tmpPtr = new VLASubarrayFilter(*this);
  AlwaysAssert(tmpPtr != 0, AipsError);
  return tmpPtr;
}

Bool VLASubarrayFilter::ok() const {
  // The LogIO class is only constructed if an error is detected for
  // performance reasons. Both function static and file static variables
  // where considered and rejected for this purpose.
  if (!VLAFilter::ok()) return false; 
  if (itsArray > 5) {
    LogIO logErr(LogOrigin("VLASubarrayFilter", "ok()"));
    logErr << LogIO::SEVERE 
 	   << "The subarray id must be less than 5"
	   << LogIO::POST;
    return false;
  }
  return true;
}
// Local Variables: 
// compile-command: "gmake VLASubarrayFilter"
// End: 
