//# PBMath1DAiry.cc: Implementation for PBMath1DAiry
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
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicMath/Math.h>
#include <synthesis/TransformMachines/PBMath1DAiry.h>
#include <casa/Quanta.h>
#include <measures/Measures.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

PBMath1DAiry::PBMath1DAiry(Quantity dishDiam, Quantity blockedDiam,
			   Quantity maxRad, Quantity refFreq, 
			   BeamSquint squint,
			   Bool useSymmetricBeam) :
  PBMath1D(maxRad, refFreq, true, squint, useSymmetricBeam),
  dishDiam_p(dishDiam),  
  blockedDiam_p(blockedDiam)
{
  fillPBArray();    

  if (useSymmetricBeam) {
    symmetrizeSquintedBeam();
  }
};

PBMath1DAiry::~PBMath1DAiry()
{
};


PBMath1DAiry& PBMath1DAiry::operator=(const PBMath1DAiry& other)
{
  if (this == &other)
    return *this;

  PBMath1D::operator=(other);
  dishDiam_p = other.dishDiam_p;
  blockedDiam_p = other.blockedDiam_p;

  return *this;
};



void PBMath1DAiry::fillPBArray()
{

  LogIO os(LogOrigin("PBMath1DAiry", "fillPBArray"));
  uInt nSamples=10000;
  vp_p.resize(nSamples);

  inverseIncrementRadius_p=Double(nSamples-1)/maximumRadius_p.getValue("'");
  
  // This scales the maximum radius from arcminutes on the sky at
  // 1 GHz for a 24.5 m unblocked aperture to the J1 Bessel function
  // coordinates (7.016 at the 2ns null).
  Double dimensionless_maxRad = maximumRadius_p.getValue("'") * 
    7.016/(1.566*60.) * dishDiam_p.getValue("m") / 24.5;  
  Double dimensionless_iiR = Double(nSamples-1)/dimensionless_maxRad;
  Double x;
  Double bd;
  uInt i;
  bd = blockedDiam_p.getValue("m");
  if (bd == 0.0) {
    vp_p(0) = 1.0;
    for(i=1;i<nSamples;i++) {
      x = i / dimensionless_iiR;
      vp_p(i) = 2.0 * j1(x)/x;
    }
  } else {
    Double areaRatio = square( dishDiam_p.getValue("m")/ blockedDiam_p.getValue("m") );
    Double areaNorm = areaRatio - 1.0;
    Double lengthRatio = dishDiam_p.getValue("m")/ blockedDiam_p.getValue("m");
    vp_p(0) = 1.0;
    //    cout << " areaRat = " << areaRatio << endl;
    //    cout << " areaNrom = " << areaNorm << endl;
    //    cout << " lengthRat = " << lengthRatio << endl;
    for(i=1;i<nSamples;i++) {
      x = i / dimensionless_iiR;
      vp_p(i) = ( areaRatio * 2.0 * j1(x)/x 
		  - 2.0 * j1( x * lengthRatio)/(x*lengthRatio) )/ areaNorm;
    }
  }
};


// Bool PBMath1DAiry::flushToTable(Table& beamSubTable, Int iRow)
//{};

void
PBMath1DAiry::summary(Int nValues)
{
  PBMath1D::summary(nValues);
   LogIO os(LogOrigin("PBMath1DAiry", "summary"));
   os << "   Dish Diameter: " << dishDiam_p.getValue("m") << " m " <<  LogIO::POST;
   os << "Blocked Diameter: " << blockedDiam_p.getValue("m") << " m " <<  LogIO::POST;
};

} //# NAMESPACE CASA - END

