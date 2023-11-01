//# PBMath1DPoly.cc: Implementation for PBMath1DPoly
//# Copyright (C) 1996,1997,1998,1999,2003
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
#include <synthesis/TransformMachines/PBMath1DPoly.h>
#include <casacore/casa/Quanta.h>
#include <casacore/measures/Measures.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

PBMath1DPoly::PBMath1DPoly(const Vector<Double>& coeff, Quantity maxRad,
			   Quantity refFreq, 
			   Bool isThisVP,
			   BeamSquint squint,
			   Bool useSymmetricBeam) :
  PBMath1D(maxRad, refFreq, isThisVP, squint, useSymmetricBeam),
  coeff_p(coeff)
{
  fillPBArray();    

  if (useSymmetricBeam) {
    symmetrizeSquintedBeam();
  }
};



PBMath1DPoly::~PBMath1DPoly()
{
};


PBMath1DPoly& PBMath1DPoly::operator=(const PBMath1DPoly& other)
{
  if (this == &other)
    return *this;

  PBMath1D::operator=(other);
  coeff_p = other.coeff_p;
  return *this;
};



void PBMath1DPoly::fillPBArray()
{

  LogIO os(LogOrigin("PBMath1DPoly", "fillPBArray"));
  uInt nSamples=10000;
  vp_p.resize(nSamples);

  inverseIncrementRadius_p=Double(nSamples-1)/maximumRadius_p.getValue("'");
  Double x2;   
  Double y;
  Double taper;

  for(uInt i=0;i<nSamples;i++) {
    taper = 0.0;
    x2 = square( (Double(i)/inverseIncrementRadius_p) );
    y = 1;
    for (uInt j=0;j<coeff_p.nelements();j++) {
      taper += y * coeff_p(j);
      y *= x2;
    }
    if (isThisVP_p) {
      vp_p(i) = taper;
    } else {
      vp_p(i) = sqrt(taper);
    }
  }
};


//Bool PBMath1DPoly::flushToTable(Table& beamSubTable, Int iRow)
//{};


void
PBMath1DPoly::summary(Int nValues)
{
  PBMath1D::summary(nValues);
   LogIO os(LogOrigin("PBMath1DPoly", "summary"));
   os << "Even Powered Coefficients: " << coeff_p <<  LogIO::POST;
};

} //# NAMESPACE CASA - END

