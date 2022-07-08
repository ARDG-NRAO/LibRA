//# PBMath1DNumeric.h: Definitions of 1-D Numeric PBMath objects
//# Copyright (C) 1996,1997,1998,2000,2003
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

#ifndef SYNTHESIS_PBMATH1DNUMERIC_H
#define SYNTHESIS_PBMATH1DNUMERIC_H

#include <casacore/casa/aips.h>
#include <synthesis/TransformMachines/PBMath1D.h>
#include <casacore/measures/Measures.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//#forward

// <summary> 1-D Numeric Primary Beam Model </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="PBMathInterface">PBMathInterface</linkto> class
// <li> <linkto class="PBMath1D">PBMath1D</linkto> class
// </prerequisite>
//
// <etymology>
//  PBMath1DNumeric: derived from  PBMath1D, implements a numeric PB and VP
// </etymology>
//
// <synopsis> 
// See PBMath1D for a general synopsis of the 1D PB types.
// 
// The user supplies a vector which is a numerical representation
// of a voltage [attern (hey, if you have a PB, just take the square
// root, and look out for sidelobes which could be negative).
// The first element in the vector needs to be 1.0, the center of the
// voltage pattern.  The last element of the vector is the value of the
// VP at the maximumRadius.  The maximumRadius and the reference frequency at
// which the tabulated VP is intended are also required for construction.
// The PBMath1DNumeric constructor proceeds by performing SINC interpolation
// on the input vector to generate the highly oversampled lookup vector.
// 
// </synopsis> 
//
//
// <example>
// <srcblock>
//    casacore::Vector<casacore::Float> vp(10);
//    vp(0) = 1.0f;
//    vp(1) = 0.932f;
//    vp(2) = 0.7462f;
//    vp(3) = 0.4914f;
//    vp(4) = 0.2308f;
//    vp(5) = 0.02183f;   // first null
//    vp(6) = -0.1005f;
//    vp(7) = -0.1318f;
//    vp(8) = -0.09458f;
//    vp(9) = -0.0269f;
//    casacore::Quantity maxRad(1.032,"deg");  
//    casacore::Quantity refFreq(1.414, "GHz");
//    PBMath1DNumeric numPB (vp, maxRad, refFreq);
//    numPB.applyPB( im1, im2, pointingDir);
// </srcblock>
// </example>
//
// <motivation>
// All of the 1-D PB types have everything in common except for the
// details of their parameterization.  This lightweight class
// deals with those differences: construction, filling the PBArray
// from construction parameters, and flushing to disk.
// The Numeric type is very handy: someone can take a sample
// illumination pattern, FT, and take a slice of the resulting voltage
// pattern and construct a Numerical VP from that slice.
// </motivation>
//
// <todo asof="98/10/21">
// <li> constructor from a casacore::MS beam subtable
// <li> flush to casacore::MS beam subtable
// </todo>

 
class PBMath1DNumeric : public PBMath1D {
public:

  PBMath1DNumeric();

  // Instantiation from arguments; default = no squint
  // squint is the offset from pointing center if the casacore::Stokes R beam
  // useSymmetricBeam forces a fit to the squinted beam
  PBMath1DNumeric(const casacore::Vector<casacore::Float>& numericArray, casacore::Quantity maxRad, casacore::Quantity refFreq, 
		  casacore::Bool isThisVP=false,
		  BeamSquint squint=BeamSquint(casacore::MDirection(casacore::Quantity(0.0, "deg"),
							  casacore::Quantity(0.0, "deg"),
							  casacore::MDirection::Ref(casacore::MDirection::AZEL)),
					       casacore::Quantity(1.0, "GHz")),
		  casacore::Bool useSymmetricBeam=false);

  // Instantiation from a row in the Beam subTable
  // PBMath1DNumeric(const casacore::Table& BeamSubTable, casacore::Int row, 
  //	casacore::Bool useSymmetricBeam=false);

  // Copy constructor
  // PBMath1DGNumeric(const PBMath1DNumeric& other);

  // Assignment operator, by reference
  PBMath1DNumeric& operator=(const PBMath1DNumeric& other);

  //destructor
 ~PBMath1DNumeric();  

  // Get the type of PB this is
  PBMathInterface::PBClass whichPBClass() { return PBMathInterface::NUMERIC; }  
  
  // Flush the construction parameters to disk
  // casacore::Bool flushToTable(casacore::Table& beamSubTable, casacore::Int iRow);

  // Summarize the construction data for this primary beam
  void summary(casacore::Int nValues=0);

  virtual casacore::Int support (const casacore::CoordinateSystem& cs);

protected:

  // Fill in vp_p array from construction parameters
  void fillPBArray();

private:    

 casacore::Vector<casacore::Float> numericArray_p;

};


} //# NAMESPACE CASA - END

#endif
