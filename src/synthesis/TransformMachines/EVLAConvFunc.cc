// -*- C++ -*-
//# EVLAConvFunc.cc: Implementation of the EVLAConvFunc class
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
#include <synthesis/TransformMachines/EVLAConvFunc.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines/BeamCalc.h>
#include <synthesis/TransformMachines/WTerm.h>
//
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// TEMPS
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//
#define CONVSIZE (1024*2)
#define CONVWTSIZEFACTOR 1.0
#define OVERSAMPLING 20
#define THRESHOLD 1E-4

using namespace casacore;
namespace casa{
  
  EVLAConvFunc& EVLAConvFunc::operator=(const EVLAConvFunc& other)
  {
    if(this!=&other) 
      {
	ConvolutionFunction::operator=(other);
	logIO_p = other.logIO_p;
	//	setParams(other.polMap_p, other.feedStokes_p);
	setPolMap(other.polMap_p);
	bandID_p=other.bandID_p;
	Diameter_p=other.Diameter_p;
	Nant_p=other.Nant_p;
	HPBW=other.HPBW;
	sigma=other.sigma;
      }
    return *this;
  }
  Int EVLAConvFunc::getVLABandID(Double& freq,String&telescopeName)
  {
    if (telescopeName=="VLA")
      {
	if ((freq >=1.34E9) && (freq <=1.73E9))
	  return BeamCalc_VLA_L;
	else if ((freq >=4.5E9) && (freq <=5.0E9))
	  return BeamCalc_VLA_C;
	else if ((freq >=8.0E9) && (freq <=8.8E9))
	  return BeamCalc_VLA_X;
	else if ((freq >=14.4E9) && (freq <=15.4E9))
	  return BeamCalc_VLA_U;
	else if ((freq >=22.0E9) && (freq <=24.0E9))
	  return BeamCalc_VLA_K;
	else if ((freq >=40.0E9) && (freq <=50.0E9))
	  return BeamCalc_VLA_Q;
	else if ((freq >=100E6) && (freq <=300E6))
	  return BeamCalc_VLA_4;
      }
    else 
      if (telescopeName=="EVLA")
	{
	  if ((freq >=0.6E9) && (freq <=2.0E9))
	    return BeamCalc_EVLA_L;
	  else if ((freq >=2.0E9) && (freq <=4.0E9))
	    return BeamCalc_EVLA_S;
	  else if ((freq >=4.0E9) && (freq <=8.0E9))
	    return BeamCalc_EVLA_C;
	  else if ((freq >=8.0E9) && (freq <=12.0E9))
	    return BeamCalc_EVLA_X;
	  else if ((freq >=12.0E9) && (freq <=18.0E9))
	    return BeamCalc_EVLA_U;
	  else if ((freq >=18.0E9) && (freq <=26.5E9))
	    return BeamCalc_EVLA_K;
	  else if ((freq >=26.5E9) && (freq <=40.8E9))
	    return BeamCalc_EVLA_A;
	  else if ((freq >=4.0E9) && (freq <=50.0E9))
	    return BeamCalc_EVLA_Q;
	}
    ostringstream mesg;
    mesg << telescopeName << "/" << freq << "(Hz) combination not recognized.";
    throw(SynthesisError(mesg.str()));
  }
  
  void EVLAConvFunc::setPolMap(const Vector<Int>& polMap) 
  {polMap_p.resize(0);polMap_p=polMap;};
  void EVLAConvFunc::setFeedStokes(const Vector<Int>& feedStokes) 
  {feedStokes_p.resize(0);feedStokes_p=feedStokes;};
  
