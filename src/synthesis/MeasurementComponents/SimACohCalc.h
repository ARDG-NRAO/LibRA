//# SimACohCalc.h: Definition for Simulated additive errors, calculated from obs. parms
//# Copyright (C) 1996,1997,1999,2000
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
//# $Id$

#ifndef SYNTHESIS_SIMACOHCALC_H
#define SYNTHESIS_SIMACOHCALC_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicMath/Random.h>
#include <synthesis/MeasurementComponents/ACoh.h>
#include <casacore/casa/Quanta/Quantum.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// SimACohCalc: Model additive noise errors for the VisEquation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto module="MeasurementComponents">MeasurementComponents</linkto> module
//   <li> <linkto class="VisSet">VisSet</linkto> class
//   <li> <linkto class="VisEquation">VisEquation</linkto> class
//   <li> <linkto class="SimACoh">SimACoh</linkto> class
// </prerequisite>
//
// <etymology>
// SimACohCalc describes random additive errors to be used in
// the <linkto class="VisEquation">VisEquation</linkto>, calculated from parameters
// such as bandwidth, system temperature, integration time, dish diameter, and efficiencies. 
// </etymology>
//
// <synopsis> 
//
// See <linkto class="VisEquation">VisEquation</linkto> for definition of the
// Visibility Measurement Equation. See <linkto class="ACoh">ACoh</linkto>
// for how SimACohCalc is to be used.
// </synopsis> 
//
// <motivation>
// The properties of an additive component must be described
// for the <linkto class="VisEquation">VisEquation</linkto>.
// </motivation>
//
// <todo asof="97/10/01">
// </todo>

class SimACohCalc  : public ACoh
{
public:
  // Construct from VisSet, seed and rms of additive noise (normal dist.)
  // NOTE: rms is for the tau = 0.0 case, and is augmented as
  // appropriate given tau, tatmos, trx (ie, all NON-atmospheric noise
  // contributions), and airmass= 1/sin(ELEVATION).
  // If tau = 0.0, this defaults to the SimACoh case.
  SimACohCalc(const casacore::Int seed=1, 
	      const casacore::Float antefficiency=0.80, 
	      const casacore::Float correfficiency=0.85,  
	      const casacore::Float spillefficiency=0.85,
	      const casacore::Float tau=0.0, 
	      const casacore::Quantity& trx=50,
	      const casacore::Quantity& tatmos=250,
	      const casacore::Quantity& tcmb=2.7);
 
  // virtual destructor
  virtual ~SimACohCalc();

  // Apply additive error (Inverse is minus)
  // <group>
  virtual VisBuffer& apply(VisBuffer& vb);
  virtual VisBuffer& applyInv(VisBuffer& vb);
  // </group>
private:
  casacore::Bool solve(VisEquation& /*ve*/) {return true;}
  SimACohCalc();

  casacore::MLCG rndGen_p;
  casacore::Normal noiseDist_p;  
  casacore::Float antefficiency_p;
  casacore::Float correfficiency_p;
  casacore::Float spillefficiency_p;
  casacore::Float tau_p;
  casacore::Quantity trx_p;
  casacore::Quantity tatmos_p;
  casacore::Quantity tcmb_p;



};


} //# NAMESPACE CASA - END

#endif
