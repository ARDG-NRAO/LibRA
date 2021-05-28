// -*- C++ -*-
//# VLAIlluminationConvFunc.cc: Implementation for VLAIlluminationConvFunc
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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

#define USETABLES 1
#include <synthesis/TransformMachines2/VLACalcIlluminationConvFunc.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LatticeMath/LatticeFFT.h>
#include <images/Images/ImageRegrid.h>
#include <images/Images/PagedImage.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Utilities/Assert.h>
#include <casa/OS/File.h>
#include <fstream>
#include <casa/sstream.h>
#include <casa/OS/Timer.h>
#include <msvis/MSVis/VisibilityIterator2.h>
using namespace casacore;
namespace casa{
  namespace refim{
  //
  //------------------------------------------------------------------------
  //
  VLACalcIlluminationConvFunc::VLACalcIlluminationConvFunc():
    IlluminationConvFunc(),convFunc_p(),resolution(),pbRead_p(false),freq_p(0),lastPA(0),ap()
  {

    LogIO logIO(LogOrigin("VLACalcIlluminationConvFunc","ctor"));
    ap.oversamp = 3;
    ap.x0=-13.0; ap.y0=-13.0;
    ap.dx=0.5; ap.dy=0.5;

    ap.nx=ap.ny=104;
    ap.pa=lastPA=18000000;
    ap.freq=1.4;
    ap.band = BeamCalc_VLA_L;
    //    IPosition shape(4,ap.nx,ap.ny,4,1);
    IPosition shape(4,ap.nx,ap.ny,1,1);//Set poln. axis to be
				       //degenerate (len=1).  This is
				       //set to 2 if cross-hand
				       //functions are requested.
    ap.aperture = new TempImage<Complex>();
    if (maximumCacheSize() > 0) ap.aperture->setMaximumCacheSize(maximumCacheSize());
    ap.aperture->resize(shape);

  }


  CoordinateSystem VLACalcIlluminationConvFunc::makeUVCoords(CoordinateSystem& imageCoordSys,
							     IPosition& shape,
							     Double /*refFreq*/)
  {
    CoordinateSystem FTCoords = imageCoordSys;
    Int dirIndex=FTCoords.findCoordinate(Coordinate::LINEAR);

    // If LINEAR axis is found, assume that the coordsys is alread in FT domain
    if (dirIndex >= 0) return FTCoords; 

    dirIndex=FTCoords.findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate dc=imageCoordSys.directionCoordinate(dirIndex);
    Vector<Bool> axes(2); axes=true;
    Vector<Int> dirShape(2); dirShape(0)=shape(0);dirShape(1)=shape(1);
    Coordinate* FTdc=dc.makeFourierCoordinate(axes,dirShape);
    // if (refFreq > 0)
    //   {
    // 	Int index1 = FTCoords.findCoordinate(Coordinate::SPECTRAL);
    // 	SpectralCoordinate SpC = FTCoords.spectralCoordinate(index1);
    // 	Vector<Double> refVal = SpC.referenceValue();
    // 	refVal = refFreq;
    // 	SpC.setReferenceValue(refVal);
    //   }

    FTCoords.replaceCoordinate(*FTdc,dirIndex);
    delete FTdc;

    return FTCoords;
  }
  
//  CoordinateSystem VLACalcIlluminationConvFunc::makeJonesCoords(CoordinateSystem& imageCoordsys)
//
//
  //----------------------------------------------------------------------
  // Write PB to the pbImage
  //
  void VLACalcIlluminationConvFunc::applyPB(ImageInterface<Float>& /*pbImage*/,
					    //const VisBuffer2& vb, 
					    Double& /*pa*/,
					    const Vector<Float>& /*paList*/, 
					    Int /*bandID*/, Bool /*doSquint*/)
  {
    throw(AipsError("applyPB(paList) called!"));
    // CoordinateSystem skyCS(pbImage.coordinates());
    // IPosition skyShape(pbImage.shape());
    // TempImage<Complex> uvGrid;
    // if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());
    // //    regridAperture(skyCS, skyShape, uvGrid, vb, paList, false, bandID);
    // regridAperture(skyCS, skyShape, uvGrid, pa, paList, doSquint, bandID);

    // fillPB(*(ap.aperture),pbImage);
  }
  void VLACalcIlluminationConvFunc::applyPB(ImageInterface<Float>& pbImage,
					    //const VisBuffer2& vb, 
					    Double& pa,
					    Int bandID,
					    Bool doSquint, Double freqVal)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape());

    TempImage<Complex> uvGrid;
    if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());
    //    regridAperture(skyCS, skyShape, uvGrid, vb,false, bandID);
    regridAperture(skyCS, skyShape, uvGrid, pa, doSquint, bandID, 0, freqVal);
    fillPB(*(ap.aperture),pbImage);
  }
  void VLACalcIlluminationConvFunc::applyPB(ImageInterface<Complex>& pbImage, 
					    //const VisBuffer2& vb,
					    Double& pa,
					    Bool doSquint, Int bandID,Int muellerTerm, Double freqVal)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape()); 
//    cout<<"M = "<<muellerTerm<<"\n";
//    cout<<"bandID = "<<bandID<<"\n";
//    cout<<"doSquint = "<<doSquint<<"\n";
    TempImage<Complex> uvGrid;

    if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());
    //    regridAperture(skyCS, skyShape, uvGrid, vb, true, bandID);
    Int index= skyCS.findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate spCS = skyCS.spectralCoordinate(index);
