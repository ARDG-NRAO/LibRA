//# PBMath1DNumeric.cc: Implementation for PBMath1DNumeric
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
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <synthesis/TransformMachines/PBMath1DNumeric.h>
#include <casacore/casa/Quanta.h>
#include <casacore/measures/Measures.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

PBMath1DNumeric::PBMath1DNumeric(const Vector<Float>& numericArray,
				 Quantity maxRad, Quantity refFreq, 
				 Bool isThisVP,
				 BeamSquint squint,
				 Bool useSymmetricBeam) :
  PBMath1D(maxRad, refFreq, isThisVP, squint, useSymmetricBeam),
  numericArray_p(numericArray)
{
  fillPBArray();    

  if (useSymmetricBeam) {
    symmetrizeSquintedBeam();
  }
};




PBMath1DNumeric::~PBMath1DNumeric()
{
};


PBMath1DNumeric& PBMath1DNumeric::operator=(const PBMath1DNumeric& other)
{
  if (this == &other)
    return *this;

  PBMath1D::operator=(other);
  numericArray_p = other.numericArray_p;
  return *this;
};




void PBMath1DNumeric::fillPBArray()
{

  LogIO os(LogOrigin("PBMath1DNumeric", "fillPBArray"));
  Int nSamples=10000;
  vp_p.resize(nSamples);
  Vector<Float> vp_temp(2*nSamples); 

  inverseIncrementRadius_p=Double(nSamples-1)/maximumRadius_p.getValue("'");

  // Sinc interpolation to fill up vp_p from numericArray_p:
  {


    Vector<Float> beam2(2*(numericArray_p.nelements()-1));
    beam2 = 0.0;
    uInt n2 = beam2.nelements();
    uInt i;
    for (i=0; i<numericArray_p.nelements()-1;i++) {
      beam2(n2/2+i) = numericArray_p(i);
      beam2(n2/2-i) = numericArray_p(i);
    }
    beam2(0) = numericArray_p(numericArray_p.nelements()-1);

    Vector<Complex> fft2;


    FFTServer<Float, Complex> server(IPosition(1, beam2.nelements()));
    server.fft0(fft2, beam2);
    
    // Now 0-pad fft2
    Vector<Complex> fft2pad(nSamples+1); 

    for (i=0; i<fft2pad.nelements();i++) {
      fft2pad(i) = 0.0;
    }

    for (i=0; i<fft2.nelements();i++) {
      fft2pad(i) = fft2(i);
    }

    // and FT back into vp_temp
    server.fft0(vp_temp, fft2pad);

    uInt j;
    Float taper;
    for (i=vp_temp.nelements()/2,j=0; i< vp_temp.nelements();i++,j++) {
      taper = vp_temp(i)/vp_temp(vp_temp.nelements()/2);
      if (isThisVP_p) {
	vp_p(j) =  taper;
      } else {
	// this will have sign problems!  should do this via vp, not pb!
	if (taper <= 0.0) {
	  vp_p(j) = 0.0;
	} else {
	  vp_p(j) = sqrt(taper);
	}
      }
    }

  }

};


//Bool PBMath1DNumeric::flushToTable(Table& beamSubTable, Int iRow)
//{};


void
PBMath1DNumeric::summary(Int nValues)
{
  PBMath1D::summary(nValues);
   LogIO os(LogOrigin("PBMath1DNumeric", "summary"));
   for (uInt i=0; i <  numericArray_p.nelements(); i++) {
     os << "Numeric values: " << i << "   " << numericArray_p(i) <<  LogIO::POST;
   }
};

  Int PBMath1DNumeric::support (const CoordinateSystem& cs){

    Int value=PBMath1D::support(cs);
    //As the maxRadius is determines where the numeric value stop...the support should be a bit larger
    value=Int(Double(value)*0.55)*2;
    return value;


  };
  

} //# NAMESPACE CASA - END

