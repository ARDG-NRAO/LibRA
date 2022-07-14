// -*- C++ -*-
//# WOnlyConvFunc.cc: Implementation of the WOnlyConvFunc class
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
#include <synthesis/TransformMachines/WOnlyConvFunc.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines/WTerm.h>
#include <synthesis/TransformMachines/PSTerm.h>
#include <casacore/images/Images/ImageInterface.h>
#include <synthesis/TransformMachines/Utils.h>
#include <synthesis/TransformMachines/CFStore.h>
#include <synthesis/TransformMachines/ConvolutionFunction.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/casa/ostream.h>
using namespace casacore;
namespace casa{
  void WOnlyConvFunc::makeConvFunction(const ImageInterface<Complex>& image,
				       const VisBuffer& vb,
				       const Int wConvSize,
				       const Float pa,
				       CFStore& cfs,
				       CFStore& cfwts)
  {
    LogIO log_l(LogOrigin("WOnlyConvFunc", "makeConvFunction"));
    Int convSize=0, convSampling=4;
    Double wScale=1;
    Array<Complex> convFunc_l;
    Double cfRefFreq=-1.0;
    
    Int nx=image.shape()(0);
    
    if(wConvSize>0) 
      {
	log_l << "Using " << wConvSize << " planes for W-projection" << LogIO::POST;
	Double maxUVW;
	maxUVW=0.25/abs(image.coordinates().increment()(0));
	log_l << "Estimating maximum possible W = " << maxUVW
	      << " (wavelengths)" << LogIO::POST;
      
	Double invLambdaC=vb.frequency()(0)/C::c;
	Double invMinL = vb.frequency()((vb.frequency().nelements())-1)/C::c;
	log_l << "wavelength range = " << 1.0/invLambdaC << " (m) to " 
	      << 1.0/invMinL << " (m)" << LogIO::POST;
	if (wConvSize > 1)
	  {
	    wScale=Float((wConvSize-1)*(wConvSize-1))/maxUVW;
	    log_l << "Scaling in W (at maximum W) = " << 1.0/wScale
		  << " wavelengths per pixel" << LogIO::POST;
	  }
      }
    // Get the coordinate system
    //
    CoordinateSystem coords(image.coordinates());
    //
    // Make a two dimensional image to calculate auto-correlation of
    // the ideal illumination pattern. We want this on a fine grid in
    // the UV plane
    //
    Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
    AlwaysAssert(directionIndex>=0, AipsError);
    DirectionCoordinate dc=coords.directionCoordinate(directionIndex);
    Vector<Double> sampling;
    sampling = dc.increment();
    
    sampling*=Double(convSampling);
    sampling*=Double(nx)/Double(convSize);

    dc.setIncrement(sampling);
    
    Vector<Double> unitVec(2);
    unitVec=convSize/2;
    dc.setReferencePixel(unitVec);
    
    // Set the reference value to that of the image
    coords.replaceCoordinate(dc, directionIndex);
    IPosition pbShape(4, convSize, convSize, 1, 1);
    TempImage<Complex> twoDPB(pbShape, coords);

    Int inner=convSize/convSampling, polInUse = 1;
    cfs.data = new Array<Complex>(IPosition(4,convSize,convSize, wConvSize,polInUse));
    convFunc_l.reference(*cfs.data);    convFunc_l=0;
    cfs.resize(wConvSize);

    IPosition start(4, 0, 0, 0, 0);
    IPosition pbSlice(4, convSize, convSize, 1, 1);
    
    Matrix<Complex> screen(convSize, convSize);

    for (Int iw=0;iw<wConvSize;iw++) 
      {
	wTerm_p->applySky(screen, iw, sampling,  wScale, convSize);

	psTerm_p->applySky(screen, /*iw,*/ sampling, /*wScale,*/ inner);

	//
	// Fill the complex image with the w-term...
	//
	IPosition PolnPlane(4,0,0,0,0);
	IPosition ndx(4,0,0,0,0);
	
	for(Int i=0;i<polInUse;i++)
	  {
	    PolnPlane(2)=i;
	    twoDPB.putSlice(screen, PolnPlane);
	  }
	
	Complex cpeak=max(twoDPB.get());
	twoDPB.put(twoDPB.get()/cpeak);
	
	CoordinateSystem cs=twoDPB.coordinates();
	Int index= twoDPB.coordinates().findCoordinate(Coordinate::SPECTRAL);
	SpectralCoordinate SpCS = twoDPB.coordinates().spectralCoordinate(index);
	
	cfRefFreq=SpCS.referenceValue()(0);
	Vector<Double> refValue; refValue.resize(1); refValue(0)=cfRefFreq;
	SpCS.setReferenceValue(refValue);
	cs.replaceCoordinate(SpCS,index);
	//
	// Now FFT and get the result back
	//
	LatticeFFT::cfft2d(twoDPB);
	//
	// Fill the convolution function planes with the result.
	//
	{
	  IPosition start(4, 0, 0, 0, 0),
	    pbSlice(4, twoDPB.shape()[0]-1, twoDPB.shape()[1]-1, polInUse, 1);
	  IPosition sliceStart(4,0,0,iw,0), 
	    sliceLength(4,convFunc_l.shape()[0]-1,convFunc_l.shape()[1]-1,1,polInUse);
	  
	  convFunc_l(Slicer(sliceStart,sliceLength)).nonDegenerate()
	    =(twoDPB.getSlice(start, pbSlice, true));
	}
      }
    //
    // Set various CF parameters
    //
    cfs.sampling(0)=convSampling;
    setSupport(convFunc_l, cfs);

    Int index=coords.findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate spCS = coords.spectralCoordinate(index);
    Vector<Double> refValue; refValue.resize(1);refValue(0)=cfRefFreq;
    spCS.setReferenceValue(refValue);
    coords.replaceCoordinate(spCS,index);

    cfs.coordSys=coords;     

    (void)pa; (void)cfwts; // To suppress compiler warnings
  };
  
