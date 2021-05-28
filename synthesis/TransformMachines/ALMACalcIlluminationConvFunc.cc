//# ALMAIlluminationConvFunc.cc: Implementation for ALMAIlluminationConvFunc
//# Copyright (C) 1996,1997,1998,1999,2000,2002
//# Associated Universities, Inc. Washington DC, USA.
//# Copyright by ESO (in the framework of the ALMA collaboration)
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
#include <synthesis/TransformMachines/ALMACalcIlluminationConvFunc.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <synthesis/TransformMachines/Utils.h>
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

using namespace casacore;
namespace casa{

  //
  //------------------------------------------------------------------------
  //
  ALMACalcIlluminationConvFunc::ALMACalcIlluminationConvFunc():
    IlluminationConvFunc(),
    otherAntRayPath_p("")
  {
    ap.oversamp = 3;
    ap.x0=-6.5; ap.y0=-6.5;
    ap.dx=0.25; ap.dy=0.25;

    ap.nx=ap.ny=52;
    ap.pa=lastPA=18000000;
    ap.freq=84.; // GHz
    ap.band = BeamCalc_ALMA_3;
    IPosition shape(4,ap.nx,ap.ny,4,1);
    ap.aperture = new TempImage<Complex>();
    if (maximumCacheSize() > 0) ap.aperture->setMaximumCacheSize(maximumCacheSize());
    ap.aperture->resize(shape);
   }


