//# MSContinuumSubtractor.h: Fit & subtract continuum from spectral line data
//# Copyright (C) 2004
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
//#
#ifndef MS_MSCONTINUUMSUBTRACTOR_H
#define MS_MSCONTINUUMSUBTRACTOR_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
namespace casacore{

class MeasurementSet;
class LogIO;
}

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>Fits and subtracts or models the continuum in spectra</summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 
// <prerequisite>
//   <li> <linkto class=casacore::MeasurementSet>MeasurementSet</linkto>
// </prerequisite>
//
// <etymology>
// This class's main aim is to subtract the continuum from spectral line data.
// </etymology>
//
// <synopsis>
// Spectral line observations often contain continuum emission which is
// present in all channels (often with small slope across band). This
// class fits this continuum and subtracts it, replacing either the
// corrected data or the model data column.
// </synopsis>
//
// <example>
// <srcBlock>
//     casacore::MS inMS(fileName);
//     MSContinuumSubtractor mssub(inMS);
//     mssub.setDataDescriptionIds(ddIds);
//     mssub.setFields(fieldIds);
//     mssub.setSolutionInterval(10.0);
//     mssub.setMode("subtract");
//     mssub.subtract();
// </srcBlock>
// A <src>MSContinuumSubtractor</src> object is constructed 
// and the continuum is subtracted with a 10.0s averaging time for the fit.
// </example>
//
// <motivation>
// This class replaces existing functionality at the glish level, in an
// attempt to speed up the continuum subtraction process.
// </motivation>
//
// <todo asof="">
// </todo>
 

class MSContinuumSubtractor
{
public:
// Constructor
   MSContinuumSubtractor (casacore::MeasurementSet& ms);

// Assignment (only copies reference to casacore::MS, need to reset selection etc)
  MSContinuumSubtractor& operator=(MSContinuumSubtractor& other);

// Destructor
  ~MSContinuumSubtractor();

// Set the required field Ids
   void setField(const casacore::String& field);
   void setFields(const casacore::Vector<casacore::Int>& fieldIds);

// Set the channels to use in the fit
   void setFitSpw(const casacore::String& fitspw);
   void setSubSpw(const casacore::String& subspw);
// Set the required spws (ddids)
   void setDataDescriptionIds(const casacore::Vector<casacore::Int>& ddIds);

// Set the solution interval in seconds, the value zero implies scan averaging
   void setSolutionInterval(casacore::Float solInt);

// Set the solution interval in seconds, the value zero implies scan averaging
   void setSolutionInterval(casacore::String solInt);

// Set the order of the fit (1=linear)
   void setOrder(casacore::Int order);

// Set the processing mode: subtract, model or replace
   void setMode(const casacore::String& mode);

// Do the subtraction (or save the model)
   void subtract();
   void subtract2();

private:
// Pointer to MS
   casacore::MeasurementSet* ms_p;
// DataDescription Ids to process
   casacore::Vector<casacore::Int> itsDDIds;
// Field Ids to process
   casacore::Vector<casacore::Int> itsFieldIds;
// Channels to use in fit
   casacore::Matrix<casacore::Int> itsFitChans;
// Channels to subtract from
   casacore::Matrix<casacore::Int> itsSubChans;
// Solution interval for fit
   casacore::Float itsSolInt;
// Order of the fit
   casacore::Int itsOrder;
// Processing mode
   casacore::String itsMode;
// Number of spws
   casacore::Int nSpw_;


};


} //# NAMESPACE CASA - END

#endif
