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
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <synthesis/TransformMachines2/EVLAAperture.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines/BeamCalc.h>
#include <synthesis/TransformMachines2/WTerm.h>
#include <synthesis/TransformMachines2/VLACalcIlluminationConvFunc.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
//
//---------------------------------------------------------------------
//

using namespace casacore;
namespace casa{
  using namespace vi;
  using namespace refim;
  using namespace SynthesisUtils;
  EVLAAperture& EVLAAperture::operator=(const EVLAAperture& other)
  {
    if(this!=&other) 
      {
	//	ConvolutionFunction::operator=(other);
	logIO_p = other.logIO_p;
	//	setParams(other.polMap_p_base, other.feedStokes_p);
	setPolMap(other.polMap_p_base);
	Diameter_p=other.Diameter_p;
	Nant_p=other.Nant_p;
	HPBW=other.HPBW;
	sigma=other.sigma;
      }
    return *this;
  }
  Int EVLAAperture::getVLABandID(Double& vbRefFreq,String&telescopeName, const CoordinateSystem& skyCoord)
  {
    LogIO log_l(LogOrigin("EVLAAperture", "getVLABandID[R&D]"));

    Double refFreq = skyCoord.spectralCoordinate(skyCoord.findCoordinate(Coordinate::SPECTRAL)).referenceValue()(0);
    cerr << "getVLABand (Global VB Ref. min, CF Ref.): " << vbRefFreq << " , " << refFreq << endl;
    if (telescopeName=="VLA")
      {
	if      ((refFreq >=1.34E9) && (refFreq <=1.73E9)) return BeamCalc_VLA_L;
	else if ((refFreq >=4.5E9)  && (refFreq <=5.0E9))  return BeamCalc_VLA_C;
	else if ((refFreq >=8.0E9)  && (refFreq <=8.8E9))  return BeamCalc_VLA_X;
	else if ((refFreq >=14.4E9) && (refFreq <=15.4E9)) return BeamCalc_VLA_U;
	else if ((refFreq >=22.0E9) && (refFreq <=24.0E9)) return BeamCalc_VLA_K;
	else if ((refFreq >=40.0E9) && (refFreq <=50.0E9)) return BeamCalc_VLA_Q;
	else if ((refFreq >=100E6)  && (refFreq <=300E6))  return BeamCalc_VLA_4;
      }
    else 
      if (telescopeName=="EVLA")
	{
	  if      ((refFreq >= 0.9E9) && (refFreq <= 2.1E9))  return BeamCalc_EVLA_L;
	  else if ((refFreq >=2.0E9)  && (refFreq <=4.0E9))   return BeamCalc_EVLA_S;
	  else if ((refFreq >=4.0E9)  && (refFreq <=8.0E9))   return BeamCalc_EVLA_C;
	  else if ((refFreq >=8.0E9)  && (refFreq <=12.0E9))  return BeamCalc_EVLA_X;
	  else if ((refFreq >=12.0E9) && (refFreq <=18.0E9))  return BeamCalc_EVLA_U;
	  else if ((refFreq >=18.0E9) && (refFreq <=26.5E9))  return BeamCalc_EVLA_K;
	  else if ((refFreq >=26.5E9) && (refFreq <=40.8E9))  return BeamCalc_EVLA_A;
	  else if ((refFreq >=40.0E9) && (refFreq <=50.0E9))  return BeamCalc_EVLA_Q;
	}
    ostringstream mesg;
    log_l << telescopeName << "/" << refFreq << "(Hz) combination not recognized." << LogIO::EXCEPTION;
    return -1;
  }
  
  void EVLAAperture::cacheVBInfo(const String& telescopeName, 
				 const Float& diameter)
  {
    telescopeName_p=telescopeName;
    Diameter_p=diameter;
  }

