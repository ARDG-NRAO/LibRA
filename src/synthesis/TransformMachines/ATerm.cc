// -*- C++ -*-
//# EVLAAperture.cc: Implementation of the EVLAAperture class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
//
#include <synthesis/TransformMachines/ATerm.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
//
//---------------------------------------------------------------------
//
using namespace casacore;
namespace casa{
  
  ATerm::ATerm()
  : CFTerms()
  {
    Int userValue;

    cachedOverSampling_p=OVERSAMPLING;
    userValue = SynthesisUtils::getenv("ATerm.OVERSAMPLING",OVERSAMPLING);
    if (userValue != cachedOverSampling_p)
      {
	cachedOverSampling_p = userValue;
	//	cerr << "Oversampling set to " << cachedOverSampling_p << endl;
      }


    cachedConvSize_p=CONVSIZE;
    userValue = SynthesisUtils::getenv("ATerm.CONVSIZE",CONVSIZE);
    if (userValue != cachedConvSize_p)
      {
	cachedConvSize_p = userValue;
	//cerr << "ConvFuncSize set to " << cachedConvSize_p << endl;
      }
  };

  Int ATerm::makePBPolnCoords(//const VisBuffer&vb,
			      const Vector<Int>& vbCorrType,
			      const Int& convSize,
			      const Int& convSampling,
			      const CoordinateSystem& skyCoord,
			      const Int& skyNx, const Int& /*skyNy*/,
			      CoordinateSystem& feedCoord)
  //				     Vector<Int>& cfStokes)
  {
    feedCoord = skyCoord;
    //
    // Make a two dimensional image to calculate auto-correlation of
    // the ideal illumination pattern. We want this on a fine grid in
    // the UV plane
    //
    Int directionIndex=skyCoord.findCoordinate(Coordinate::DIRECTION);
    AlwaysAssert(directionIndex>=0, AipsError);
    DirectionCoordinate dc=skyCoord.directionCoordinate(directionIndex);
    Vector<Double> sampling;
    sampling = dc.increment();
    sampling*=Double(convSampling);
    sampling*=Double(skyNx)/Double(convSize);
    dc.setIncrement(sampling);
    
    
    Vector<Double> unitVec(2);
    unitVec=convSize/2;
    dc.setReferencePixel(unitVec);
    
    // Set the reference value to that of the image
    feedCoord.replaceCoordinate(dc, directionIndex);

    //
    // Make an image with circular polarization axis.
    //
    Int NPol=0,M,N=0;
    M=polMap_p_base.nelements();
    for(Int i=0;i<M;i++) if (polMap_p_base(i) > -1) NPol++;
    Vector<Int> poln(NPol);
    
    Int index;
    Vector<Int> inStokes;
    index = feedCoord.findCoordinate(Coordinate::STOKES);
    inStokes = feedCoord.stokesCoordinate(index).stokes();
    N = 0;
    try
      {
	//	cerr << "### " << polMap_p_base << " " << vb.corrType() << endl;
	for(Int i=0;i<M;i++) if (polMap_p_base(i) > -1) {poln(N) = vbCorrType(i);N++;}
	StokesCoordinate polnCoord(poln);
	Int StokesIndex = feedCoord.findCoordinate(Coordinate::STOKES);
	feedCoord.replaceCoordinate(polnCoord,StokesIndex);
	//	cfStokes = poln;
      }
    catch(AipsError& x)
      {
	throw(SynthesisFTMachineError("Likely cause: Discrepancy between the poln. "
				      "axis of the data and the image specifications."));
      }
    
    return NPol;
  }

  inline Int ATerm::getConvSize() 
  {
    return cachedConvSize_p;
    // Int defaultConvSize=CONVSIZE, userValue;
    // userValue = SynthesisUtils::getenv("ATerm.CONVSIZE",CONVSIZE);
    // if (userValue != defaultConvSize)
    //   {
    // 	defaultConvSize = userValue;
    // 	cerr << "ConvFuncSize set to " << defaultConvSize << endl;
    //   }
    // return defaultConvSize;
  };

  inline Int ATerm::getOversampling() 
  {
    return cachedOverSampling_p;
    // Int defaultOversampling=OVERSAMPLING, userValue;
    // userValue = SynthesisUtils::getenv("ATerm.OVERSAMPLING",OVERSAMPLING);
    // if (userValue != defaultOversampling)
    //   {
    // 	defaultOversampling = userValue;
    // 	cerr << "Oversampling set to " << defaultOversampling << endl;
    //   }
    // return defaultOversampling;
  }
};