//    cout<<"Ref Freq for sky jones is :"<<spCS.referenceValue()(0);
    // index=skyCS.findCoordinate(Coordinate::DIRECTION);
    // AlwaysAssert(index>=0, AipsError);
    
    regridAperture(skyCS, skyShape, uvGrid, pa, doSquint, bandID, muellerTerm,freqVal);
    
    pbImage.setCoordinateInfo(skyCS);
    // {
    //   string name("aperture.im");
    //   storeImg(name,*(ap.aperture));
    // }
    fillPB(*(ap.aperture),pbImage);
    //{
    //   string name("apb.im");
    //   storeImg(name,pbImage);
    //}
  }
  //--------------------------------------------------------------------------
  // Write PB^2 to the pbImage
  //
  void VLACalcIlluminationConvFunc::applyPBSq(ImageInterface<Float>& /*pbImage*/,
					      //const VisBuffer2& vb, 
					      Double& /*pa*/,
					      const Vector<Float>& /*paList*/, 
					      Int /*bandID*/,
					      Bool /*doSquint*/)
  {
    throw(AipsError("applyPBSq(paList) called!"));
    // CoordinateSystem skyCS(pbImage.coordinates());
    // IPosition skyShape(pbImage.shape());
    // TempImage<Complex> uvGrid;
    // if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());
    // //    regridAperture(skyCS, skyShape, uvGrid, vb, paList, false, bandID);
    // regridAperture(skyCS, skyShape, uvGrid, pa, paList, doSquint, bandID);

    // fillPB(*(ap.aperture),pbImage, true);
  }
  void VLACalcIlluminationConvFunc::applyPBSq(ImageInterface<Float>& pbImage,
					      //const VisBuffer2& vb, 
					      Double& pa,
					      Int bandID,
					      Bool doSquint)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape());

    TempImage<Complex> uvGrid;
    if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());
    //    regridAperture(skyCS, skyShape, uvGrid, vb,false, bandID);
    regridAperture(skyCS, skyShape, uvGrid, pa, doSquint, bandID);
    fillPB(*(ap.aperture),pbImage,true);
  }
  void VLACalcIlluminationConvFunc::applyPBSq(ImageInterface<Complex>& pbImage, 
					      //const VisBuffer2& vb, 
					      Double& pa,
					      Int bandID,
					      Bool doSquint)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape());

    TempImage<Complex> uvGrid;
    if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());
    //    regridAperture(skyCS, skyShape, uvGrid, vb, true, bandID);
    regridAperture(skyCS, skyShape, uvGrid, pa, doSquint, bandID);
    fillPB(*(ap.aperture),pbImage, true);
  }
  //
  //--------------------------------------------------------------------------
  //
  void VLACalcIlluminationConvFunc::setApertureParams(ApertureCalcParams& ap,
						      const Float& Freq, const Float& pa, 
						      const Int& bandID,
						      const Int& /*inStokes*/,
						      const IPosition& skyShape,
						      const Vector<Double>& uvIncr)
  {
    Double Lambda = C::c/Freq;
    
    ap.pa=pa;
    ap.band = bandID;
    ap.freq = Freq/1E9;
    ap.nx = skyShape(0);           ap.ny = skyShape(1);
    ap.dx = abs(uvIncr(0)*Lambda); ap.dy = abs(uvIncr(1)*Lambda);
    ap.x0 = -(ap.nx/2)*ap.dx;      ap.y0 = -(ap.ny/2)*ap.dy;
    //
    // If cross-hand pols. are requested, we need to compute both
    // the parallel-hand aperture illuminations.
    //
      //if ((inStokes == Stokes::RL) || (inStokes == Stokes::LR))
      {
	IPosition apShape(ap.aperture->shape());
	apShape(3)=4;
	ap.aperture->resize(apShape);
      }
  }
  //
  //--------------------------------------------------------------------------
  //
  void VLACalcIlluminationConvFunc::regridApertureEngine(ApertureCalcParams& ap,
							 const Int& /*inStokes*/)
  {
    IPosition apertureShape(ap.aperture->shape());
    apertureShape(0) = ap.nx;  apertureShape(1) = ap.ny;
    ap.aperture->resize(apertureShape);
    ap.aperture->set(0.0);
    //BeamCalc::Instance()->calculateAperture(&ap,inStokes);
    //cerr << ap.aperture->shape() << " " << inStokes << endl;

    // If full-pol. imaging, compute all 4 pols., else only the one given by inStokes.
    BeamCalc::Instance()->calculateAperture(&ap);// The call in the absence of instokes allows the computation of all
    //BeamCalc::Instance()->calculateAperture(&ap,inStokes);// The call in the absence of instokes allows the computation of all
                                                            // the four jones parameters at one time.
}
  //
  //--------------------------------------------------------------------------
  //
  void VLACalcIlluminationConvFunc::regridAperture(CoordinateSystem& skyCS,
						   IPosition& skyShape,
						   TempImage<Complex>& /*uvGrid*/,
						   //const VisBuffer2& vb,
						   Double& pa,
						   Bool doSquint, Int bandID,Int muellerTerm ,Double freqVal)
  {
    LogIO logIO(LogOrigin("VLACalcIlluminationConvFunc","regrid"));
    CoordinateSystem skyCoords(skyCS);

    Int index;
    //UNUSED: Double timeValue = getCurrentTimeStamp(vb);
    AlwaysAssert(bandID>=-1, AipsError);
    if (bandID != -1) ap.band = bandID;
    //Float pa = getPA(vb);
    Float Freq, freqHi;

    if (lastPA == pa)
      {
	//	LogIO logIO;
	logIO << "Your CPU is being used to do computations for the same PA as for the previous call.  Report this!" 
	      << LogIO::NORMAL1;
      }

    
    if (freqVal > 0)
      {
	Freq=freqHi=freqVal;
	ap.freq=freqHi/1E09;
      }
    else
      {
	//throw(AipsError("Freq. < 0 in VLACICF::regrid"));

	// Vector<Double> chanFreq = vb.frequency();
	index = skyCS.findCoordinate(Coordinate::SPECTRAL);
	SpectralCoordinate SpC = skyCS.spectralCoordinate(index);
	Vector<Double> refVal = SpC.referenceValue();
	
	// freqHi = max(chanFreq);
	freqHi = refVal[0];
	// freqLo = min(chanFreq);
	Freq = freqHi ;
	ap.freq = Freq/1E9;
      }
    
    IPosition imsize(skyShape);
    CoordinateSystem uvCoords = makeUVCoords(skyCoords,imsize,freqHi);

    index = uvCoords.findCoordinate(Coordinate::LINEAR);
    LinearCoordinate lc=uvCoords.linearCoordinate(index);
    Vector<Double> uvIncr = lc.increment();
    //Double Lambda = C::c/freqHi;
    
    index = uvCoords.findCoordinate(Coordinate::STOKES);
    Int inStokes = uvCoords.stokesCoordinate(index).stokes()(0);
    
    //Vector<Int> intSkyShape=skyShape.asVector();
    setApertureParams(ap, Freq, pa, bandID, inStokes,
		      skyShape, uvIncr);
    
    regridApertureEngine(ap, inStokes);
    IPosition apertureShape(ap.aperture->shape());

    // ap.freq = Freq/1E9;
    // ap.nx = skyShape(0); ap.ny = skyShape(1);
    // ap.dx = abs(uvIncr(0)*Lambda); ap.dy = abs(uvIncr(1)*Lambda);
    // ap.x0 = -(ap.nx/2)*ap.dx; 
    // ap.y0 = -(ap.ny/2)*ap.dy;
    // ap.pa=pa;
    // if ((inStokes == Stokes::RL) || (inStokes == Stokes::LR))
    //   ap.aperture->shape()(3)=2;

    // apertureShape(0) = ap.nx;  apertureShape(1) = ap.ny;
    // ap.aperture->resize(apertureShape);
    // ap.aperture->set(0.0);
    
    // BeamCalc::Instance()->calculateAperture(&ap,inStokes);
    
    if (!doSquint)
      {
	IPosition PolnRIndex(4,0,0,0,0), PolnLIndex(4,0,0,3,0);
	IPosition tndx(4,0,0,0,0);
	for(tndx(3)=0;tndx(3)<apertureShape(3);tndx(3)++)   // The freq. axis
	  for(tndx(2)=0;tndx(2)<apertureShape(2);tndx(2)++) // The Poln. axis
	    for(tndx(1)=0;tndx(1)<apertureShape(1);tndx(1)++)   // The spatial
	      for(tndx(0)=0;tndx(0)<apertureShape(0);tndx(0)++) // axis.
		{
		  PolnRIndex(0)=PolnLIndex(0)=tndx(0);
		  PolnRIndex(1)=PolnLIndex(1)=tndx(1);
		  Complex val, Rval;
		  Float phase;
		  val = ap.aperture->getAt(tndx);
		  Rval = ap.aperture->getAt(PolnRIndex);
		  //Lval = ap.aperture->getAt(PolnLIndex);
		  phase = arg(Rval);  Rval=Complex(cos(phase),sin(phase));
		  //phase = arg(Lval);  Lval=Complex(cos(phase),sin(phase));
		  
		  // if      (tndx(2)==0) ap.aperture->putAt(val*conj(Rval),tndx);
		  // else if (tndx(2)==1) ap.aperture->putAt(val*conj(Lval),tndx);
		  // else if (tndx(2)==2) ap.aperture->putAt(val*conj(Rval),tndx);
		  // else if (tndx(2)==3) ap.aperture->putAt(val*conj(Lval),tndx);
		  ap.aperture->putAt(val*conj(Rval),tndx);
		}
      }
//    cout<<"Completed the regrid Aperture step"; 
    // Vector<Int> poln(4);
    // poln(0) = Stokes::RR;
    // poln(1) = Stokes::RL;
    // poln(2) = Stokes::LR;
    // poln(3) = Stokes::LL;
    Vector<Int> poln(1); poln(0)=inStokes;
    StokesCoordinate polnCoord(poln);
    SpectralCoordinate spectralCoord(MFrequency::TOPO,Freq,1.0,0.0);
    //    uvCoords.addCoordinate(dirCoord);
    index = uvCoords.findCoordinate(Coordinate::STOKES);
    uvCoords.replaceCoordinate(polnCoord,index);
    index = uvCoords.findCoordinate(Coordinate::SPECTRAL);
    uvCoords.replaceCoordinate(spectralCoord,index);
    //logIO << "The Stokes coordinate is", poln(0)<< LogIO::POST;
    ap.aperture->setCoordinateInfo(uvCoords);
     if (doSquint==true)
    {
    //  String name("aperture.im");
    //  storeImg(name,*(ap.aperture));
    }
    
    //
    // Now FT the re-gridded Fourier plane to get the primary beam.
    //
    ftAperture(*(ap.aperture),muellerTerm);
     if (doSquint==true)
    {
    //  String name("ftaperture.im");
    //  storeImg(name,*(ap.aperture));
    }
    
  }
  void VLACalcIlluminationConvFunc::regridAperture(CoordinateSystem& skyCS,
						   IPosition& skyShape,
						   TempImage<Complex>& uvGrid,
						   const VisBuffer2 &vb,
						   const Vector<Float>& paList,
						   Bool doSquint, Int bandID)
  {
    CoordinateSystem skyCoords(skyCS);
    
    Float pa, Freq;
    if (bandID != -1) ap.band = bandID;
    AlwaysAssert(ap.band>=-1, AipsError);
    Vector<Double> chanFreq = vb.getFrequencies(0);
    
    const MSSpWindowColumns& spwCol = 
      vb.subtableColumns().spectralWindow();
    ArrayColumn<Double> chanfreq = spwCol.chanFreq();
    ScalarColumn<Double> reffreq = spwCol.refFrequency();
    //    Freq = sum(chanFreq)/chanFreq.nelements();
    
    Freq = max(chanfreq.getColumn());
    IPosition imsize(skyShape);
    CoordinateSystem uvCoords = makeUVCoords(skyCoords,imsize,Freq);
    uvGrid.setCoordinateInfo(uvCoords);
    
    Int index = uvCoords.findCoordinate(Coordinate::LINEAR);
    LinearCoordinate lc=uvCoords.linearCoordinate(index);
    Vector<Double> incr = lc.increment();
    Double Lambda = C::c/Freq;
    ap.freq = Freq/1E9;
    
    ap.nx = skyShape(0); ap.ny = skyShape(1);
    IPosition apertureShape(ap.aperture->shape());
    apertureShape(0) = ap.nx;  apertureShape(1) = ap.ny;
    ap.aperture->resize(apertureShape);
    
    TempImage<Complex> tmpAperture;tmpAperture.resize(apertureShape);
    if (maximumCacheSize() > 0) tmpAperture.setMaximumCacheSize(maximumCacheSize());
    
    ap.dx = abs(incr(0)*Lambda); ap.dy = abs(incr(1)*Lambda);
    //	cout << ap.dx << " " << incr(0) << endl;
    //	ap.x0 = -(25.0/(2*ap.dx)+1)*ap.dx; ap.y0 = -(25.0/(2*ap.dy)+1)*ap.dy;
    // Following 3 lines go with the ANT tag in BeamCalc.cc
    //	Double antRadius=BeamCalcGeometryDefaults[ap.band].Rant;
    //	ap.x0 = -(antRadius/(ap.dx)+1)*ap.dx; 
    //	ap.y0 = -(antRadius/(ap.dy)+1)*ap.dy;
    // Following 2 lines go with the PIX tag in BeamCalc.cc
    ap.x0 = -(ap.nx/2)*ap.dx; 
    ap.y0 = -(ap.ny/2)*ap.dy;
    
    //
    // Accumulate apertures for a list of PA
    //
    for(uInt ipa=0;ipa<paList.nelements();ipa++)
      {
	pa = paList[ipa];
	
	ap.pa=pa;
	ap.aperture->set(0.0);
	BeamCalc::Instance()->calculateAperture(&ap);
	//
	// Set the phase of the aperture function to zero if doSquint==F
	// Poln. axis indices
        // 0: RR, 1:RL, 2:LR, 3:LL
	// This is electic field. 0=> Poln R, 
	//                        1=> Leakage of R->L
	//                        2=> Leakage of L->R
	//                        3=> Poln L
	//
	// The squint is removed in the following code using
	// honest-to-god pixel indexing. If this is not the most
	// efficient method of doing this in AIPS++ (i.e. instead use
	// slices etc.), then this cost will show up in making the
	// average PB.  Since this goes over each pixel of a full
	// stokes (poln. really) complex image, look here (also) for
	// optimization (if required).
	//
	if (!doSquint)
	  {
	    IPosition PolnRIndex(4,0,0,0,0), PolnLIndex(4,0,0,3,0);
	    IPosition tndx(4,0,0,0,0);
	    for(tndx(3)=0;tndx(3)<apertureShape(3);tndx(3)++)   // The freq. axis
	      for(tndx(2)=0;tndx(2)<apertureShape(2);tndx(2)++) // The Poln. axis
		for(tndx(1)=0;tndx(1)<apertureShape(1);tndx(1)++)   // The spatial
		  for(tndx(0)=0;tndx(0)<apertureShape(0);tndx(0)++) // axis.
		    {
		      PolnRIndex(0)=PolnLIndex(0)=tndx(0);
		      PolnRIndex(1)=PolnLIndex(1)=tndx(1);
		      Complex val, Rval, Lval;
		      Float phase;
		      val = ap.aperture->getAt(tndx);
		      Rval = ap.aperture->getAt(PolnRIndex);
		      Lval = ap.aperture->getAt(PolnLIndex);
		      phase = arg(Rval); Rval=Complex(cos(phase),sin(phase));
		      phase = arg(Lval); Lval=Complex(cos(phase),sin(phase));
		      
		      if      (tndx(2)==0) ap.aperture->putAt(val*conj(Rval),tndx);
		      else if (tndx(2)==1) ap.aperture->putAt(val*conj(Lval),tndx);
		      else if (tndx(2)==2) ap.aperture->putAt(val*conj(Rval),tndx);
		      else if (tndx(2)==3) ap.aperture->putAt(val*conj(Lval),tndx);
		    }
	  }
	tmpAperture += *(ap.aperture);
      }
    *(ap.aperture) = tmpAperture;
    tmpAperture.resize(IPosition(1,1));//Release temp. store.
    Vector<Int> poln(4);
    poln(0) = Stokes::RR;
    poln(1) = Stokes::RL;
    poln(2) = Stokes::LR;
    poln(3) = Stokes::LL;
    StokesCoordinate polnCoord(poln);
    SpectralCoordinate spectralCoord(MFrequency::TOPO,Freq,1.0,0.0);
    //    uvCoords.addCoordinate(dirCoord);
    index = uvCoords.findCoordinate(Coordinate::STOKES);
    uvCoords.replaceCoordinate(polnCoord,index);
    index = uvCoords.findCoordinate(Coordinate::SPECTRAL);
    uvCoords.replaceCoordinate(spectralCoord,index);
    
    ap.aperture->setCoordinateInfo(uvCoords);
     //if (doSquint==false)
     //{
     //	String name("aperture.im");
     //	storeImg(name,*(ap.aperture));
	//logIO << "The aperture has been written to aperture.im"<< LogIO::POST;
     //}
    
    ftAperture(*(ap.aperture));
    //String name("aperture.im");
    //storeImg(name,*(ap.aperture));
    //logIO << "The fourier transform of the aperture has been written to ftaperture.im"<< LogIO::POST;
  }
  
  
  
  void VLACalcIlluminationConvFunc::fillPB(ImageInterface<Complex>& inImg,
					   ImageInterface<Complex>& outImg,
					   Bool Square)
  {
    IPosition imsize(outImg.shape());
    IPosition ndx(outImg.shape());
    IPosition inShape(inImg.shape()),inNdx;
    Vector<Int> inStokes,outStokes;
    Int index,s,index1;
    
    // Timer tim;
    // tim.mark();
    index = inImg.coordinates().findCoordinate(Coordinate::STOKES);
    inStokes = inImg.coordinates().stokesCoordinate(index).stokes();
    index = outImg.coordinates().findCoordinate(Coordinate::STOKES);
    outStokes = outImg.coordinates().stokesCoordinate(index).stokes();
    index = outImg.coordinates().findCoordinate(Coordinate::SPECTRAL);
    index1 = inImg.coordinates().findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate inSpectralCoords = inImg.coordinates().spectralCoordinate(index1);
    CoordinateSystem outCS = outImg.coordinates();
    outCS.replaceCoordinate(inSpectralCoords,index);
    outImg.setCoordinateInfo(outCS);
    //tim.show("fillPB::CSStuff:");
    ndx(3)=0;
    // #ifdef HAS_OMP
    //     Int Nth=max(omp_get_max_threads()-2,1);
    // #endif
    //tim.mark();
    for(ndx(2)=0;ndx(2)<imsize(2);ndx(2)++) // The poln axes
      {
	for(s=0;s<inShape(2);s++) if (inStokes(s) == outStokes(ndx(2))) break;
	
	for(ndx(0)=0;ndx(0)<imsize(0);ndx(0)++)
	  //#pragma omp parallel default(none) firstprivate(s,iy) shared(twoPiW,convSize) num_threads(Nth)
	  {
	    //#pragma omp for
	    for(ndx(1)=0;ndx(1)<imsize(1);ndx(1)++)
	      {
		Complex cval;
		inNdx = ndx; inNdx(2)=s;
		cval = inImg.getAt(inNdx);
		if (Square) cval = cval*conj(cval);
		outImg.putAt(cval*outImg.getAt(ndx),ndx);
	      }
	  }
      }
    //tim.show("fillPB: ");
  }
  
  void VLACalcIlluminationConvFunc::fillPB(ImageInterface<Complex>& inImg,
					   ImageInterface<Float>& outImg,
					   Bool Square)
  {
    IPosition imsize(outImg.shape());
    IPosition ndx(outImg.shape());
    IPosition inShape(inImg.shape()),inNdx;
    
    Vector<Int> inStokes,outStokes;
    Int index,s;
    
    // Timer tim;
    // tim.mark();
    index = inImg.coordinates().findCoordinate(Coordinate::STOKES);
    inStokes = inImg.coordinates().stokesCoordinate(index).stokes();
    index = outImg.coordinates().findCoordinate(Coordinate::STOKES);
    outStokes = outImg.coordinates().stokesCoordinate(index).stokes();
    ndx(3)=0;
    //tim.show("fillPB::CSStuff2:");
    
    //tim.mark();
    for(ndx(2)=0;ndx(2)<imsize(2);ndx(2)++) // The poln axes
      {
	if (outStokes(ndx(2)) == Stokes::I)
	  {
	    //
	    // Fill the outImg wiht (inImg(RR)+inImage(LL))/2 
	    //
	    for(ndx(0)=0;ndx(0)<imsize(0);ndx(0)++)
	      for(ndx(1)=0;ndx(1)<imsize(1);ndx(1)++)
		{
		  Complex cval;
		  inNdx = ndx; 
		  inNdx(2)=0; cval = inImg.getAt(inNdx);
		  inNdx(2)=3; cval += inImg.getAt(inNdx);
		  cval/2;
		  
		  if (Square) cval = cval*conj(cval);
		  
		  outImg.putAt(abs(cval*outImg.getAt(ndx)),ndx);
		}
	  }
	else if ((outStokes(ndx(2)) == Stokes::RR) ||
		 (outStokes(ndx(2)) == Stokes::RL) ||
		 (outStokes(ndx(2)) == Stokes::LR) ||
		 (outStokes(ndx(2)) == Stokes::LL))
	  {
	    for(s=0;s<inShape(2);s++) if (inStokes(s) == outStokes(ndx(2))) break;
	    
	    for(ndx(0)=0;ndx(0)<imsize(0);ndx(0)++)
	      for(ndx(1)=0;ndx(1)<imsize(1);ndx(1)++)
		{
		  Complex cval;
		  inNdx = ndx; inNdx(2)=s;
		  cval = inImg.getAt(inNdx);
		  if (Square) cval = cval*conj(cval);
		  outImg.putAt(abs(cval*outImg.getAt(ndx)),ndx);
		}
	  }
	else throw(AipsError("Unsupported Stokes found in VLACalcIlluminationConvFunc::fillPB."));
      }
    //tim.show("fillPB2:");
  }
  
  /*
    void VLACalcIlluminationConvFunc::fillPB(ImageInterface<Complex>& inImg,
    ImageInterface<Float>& outImg)
    {
    IPosition imsize(outImg.shape());
    IPosition ndx(outImg.shape());
    IPosition inShape(inImg.shape()), inNdx;
    Vector<Int> inStokes,outStokes;
    Int index;
    index = inImg.coordinates().findCoordinate(Coordinate::STOKES);
    inStokes = inImg.coordinates().stokesCoordinate(index).stokes();
    index = outImg.coordinates().findCoordinate(Coordinate::STOKES);
    outStokes = outImg.coordinates().stokesCoordinate(index).stokes();
    
    ndx(3)=0;
    for(ndx(0)=0;ndx(0)<imsize(0);ndx(0)++)
    {
    
    for(ndx(1)=0;ndx(1)<imsize(1);ndx(1)++)
    {
    //
    // Average along the polarization axes and fillin the
    // amp. of the average in the output image.
    // 
    Complex cval=0.0;
    
    ndx(2)=0;cval = inImg.getAt(ndx);
    for(ndx(2)=0;ndx(2)<imsize(2);ndx(2)++) // The poln axes
    outImg.putAt(abs(cval*outImg.getAt(ndx)),ndx);
    }
    }
    }
    
  */
  
