//# PBMath1DIPoly.cc: Implementation for PBMath1DIPoly
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
#include <synthesis/TransformMachines/PBMath1DIPoly.h>
#include <casacore/casa/Quanta.h>
#include <casacore/measures/Measures.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

PBMath1DIPoly::PBMath1DIPoly(const Vector<Double>& coeff, Quantity maxRad,
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

PBMath1DIPoly::PBMath1DIPoly(const Matrix<Double>& coeff,
			     const Vector<Double>& freqs, Quantity maxRad,
			     Quantity refFreq, 
			     Bool isThisVP, 
			     BeamSquint squint,
			     Bool useSymmetricBeam) :
  PBMath1D(maxRad, refFreq, isThisVP, squint, useSymmetricBeam),
  wbcoeff_p(coeff)
{
  wFreqs_p=freqs;
  wideFit_p = true;
  if (coeff.ncolumn()!=freqs.nelements()) {
    throw(AipsError("PBMath1DIPoly:: - coeff and freqs arguments do not match"));
  }
  fillPBArray();    

  if (useSymmetricBeam) {
    symmetrizeSquintedBeam();
  }
};



PBMath1DIPoly::~PBMath1DIPoly()
{
};


PBMath1DIPoly& PBMath1DIPoly::operator=(const PBMath1DIPoly& other)
{
  if (this == &other)
    return *this;

  PBMath1D::operator=(other);
  coeff_p = other.coeff_p;
  wbcoeff_p = other.wbcoeff_p;
  return *this;
};




void PBMath1DIPoly::fillPBArray()
{

  LogIO os(LogOrigin("PBMath1DIPoly", "fillPBArray"));
  uInt nSamples=10000;
  vp_p.resize(nSamples);
  Int nfreq = wFreqs_p.nelements();
  bool wide = nfreq>0;
  if (wide) wbvp_p.resize(nSamples,nfreq);
  if (wide) os << "Using wideband interpolated beam pattern"<<LogIO::NORMAL;

  inverseIncrementRadius_p=Double(nSamples-1)/maximumRadius_p.getValue("'");
  Double x2;   Double y;
  Double taper;
  for(Int n=0; n<max(1,nfreq); n++) {
    for(uInt i=0;i<nSamples;i++) {
      taper = 0.0;
      x2 = square( (Double(i)/inverseIncrementRadius_p) );
      y = 1;
      if (wide) {
	for (uInt j=0; j<wbcoeff_p.nrow(); j++) {
	  taper += y * wbcoeff_p(j,n);
	  y *= x2;
	}
      } else {
	for (uInt j=0;j<coeff_p.nelements();j++) {
	  taper += y * coeff_p(j);
	  y *= x2;
	}
      }
      if (taper != 0.0) {
	if (isThisVP_p) {
	  taper = 1.0/taper;
	} else {
	  taper = 1.0/sqrt(taper);
	}
      }
      if (wide) {
	wbvp_p(i,n) = taper;
      } else {
	vp_p(i) = taper;
      }
    }
  }
};


// Bool PBMath1DIPoly::flushToTable(Table& beamSubTable, Int iRow)
// {};


void
PBMath1DIPoly::summary(Int nValues)
{
  PBMath1D::summary(nValues);
  LogIO os(LogOrigin("PBMath1DIPoly", "summary"));
  Int nfreq = wFreqs_p.nelements();	
  if (nfreq==0) {
    os << "Even Powered Coefficients: " << coeff_p <<  LogIO::POST;
  } else {
    for (Int i=0; i<nfreq; i++) {
      os << " Even Powered Coefficients for "<<wFreqs_p(i)/1e9<<" GHz: "<<wbcoeff_p.column(i)
         << endl;
    }
    os << LogIO::POST;
  }
};

} //# NAMESPACE CASA - END

