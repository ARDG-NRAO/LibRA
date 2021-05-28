//# BJonesPoly.h: A bandpass visibility Jones matrix of polynomial form
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# $Id: BJonesPoly.h,v 19.9 2004/11/30 17:50:47 ddebonis Exp $

#ifndef SYNTHESIS_BPOLY_H
#define SYNTHESIS_BPOLY_H

#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/StandardVisCal.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> A bandpass visibility Jones matrix of polynomial form
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto module="BJones">BJones</linkto> module
// </prerequisite>
//
// <etymology>
// BJonesPoly describes a bandpass visibility Jones matrix of
// polynomial form.
// </etymology>
//
// <synopsis> 
//
// See <linkto class="VisEquation">VisEquation</linkto> for definition of the
// Visibility Measurement Equation.
//
// The elements of BJonesPoly are only non-zero on the diagonal.
//
// </synopsis> 
//
// <motivation>
// Support polynomial bandpass calibration.
// </motivation>
//
// <todo asof="02/02/01">
// i) Support solution intervals
// </todo>

class VisEquation;
class VisBuffGroupAcc;

class BJonesPoly : public BJones {
public:

  // Construct from a visibility data set
  BJonesPoly (VisSet& vs);

  // Construct from a MSMetaInfoForCal
  BJonesPoly (const MSMetaInfoForCal& msmc);

  // Destructor
  virtual ~BJonesPoly() {};

  // Return the type enum
  virtual Type type() { return VisCal::B; };

  // Return type name as string
  virtual casacore::String typeName()     { return "BPOLY"; };
  virtual casacore::String longTypeName() { return "B Jones Poly (bandpass)"; };

  // BPOLY gathers generically...
  virtual casacore::Bool useGenericGatherForSolve() { return true; };
  // ...then solves for itself per solution:
  virtual casacore::Bool useGenericSolveOne() { return false; };

  // Type of Jones matrix according to nPar()
  virtual Jones::JonesType jonesType() { return Jones::Diagonal; };

  // Set the solver parameters
  using BJones::setSolve;
  virtual void setSolve(const casacore::Record& solvepar);

  // Set the interpolation parameters
  using BJones::setApply;
  virtual void setApply(const casacore::Record& applypar);

  // Solve 
  //   (old self-directed gather and solve)
  //  virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve);
  //   (new per-solution self-solving inside generic gather)
  virtual void selfSolveOne(VisBuffGroupAcc& vbga);


protected:

  // BPOLY has two trivial casacore::Complex parameter (formed at fill)
  virtual casacore::Int nPar() { return 2; };

  // Calculate current parameters
  virtual void calcPar();

  virtual void loadMemCalTable (casacore::String applyTable,casacore::String field);

private:

  // The underlying VisSet
  VisSet* vs_p;

  // Private variables containing the solver parameters
  casacore::Int degamp_p, degphase_p;
  casacore::Bool visnorm_p;
  casacore::Int maskcenter_p;
  casacore::Float maskedge_p;

  // Derived solver parameters
  // Center mask half width (in channels)
  casacore::Int maskcenterHalf_p;
  // Fractional edge mask
  casacore::Float maskedgeFrac_p;

  // Some meta info
  casacore::Double solTimeStamp;
  casacore::Int solSpwId;
  casacore::Int solFldId;

  // CalDescId per spw
  casacore::Vector<casacore::Int> calDescId_p;

  // Determine if a given channel is masked
  casacore::Bool maskedChannel (const casacore::Int& chan, const casacore::Int& nChan);

  // Update the output calibration table to include the
  // current solution parameters
  void updateCalTable (const casacore::String& freqGrpName, 
		       const casacore::Vector<casacore::Int>& antennaId,
		       const casacore::Vector<casacore::String>& polyType, 
		       const casacore::Vector<casacore::Complex>& scaleFactor,
		       const casacore::Matrix<casacore::Double>& validDomain,
		       const casacore::Matrix<casacore::Double>& polyCoeffAmp,
		       const casacore::Matrix<casacore::Double>& polyCoeffPhase,
		       const casacore::Vector<casacore::String>& phaseUnits,
		       const casacore::Vector<casacore::Complex>& sideBandRef,
		       const casacore::Vector<casacore::MFrequency>& refFreq, 
		       const casacore::Vector<casacore::Int>& refAnt);

  // Compute a casacore::Chebyshev polynomial using the CLIC library
  casacore::Double getChebVal (const casacore::Vector<casacore::Double>& coeff, const casacore::Double& xinit,
		     const casacore::Double& xfinal, const casacore::Double& x);

  // Load bandpass parameters from a calibration table and
  // pre-compute the corrections (and their inverse)
  //  void load (const casacore::String& applyTable);

  // Utility function to return the bandwidth-weighted average 
  // frequency for a set of spectral window id.'s
  casacore::Double meanFrequency (const casacore::Vector<casacore::Int>& spwids);

  // Utility function to return the frequency group name for a given spw. id.
  casacore::String freqGrpName (const casacore::Int& spwId);

  // Utility function to return the spw id.'s in a given freq. group
  casacore::Vector<casacore::Int> spwIdsInGroup (const casacore::String& freqGrpName);

  // Utility function to return the frequency axis for a given spw. id.
  casacore::Vector<casacore::Double> freqAxis (const casacore::Int& spwId);

  void plotsolve2(const casacore::Vector<casacore::Double>& x, 
		  const casacore::Matrix<casacore::Double>& ampdata, 
		  const casacore::Matrix<casacore::Double>& phadata, 
		  const casacore::Matrix<casacore::Double>& wtdata, 
		  const casacore::Vector<casacore::Int>& ant1idx, const casacore::Vector<casacore::Int>& ant2idx, 
		  const casacore::Vector<casacore::Double>& amperr, casacore::Matrix<casacore::Double>& ampcoeff, 
		  const casacore::Vector<casacore::Double>& phaerr, casacore::Matrix<casacore::Double>& phacoeff) ;

};


} //# NAMESPACE CASA - END

#endif