//  void VLACalcIlluminationConvFunc::ftAperture(TempImage<Complex>& uvgrid, Bool makeMueller)
  void VLACalcIlluminationConvFunc::ftAperture(TempImage<Complex>& uvgrid, Int muellerTerm)
  {
    //
    // Make SkyJones
    //
    // {
       //String name("uvgrid.im");
       //storeImg(name,uvgrid);
    // }
    LatticeFFT::cfft2d(uvgrid);
//    {
//      Int index = uvgrid.coordinates().findCoordinate(Coordinate::STOKES);
//      Int inStokes = uvgrid.coordinates().stokesCoordinate(index).stokes()(0);
//      IPosition shape = uvgrid.shape();
      //ostringstream tt;
      //String name("ftuvgrid.im");
      //tt << name << "_"<< inStokes;
      //storeImg(String(tt),uvgrid);
//    }
    // {
       //String name("ftuvgrid.im");
       //storeImg(name,uvgrid);
    // }
    
    // Now make SkyMuller
    //
    //skyMuller(uvgrid);
    Int tempmueller=-1; // Set to -1 to use sanjay's code which pass the regressions.
    tempmueller=0;    // Full-pol. imaging is requested, use PJ's code.  Else use SB's.

    //if (uvgrid.shape()(3) > 2) tempmueller=0;

    if (tempmueller==0)
      skyMuller(uvgrid,muellerTerm);
    else
      skyMuller(uvgrid,-1);
  }
  
  void VLACalcIlluminationConvFunc::loadFromImage(String& /*fileName*/)
  {
    throw(AipsError("VLACalcIlluminationConvFunc::loadFromImage() not yet supported."));
  };
  
  void VLACalcIlluminationConvFunc::skyMuller(Array<Complex>& buf,
					      const IPosition& shape,
					      const Int& inStokes)
  {
    Array<Complex> tmp;
    IPosition t(4,0,0,0,0),n0(4,0,0,0,0),n1(4,0,0,0,0);
    Float peak;
    peak=0;
    for(t(2)=0;t(2)<shape(2);t(2)++)
      for(t(1)=0;t(1)<shape(1);t(1)++)
	for(t(0)=0;t(0)<shape(0);t(0)++)
	  if (abs(buf(t)) > peak) peak = abs(buf(t));
    if (peak > 1E-8)
      for(t(3)=0;t(3)<shape(3);t(3)++)       // Freq axis
	for(t(2)=0;t(2)<shape(2);t(2)++)     // Poln axis
	  for(t(1)=0;t(1)<shape(1);t(1)++)   // y axis
	    for(t(0)=0;t(0)<shape(0);t(0)++) // X axis
	      buf(t) = buf(t)/peak;
    // {
    //   skyJones.put(buf);
    //   String name("skyjones.im");
    //   storeImg(name,skyJones);
    // }
    
    tmp.assign(buf);

    t(0)=t(1)=t(2)=t(3)=0;
    
    if ((inStokes == Stokes::RR) || (inStokes == Stokes::LL))
      {
	t(2)=0;n0(2)=0;n1(2)=0; //RR
	for(  n0(0)=n1(0)=t(0)=0;n0(0)<shape(0);n0(0)++,n1(0)++,t(0)++)
	  for(n0(1)=n1(1)=t(1)=0;n0(1)<shape(1);n0(1)++,n1(1)++,t(1)++)
            buf(t) = (tmp(n0)*conj(tmp(n1)));
      }
    
    if ((inStokes == Stokes::LR) || (inStokes == Stokes::RL))
      {
	t(2)=0;n0(3)=1;n1(2)=0; //LR
	for(  n0(0)=n1(0)=t(0)=0;n0(0)<shape(0);n0(0)++,n1(0)++,t(0)++)
	  for(n0(1)=n1(1)=t(1)=0;n0(1)<shape(1);n0(1)++,n1(1)++,t(1)++)
	    buf(t) = (tmp(n0)*conj(tmp(n1)));
	     
      }
    
//    if (inStokes == Stokes::RL)
//      {
//	t(2)=0;n0(2)=0;n1(3)=1; //LR
//	for(  n0(0)=n1(0)=t(0)=0;n0(0)<shape(0);n0(0)++,n1(0)++,t(0)++)
//	  for(n0(1)=n1(1)=t(1)=0;n0(1)<shape(1);n0(1)++,n1(1)++,t(1)++)
//	    buf(t) = (tmp(n0)*conj(tmp(n1)));

//      }
      //{
         //skyJones.put(buf);
	// ostringstream tt;
         //String name("skyjones.im");
         //tt << name << "_" << inStokes;    
         //storeImg(tt.string(),skyJones);
       //}	

    //cout<<"Regular skyjones \n";
  }
  
