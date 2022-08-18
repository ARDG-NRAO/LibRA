//# VLACalibratorFilter.cc:
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

#include <nrao/VLA/VLACalibratorFilter.h>
#include <nrao/VLA/VLALogicalRecord.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/Regex.h>

VLACalibratorFilter::VLACalibratorFilter()
  :itsCode('#')
{
  DebugAssert(ok(), AipsError);
}

VLACalibratorFilter::VLACalibratorFilter(const Char calcode) 
  :itsCode(calcode)
{
  itsCode = upcase(String(calcode)).elem(0);
  DebugAssert(ok(), AipsError);
}

VLACalibratorFilter::VLACalibratorFilter(const VLACalibratorFilter& other) 
  :VLAFilter(), itsCode(other.itsCode)
{
  DebugAssert(ok(), AipsError);
}

VLACalibratorFilter::~VLACalibratorFilter() {
  DebugAssert(ok(), AipsError);
}

VLACalibratorFilter& VLACalibratorFilter::
operator=(const VLACalibratorFilter& other) {
  if (this != &other) {
    itsCode = other.itsCode;
  }
  DebugAssert(ok(), AipsError);
  return *this;
}

Bool VLACalibratorFilter::passThru(const VLALogicalRecord& record) const {
  DebugAssert(ok(), AipsError);
  const Char thisCode = record.SDA().calCode().elem(0);
//   cerr << "This record:" << thisCode
//         << ": required code: " << itsCode;
  if (itsCode == '#') {
//     cerr << " all match" << endl;
    return true;
  }
  if (itsCode == thisCode) {
//     cerr << " exact match" << endl;
    return true;
  }
  if (itsCode == '*' && thisCode != ' ') {
//     cerr << " cal match" << endl;
    return true;
  }
//   cerr << " NO match" << endl;
  return false;
}

VLAFilter* VLACalibratorFilter::clone() const {
  DebugAssert(ok(), AipsError);
  VLACalibratorFilter* tmpPtr = new VLACalibratorFilter(*this);
  AlwaysAssert(tmpPtr != 0, AipsError);
  return tmpPtr;
}

Bool VLACalibratorFilter::ok() const {
  // The LogIO class is only constructed if an error is detected for
  // performance reasons. Both function static and file static variables
  // where considered and rejected for this purpose.
  if (!VLAFilter::ok()) return false; 
  Regex allowedCodes("[[:upper:][:digit:] *#]");
  if (!String(itsCode).matches(allowedCodes)) {
    LogIO logErr(LogOrigin("VLACalibratorFilter", "ok()"));
    logErr << LogIO::SEVERE 
	   << "The calibrator code is not one of the allowed values"
	   << "It is:" << String(itsCode) << ":"
	   << LogIO::POST;
    return false;
  }
  return true;
}

// Local Variables: 
// compile-command: "gmake VLACalibratorFilter"
// End: 
