//# VLAFilterSet.cc:
//# Copyright (C) 1999,2000,2002
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

#include <nrao/VLA/VLAFilterSet.h>
#include <nrao/VLA/VLAFilter.h>
// #include <nrao/VLA/VLALogicalRecord.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Utilities/Assert.h>

VLAFilterSet::VLAFilterSet()
  :itsFilters()
{
  //  cerr << "VLAFilterSet::VLAFilterSet()" << endl;
  DebugAssert(ok(), AipsError);
}

VLAFilterSet::VLAFilterSet(const VLAFilterSet& other) 
  :itsFilters(other.nelements(), (VLAFilter*)0)
{
  //  cerr << "VLAFilterSet::VLAFilterSet(...) - starting" << endl;
  copyFilters(other);
  DebugAssert(ok(), AipsError);
  //  cerr << "VLAFilterSet::VLAFilterSet(...) - done" << endl;
}

VLAFilterSet::~VLAFilterSet() {
  //  cerr << "VLAFilterSet::~VLAFilterSet()" << endl;
  DebugAssert(ok(), AipsError);
  deleteAllFilters();
  //  cerr << "Done VLAFilterSet::~VLAFilterSet()" << endl;
}

VLAFilterSet& VLAFilterSet::operator=(const VLAFilterSet& other) {
  //  cerr << "starting VLAFilterSet::operator=" << endl;
  DebugAssert(ok(), AipsError);
  if (this != &other) {
    deleteAllFilters();
    itsFilters.resize(other.nelements(), true, false);
    itsFilters = 0;
    copyFilters(other);
    DebugAssert(ok(), AipsError);
  }
  //  cerr << "Done VLAFilterSet::operator=" << endl;
  return *this;
}

void VLAFilterSet::addFilter(const VLAFilter& filter) {
  const uInt n = nelements();
  itsFilters.resize(n+1, true, true);
  itsFilters[n] = filter.clone();
  DebugAssert(ok(), AipsError);
}

void VLAFilterSet::removeFilter(const uInt which) {
  AlwaysAssert(which < nelements(), AipsError);
  itsFilters.remove(which, true);
  DebugAssert(ok(), AipsError);
}

const VLAFilter& VLAFilterSet::filter(const uInt which) const {
  AlwaysAssert(which < nelements(), AipsError);
  DebugAssert(ok(), AipsError);
  return *(itsFilters[which]);
}

Bool VLAFilterSet::passThru(const VLALogicalRecord& record) const {
  DebugAssert(ok(), AipsError);
  const uInt n = nelements();
  uInt i = 0;
  while (i < n && itsFilters[i]->passThru(record)) {
    i++;
  }
  if (i != n) {
    return false;
  } else {
    return true;
  }
}

Bool VLAFilterSet::ok() const {
  // The LogIO class is only constructed if an error is detected for
  // performance reasons. Both function static and file static variables
  // where considered and rejected for this purpose.
  const uInt n = nelements();
  for (uInt i = 0; i < n; i++) {
    if (itsFilters[i] == 0) {
      //      cerr << "****************************" << i << ":" << n << endl;
      LogIO logErr(LogOrigin("VLAFilterSet", "ok()"));
      logErr << LogIO::SEVERE 
	     << "Cannot have a null filter in the set"
	     << LogIO::POST;
      return false;
    }
  }
  for (uInt i = 0; i < n; i++) {
    if (!itsFilters[i]->ok()) {
      LogIO logErr(LogOrigin("VLAFilterSet", "ok()"));
      logErr << LogIO::SEVERE 
	     << "Filter " << i << "is not ok"
	     << LogIO::POST;
      return false;
    }
  }
  return true;
}

void VLAFilterSet::deleteAllFilters() {
  for (uInt i = 0; i < nelements(); i++) {
    delete itsFilters[i];
    itsFilters[i] = 0;
  }
}

void VLAFilterSet::copyFilters(const VLAFilterSet& other) {
  const uInt n = other.nelements();
  for (uInt i = 0; i < n; i++) {
    DebugAssert(itsFilters[i] == 0, AipsError);
    itsFilters[i] = other.filter(i).clone();
  }
}

// Local Variables: 
// compile-command: "gmake VLAFilterSet"
// End: 
