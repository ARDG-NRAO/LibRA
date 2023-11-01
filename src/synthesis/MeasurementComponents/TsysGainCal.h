//# TsysGainCal.h: Declaration of Tsys calibration
//# Copyright (C) 1996,1997,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#

#ifndef SYNTHESIS_TSYSGAINCAL_H
#define SYNTHESIS_TSYSGAINCAL_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <synthesis/MeasurementComponents/VisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/StandardVisCal.h>
// not yet:#include <synthesis/MeasurementComponents/CalCorruptor.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
class VisEquation;
//not yet: class TJonesCorruptor;


// **********************************************************
//  Standard Tsys Spectrum from SYSCAL table
//

class StandardTsys : public BJones {
public:

  // Constructor
  StandardTsys(VisSet& vs);
  StandardTsys(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  StandardTsys(const MSMetaInfoForCal& msmc);
  StandardTsys(const casacore::Int& nAnt);

  virtual ~StandardTsys();

  // Return the type enum (for now, pretend we are B)
  virtual Type type() { return VisCal::B; };

  // Return type name as string (ditto)
  virtual casacore::String typeName()     { return "B TSYS"; };
  virtual casacore::String longTypeName() { return "B TSYS (freq-dep Tsys)"; };

  // Tsys are casacore::Float parameters
  virtual VisCalEnum::VCParType parType() { return VisCalEnum::REAL; };

  // Local setSpecify
  using BJones::setSpecify;
  virtual void setSpecify(const casacore::Record& specify);

  // Specific specify() that reads the SYSCAL subtable
  virtual void specify(const casacore::Record& specify);

  // In general, we are freq-dep
  virtual casacore::Bool freqDepPar() { return freqDepTsys_; };

  // Specialized to turn on spectral weight calibration
  virtual void correct2(vi::VisBuffer2& vb, casacore::Bool trial=false, 
			casacore::Bool doWtSp=false, casacore::Bool dosync=true);

protected:

  // The Jones matrix elements are not the parameters
  //  ( j = sqrt(p) )
  virtual casacore::Bool trivialJonesElem() { return false; };

  // Specialized calcPar that does some sanity checking
  virtual void calcPar();

  // Invert doInv for Tsys corrections
  virtual void syncJones(const casacore::Bool& doInv) { BJones::syncJones(!doInv); };
  
  // Calculate Jones matrix elements from Tsys (j = sqrt(p) )
  virtual void calcAllJones();

  // Local version that arrange channelized correction
  virtual void syncWtScale();

  // Calculate weight scale
  virtual void calcWtScale();
  virtual void calcWtScale2();  // called by local syncWtScale only

  using BJones::keepNCT;
  virtual void keepNCT(const casacore::Vector<casacore::Int>& ants);

private:

  // The name of the SYSCAL table
  casacore::String sysCalTabName_;

  // Signal formation of channelized weight calibration
  casacore::Bool freqDepCalWt_;

  // Signal presence of channelized Tsys data
  casacore::Bool freqDepTsys_;

  // <nothing>


};

} //# NAMESPACE CASA - END

#endif

