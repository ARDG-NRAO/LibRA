//# EVLASwPow.h: Declaration of EVLA Switched Power Calibration
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

#ifndef SYNTHESIS_EVLASWPOW_H
#define SYNTHESIS_EVLASWPOW_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <synthesis/MeasurementComponents/VisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/StandardVisCal.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
class VisEquation;


// **********************************************************
//  EVLA switched power Gain and Tsys
//


class EVLASwPow : public GJones {
public:

  enum SPType{SWPOW,EVLAGAIN=SWPOW,RQ,SWPOVERRQ,NONE};
  
  static SPType sptype(casacore::String name);
  static casacore::String sptype(SPType sptype);

  // Constructor
  EVLASwPow(VisSet& vs);
  EVLASwPow(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  EVLASwPow(const MSMetaInfoForCal& msmc);
  EVLASwPow(const casacore::Int& nAnt);

  virtual ~EVLASwPow();

  // Return the type enum (for now, pretend we are G)
  virtual Type type() { return VisCal::G; };

  // EVLA Gain and Tsys are casacore::Float parameters
  virtual VisCalEnum::VCParType parType() { return VisCalEnum::REAL; };

  // Return type name as string (ditto)
  virtual casacore::String typeName()     { return "G EVLASWPOW"; };
  virtual casacore::String longTypeName() { return "G EVLASWPOW (Switched-power gain)"; };

  // Local setSpecify
  using GJones::setSpecify;
  virtual void setSpecify(const casacore::Record& specify);

  // Specific specify() that reads the SYSCAL subtable
  virtual void specify(const casacore::Record& specify);

  // In general, we are freq-dep
  virtual casacore::Bool freqDepPar() { return false; };


protected:

  // There are 4 parameters (Gain and Tsys for each pol)
  virtual casacore::Int nPar() { return 4; };  

  // The parameter array is not (just) the Jones matrix element array
  virtual casacore::Bool trivialJonesElem() { return false; };

  // Calculate Jones matrix elements (slice out the gains)
  virtual void calcAllJones();

  // Synchronize the weight-scaling factors
  //  Weights are multiplied by 1/Tsys(K) per antenna
  virtual void syncWtScale();

  // Experimenting with updateWt
  //virtual void updateWt(casacore::Matrix<casacore::Float>& wt,const casacore::Int& a1,const casacore::Int& a2);

private:

  // Fill the Tcals from the CALDEVICE table
  void fillTcals();

  // The name of the SYSCAL table
  casacore::String sysPowTabName_,calDevTabName_;

  // Tcal storage
  casacore::Cube<casacore::Float> tcals_;

  // Digital factors for the EVLA
  casacore::Float correff_;      // net corr efficiency (lossy)
  casacore::Float frgrotscale_;  // fringe rotation scale (lossless)
  //casacore::Float nyquist_;      // 2*dt*dv

  // Effective per-chan BW, per spw for weight calculation
  casacore::Vector<casacore::Double> effChBW_;

};




} //# NAMESPACE CASA - END

#endif