  void WOnlyConvFunc::setSupport(Array<Complex>& convFunc, CFStore& cfs)
  {
    Int maxConvSupport=-1, polInUse = 1;
    Int ConvFuncOrigin=convFunc.shape()[0]/2;  // Conv. Func. is half that size of convSize
    IPosition ndx(4,ConvFuncOrigin,0,0,0), cfShape(convFunc.shape());
    
    //UNUSED: Int maxConvWtSupport=0, supportBuffer;
    for (Int iw=0;iw<cfShape[2];iw++)
      {
	Bool found=false;
	Float threshold;
	Int R;
	ndx(2) = iw;
	
	ndx(0)=ndx(1)=ConvFuncOrigin;
	ndx(2) = iw;
	threshold   = abs(convFunc(ndx))*wTerm_p->getSupportThreshold();
	//
	// Find the support size of the conv. function in pixels
	//
	//UNUSED: Int wtR;
	found = findSupport(convFunc,threshold,ConvFuncOrigin,R);
	//
	// Set the support size for each W-plane and for all
	// Pol-planes.  Assuming that support size for all Pol-planes
	// is same.
	//
	if(found) 
	  {
	    //	    Int maxR=R;//max(ndx(0),ndx(1));
	    for(Int ipol=0;ipol<polInUse;ipol++)
	      {
		cfs.xSupport(iw)=cfs.ySupport(iw)=Int(R/cfs.sampling(0));
		cfs.xSupport(iw)=cfs.ySupport(iw)=Int(0.5+Float(R)/cfs.sampling(0))+1;

		if (cfs.maxXSupport == -1)
		  if (cfs.xSupport(iw) > maxConvSupport)
		    maxConvSupport = cfs.xSupport(iw);
	      }
	  }
      }
  }

  //
  //-----------------------------------------------------------------------
  //
  Bool WOnlyConvFunc::findSupport(Array<Complex>& func, Float& threshold,
				  Int& origin, Int& R)
  {
    LogIO log_l(LogOrigin("WOnlyConvFunc", "findSupport"));
    Double NSteps;
    Int PixInc=1;
    Vector<Complex> vals;
    IPosition ndx(4,origin,0,0,0);
    Bool found=false;
    IPosition cfShape=func.shape();
    Int convSize = cfShape(0);
    for(R=convSize/4;R>1;R--)
      {
	NSteps = 90*R/PixInc; //Check every PixInc pixel along a
	//circle of radious R
	vals.resize((Int)(NSteps+0.5));
	vals=0;
	for(Int th=0;th<NSteps;th++)
	  {
	    ndx(0)=(int)(origin + R*sin(2.0*M_PI*th*PixInc/R));
	    ndx(1)=(int)(origin + R*cos(2.0*M_PI*th*PixInc/R));
	    
	    if ((ndx(0) < cfShape(0)) && (ndx(1) < cfShape(1)))
	      vals(th)=func(ndx);
	  }
	if (max(abs(vals)) > threshold)
	  {found=true;break;}
      }
    return found;
  };

};