//  void VLACalcIlluminationConvFunc::skyMuller(ImageInterface<Complex>& skyJones)
//  {
//    Int index = skyJones.coordinates().findCoordinate(Coordinate::STOKES);
//    Int inStokes = skyJones.coordinates().stokesCoordinate(index).stokes()(0);
//    Array<Complex> buf=skyJones.get();
    //Vector<Int> shape(buf.shape().asVector());
//    IPosition shape=skyJones.shape();
//    skyMuller(buf,shape, inStokes);
//    skyJones.put(buf);
//  }

  void VLACalcIlluminationConvFunc::skyMuller(ImageInterface<Complex>& skyJones, Int muellerTerm)
  {
    Int index = skyJones.coordinates().findCoordinate(Coordinate::STOKES);
    Int inStokes = skyJones.coordinates().stokesCoordinate(index).stokes()(0);
    Array<Complex> buf=skyJones.get(),tmp;
    IPosition shape=skyJones.shape();
    if(muellerTerm == -1)
    {
      //cout<<"####Temp - bypassing the full mueller convolution function generation \n";    
	skyMuller(buf,shape,inStokes);
	skyJones.put(buf);
    }
    else
    {
      //cout<<"####Temp - Proceeding with IQUV convolution function generation\n";
    Array<Complex> M0,M1,M2,M3;
    //Vector<Int> shape(buf.shape().asVector());
//    IPosition shape=skyJones.shape();
//    cout<<"The shape of sky Jones matrix is "<<shape<<"\n";
    Array<Complex> Jp,Jq,Jpq,Jqp;
//    skyMuller(buf,shape, inStokes);

    // if (muellerTerm == 5)
    // {
    //   skyJones.put(buf);
    //   String name("skyjones5.im");
    //   storeImg(name,skyJones);
    // }
    // Int index = skyJones.coordinates().findCoordinate(Coordinate::STOKES);
    // Int inStokes = skyJones.coordinates().stokesCoordinate(index).stokes()(0);
    // Array<Complex> buf=skyJones.get(),tmp;
    
    IPosition t(4,0,0,0,0),n0(4,0,0,0,0),n1(4,0,0,0,0);
    
    skyJones.put(buf);
//    ostringstream tt;
//    String name("skyjones.im");
//    tt << name << "_"<< inStokes;
//    storeImg(String(tt),skyJones);
//    skyJones.put(buf);
//  cout<<"Finished writing the initial sky jones \n";
//    exit(0);
//    skyMuller(buf,shape, inStokes);

    Float Normalizesq,pqscale=100.0;
    Int midx,midy;
    Normalizesq=0;
    midx = shape(0)/2; // This gives the central pixel(not the peak) of RR and LL.
    midy = shape(1)/2; // This normalization scheme was tested to be correct in the python code.
    
    tmp=buf;
    for(t(2)=0;t(2)<shape(2);t(2)++) // Start with poln axis as we want to loop in freq for the moment
	for(t(3)=0;t(3)<shape(3);t(3)++) // The freq axis each one of 4 chans contains a jones element
	    for(t(1)=0;t(1)<shape(1);t(1)++)
		for(t(0)=0;t(0)<shape(0);t(0)++)
		    if((t(0)== midx)&&(t(1)==midy))
			Normalizesq = Normalizesq + abs(buf(t)*buf(t))/2.0; // This needs to be changed so that Normalizesq stays complex 


    for(t(2)=0;t(2)<shape(2);t(2)++)
	 for(t(3)=0;t(3)<shape(3);t(3)++)
	    for(t(1)=0;t(1)<shape(1);t(1)++)
	  	for(t(0)=0;t(0)<shape(0);t(0)++)
		   if((t(3)==1)||(t(3)==2))
		   	tmp(t)=pqscale*conj(tmp(t)/sqrt(Normalizesq)); // This is the crosshand scale factor to be determined and hardcoded, currently is arbitrary.
		   
		   else
		   	tmp(t)=conj(tmp(t)/sqrt(Normalizesq));
	
//  cout<<"The Jones Matrix has been normalized using:"<< sqrt(Normalizesq)<<"\n";
    skyJones.put(tmp);
   // ostringstream tt1;
   // String name1("skyjones_normalized_conj.im");
   // tt1 << name1 << "_"<< inStokes;
   // storeImg(String(tt1),skyJones);
   // // exit(0);
   // // skyJones.put(tmp);
   // cout<<"Finished writing the normalized conjugate sky jones \n";
		
//    cout<<"Begining the compute of Mueller Matrix term images \n";

    IPosition sliceStart0(4,0,0,0,0),sliceStart1(4,0,0,0,1),sliceLength0(4,shape(0),shape(1),1,1),sliceLength1(4,shape(0),shape(1),1,1);
    IPosition sliceStart2(4,0,0,0,2),sliceStart3(4,0,0,0,3),sliceLength2(4,shape(0),shape(1),1,1),sliceLength3(4,shape(0),shape(1),1,1);
    Slicer s0(sliceStart0,sliceLength0),s1(sliceStart1,sliceLength1);
    Slicer s2(sliceStart2,sliceLength2),s3(sliceStart3,sliceLength3);

//  For the sake of pixel math we are resizing the four initial jones buffers.
    IPosition shp=buf.shape();
//    shp = buf.shape();
//    shp(2)=shp(3);
//    shp(3)=1;
    shp(2)=shp(3)=1;
    Jp.resize(shp);
    Jpq.resize(shp);
    Jqp.resize(shp);
    Jq.resize(shp);
//    cout<<"The Jones buffer shape is :"<< shp <<"\n";

    Jp(s0)=tmp(s0);Jpq(s0)=tmp(s1);
    Jqp(s0)=tmp(s2);Jq(s0)=tmp(s3);
    M0=M1=M2=M3=tmp;
//  We will initialize the Mueller rows to zero as per need and then slice and return with only the first slice with written values
      
    M0(s0)=Jp*conj(Jp); M0(s1)=Jp*conj(Jpq); M0(s2)=Jpq*conj(Jp); M0(s3)=Jpq*conj(Jpq);
    M1(s0)=Jp*conj(Jqp); M1(s1)=Jp*conj(Jq); M1(s2)=Jpq*conj(Jqp); M1(s3)=Jpq*conj(Jq);
    M2(s0)=Jqp*conj(Jp); M2(s1)=Jqp*conj(Jpq); M2(s2)=Jq*conj(Jq); M2(s3)=Jq*conj(Jpq);
    M3(s0)=Jqp*conj(Jqp); M3(s1)=Jqp*conj(Jq); M3(s2)=Jq*conj(Jqp); M3(s3)=Jq*conj(Jq);
//  cout<<"Slicing Complete"<<"\n";
//    cout<<"Mueller row selection in place is :" << muellerTerm << "\n";
    if (muellerTerm <= 3)
      {
          M0=0;
          if (muellerTerm==0) {
              M0(s0)=Jp*conj(Jp);
          }
          else if (muellerTerm==1){
              M0(s0)=Jp*conj(Jpq);
          }
          else if (muellerTerm==2){
              M0(s0)=Jpq*conj(Jp);
          }
          else {
              M0(s0)=Jpq*conj(Jpq);
          }
          skyJones.put(M0);
          // String name2("M0.im");
          // storeImg(name2,skyJones);
	  // cout<<"Writing M0 to disk, muellerTerm : "<< muellerTerm <<"\n";
          M0.resize();
      }
      else if ((4<=muellerTerm)&&(muellerTerm<8))
      {
          M1=0;
          if (muellerTerm==4) {
              M1(s0)=Jp*conj(Jqp);
          }
          else if (muellerTerm==5){
              M1(s0)=Jp*conj(Jq);
          }
          else if (muellerTerm==6){
             M1(s0)=Jpq*conj(Jqp);
          }
          else {
              M1(s0)=Jpq*conj(Jq);
          }
          skyJones.put(M1);
          // String name3("M1.im");
          // storeImg(name3,skyJones);
	  // cout<<"Writing M1 to disk, muellerTerm : "<< muellerTerm <<"\n";
	  M1.resize();
     }
     else if ((8<=muellerTerm)&&(muellerTerm<12))
     {
          M2=0;
          if (muellerTerm==8) {
              M2(s0)=Jqp*conj(Jp);
          }
          else if (muellerTerm==9){
              M2(s0)=Jqp*conj(Jpq);
          }
          else if (muellerTerm==10){
              M2(s0)=Jq*conj(Jp);
          }
          else {
              M2(s0)=Jq*conj(Jpq);
          }
          skyJones.put(M2);
          // String name4("M2.im");
          // storeImg(name4,skyJones);
          // cout<<"Writing M2 to disk, muellerTerm : "<< muellerTerm <<"\n";
	  M2.resize();
     }
     else if ((12<=muellerTerm)&&(muellerTerm<16))
     {
          M3=0;
          if (muellerTerm==12) {
              M3(s0)=Jqp*conj(Jqp);
          }
          else if (muellerTerm==13){
              M3(s0)=Jqp*conj(Jq);
          }
          else if (muellerTerm==14){
              M3(s0)=Jq*conj(Jqp);
          }
          else {
              M3(s0)=Jq*conj(Jq);
          }
          skyJones.put(M3);
          // String name5("M3.im");
          // storeImg(name5,skyJones);
          // cout<<"Writing M3 to disk, muellerTerm : "<< muellerTerm <<"\n";
	  M3.resize();
     }
//    cout<<"Mueller Matrix row computation complete \n";	
     }
//    skyJones.put(buf);

    }
  

  
  void VLACalcIlluminationConvFunc::makeFullJones(ImageInterface<Complex>& /*pbImage*/, 
						  const VisBuffer2& /*vb*/, 
						  Bool /*doSquint*/,
						  Int /*bandID*/, 
						  Double /*freqVal*/) 
  {}

  };  
};
