//# PBMath1DIPoly.h: Definitions of 1-D Inverse casacore::Polynomial PBMath objects
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

#ifndef SYNTHESIS_PBMATH1DIPOLY_H
#define SYNTHESIS_PBMATH1DIPOLY_H

#include <casa/aips.h>
#include <synthesis/TransformMachines/PBMath1D.h>
#include <measures/Measures.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//#forward

// <summary> 
// PBMath1DIPoly is a 1-D Inverse casacore::Polynomial Expansion for a Primary Beam
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="PBMathInterface">PBMathInterface</linkto> class
// <li> <linkto class="PBMath1D">PBMath1D</linkto> class
// </prerequisite>
//
// <etymology>
// PBMath1DIPoly: derived from  PBMath1D, implements an Inverse casacore::Polynomial PB and VP
// </etymology>
//
// <synopsis> 
//  See PBMath1D for a general synopsis of the 1D PB types.
//
// VP(x) = sum_i  coeff_i *  x ^{2i}, 
// out to maximumRadius (referenced to the reference frequency)
// 
// x is in arcminutes, referenced to the reference frequency
//
// This move may make me a bit unpopular: since the PBMath1D constructors
// construct a Voltage Pattern, the coeficienct for a polynomial type must
// be in terms of the VP, not the PB.  Hence, go back to the original PB
// data, take the square root, and fit the same type polynomial to the
// VP data.
//
// Inverse polynomial VP model: fit polynomial to 1/(VP_data).
// Optionally, you can fit to 1/(VP_data) - 1, ignoring the constant
// polynomial term so that the PB is constrained to be 1.0 at the beam
// center.
// </synopsis> 
//
//
// <example>
// <srcblock>
//    coef(0) = 1.0;     / VLA model
//    coef(1) = 7.22110e-04; 
//    coef(2) = 1.21421e-07;
//    coef(3) = 9.68612e-11; 
//    coef(4) = 1.86268e-13;
//    PBMath1DIPoly ipolyPB( coef, casacore::Quantity(43.0,"'"), casacore::Quantity(1.0,"GHz"));
//
// </srcblock>
// </example>
//
// <motivation>
// All of the 1-D PB types have everything in common except for the
// details of their parameterization.  This lightweight class
// deals with those differences: construction, filling the PBArray
// from construction parameters, and flushing to disk.
// The inverse polynomial PB type is used for VLA and ATNF models.
// </motivation>
//
// <todo asof="98/10/21">
// <li> constructor from a casacore::MS beam subtable
// <li> flush to casacore::MS beam subtable
// </todo>

 
class PBMath1DIPoly : public PBMath1D {
public:

  PBMath1DIPoly() {};

  // Instantiation from arguments; default = no squint
  // squint is the offset from pointing center if the casacore::Stokes R beam
  // useSymmetricBeam forces a fit to the squinted beam
  PBMath1DIPoly(const casacore::Vector<casacore::Double>& coeff, casacore::Quantity maxRad, 
		casacore::Quantity refFreq, 
		casacore::Bool isThisVP=false,
		BeamSquint squint=BeamSquint(casacore::MDirection(casacore::Quantity(0.0, "deg"),
							casacore::Quantity(0.0, "deg"),
							casacore::MDirection::Ref(casacore::MDirection::AZEL)),
					     casacore::Quantity(1.0, "GHz")),
		casacore::Bool useSymmetricBeam=false);

  // Instantiation from arguments; default = no squint
  // squint is the offset from pointing center if the casacore::Stokes R beam
  // useSymmetricBeam forces a fit to the squinted beam
  // Each column in coeff corresponds to a frequency in freqs
  PBMath1DIPoly(const casacore::Matrix<casacore::Double>& coeff, const casacore::Vector<casacore::Double>& freqs,
		casacore::Quantity maxRad, 
		casacore::Quantity refFreq, 
		casacore::Bool isThisVP=false,
		BeamSquint squint=BeamSquint(casacore::MDirection(casacore::Quantity(0.0, "deg"),
							casacore::Quantity(0.0, "deg"),
							casacore::MDirection::Ref(casacore::MDirection::AZEL)),
					     casacore::Quantity(1.0, "GHz")),
		casacore::Bool useSymmetricBeam=false);

  // Instantiation from a row in the Beam subTable
  // PBMath1DIPoly(const casacore::Table& BeamSubTable, casacore::Int row,
  // 	       casacore::Bool useSymmetricBeam=false);

  // Copy constructor
  // PBMath1DIPoly(const PBMath1DIPoly& other);

  // Assignment operator, by reference
  PBMath1DIPoly& operator=(const PBMath1DIPoly& other);

  // Clone the object
  //  casacore::CountedPtr<PBMathInterface> clone();


  ~PBMath1DIPoly();  

  // Get the type of PB this is
  PBMathInterface::PBClass whichPBClass() { return PBMathInterface::IPOLY; }  

  // Flush the construction parameters to disk
  // casacore::Bool flushToTable(casacore::Table& beamSubTable, casacore::Int iRow);

  // Summarize the construction data for this primary beam
  void summary(casacore::Int nValues=0);

protected:

  // Fill in vp_p array from construction parameters
  void fillPBArray();

private:    

  casacore::Vector<casacore::Double> coeff_p;
  casacore::Matrix<casacore::Double> wbcoeff_p;

};


} //# NAMESPACE CASA - END

#endif
