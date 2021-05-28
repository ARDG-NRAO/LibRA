//# VLAObsModeFilter.cc:
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

#include <nrao/VLA/VLAObsModeFilter.h>
#include <nrao/VLA/VLALogicalRecord.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Logging/LogIO.h>
#include <casa/Utilities/Assert.h>
#include <casa/Arrays/ArrayLogical.h>


VLAObsModeFilter::VLAObsModeFilter()
  :itsObsModes()
{
  // itsObsModes is optionally user supplied - if empty, all supported modes are OK
  DebugAssert(ok(), AipsError);
  setModeInternals();
}

VLAObsModeFilter::VLAObsModeFilter(const Vector<String>& obsModes) 
    :itsObsModes(obsModes)
{
  // any unsupported modes in obsModes will be skipped using itsSupportedModes as a check
  DebugAssert(ok(), AipsError);
  setModeInternals();
}

VLAObsModeFilter::VLAObsModeFilter(const VLAObsModeFilter& other) 
  : VLAFilter(),
  itsObsModes()
{
  DebugAssert(ok(), AipsError);
  itsObsModes.resize();
  itsObsModes = other.itsObsModes;
  setModeInternals();
}

VLAObsModeFilter::~VLAObsModeFilter() {
  DebugAssert(ok(), AipsError);
}

VLAObsModeFilter& VLAObsModeFilter::
operator=(const VLAObsModeFilter& other) {
    if (this != &other) {
        itsObsModes.resize();
        itsObsModes=other.itsObsModes;
    }
    DebugAssert(ok(), AipsError);
    return *this;
}

Bool VLAObsModeFilter::passThru(const VLALogicalRecord& record) const {
    // it must be found in itsSupportedModes
    if (anyEQ(itsSupportedModes,record.SDA().obsMode())) {
        // and itsObsModes must either be empty or this must be found there
        if (itsObsModes.nelements()==0 ||
            anyEQ(itsObsModes,record.SDA().obsMode()) ) {
            //    cerr << " match" << endl;
            return true;
        } else {
            //    cerr << " NO match" << endl;
            return false;
        }
    } else {
        // unsupported mode, warn?
        if (itsUnsupportedModes.count(record.SDA().obsMode())==0) {
            // unknown mode, add it
            itsUnsupportedModes[record.SDA().obsMode()] = modeDescWarn("unknown mode",false);
        }
        if (!itsUnsupportedModes.at(record.SDA().obsMode()).second) {
            // this warning only happens during fill, and it's less confusing if it appears to come from there
            LogIO logErr(LogOrigin("VLAFiller","fill"));
            logErr << LogIO::WARN
                   << "Unsupported observing mode: " << record.SDA().obsMode()
                   << " " << itsUnsupportedModes.at(record.SDA().obsMode()).first
                   << LogIO::POST;
            itsUnsupportedModes.at(record.SDA().obsMode()).second = true;
        }
        return false;
    }
    // it can't get here, but just in case
    return false;
}

VLAFilter* VLAObsModeFilter::clone() const {
  DebugAssert(ok(), AipsError);
  VLAObsModeFilter* tmpPtr = new VLAObsModeFilter(*this);
  AlwaysAssert(tmpPtr != 0, AipsError);
  return tmpPtr;
}

Bool VLAObsModeFilter::ok() const {
  // The LogIO class is only constructed if an error is detected for
  // performance reasons. Both function static and file static variables
  // where considered and rejected for this purpose.
  if (!VLAFilter::ok()) return false; 
  return true;
}

void VLAObsModeFilter::setModeInternals()
{
  // Permit only sensible modes for now
  itsSupportedModes.resize(9);
  itsSupportedModes[0]="  ";   // standard
  itsSupportedModes[1]="H ";   // holography
  itsSupportedModes[2]="S ";   // solar
  itsSupportedModes[3]="SP";   // solar (low accuracy)
  itsSupportedModes[4]="VA";   // self-phasing (AD)
  itsSupportedModes[5]="VB";   // self-phasing (BC)
  itsSupportedModes[6]="VL";   // self-phasing (CD)
  itsSupportedModes[7]="VR";   // self-phasing (AB)
  itsSupportedModes[8]="VX";   // phasing from prior scan

  // used for error reporting - appendix D.2 : VLA Archive Data Format
  itsUnsupportedModes.clear();
  itsUnsupportedModes["D "] = modeDescWarn("delay center determination mode",false);
  itsUnsupportedModes["IR"] = modeDescWarn("interferometer reference pointing mode",false);
  itsUnsupportedModes["IA"] = modeDescWarn("interferometer pointing mode A (IF)",false);
  itsUnsupportedModes["IB"] = modeDescWarn("interferometer pointing mode B (IF)",false);
  itsUnsupportedModes["IC"] = modeDescWarn("interferometer pointing mode C (IF)",false);
  itsUnsupportedModes["ID"] = modeDescWarn("interferometer pointing mode D (IF)",false);
  itsUnsupportedModes["JA"] = modeDescWarn("JPL mode A (IF)",false);
  itsUnsupportedModes["JB"] = modeDescWarn("JPL mode B (IF)",false);
  itsUnsupportedModes["JC"] = modeDescWarn("JPL mode C (IF)",false);
  itsUnsupportedModes["JD"] = modeDescWarn("JPL mode D (IF)",false);
  itsUnsupportedModes["PA"] = modeDescWarn("single dish pointing mode A (IF)",false);
  itsUnsupportedModes["PB"] = modeDescWarn("single dish pointing mode B (IF)",false);
  itsUnsupportedModes["PC"] = modeDescWarn("single dish pointing mode C (IF)",false);
  itsUnsupportedModes["PD"] = modeDescWarn("single dish pointing mode D (IF)",false);
  itsUnsupportedModes["TB"] = modeDescWarn("test back-end and front-end",false);
  itsUnsupportedModes["TE"] = modeDescWarn("tipping curve",false);
  itsUnsupportedModes["TF"] = modeDescWarn("test front-end",false);
  itsUnsupportedModes["VS"] = modeDescWarn("single dish VLBI",false);
}

// Local Variables: 
// compile-command: "gmake VLAObsModeFilter"
// End: 
