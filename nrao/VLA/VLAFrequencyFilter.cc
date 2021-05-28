//# VLAFrequencyFilter.cc:
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

#include <nrao/VLA/VLAFrequencyFilter.h>
#include <nrao/VLA/VLALogicalRecord.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Logging/LogIO.h>
#include <casa/Utilities/Assert.h>
#include <casa/BasicSL/Constants.h>
#include <casa/BasicMath/Math.h>
#include <measures/Measures/MeasConvert.h>
#include <casa/Quanta/MVFrequency.h>
#include <casa/iomanip.h>

VLAFrequencyFilter::VLAFrequencyFilter()
  :itsTopEdge(C::dbl_max),
   itsBottomEdge(0.0)
{
  DebugAssert(ok(), AipsError);
}

VLAFrequencyFilter::VLAFrequencyFilter(const MVFrequency& centreFrequency,
				       const MVFrequency& bandwidth)
  :itsTopEdge(0.0),
   itsBottomEdge(0.0)
{
  const Double halfBandwidth = bandwidth.getValue()/2.0;
  const Double refFreq = centreFrequency.getValue();
  itsTopEdge = refFreq + halfBandwidth;
  itsBottomEdge = refFreq - halfBandwidth;
  DebugAssert(ok(), AipsError);
}

VLAFrequencyFilter::VLAFrequencyFilter(const VLAFrequencyFilter& other) 
  : VLAFilter(),
  itsTopEdge(other.itsTopEdge),
   itsBottomEdge(other.itsBottomEdge)
{
  DebugAssert(ok(), AipsError);
}

VLAFrequencyFilter::~VLAFrequencyFilter() {
  DebugAssert(ok(), AipsError);
}

VLAFrequencyFilter& VLAFrequencyFilter::
operator=(const VLAFrequencyFilter& other) {
  if (this != &other) {
    itsTopEdge = other.itsTopEdge;
    itsBottomEdge = other.itsBottomEdge;
  }
  DebugAssert(ok(), AipsError);
  return *this;
}

void VLAFrequencyFilter::refFrequency(const MVFrequency& refFrequency) {
  const Double halfBandwidth = (itsTopEdge - itsBottomEdge)/2.0;
  const Double refFreq = refFrequency.getValue();
  itsTopEdge = refFreq + halfBandwidth;
  itsBottomEdge = refFreq - halfBandwidth;
  DebugAssert(ok(), AipsError);
}

void VLAFrequencyFilter::bandwidth(const MVFrequency& bandwidth) {
  const Double refFreq = (itsTopEdge - itsBottomEdge)/2.0;
  const Double halfBandwidth = bandwidth/2.0;
  itsTopEdge = refFreq + halfBandwidth;
  itsBottomEdge = refFreq - halfBandwidth;
  DebugAssert(ok(), AipsError);
}

Bool VLAFrequencyFilter::passThru(const VLALogicalRecord& record) const {
  // first workout which CDA's are used in this input record.
  // for each CDA used.
  const VLASDA& sda =  record.SDA();
  for (uInt c = 0; c < 4; c++) {
    if (record.CDA(c).isValid()) {
      const VLAEnum::CDA whichCDA = static_cast<VLAEnum::CDA>(c);
      const Double bottomEdge = sda.edgeFrequency(whichCDA);
      const Double topEdge = bottomEdge + 
	sda.nChannels(whichCDA) * sda.channelWidth(whichCDA);
//       cerr << setprecision(4) << "This record (CDA" << c << "): " 
// 	   << bottomEdge << " - " << topEdge
// 	   << "\trequired range:" << itsBottomEdge 
// 	   << " - " << itsTopEdge;
      if (itsTopEdge > bottomEdge && itsBottomEdge < topEdge) {
// 	cerr << " match" << endl;
	return true;
      }
//       cerr << " NO match" << endl;
    }
  }
  return false;
}

VLAFilter* VLAFrequencyFilter::clone() const {
  DebugAssert(ok(), AipsError);
  VLAFrequencyFilter* tmpPtr = new VLAFrequencyFilter(*this);
  AlwaysAssert(tmpPtr != 0, AipsError);
  return tmpPtr;
}

Bool VLAFrequencyFilter::ok() const {
  // The LogIO class is only constructed if an error is detected for
  // performance reasons. Both function static and file static variables
  // where considered and rejected for this purpose.
  if (!VLAFilter::ok()) return false; 
  if (itsBottomEdge < 0.0) { 
    LogIO logErr(LogOrigin("VLAFrequencyFilter", "ok()"));
    logErr << LogIO::SEVERE 
 	   << "The bottom of the selected frequency range cannot be negative"
 	   << LogIO::POST;
    return false;
  }
  if (near(itsBottomEdge, itsTopEdge)) { 
    LogIO logErr(LogOrigin("VLAFrequencyFilter", "ok()"));
    logErr << LogIO::SEVERE 
 	   << "The bandwidth cannot be zero"
 	   << LogIO::POST;
    return false;
  }
  if (itsBottomEdge > itsTopEdge) { 
    LogIO logErr(LogOrigin("VLAFrequencyFilter", "ok()"));
    logErr << LogIO::SEVERE 
 	   << "The bandwidth cannot be negative"
 	   << LogIO::POST;
    return false;
  }
  return true;
}

// Local Variables: 
// compile-command: "gmake VLAFrequencyFilter"
// End: 