  void EVLAAperture::cacheVBInfo(const VisBuffer2& vb)
  {
    const Vector<String> telescopeNames=vb.subtableColumns()
      .observation().telescopeName().getColumn();
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
	    String mesg="We do not (yet) handle multiple telescopes for A-Projection!\n";
	    mesg += "Not yet a \"priority\"!!";
	    SynthesisError err(mesg);
	    throw(err);
	  }
      }
    telescopeName_p=telescopeNames[0];

    //    MSSpWindowColumns mssp(vb.msColumns().spectralWindow());
    //Freq = vb.msColumns().spectralWindow().refFrequency()(0);
    Diameter_p=0;
    Nant_p     = vb.subtableColumns().antenna().nrow();
    for (Int i=0; i < Nant_p; i++)
      if (!vb.subtableColumns().antenna().flagRow()(i))
	{
	  Diameter_p = vb.subtableColumns().antenna().dishDiameter().getColumn()(i);
	  break;
	}
    if (Diameter_p == 0)
      {
	logIO() << LogOrigin("EVLAAperture", "cacheVBInfo")
		<< "No valid or finite sized antenna found in the antenna table. "
		<< "Assuming diameter = 25m."
		<< LogIO::WARN
		<< LogIO::POST;
	Diameter_p=25.0;
      }
    cacheVBInfo(telescopeNames[0], Diameter_p);
  }

  Int EVLAAperture::getBandID(const Double& freq, const String& telescopeName, const String& bandName)
  {
    Int bandID=0;
    if (!isNoOp())
      {
	// First #-separated token in bandName_p is the name of the band used
	Vector<String> tokens = SynthesisUtils::parseBandName(bandName);
	bandID = BeamCalc::Instance()->getBandID(freq,telescopeName,tokens(0));
      }
    
    return bandID;
  };

  int EVLAAperture::getVisParams(const VisBuffer2& vb,const CoordinateSystem& /*im*/)
  {
    throw(AipsError("EVLAAperture::getVisParams() called"));
    Double Freq;
    cacheVBInfo(vb);
    
    Freq = vb.subtableColumns().spectralWindow().refFrequency()(0);
    Double Lambda=C::c/Freq;
    HPBW = Lambda/(Diameter_p*sqrt(log(2.0)));
    sigma = 1.0/(HPBW*HPBW);

    return getBandID(Freq, telescopeName_p,bandName_p);
    // Int bandID=0;
    // if (!isNoOp())
    //   bandID = BeamCalc::Instance()->getBandID(Freq,telescopeName_p);
    
    // return bandID;
  }
  
  Int EVLAAperture::makePBPolnCoords(const VisBuffer2&vb,
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
//    cout<<"Direction index is "<< directionIndex<<endl;
    AlwaysAssert(directionIndex>=0, AipsError);
    DirectionCoordinate dc=skyCoord.directionCoordinate(directionIndex);
    Vector<Double> sampling;
    sampling = dc.increment();
//    cout<<"Image sampling set to : "<<sampling<<endl;
    sampling*=Double(convSampling);
    sampling*=Double(skyNx)/Double(convSize);
    dc.setIncrement(sampling);
//    cout<<"Resized sampling is : "<<sampling<<endl;
    
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
	for(Int i=0;i<M;i++) if (polMap_p_base(i) > -1) {poln(N) = vb.correlationTypes()(i);N++;}
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
  
  Bool EVLAAperture::findSupport(Array<Complex>& func, Float& threshold,Int& origin, Int& R)
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
  
  void EVLAAperture::makeFullJones(ImageInterface<Complex>& pbImage,
				   const VisBuffer2& vb,
				   Bool doSquint, Int& bandID, Double freqVal)
  {

    if (!isNoOp())
      {
	VLACalcIlluminationConvFunc vlaPB;
	Long cachesize=(HostInfo::memoryTotal(true)/8)*1024;
	vlaPB.setMaximumCacheSize(cachesize);
	bandID = getBandID(freqVal,telescopeName_p,bandName_p);
	vlaPB.makeFullJones(pbImage,vb, doSquint, bandID, freqVal);
      }
  }

  void EVLAAperture::applySky(ImageInterface<Complex>& outImages,
			      const VisBuffer2& vb, 
			      const Bool doSquint,
			      const Int& cfKey,
			      const Int& muellerTerm,
			      const Double freqVal)
  {
//     (void)cfKey;
//     if (!isNoOp())
//       {
// 	VLACalcIlluminationConvFunc vlaPB;
// 	Long cachesize=(HostInfo::memoryTotal(true)/8)*1024;
// 	vlaPB.setMaximumCacheSize(cachesize);
// 	bandID = getBandID(freqVal,telescopeName_p);
// //	cout<<"EVLAAperture : muellerTerm"<<muellerTerm <<"\n";
// 	//vlaPB.applyPB(outImages, doSquint,bandID,muellerTerm,freqVal);
// 	Double pa=getPA(vb);
// 	vlaPB.applyPB(outImages, pa, doSquint,bandID,muellerTerm,freqVal);
	
//       }
    Double pa=getPA(vb);
    applySky(outImages,pa,doSquint,cfKey,muellerTerm, freqVal);
  }
  //
  // This one without VB.  Should become the default (and the only one!).
  //
  void EVLAAperture::applySky(ImageInterface<Complex>& outImages,
			      const Double& pa,
			      const Bool doSquint,
			      const Int& cfKey,
			      const Int& muellerTerm,
			      const Double freqVal)
  {
    (void)cfKey;
    if (!isNoOp())
      {
	VLACalcIlluminationConvFunc vlaPB;
	Long cachesize=(HostInfo::memoryTotal(true)/8)*1024;
	vlaPB.setMaximumCacheSize(cachesize);
	Int bandID;
	bandID = getBandID(freqVal,telescopeName_p,bandName_p);
	//vlaPB.applyPB(outImages, doSquint,bandID,muellerTerm,freqVal);
	Double pa_l=pa;  // Due to goofup in making sure complier type checking does not come in the way!
	vlaPB.applyPB(outImages, pa_l, doSquint,bandID,muellerTerm,freqVal);
	
      }
  }

  void EVLAAperture::applySky(ImageInterface<Float>& outImages,
			      const VisBuffer2& vb, 
			      const Bool doSquint,
			      const Int& cfKey,
			      const Int& muellerTerm,
			      const Double freqVal)
  {
    (void)cfKey;
    (void)muellerTerm;
    if (!isNoOp())
      {
	VLACalcIlluminationConvFunc vlaPB;
	Long cachesize=(HostInfo::memoryTotal(true)/8)*1024;
	vlaPB.setMaximumCacheSize(cachesize);
	Int bandID;
	bandID = getBandID(freqVal,telescopeName_p,bandName_p);

	Double pa=getPA(vb);
	vlaPB.applyPB(outImages, pa, bandID, doSquint,freqVal);
      }
  }
  
};
