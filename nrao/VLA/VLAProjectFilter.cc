//# VLAProjectFilter.cc:
//# Copyright (C) 1999,2000,2001
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

#include <nrao/VLA/VLAProjectFilter.h>
#include <nrao/VLA/VLALogicalRecord.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Logging/LogIO.h>
#include <casa/Utilities/Assert.h>

VLAProjectFilter::VLAProjectFilter()
  :itsProject("")
{
  DebugAssert(ok(), AipsError);
}

VLAProjectFilter::VLAProjectFilter(const String& projectName) 
  :itsProject(projectName)
{
  itsProject.upcase();
  DebugAssert(ok(), AipsError);
}

VLAProjectFilter::VLAProjectFilter(const VLAProjectFilter& other) 
  : VLAFilter(),
  itsProject(other.itsProject)
{
  DebugAssert(ok(), AipsError);
}

VLAProjectFilter::~VLAProjectFilter() {
  DebugAssert(ok(), AipsError);
}

VLAProjectFilter& VLAProjectFilter::operator=(const VLAProjectFilter& other) {
  if (this != &other) {
    itsProject = other.itsProject;
  }
  DebugAssert(ok(), AipsError);
  return *this;
}

Bool VLAProjectFilter::passThru(const VLALogicalRecord& record) const {
//   cerr << "This record ID: |" << record.SDA().obsId()
//        << "| \trequired ID: |" << itsProject << "|";
  if (itsProject.empty()) return true;
  const String thisProject = upcase(record.SDA().obsId());
  if (thisProject.matches(itsProject)) {
    //     cerr << " match" << endl;
    return true;
  } else {
//     cerr << " NO match" << endl;
    return false;
  }
}

VLAFilter* VLAProjectFilter::clone() const {
  DebugAssert(ok(), AipsError);
  VLAProjectFilter* tmpPtr = new VLAProjectFilter(*this);
  AlwaysAssert(tmpPtr != 0, AipsError);
  return tmpPtr;
}

Bool VLAProjectFilter::ok() const {
  // The LogIO class is only constructed if an error is detected for
  // performance reasons. Both function static and file static variables
  // where considered and rejected for this purpose.
  if (!VLAFilter::ok()) return false; 
  if (itsProject.length() > 6) {
    LogIO logErr(LogOrigin("VLAProjectFilter", "ok()"));
    logErr << LogIO::SEVERE 
	   << "The project name must have no more than 6 characters"
           << LogIO::POST;
    return false;
  }
  return true;
}

// Local Variables: 
// compile-command: "gmake VLAProjectFilter"
// End: 
