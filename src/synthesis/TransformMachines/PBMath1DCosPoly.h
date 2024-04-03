//# PBMath1DCosPoly.h: Definitions of 1-D CosPolynomial PBMath objects
//# Copyright (C) 1996,1997,1998,2003
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

#ifndef SYNTHESIS_PBMATH1DCOSPOLY_H
#define SYNTHESIS_PBMATH1DCOSPOLY_H

#include <casacore/casa/aips.h>
#include <synthesis/TransformMachines/PBMath1D.h>
#include <casacore/measures/Measures.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// PBMath1DCosPoly is a 1-D casacore::Polynomial Cosine Expansion for a Primary Beam
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="PBMathInterface">PBMathInterface</linkto> class
// <li> <linkto class="PBMath1D">PBMath1D</linkto> class
// </prerequisite>
//
// <etymology>
// PBMath1DCosPoly: derived from  PBMath1D, implements a polynomial of Cosines of different widths
// </etymology>
//
// <synopsis> 
// PBMath1DCosPoly: a voltage pattern expressed as a
// polynomial of cosines (all powers of them):
// 
// VP(x) = sum_i  coeff_i * cos( scale_i * x )^{i}
// 
// x is in arcminutes, referenced to 1GHz frequency;
// the argument of the cosines are in radians
// </synopsis> 
//
//
// <example>
// <srcblock>
//    casacore::Vector<casacore::Double> cosCoef(4);
//    casacore::Vector<casacore::Double> cosScale(4);
//    cosCoef.set(0.0);
//    cosScale.set(0.0);
//    cosCoef(3) = 1.0;
//    cosScale(3) = 0.01891; // 0.01891 = 0.065 * 1000(MHz/GHz) /(60(arcm/deg)) * 2pi/180
//    PBMath1DCosPoly cosPB(cosCoef, cosScale,
//                          casacore::Quantity(1.0, "deg"), 
//                          casacore::Quantity(1.0, "GHz")  );
//    cosPB.applyPB( im3, im4, pointingDir);
// </srcblock>
// </example>
//
// <motivation>
// All of the 1-D PB types have everything in common except for the
// details of their parameterization.  This lightweight class
// deals with those differences: construction, filling the PBArray
// from construction parameters, and flushing to disk.
// WSRT uses this model.
// </motivation>
//
// <todo asof="98/10/21">
// <li> constructor from a casacore::MS beam subtable
// <li> flush to casacore::MS beam subtable
// </todo>

 
class PBMath1DCosPoly : public PBMath1D {
public:

  PBMath1DCosPoly();

  // Instantiation from arguments; default = no squint
  // squint is the offset from pointing center if the casacore::Stokes R beam
  // useSymmetricBeam forces a fit to the squinted beam
  PBMath1DCosPoly(const casacore::Vector<casacore::Double>& coeff, 
		  const casacore::Vector<casacore::Double>& cosScale, casacore::Quantity maxRad, 
		  casacore::Quantity refFreq, 
		  casacore::Bool isThisVP=false,
		  BeamSquint squint=BeamSquint(casacore::MDirection(casacore::Quantity(0.0, "deg"),
							  casacore::Quantity(0.0, "deg"),
							  casacore::MDirection::Ref(casacore::MDirection::AZEL)),
					       casacore::Quantity(1.0, "GHz")),
		  casacore::Bool useSymmetricBeam=false);

  // Instantiation from a row in the Beam subTable
  // PBMath1DCosPoly(const casacore::Table& BeamSubTable, casacore::Int row,
  //	  casacore::Bool useSymmetricBeam=false);

  // Copy constructor
  // PBMath1DCosPoly(const PBMath1DCosPoly& other);

  // Assignment operator, by reference
  PBMath1DCosPoly& operator=(const PBMath1DCosPoly& other);

  // Clone the object
  //  casacore::CountedPtr<PBMathInterface> clone();

  // destructor
  ~PBMath1DCosPoly();

  // Get the type of PB this is
  PBMathInterface::PBClass whichPBClass() { return PBMathInterface::COSPOLY; }

  // Flush the construction parameters to disk
  // casacore::Bool flushToTable(casacore::Table& beamSubTable, casacore::Int iRow);

  // Summarize the construction data for this primary beam
  void summary(casacore::Int nValues=0);


protected:

  // Fill in vp_p array from construction parameters
  void fillPBArray();

private:    

  casacore::Vector<casacore::Double> coeff_p;
  casacore::Vector<casacore::Double> cosScale_p;

};


} //# NAMESPACE CASA - END

#endif
