//# GJonesPoly.h: Electronic gain Jones matrix as polynomials over time
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
//# $Id: GJonesPoly.h,v 19.7 2004/11/30 17:50:48 ddebonis Exp $

#ifndef SYNTHESIS_GSPLINE_H
#define SYNTHESIS_GSPLINE_H

#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/StandardVisCal.h>
#include <synthesis/CalTables/GJonesMBuf.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> Electronic gain Jones matrix as spline polynomial over time
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto module="GJones">GJones</linkto> module
//   <li> <linkto module="GJonesPoly">GJonesPoly</linkto> module
// </prerequisite>
//
// <etymology>
// GJonesSpline describes the electronic gain visibility Jones matrix 
// parametrized as a spline polynomial over time.
// </etymology>
//
// <synopsis> 
//
// See <linkto class="VisEquation">VisEquation</linkto> for definition of the
// Visibility Measurement Equation.
//
// The elements of GJonesSpline are only non-zero on the diagonal.
//
// </synopsis> 
//
// <motivation>
// Support electronic gain calibration using splines polynomials over time.
// </motivation>
//
// <todo asof="02/02/01">
// i) Support solution and pre-averaging intervals
// </todo>

class VisEquation;

class GJonesSpline : public GJones {
public:

  // Construct from a visibility data set
  GJonesSpline (VisSet& vs);

  // Construct from a MSMetaInfoForCal
  GJonesSpline (const MSMetaInfoForCal& msmc);

  // Destructor
  virtual ~GJonesSpline();

  // Return the type enum
  virtual Type type() { return VisCal::G; };

  // Return type name as a string
  virtual casacore::String typeName()     { return "GSPLINE"; };
  virtual casacore::String longTypeName() { return "G Jones SPLINE (elec. gain)"; };


  // GSpline gathers/solves for itself
  virtual casacore::Bool useGenericGatherForSolve() { return false; };

  // Type of Jones matrix according to nPar()
  // TBD:
  virtual Jones::JonesType jonesType() { return Jones::Diagonal; };

  // Set the solver parameters
  using GJones::setSolve;
  virtual void setSolve(const casacore::Record& solvepar);

  // Set the interpolation parameters
  using GJones::setApply;
  virtual void setApply(const casacore::Record& applypar);

  // Solve
  virtual void selfGatherAndSolve (VisSet& vs, VisEquation& ve);

/*
  // Set raw phase transfer from another spw
  void setRawPhaseVisSet(VisSet& rawvs_p);

  // Set parameters for phase wrapping resolution
  void setPhaseWrapHelp(const casacore::Int& numpoi, const casacore::Double& phaseWrap);
*/
  // Expose calculating pars for plotting
  inline void doCalcPar() { calcPar(); }

 protected:

  // GSPLINE has one trivial casacore::Complex parameter (single pol, for now)
  // TBD:
  virtual casacore::Int nPar() { return 2; };

  // Calc G pars from spline info
  virtual void calcPar();

 private:

  // The underlying VisSet
  VisSet* vs_p;

  // Private variables containing the solver parameters
  casacore::Bool solveAmp_p, solvePhase_p;
  casacore::Double splinetime_p;

  // casacore::Time for which the current calibration cache is valid
  //casacore::Double cacheTimeValid_p;

  // Ptr to a calibration buffer associated with the calibration
  // solutions which are to be interpolated and applied to the data
  GJonesSplineMBuf* calBuffer_p;


  // Parameters for raw phase removal from another spw
  //VisSet* rawvs_p;
  //casacore::Bool rawPhaseRemoval_p;
  std::map<casacore::String, casacore::Int> timeValueMap_p;
  casacore::Matrix<casacore::Double> rawPhase_p;

  // Solution timestamp
  casacore::Double solTimeStamp_p;


  // Create and fill an empty output calibration buffer
  void newCalBuffer (const casacore::Vector<casacore::Int>& fieldIdKeys, 
		     const casacore::Vector<casacore::Int>& antennaId);

  // Compute the number of spline knots required and their location
  casacore::Int getKnots (const casacore::Vector<casacore::Double>& times, casacore::Vector<casacore::Double>& knots);

  // Update the output calibration table
  virtual void updateCalTable (const casacore::Vector<casacore::Int>& fieldIdKeys,
			       const casacore::Vector<casacore::Int>& antennaId,
			       const casacore::Vector<casacore::String>& freqGrpName,
			       const casacore::Vector<casacore::String>& polyType,
			       const casacore::Vector<casacore::String>& polyMode,
			       const casacore::Vector<casacore::Complex>& scaleFactor,
			       const casacore::Matrix<casacore::Double>& polyCoeffAmp,
			       const casacore::Matrix<casacore::Double>& polyCoeffPhase,
			       const casacore::Vector<casacore::String>& phaseUnits,
			       const casacore::Vector<casacore::Double>& splineKnotsAmp,
			       const casacore::Vector<casacore::Double>& splineKnotsPhase,
			       const casacore::Vector<casacore::MFrequency>& refFreq,
			       const casacore::Vector<casacore::Int>& refAnt);
  
  // Compute a spline polynomial value
  casacore::Double getSplineVal (casacore::Double x, casacore::Vector<casacore::Double>& knots,
		       casacore::Vector<casacore::Double>& coeff);


/*
  // fill the raw phase buffer for usage
  void fillRawPhaseBuff();

  // return the rawphase of 
  casacore::Double getRawPhase(casacore::Int ant1, casacore::Int ant2, casacore::Double time);

*/

  //Plot solutions as compare with data
  void plotsolve(const casacore::Vector<casacore::Double>& x, 
		 const casacore::Matrix<casacore::Double>& yall, 
		 const casacore::Matrix<casacore::Double>& weightall, 
		 const casacore::Vector<casacore::Double>& errall, 
		 casacore::Matrix<casacore::Double>& coeff, casacore::Bool phasesoln);

  // Return all field id.'s in the underlying MS
  casacore::Vector<casacore::Int> fieldIdRange();

  //Logging solution and rms
  void writeAsciiLog(const casacore::String& filename, const casacore::Matrix<casacore::Double>& coeff, const casacore::Vector<casacore::Double>& rmsFit, casacore::Bool phasesoln);

  casacore::Int numpoint_p;
  casacore::Double phaseWrap_p;


};


} //# NAMESPACE CASA - END

#endif