  CoordinateSystem ALMACalcIlluminationConvFunc::makeUVCoords(CoordinateSystem& imageCoordSys,
							     IPosition& shape,
                                                              Double /*refFreq*/)
  {
    CoordinateSystem FTCoords = imageCoordSys;

    Int dirIndex=FTCoords.findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate dc=imageCoordSys.directionCoordinate(dirIndex);
    Vector<Bool> axes(2); axes=true;
    Vector<Int> dirShape(2); dirShape(0)=shape(0);dirShape(1)=shape(1);
    Coordinate* FTdc=dc.makeFourierCoordinate(axes,dirShape);

    FTCoords.replaceCoordinate(*FTdc,dirIndex);
    delete FTdc;

    return FTCoords;
  }
  //----------------------------------------------------------------------
  // Write PB to the pbImage
  //
  void ALMACalcIlluminationConvFunc::applyPB(ImageInterface<Float>& pbImage,
					     const VisBuffer& vb,
					     Bool doSquint,
					     Int cfKey)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape());

    TempImage<Complex> uvGrid;
    if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());
    MVFrequency freqQ(vb.msColumns().spectralWindow().refFrequencyQuant()(0));
    MEpoch obsTime(vb.msColumns().timeQuant()(0));
    String antType = ALMAAperture::antTypeStrFromType(ALMAAperture::antennaTypesFromCFKey(cfKey)[0]); // take the first antenna
    Int bandID = BeamCalc::Instance()->getBandID(freqQ.getValue(), "ALMA", ""/*bandname*/, antType, obsTime, otherAntRayPath_p);

    regridAperture(skyCS, skyShape, uvGrid, vb, doSquint, bandID);
    fillPB(*(ap.aperture),pbImage);
  }
  void ALMACalcIlluminationConvFunc::applyPB(ImageInterface<Complex>& pbImage, 
					     const VisBuffer& vb,
					     Bool doSquint,
					     Int cfKey)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape());

    TempImage<Complex> uvGrid;
    if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());

    MVFrequency freqQ(vb.msColumns().spectralWindow().refFrequencyQuant()(0));
    MEpoch obsTime(vb.msColumns().timeQuant()(0));
    String antType = ALMAAperture::antTypeStrFromType(ALMAAperture::antennaTypesFromCFKey(cfKey)[0]); // take the first antenna
    String antType2 = ALMAAperture::antTypeStrFromType(ALMAAperture::antennaTypesFromCFKey(cfKey)[1]); // take the first antenna
    
    antType=String("DA");
    antType2 = antType;

    cout << "cfkey, type1, type2 " << cfKey << " " << antType << " " << antType2 << endl;
    Int bandID = BeamCalc::Instance()->getBandID(freqQ.getValue(), "ALMA", ""/*bandname*/, antType, obsTime, otherAntRayPath_p);

    regridAperture(skyCS, skyShape, uvGrid, vb, doSquint, bandID);
    pbImage.setCoordinateInfo(skyCS);
    fillPB(*(ap.aperture),pbImage);
  }

  void ALMACalcIlluminationConvFunc::applyPB(ImageInterface<Float>& pbImage,
					     const String& telescope, const MEpoch& obsTime, 
					     const String& antType0, const String& /*antType1*/,
					     const MVFrequency& freqQ, Double pa,
					     Bool doSquint)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape());

    TempImage<Complex> uvGrid;
    if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());

    Int bandID = BeamCalc::Instance()->getBandID(freqQ.getValue(), telescope, ""/*bandname*/, antType0, obsTime, otherAntRayPath_p);
    // antType1 ignored for the moment
    regridAperture(skyCS, skyShape, uvGrid, telescope, freqQ, pa, doSquint, bandID);
    fillPB(*(ap.aperture),pbImage);
  }

  void ALMACalcIlluminationConvFunc::applyPB(ImageInterface<Complex>& pbImage, 
					     const String& telescope, const MEpoch& obsTime, 
					     const String& antType0, const String& /*antType1*/,
					     const MVFrequency& freqQ, Double pa,
					     Bool doSquint)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape());

    TempImage<Complex> uvGrid;
    if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());

    Int bandID = BeamCalc::Instance()->getBandID(freqQ.getValue(), telescope, ""/*bandname*/, antType0, obsTime, otherAntRayPath_p);
    // antType1 ignored for the moment
    regridAperture(skyCS, skyShape, uvGrid, telescope, freqQ, pa, doSquint, bandID);
    pbImage.setCoordinateInfo(skyCS);
    fillPB(*(ap.aperture),pbImage);
  }

  void ALMACalcIlluminationConvFunc::applyVP(ImageInterface<Complex>& pbImage, 
					     const String& telescope, const MEpoch& obsTime, 
					     const String& antType0, const String& /*antType1*/,
					     const MVFrequency& freqQ, Double pa,
					     Bool doSquint)
  {
    CoordinateSystem skyCS(pbImage.coordinates());
    IPosition skyShape(pbImage.shape());

    TempImage<Complex> uvGrid;
    if (maximumCacheSize() > 0) uvGrid.setMaximumCacheSize(maximumCacheSize());

    Int bandID = BeamCalc::Instance()->getBandID(freqQ.getValue(), telescope, ""/*bandname*/, antType0, obsTime, otherAntRayPath_p);
    // antType1 ignored for the moment
    regridAperture(skyCS, skyShape, uvGrid, telescope, freqQ, pa, doSquint, bandID);
    pbImage.setCoordinateInfo(skyCS);
    fillVP(*(ap.aperture),pbImage);
  }

  //
  //--------------------------------------------------------------------------
  //
  void ALMACalcIlluminationConvFunc::regridAperture(CoordinateSystem& skyCS,
						   IPosition& skyShape,
						   TempImage<Complex>& uvGrid,
						   const VisBuffer& vb,
						   Bool doSquint, Int bandID)
  {
    LogIO logIO(LogOrigin("ALMACalcIlluminationConvFunc","regrid"));
    CoordinateSystem skyCoords(skyCS);

    //UNUSED: Int index;
    Float pa;

    pa = getPA(vb);

    String telescopeName=vb.msColumns().observation().telescopeName().getColumn()[0];

    Float freqHi;
    Vector<Double> chanFreq = vb.frequency();

    freqHi = max(chanFreq);
    //freqLo = min(chanFreq);
    //Freq   = freqHi;

    regridAperture(skyCS, skyShape, uvGrid, telescopeName, MVFrequency(freqHi), 
		   pa, doSquint, bandID);
  }

  void ALMACalcIlluminationConvFunc::regridAperture(CoordinateSystem& skyCS,
						   IPosition& skyShape,
						   TempImage<Complex>& uvGrid,
						   const VisBuffer &vb,
						   const Vector<Float>& paList,
						   Bool doSquint, Int bandID)
  {
    CoordinateSystem skyCoords(skyCS);

    Float pa, Freq;
    if (bandID != -1) ap.band = bandID;
    AlwaysAssert(ap.band>=-1, AipsError);
    Vector<Double> chanFreq = vb.frequency();

    const MSSpWindowColumns& spwCol = vb.msColumns().spectralWindow();
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
    Vector<Int> poln(4);
    poln(0) = Stokes::XX;
    poln(1) = Stokes::XY;
    poln(2) = Stokes::YX;
    poln(3) = Stokes::YY;

    for(uInt ipa=0;ipa<paList.nelements();ipa++)
      {
	pa = paList[ipa];

	ap.pa=pa;
	ap.aperture->set(0.0);
	for(uInt i=0; i<4; i++){
	  BeamCalc::Instance()->calculateApertureLinPol(&ap, poln(i));
	}
	//
	// Set the phase of the aperture function to zero if doSquint==F
	// Poln. axis indices
        // 0: XX, 1:XY, 2:YX, 3:YY
	// This is electic field. 0=> Poln X, 
	//                        1=> Leakage of X->Y
	//                        2=> Leakage of Y->X
	//                        3=> Poln Y
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
	    IPosition PolnXIndex(4,0,0,0,0), PolnYIndex(4,0,0,3,0);
	    IPosition tndx(4,0,0,0,0);
	    for(tndx(3)=0;tndx(3)<apertureShape(3);tndx(3)++)   // The freq. axis
	      for(tndx(2)=0;tndx(2)<apertureShape(2);tndx(2)++) // The Poln. axis
		for(tndx(1)=0;tndx(1)<apertureShape(1);tndx(1)++)   // The spatial
		  for(tndx(0)=0;tndx(0)<apertureShape(0);tndx(0)++) // axis.
		    {
		      PolnXIndex(0)=PolnYIndex(0)=tndx(0);
		      PolnXIndex(1)=PolnYIndex(1)=tndx(1);
		      Complex val, Xval, Yval;
		      Float phase;
		      val = ap.aperture->getAt(tndx);
		      Xval = ap.aperture->getAt(PolnXIndex);
		      Yval = ap.aperture->getAt(PolnYIndex);
		      phase = arg(Xval); Xval=Complex(cos(phase),sin(phase));
		      phase = arg(Yval); Yval=Complex(cos(phase),sin(phase));
		      
		      if      (tndx(2)==0) ap.aperture->putAt(val*conj(Xval),tndx);
		      else if (tndx(2)==1) ap.aperture->putAt(val*conj(Yval),tndx);
		      else if (tndx(2)==2) ap.aperture->putAt(val*conj(Xval),tndx);
		      else if (tndx(2)==3) ap.aperture->putAt(val*conj(Yval),tndx);
		    }
	  }
	tmpAperture += *(ap.aperture);
      }
    *(ap.aperture) = tmpAperture;
    tmpAperture.resize(IPosition(1,1));//Release temp. store.

    StokesCoordinate polnCoord(poln);
    SpectralCoordinate spectralCoord(MFrequency::TOPO,Freq,1.0,0.0);
    //    uvCoords.addCoordinate(dirCoord);
    index = uvCoords.findCoordinate(Coordinate::STOKES);
    uvCoords.replaceCoordinate(polnCoord,index);
    index = uvCoords.findCoordinate(Coordinate::SPECTRAL);
    uvCoords.replaceCoordinate(spectralCoord,index);

    ap.aperture->setCoordinateInfo(uvCoords);
    // if (doSquint==false)
    //   {
    // 	String name("apperture.im");
    // 	storeImg(name,*(ap.aperture));
    //   }
    
    ftAperture(*(ap.aperture));
  }


  void ALMACalcIlluminationConvFunc::regridAperture(CoordinateSystem& skyCS,
						    IPosition& skyShape,
						    TempImage<Complex>& /*uvGrid*/,
						    const String& telescope,
						    const MVFrequency& freqQ,
						    Float pa,
						    Bool doSquint, 
						    Int bandID)
  {
    LogIO logIO(LogOrigin("ALMACalcIlluminationConvFunc","regridAperture"));
    CoordinateSystem skyCoords(skyCS);

    Int index;
    if (bandID != -1) ap.band = bandID;
    AlwaysAssert(ap.band>=-1, AipsError);

    String telescopeName=telescope;

    Float Freq;

    if (lastPA == pa){
      logIO << "Your CPU is being used to do computations for the same PA as for the previous call.  Report this!" 
	    << LogIO::NORMAL1;
    }

    lastPA = pa;
    
    Freq = freqQ.getValue();
    ap.freq = Freq/1E9;

    IPosition imsize(skyShape);

    CoordinateSystem uvCoords = makeUVCoords(skyCoords,imsize,freqQ.getValue());
    
    index = uvCoords.findCoordinate(Coordinate::LINEAR);
    LinearCoordinate lc=uvCoords.linearCoordinate(index);
    Vector<Double> incr = lc.increment();
    Double Lambda = C::c/Freq;
      
    index = skyCS.findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate SpC = skyCS.spectralCoordinate(index);
    Vector<Double> refVal = SpC.referenceValue();
    refVal(0) = freqQ.getValue();
    SpC.setReferenceValue(refVal);
    skyCS.replaceCoordinate(SpC,index);
    
    ap.nx = skyShape(0); ap.ny = skyShape(1);
    IPosition apertureShape(ap.aperture->shape());
    apertureShape(0) = ap.nx;  apertureShape(1) = ap.ny;
    ap.aperture->resize(apertureShape);
    ap.dx = abs(incr(0)*Lambda); ap.dy = abs(incr(1)*Lambda);
    
    //cout << " dx dy " << ap.dx << " " << ap.dy << endl;

    ap.x0 = -(ap.nx/2)*ap.dx; 
    ap.y0 = -(ap.ny/2)*ap.dy;
    
    ap.pa=pa;
    ap.aperture->set(0.0);

    Vector<Int> poln(4);
    poln(0) = Stokes::XX;
    poln(1) = Stokes::XY;
    poln(2) = Stokes::YX;
    poln(3) = Stokes::YY;

    for(uInt i=0; i<4; i++){
      BeamCalc::Instance()->calculateApertureLinPol(&ap, poln(i));
    }

    //
    // Set the phase of the aperture function to zero if doSquint==F
    // Poln. axis indices
    // 0: XX, 1:XY, 2:YX, 3:YY
    // This is electic field. 0=> Poln X, 
    //                        1=> Leakage of X->Y
    //                        2=> Leakage of Y->X
    //                        3=> Poln Y
    //
    // The squint is removed in the following code using
    // honest-to-god pixel indexing. If this is not the most
    // efficient method of doing this in AIPS++ (i.e. instead use
    // slices etc.), then this cost will show up in making the
    // average PB.  Since this goes over each pixel of a full
    // stokes (poln. really) complex image, look here (also) for
    // optimization (if required).
    //
    if (!doSquint){
      IPosition PolnXIndex(4,0,0,0,0), PolnYIndex(4,0,0,3,0);
      IPosition tndx(4,0,0,0,0);
      for(tndx(3)=0;tndx(3)<apertureShape(3);tndx(3)++)   // The freq. axis
	for(tndx(2)=0;tndx(2)<apertureShape(2);tndx(2)++) // The Poln. axis
	  for(tndx(1)=0;tndx(1)<apertureShape(1);tndx(1)++)   // The spatial
	    for(tndx(0)=0;tndx(0)<apertureShape(0);tndx(0)++) // axis.
	      {
		PolnXIndex(0)=PolnYIndex(0)=tndx(0);
		PolnXIndex(1)=PolnYIndex(1)=tndx(1);
		Complex val, Xval, Yval;
		Float phase;
		val = ap.aperture->getAt(tndx);
		Yval = ap.aperture->getAt(PolnYIndex);
		phase = arg(Xval);  Xval=Complex(cos(phase),sin(phase));
		phase = arg(Yval);  Yval=Complex(cos(phase),sin(phase));
		
		if      (tndx(2)==0) ap.aperture->putAt(val*conj(Xval),tndx);
		else if (tndx(2)==1) ap.aperture->putAt(val*conj(Yval),tndx);
		else if (tndx(2)==2) ap.aperture->putAt(val*conj(Xval),tndx);
		else if (tndx(2)==3) ap.aperture->putAt(val*conj(Yval),tndx);
	      }
    }
    
    StokesCoordinate polnCoord(poln);
    SpectralCoordinate spectralCoord(MFrequency::TOPO,Freq,1.0,0.0);
    
    index = uvCoords.findCoordinate(Coordinate::STOKES);
    Int inStokes = uvCoords.stokesCoordinate(index).stokes()(0);

    uvCoords.replaceCoordinate(polnCoord,index);
    index = uvCoords.findCoordinate(Coordinate::SPECTRAL);
    uvCoords.replaceCoordinate(spectralCoord,index);
    
    ap.aperture->setCoordinateInfo(uvCoords);
    //
    // Now FT the re-gridded Fourier plane to get the primary beam.
    //
    cout << "**Writing ALMA Apertures for Pol " << inStokes << " to disk" << endl;
    String rname("aperture_pol"+String::toString(inStokes)+".im");
    storeImg(rname, *(ap.aperture) , true);
    cout << "Done writing apertures to disk" << endl;

    ftAperture(*(ap.aperture));
    
  }
  
  void ALMACalcIlluminationConvFunc::fillPB(ImageInterface<Complex>& inImg,
					   ImageInterface<Complex>& outImg,
					   Bool Square)
  {
    IPosition imsize(outImg.shape());
    IPosition ndx(outImg.shape());
    IPosition inShape(inImg.shape()),inNdx;
    Vector<Int> inStokes,outStokes;
    Int index,s,index1;
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

    ndx(3)=0;
    for(ndx(2)=0;ndx(2)<imsize(2);ndx(2)++){ // The poln axes
      Bool found = false;
      for(s=0;s<inShape(2);s++){
	if (inStokes(s) == outStokes(ndx(2))){
	  found = true;
	  break;
	}
      }

      if(found){
	for(ndx(0)=0;ndx(0)<imsize(0);ndx(0)++){
	  for(ndx(1)=0;ndx(1)<imsize(1);ndx(1)++){
	    Complex cval;
	    inNdx = ndx; inNdx(2)=s;
	    cval = inImg.getAt(inNdx);
	    if (Square){
	      cval = cval*conj(cval);
	    }
	    outImg.putAt(cval*outImg.getAt(ndx),ndx);
	  }
	}
      }
      else{
	cerr << "Stokes " << outStokes(ndx(2)) << " not found in response image " << endl;
      }	

    }

  }

  void ALMACalcIlluminationConvFunc::fillVP(ImageInterface<Complex>& inImg,
					    ImageInterface<Complex>& outImg,
					    Bool Square)
  {
    IPosition imsize(outImg.shape());
    IPosition ndx(outImg.shape());
    IPosition inShape(inImg.shape()),inNdx;
    Vector<Int> inStokes,outStokes;
    Int index,s,index1;
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

    ndx(3)=0;
    for(ndx(2)=0;ndx(2)<imsize(2);ndx(2)++) // The poln axes
      {
	for(s=0;s<inShape(2);s++) if (inStokes(s) == outStokes(ndx(2))) break;
	
	for(ndx(0)=0;ndx(0)<imsize(0);ndx(0)++)
	  for(ndx(1)=0;ndx(1)<imsize(1);ndx(1)++)
	    {
	      Complex cval;
	      inNdx = ndx; inNdx(2)=s;
	      cval = inImg.getAt(inNdx);
	      cval = sqrt(sqrt(real(cval*conj(cval))));
	      if (Square) cval = cval*cval;
	      outImg.putAt(cval*outImg.getAt(ndx),ndx);
	    }
      }
  }
  
  void ALMACalcIlluminationConvFunc::fillPB(ImageInterface<Complex>& inImg,
					   ImageInterface<Float>& outImg,
					   Bool Square)
  {
    IPosition imsize(outImg.shape());
    IPosition ndx(outImg.shape());
    IPosition inShape(inImg.shape()),inNdx;
    
    Vector<Int> inStokes,outStokes;
    Int index,s;
    index = inImg.coordinates().findCoordinate(Coordinate::STOKES);
    inStokes = inImg.coordinates().stokesCoordinate(index).stokes();
    index = outImg.coordinates().findCoordinate(Coordinate::STOKES);
    outStokes = outImg.coordinates().stokesCoordinate(index).stokes();
    ndx(3)=0;

    for(ndx(2)=0;ndx(2)<imsize(2);ndx(2)++) // The poln axes
      {
	if (outStokes(ndx(2)) == Stokes::I)
	  {
	    //
	    // Fill the outImg wiht (inImg(XX)+inImage(YY))/2 
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
	else if ((outStokes(ndx(2)) == Stokes::XX) ||
		 (outStokes(ndx(2)) == Stokes::XY) ||
		 (outStokes(ndx(2)) == Stokes::YX) ||
		 (outStokes(ndx(2)) == Stokes::YY))
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
	else throw(AipsError("Unsupported Stokes found in ALMACalcIlluminationConvFunc::fillPB."));
      }
  }
  
  void ALMACalcIlluminationConvFunc::ftAperture(TempImage<Complex>& uvgrid)
  {
    //
    // Make SkyJones
    //
    LatticeFFT::cfft2d(uvgrid);
    //
    // Now make SkyMuller
    //
    skyMuller(uvgrid);
  }
  
  void ALMACalcIlluminationConvFunc::loadFromImage(String& /*fileName*/)
  {
    throw(AipsError("ALMACalcIlluminationConvFunc::loadFromImage() not yet supported."));
  };

  void ALMACalcIlluminationConvFunc::skyMuller(ImageInterface<Complex>& skyJones)
  {
    Array<Complex> buf=skyJones.get(),tmp;

    IPosition shape(buf.shape());
    IPosition sliceStart0(4,0,0,0,0),sliceStart1(4,0,0,0,0),
      sliceLength(4,shape(0),shape(1),1,1);
    //
    // Giving up on fancy slicing of arrays etc. (the commented code
    // below).  Just do pixel-by-pixel multiplications for of the
    // Jones planes. For now, computing only the diagonal of the
    // SkyMuller.
    //
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
	  for(t(1)=0;t(1)<shape(1);t(1)++)   // Y axis
	    for(t(0)=0;t(0)<shape(0);t(0)++) // X axis
	      buf(t) = buf(t)/peak;

    tmp = buf;

    t(0)=t(1)=t(2)=t(3)=0;

    t(2)=0; n0(2)=0; n1(2)=0; //XX
    for( n0(0)=n1(0)=t(0)=0; n0(0)<shape(0); n0(0)++,n1(0)++,t(0)++)
      for(n0(1)=n1(1)=t(1)=0; n0(1)<shape(1); n0(1)++,n1(1)++,t(1)++)
	buf(t) = (tmp(n0)*conj(tmp(n1)));

    t(2)=1;n0(2)=1;n1(2)=1; //XY
    for(n0(0)=n1(0)=t(0)=0; n0(0)<shape(0); n0(0)++,n1(0)++,t(0)++)
      for(n0(1)=n1(1)=t(1)=0; n0(1)<shape(1); n0(1)++,n1(1)++,t(1)++)
	buf(t) = (tmp(n0)*conj(tmp(n1)));

    t(2)=2;n0(2)=2;n1(2)=2; //YX
    for(n0(0)=n1(0)=t(0)=0; n0(0)<shape(0); n0(0)++,n1(0)++,t(0)++)
      for(n0(1)=n1(1)=t(1)=0; n0(1)<shape(1); n0(1)++,n1(1)++,t(1)++)
	buf(t) = (tmp(n0)*conj(tmp(n1)));

    t(2)=3;n0(2)=3;n1(2)=3; //YY
    for(n0(0)=n1(0)=t(0)=0; n0(0)<shape(0); n0(0)++,n1(0)++,t(0)++)
      for(n0(1)=n1(1)=t(1)=0; n0(1)<shape(1); n0(1)++,n1(1)++,t(1)++)
	buf(t) = (tmp(n0)*conj(tmp(n1)));
    /*
    sliceStart0(3)=0; sliceStart1(3)=0;
    Slicer s0(sliceStart0,sliceLength),s1(sliceStart1,sliceLength);

    buf(s0) = tmp(s1);
    buf(s0) *= tmp(s1);
    //
    // Muller[1,1]
    //
    sliceStart0(3)=0; sliceStart1(3)=1;
    buf(Slicer(sliceStart0,sliceLength)) = tmp(Slicer(sliceStart0,sliceLength))
	*tmp(Slicer(sliceStart1,sliceLength));
    //
    // Muller[2,2]
    //
    sliceStart0(3)=1; sliceStart1(3)=0;
    buf(Slicer(sliceStart0,sliceLength)) = tmp(Slicer(sliceStart0,sliceLength))
	*tmp(Slicer(sliceStart1,sliceLength));
    //
    // Muller[3,3]
    //
    sliceStart0(3)=2; sliceStart1(3)=0;
    buf(Slicer(sliceStart0,sliceLength)) = tmp(Slicer(sliceStart0,sliceLength))
	*tmp(Slicer(sliceStart1,sliceLength));
	*/
    skyJones.put(buf);
  }

};
