//# PBMath1DGauss.cc: Implementation for PBMath1DGauss
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
 
#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/BasicMath/Math.h>
#include <synthesis/TransformMachines/PBMath1DGauss.h>
#include <casacore/measures/Measures.h>
#include <casacore/casa/Utilities/Assert.h>




using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

PBMath1DGauss::PBMath1DGauss(Quantity halfWidth, Quantity maxRad, Quantity refFreq, 
			     Bool isThisVP,
			     BeamSquint squint,
			     Bool useSymmetricBeam) :
  PBMath1D(maxRad, refFreq, isThisVP, squint, useSymmetricBeam),
  halfWidth_p(halfWidth)
{

  // convert instantiation parameters to GHz*arcmin reference
  halfWidth_p = halfWidth_p * fScale_p;

  // fill the vp_p arrays
  fillPBArray();    

  if (useSymmetricBeam) {
    symmetrizeSquintedBeam();
  }
};


PBMath1DGauss::~PBMath1DGauss()
{
};


PBMath1DGauss& PBMath1DGauss::operator=(const PBMath1DGauss& other)
{
  if (this == &other)
    return *this;

  PBMath1D::operator=(other);
  halfWidth_p = other.halfWidth_p;
  return *this;
};



void PBMath1DGauss::fillPBArray()
{

  LogIO os(LogOrigin("PBMath1DGauss", "fillPBArray"));
  Int nSamples=10000;
  vp_p.resize(nSamples); 

  inverseIncrementRadius_p=Double(nSamples-1)/maximumRadius_p.getValue("'");
  
  Double fact = sqrt(1.0/log(2.0)) * halfWidth_p.getValue("'");
  // square (Math.h) mysteriously comes up as an undefined reference 
  Double fact2 = fact*fact;
  Double iir2 = inverseIncrementRadius_p * inverseIncrementRadius_p;
  
  for(Int i=0;i<nSamples;i++) {
    if (isThisVP_p) {
      vp_p(i) = exp(-(  ((Double)(i*i)) / iir2  / fact2) );
    } else {
      vp_p(i) = sqrt( exp(-(  ((Double)(i*i)) / iir2  / fact2) ) );
    }
  }

};

void
PBMath1DGauss::summary(Int nValues)
{
  PBMath1D::summary(nValues);
   LogIO os(LogOrigin("PBMath1DGauss", "summary"));
   os << "Gaussian HWHM: " << halfWidth_p.getValue("'") << " arcmin" <<  LogIO::POST;
};


} //# NAMESPACE CASA - END