  int EVLAConvFunc::getVisParams(const VisBuffer& vb)
  {
    Double Freq;

    Vector<String> telescopeNames=vb.msColumns().observation().telescopeName().getColumn();
    for(uInt nt=0;nt<telescopeNames.nelements();nt++)
      {
	if ((telescopeNames(nt) != "VLA") && (telescopeNames(nt) != "EVLA"))
	  {
	    String mesg="We can handle only (E)VLA antennas for now.\n";
	    mesg += "Erroneous telescope name = " + telescopeNames(nt) + ".";
	    SynthesisError err(mesg);
	    throw(err);
	  }
	if (telescopeNames(nt) != telescopeNames(0))
	  {
	    String mesg="We do not (yet) handle inhomogeneous arrays for A-Projection!\n";
	    mesg += "Not yet a \"priority\"!!";
	    SynthesisError err(mesg);
	    throw(err);
	  }
      }
    //    MSSpWindowColumns mssp(vb.msColumns().spectralWindow());
    Freq = vb.msColumns().spectralWindow().refFrequency()(0);
    Diameter_p=0;
    Nant_p     = vb.msColumns().antenna().nrow();
    for (Int i=0; i < Nant_p; i++)
      if (!vb.msColumns().antenna().flagRow()(i))
	{
	  Diameter_p = vb.msColumns().antenna().dishDiameter()(i);
	  break;
	}
    if (Diameter_p == 0)
      {
	logIO() << LogOrigin("EVLAConvFunc", "getVisParams")
		<< "No valid or finite sized antenna found in the antenna table. "
		<< "Assuming diameter = 25m."
		<< LogIO::WARN
		<< LogIO::POST;
	Diameter_p=25.0;
      }
    
    Double Lambda=C::c/Freq;
    HPBW = Lambda/(Diameter_p*sqrt(log(2.0)));
    sigma = 1.0/(HPBW*HPBW);
    //    awEij.setSigma(sigma);
    Int bandID = getVLABandID(Freq,telescopeNames(0));
    return bandID;
  }
  
  Int EVLAConvFunc::makePBPolnCoords(const VisBuffer&vb,
				     const Vector<Int>& polMap,
				     const Int& convSize,
				     const Int& convSampling,
				     const CoordinateSystem& skyCoord,
				     const Int& skyNx, const Int& /*skyNy*/,
				     CoordinateSystem& feedCoord,
				     Vector<Int>& cfStokes)
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
    M=polMap.nelements();
    for(Int i=0;i<M;i++) if (polMap(i) > -1) NPol++;
    Vector<Int> poln(NPol);
    
    Int index;
    Vector<Int> inStokes;
    index = feedCoord.findCoordinate(Coordinate::STOKES);
    inStokes = feedCoord.stokesCoordinate(index).stokes();
    N = 0;
    try
      {
	for(Int i=0;i<M;i++) if (polMap(i) > -1) {poln(N) = vb.corrType()(i);N++;}
	StokesCoordinate polnCoord(poln);
	Int StokesIndex = feedCoord.findCoordinate(Coordinate::STOKES);
	feedCoord.replaceCoordinate(polnCoord,StokesIndex);
	cfStokes = poln;
      }
    catch(AipsError& x)
      {
	throw(SynthesisFTMachineError("Likely cause: Discrepancy between the poln. "
				      "axis of the data and the image specifications."));
      }
    
