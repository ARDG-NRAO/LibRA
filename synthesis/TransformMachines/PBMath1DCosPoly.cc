//# PBMath1DCosPoly.cc: Implementation for PBMath1DCosPoly
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
 
#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicMath/Math.h>
#include <synthesis/TransformMachines/PBMath1DCosPoly.h>
#include <casa/Quanta.h>
#include <measures/Measures.h>
#include <casa/Arrays/ArrayMath.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

PBMath1DCosPoly::PBMath1DCosPoly(const Vector<Double>& coeff, 
				 const Vector<Double>& cosScale,
				 Quantity maxRad,
				 Quantity refFreq, 
				 Bool isThisVP,
				 BeamSquint squint,
				 Bool useSymmetricBeam) :
  PBMath1D(maxRad, refFreq, isThisVP, squint, useSymmetricBeam),
  coeff_p(coeff),
  cosScale_p(cosScale)
{
  cosScale_p = cosScale / fScale_p;
  
  fillPBArray();    

  if (useSymmetricBeam) {
    symmetrizeSquintedBeam();
  }
};




PBMath1DCosPoly::~PBMath1DCosPoly()
{
};


PBMath1DCosPoly& PBMath1DCosPoly::operator=(const PBMath1DCosPoly& other)
{
  if (this == &other)
    return *this;

  PBMath1D::operator=(other);
  coeff_p=other.coeff_p;
  cosScale_p=other.cosScale_p;
  return *this;
};




void PBMath1DCosPoly::fillPBArray()
{

  LogIO os(LogOrigin("PBMath1DCosPoly", "fillPBArray"));
  uInt nSamples=10000;
  vp_p.resize(nSamples);

  inverseIncrementRadius_p=Double(nSamples-1)/maximumRadius_p.getValue("'");
  Double x;
  Double y;

  Double VP;
  //  cout << "Coeff: " << coeff_p << endl;
  //  cout << "Scaling: " << cosScale_p << endl;

  uInt k;
  uInt j;
  for(uInt i=0;i<nSamples;i++) {
    VP = 0.0;    
    for (j=0;j<coeff_p.nelements();j++) {
      x = cos( cosScale_p(j)*Double(i)/inverseIncrementRadius_p );
      if (x != 0) {
	y = 1;
	for (k=0;k<=j;k++) {  // raise x^j power, will handle negatives too
	  y *= x;
	}
	VP +=   coeff_p(j) * y;
      }
    }
    if (isThisVP_p) {
      vp_p(i) = VP;
    } else {
      vp_p(i) = sqrt( VP );
    }
  }
};


//Bool PBMath1DCosPoly::flushToTable(Table& beamSubTable, Int iRow)
//{};

void
PBMath1DCosPoly::summary(Int nValues)
{
  PBMath1D::summary(nValues);
   LogIO os(LogOrigin("PBMath1DCosPoly", "summary"));
   os << "Cosine coefficients: " << coeff_p <<  LogIO::POST;
   os << "Cosine scaling:      " << cosScale_p <<  LogIO::POST;
};

} //# NAMESPACE CASA - END