    return NPol;
  }
  
  Bool EVLAConvFunc::findSupport(Array<Complex>& func, Float& threshold,Int& origin, Int& R)
  {
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
  }
  
  void EVLAConvFunc::makeConvFunction(const ImageInterface<Complex>& image,
				      const VisBuffer& vb,
				      const Int wConvSize,
				      //				      const Vector<Int>& polMap,
				      Float pa,
				      Float dpa,
				      //				      Vector<Int>& cfStokes,
				      CFStore& cfs,
				      CFStore& cfwts,Bool /*fillCF*/)
  {
    LogIO log_l(LogOrigin("EVLAConvFunc", "makeConvFunction"));
    Int convSize, convSampling, polInUse;
    Double wScale=1;
    Array<Complex> convFunc_l, convWeights_l;

    (void)dpa;
    
    Int nx=image.shape()(0);
    if (bandID_p == -1) bandID_p=getVisParams(vb);
    
    log_l << "Making a new convolution function for PA="
	  << pa*(180/C::pi) << "deg"
	  << LogIO::NORMAL << LogIO::POST;
    
    if(wConvSize>0) {
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
    //  
    // Get the coordinate system
    //
    CoordinateSystem coords(image.coordinates());
    
    //
    // Set up the convolution function. 
    //
    if(wConvSize>0) 
      {
	if(wConvSize>256) 
	  {
	    convSampling=4;
	    convSize=min(nx,512);
	  }
	else 
	  {
	    convSampling=4;
	    convSize=min(nx,2048);
	  }
      }
    else 
      {
	convSampling=4;
	convSize=nx;
      }
    convSampling=OVERSAMPLING;
    convSize=CONVSIZE;
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
    //    sampling /= Double(2.0);
    
    sampling*=Double(convSampling);
    sampling*=Double(nx)/Double(convSize);
    
    // cerr << "Sampling on the sky = " << dc.increment() << " " << nx << "x" << ny << endl;
    // cerr << "Sampling on the PB  = " << sampling << " " << convSize << "x" << convSize << endl;
    dc.setIncrement(sampling);
    
    Vector<Double> unitVec(2);
    unitVec=convSize/2;
    dc.setReferencePixel(unitVec);
    
    // Set the reference value to that of the image
    coords.replaceCoordinate(dc, directionIndex);
    
    //
    // Make an image with circular polarization axis.  Return the
    // no. of vis. poln. planes that will be used in making the user
    // defined Stokes image.
    //
    polInUse=makePBPolnCoords(vb, polMap_p, convSize, convSampling, 
			      image.coordinates(),nx,nx,
			      coords,feedStokes_p);
    //------------------------------------------------------------------
    //
    // Make the sky Stokes PB.  This will be used in the gridding
    // correction
    //
    //------------------------------------------------------------------
    IPosition pbShape(4, convSize, convSize, polInUse, 1);
    TempImage<Complex> twoDPB(pbShape, coords);
    
    IPosition pbSqShp(pbShape);
    //    pbSqShp[0] *= 2;    pbSqShp[1] *= 2;
    
    unitVec=pbSqShp[0]/2;
    dc.setReferencePixel(unitVec);
    // sampling *= Double(2.0);
    // dc.setIncrement(sampling);
    coords.replaceCoordinate(dc, directionIndex);
    
    TempImage<Complex> twoDPBSq(pbSqShp,coords);
    //    twoDPB.setMaximumCacheSize(cachesize);
    twoDPB.set(Complex(1.0,0.0));
    //    twoDPBSq.setMaximumCacheSize(cachesize);
    twoDPBSq.set(Complex(1.0,0.0));
    //
    // Accumulate the various terms that constitute the gridding
    // convolution function.
    //
    //------------------------------------------------------------------
    
    Int inner=convSize/convSampling;
    //    inner = convSize/2;
    
    // Vector<Double> cfUVScale(3,0),cfUVOffset(3,0);
    
    // cfUVScale(0)=Float(twoDPB.shape()(0))*sampling(0);
    // cfUVScale(1)=Float(twoDPB.shape()(1))*sampling(1);
    // cfUVOffset(0)=Float(twoDPB.shape()(0))/2;
    // cfUVOffset(1)=Float(twoDPB.shape()(1))/2;
    // // cerr << wScale << " " << cfUVScale << endl;
    // // cerr << uvOffset << " " << cfUVOffset << endl;
    // // ConvolveGridder<Double, Complex>
    // //   //      ggridder(IPosition(2, inner, inner), cfUVScale, cfUVOffset, "SF");
    // //   ggridder(IPosition(2, inner, inner), uvScale, uvOffset, "SF");
    
    // // convFuncCache[PAIndex] = new Array<Complex>(IPosition(4,convSize/2,convSize/2,
    // // 							  wConvSize,polInUse));
    // // convWeightsCache[PAIndex] = new Array<Complex>(IPosition(4,convSize/2,convSize/2,
    // // 							     wConvSize,polInUse));
    // // convFuncCache[PAIndex] = new Array<Complex>(IPosition(4,convSize,convSize,
    // // 							  wConvSize,polInUse));
    // // convWeightsCache[PAIndex] = new Array<Complex>(IPosition(4,convSize,convSize,
    // //    							     wConvSize,polInUse));
    cfs.data = new Array<Complex>(IPosition(4,convSize,convSize,
					    wConvSize,polInUse));
    cfwts.data = new Array<Complex>(IPosition(4,convSize,convSize,
					 wConvSize,polInUse));
    convFunc_l.reference(*cfs.data);
    convWeights_l.reference(*cfwts.data);
    convFunc_l=0;
    convWeights_l=0.0;
    cfs.resize(wConvSize);
    cfwts.resize(wConvSize);

    IPosition start(4, 0, 0, 0, 0);
    IPosition pbSlice(4, convSize, convSize, 1, 1);
    
    Matrix<Complex> screen(convSize, convSize);
    //    if (paChangeDetector.changed(vb,0)) paChangeDetector.update(vb,0);
    VLACalcIlluminationConvFunc vlaPB;
    WTerm wterm;
    Long cachesize=(HostInfo::memoryTotal(true)/8)*1024;
    vlaPB.setMaximumCacheSize(cachesize);
    
    for (Int iw=0;iw<wConvSize;iw++) 
      {
	screen = 1.0;
	
	/*
	  screen=0.0;
	  // First the spheroidal function
	  //
	  //      inner=convSize/2;
	  //	screen = 0.0;
	  Vector<Complex> correction(inner);
	  for (Int iy=-inner/2;iy<inner/2;iy++) 
	  {
	  ggridder.correctX1D(correction, iy+inner/2);
	  for (Int ix=-inner/2;ix<inner/2;ix++) 
	  screen(ix+convSize/2,iy+convSize/2)=correction(ix+inner/2);
	  // if (iy==0)
	  //   for (Int ii=0;ii<inner;ii++)
	  // 	cout << ii << " " << correction(ii) << endl;
	  }
	*/
	//
	// Now the w term
	//
	wterm.applySky(screen, iw, sampling, wScale, inner);
	//
	// Fill the complex image with the w-term...
	//
	IPosition PolnPlane(4,0,0,0,0);
	IPosition ndx(4,0,0,0,0);
	
	for(Int i=0;i<polInUse;i++)
	  {
	    PolnPlane(2)=i;
	    twoDPB.putSlice(screen, PolnPlane);
	    twoDPBSq.putSlice(screen, PolnPlane);
	  }
	//
	// Apply the PB...
	//
	Bool doSquint=true;
	Double pa=getPA(vb);
	vlaPB.applyPB(twoDPB, pa, bandID_p, doSquint);
	doSquint = false;
	//	vlaPB.applyPBSq(twoDPBSq, vb, bandID_p, doSquint);
	vlaPB.applyPB(twoDPBSq, pa, bandID_p, doSquint);
	/*
	// 	twoDPB.put(abs(twoDPB.get()));
	// 	twoDPBSq.put(abs(twoDPBSq.get()));
	*/
	
	// {
	//   String name("twoDPB.before.im");
	//   storeImg(name,twoDPB);
	// }
	
	Complex cpeak=max(twoDPB.get());
	twoDPB.put(twoDPB.get()/cpeak);
	cpeak=max(twoDPBSq.get());
	twoDPBSq.put(twoDPBSq.get()/cpeak);
	//	twoDPBSq.set(1.0);
	// {
	//   String name("twoDPB.im");
	//   storeImg(name,twoDPB);
	// }
	
	CoordinateSystem cs=twoDPB.coordinates();
	Int index= twoDPB.coordinates().findCoordinate(Coordinate::SPECTRAL);
	SpectralCoordinate SpCS = twoDPB.coordinates().spectralCoordinate(index);
	
	Double cfRefFreq=SpCS.referenceValue()(0);
	Vector<Double> refValue; refValue.resize(1); refValue(0)=cfRefFreq;
	SpCS.setReferenceValue(refValue);
	cs.replaceCoordinate(SpCS,index);
	//
	// Now FFT and get the result back
	//
	// {
	//   String name("twoDPB.im");
	//   storeImg(name,twoDPB);
	// }
	LatticeFFT::cfft2d(twoDPB);
	LatticeFFT::cfft2d(twoDPBSq);
	// {
	//   String name("twoDPBFT.im");
	//   storeImg(name,twoDPB);
	// }
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
	  
	  IPosition shp(twoDPBSq.shape());
	  
	  IPosition sqStart(4, 0, 0, 0, 0),
	    pbSqSlice(4, shp[0]-1, shp[1]-1, polInUse, 1);
	  IPosition sqSliceStart(4,0,0,iw,0), 
	    sqSliceLength(4,shp[0]-1,shp[1]-1,1,polInUse);
	  
	  convWeights_l(Slicer(sqSliceStart,sqSliceLength)).nonDegenerate()
	    =(twoDPBSq.getSlice(sqStart, pbSqSlice, true));
	  
	}
      }
    
    {
      Complex cpeak = max(convFunc_l);
      convFunc_l/=cpeak;
      cpeak=max(convWeights_l);
      convWeights_l/=cpeak;
    }
    //
    // Find the convolution function support size.  No assumption
    // about the symmetry of the conv. func. can be made (except that
    // they are same for all poln. planes).
    //
    
    Int maxConvSupport=-1;
    Int ConvFuncOrigin=convFunc_l.shape()[0]/2;  // Conv. Func. is half that size of convSize
    IPosition ndx(4,ConvFuncOrigin,0,0,0);
    
    Int maxConvWtSupport=0, supportBuffer;
    for (Int iw=0;iw<wConvSize;iw++)
      {
	Bool found=false;
	Float threshold;
	Int R;
	ndx(2) = iw;
	
	ndx(0)=ndx(1)=ConvFuncOrigin;
	ndx(2) = iw;
	//	Complex maxVal = max(convFunc);
	threshold = abs(convFunc_l(ndx))*THRESHOLD;
	//
	// Find the support size of the conv. function in pixels
	//
	Int wtR;
	found = findSupport(convWeights_l,threshold,ConvFuncOrigin,wtR);
	found = findSupport(convFunc_l,threshold,ConvFuncOrigin,R);
	
	//	R *=2.5;
	//
	// Set the support size for each W-plane and for all
	// Pol-planes.  Assuming that support size for all Pol-planes
	// is same.
	//
	if(found) 
	  {
	    //	    Int maxR=R;//max(ndx(0),ndx(1));
	    cfs.sampling(0)=cfwts.sampling(0)=convSampling;
	    for(Int ipol=0;ipol<polInUse;ipol++)
	      {
		cfs.xSupport(iw)=cfs.ySupport(iw)=Int(R/cfs.sampling(0));
		cfs.xSupport(iw)=cfs.ySupport(iw)=Int(0.5+Float(R)/cfs.sampling(0))+1;

		cfwts.xSupport(iw)=cfwts.ySupport(iw)=Int(R*CONVWTSIZEFACTOR/
							  cfwts.sampling(0));
		cfwts.xSupport(iw)=cfwts.ySupport(iw)=Int(0.5+Float(R)*CONVWTSIZEFACTOR/
							  cfwts.sampling(0))+1;

		if (cfs.maxXSupport == -1)
		  if (cfs.xSupport(iw) > maxConvSupport)
		    maxConvSupport = cfs.xSupport(iw);
		  maxConvWtSupport=cfwts.xSupport(iw);//HOW CAN THIS BE RIGHT!!!!
	      }
	  }
      }
    
    if(cfs.xSupport(0)<1) 
      log_l << "Convolution function is misbehaved - support seems to be zero"
	      << LogIO::EXCEPTION;
    
    log_l << "Re-sizing the convolution functions"
	  << LogIO::POST;
    
    {
      supportBuffer = OVERSAMPLING;
      Int bot=(Int)(ConvFuncOrigin-cfs.sampling[0]*maxConvSupport-supportBuffer),//-convSampling/2, 
	top=(Int)(ConvFuncOrigin+cfs.sampling[0]*maxConvSupport+supportBuffer);//+convSampling/2;
      bot = max(0,bot);
      top = min(top, convFunc_l.shape()(0)-1);
      {
	Array<Complex> tmp;
	IPosition blc(4,bot,bot,0,0), trc(4,top,top,wConvSize-1,polInUse-1);
	//
	// Cut out the conv. func., copy in a temp. array, resize the
	// CFStore.data, and copy the cutout version to CFStore.data.
	//
	tmp = convFunc_l(blc,trc);
	cfs.data->resize(tmp.shape());
	*cfs.data = tmp; 
	convFunc_l.reference(*cfs.data);
      }
      
      supportBuffer = (Int)(OVERSAMPLING*CONVWTSIZEFACTOR);
      bot=(Int)(ConvFuncOrigin-cfwts.sampling[0]*maxConvWtSupport-supportBuffer);
      top=(Int)(ConvFuncOrigin+cfwts.sampling[0]*maxConvWtSupport+supportBuffer);
      bot=max(0,bot);
      top=min(top,convWeights_l.shape()(0)-1);
      {
      	Array<Complex> tmp;
      	IPosition blc(4,bot,bot,0,0), trc(4,top,top,wConvSize-1,polInUse-1);
      
      	tmp = convWeights_l(blc,trc);
	cfwts.data->resize(tmp.shape());
	*cfwts.data = tmp;
	convWeights_l.reference(*cfwts.data);
      }
    }    
    
    //
    // Normalize such that plane 0 sums to 1 (when jumping in steps of
    // convSampling).  This is really not necessary here since we do
    // the normalizing by the area more accurately in the gridder
    // (fpbwproj.f).
    //
    ndx(2)=ndx(3)=0;
    
    
    Complex pbSum=0.0;
    IPosition peakPix(ndx);
    
    Int Nx = convFunc_l.shape()(0), Ny=convFunc_l.shape()(1);
    
    for(Int nw=0;nw<wConvSize;nw++)
      for(Int np=0;np<polInUse;np++)
	{
	  ndx(2) = nw; ndx(3)=np;
	  {
	    //
	    // Locate the pixel with the peak value.  That's the
	    // origin in pixel co-ordinates.
	    //
	    Float peak=0;
	    peakPix = 0;
	    for(ndx(1)=0;ndx(1)<convFunc_l.shape()(1);ndx(1)++)
	      for(ndx(0)=0;ndx(0)<convFunc_l.shape()(0);ndx(0)++)
		if (abs(convFunc_l(ndx)) > peak) {peakPix = ndx;peak=abs(convFunc_l(ndx));}
	  }
	  
	  ConvFuncOrigin = peakPix(0);
	  //	  ConvFuncOrigin = convFunc.shape()(0)/2+1;
	  //	  Int thisConvSupport=convSampling*convSupport(nw,np,lastPASlot);
	  Int thisConvSupport=cfs.xSupport(nw);
	  pbSum=0.0;
	  
	  for(Int iy=-thisConvSupport;iy<thisConvSupport;iy++)
	    for(Int ix=-thisConvSupport;ix<thisConvSupport;ix++)
	      {
		ndx(0)=ix*(Int)cfs.sampling[0]+ConvFuncOrigin;
		ndx(1)=iy*(Int)cfs.sampling[0]+ConvFuncOrigin;
		pbSum += real(convFunc_l(ndx));
	      }
	  if(pbSum>0.0)  
	    {
	      //
	      // Normalize each Poln. plane by the area under its convfunc.
	      //
	      Nx = convFunc_l.shape()(0), Ny = convFunc_l.shape()(1);
	      for (ndx(1)=0;ndx(1)<Ny;ndx(1)++) 
		for (ndx(0)=0;ndx(0)<Nx;ndx(0)++) 
		  {
		    convFunc_l(ndx) /= pbSum;
		  }
	      
	      Nx = convWeights_l.shape()(0); Ny = convWeights_l.shape()(1);
	      for (ndx(1)=0;  ndx(1)<Ny;  ndx(1)++) 
		for (ndx(0)=0;  ndx(0)<Nx;  ndx(0)++) 
		  {
		    convWeights_l(ndx) /= pbSum*pbSum;
		  }
	    }
	  else 
	    throw(SynthesisFTMachineError("Convolution function integral is not positive"));
	  
	  Vector<Float> maxVal(convWeights_l.shape()(2));
	  Vector<IPosition> posMax(convWeights_l.shape()(2));
	  SynthesisUtils::findLatticeMax(convWeights_l,maxVal,posMax); 
	}
    
    Int index=coords.findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate spCS = coords.spectralCoordinate(index);
    Vector<Double> refValue; refValue.resize(1);refValue(0)=spCS.referenceValue()(0);
    spCS.setReferenceValue(refValue);
    coords.replaceCoordinate(spCS,index);

    cfs.coordSys=coords;         cfwts.coordSys=coords; 
    cfs.pa=Quantity(pa,"rad");   cfwts.pa=Quantity(pa,"rad");
  }
  //
  //---------------------------------------------------------------------
  //---------------------------------------------------------------------
  // TEMPS
  //---------------------------------------------------------------------
  //---------------------------------------------------------------------
  //
};
