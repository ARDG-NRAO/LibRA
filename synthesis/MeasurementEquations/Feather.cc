//# Feather.cc: Implementation of Feather.h
//# Copyright (C) 1997-2013
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $kgolap$
#include <cmath>
#include <synthesis/MeasurementEquations/Feather.h>
#include <synthesis/MeasurementEquations/Imager.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casa/OS/HostInfo.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/IPosition.h>
#include <casa/iostream.h>
#include <casa/Logging.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <scimath/Mathematics/MathFunc.h>
#include <tables/TaQL/ExprNode.h>
#include <casa/Utilities/Assert.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Slice.h>
#include <images/Images/TempImage.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/ImageRegrid.h>
#include <images/Images/ImageUtilities.h>
#include <synthesis/TransformMachines/PBMath.h>
#include <lattices/LEL/LatticeExpr.h> 
#include <lattices/LatticeMath/LatticeFFT.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <coordinates/Coordinates/CoordinateUtil.h>
#include <coordinates/Coordinates/Projection.h>
#include <coordinates/Coordinates/ObsInfo.h>
#if ! defined(CASATOOLS)
#include <casadbus/plotserver/PlotServerProxy.h>
#include <casadbus/utilities/BusAccess.h>
#include <casadbus/session/DBusSession.h>
#endif

#include <components/ComponentModels/GaussianDeconvolver.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


  Feather::Feather(): dishDiam_p(-1.0), cweightCalced_p(false), cweightApplied_p(false), sdScale_p(1.0){
    highIm_p=NULL;
    lowIm_p=NULL;
    lowImOrig_p=NULL;
    cwImage_p=NULL;
    cwHighIm_p=NULL;
  }
  Feather::Feather(const ImageInterface<Float>& SDImage, const ImageInterface<Float>& INTImage, Float sdscale) : dishDiam_p(-1.0), cweightCalced_p(false), cweightApplied_p(false), sdScale_p(sdscale){
    
    setINTImage(INTImage);
    lowImOrig_p=NULL;
    setSDImage(SDImage);
    
    cwImage_p=NULL;
    cwHighIm_p=NULL;
    
  }
  Feather::~Feather(){
    highIm_p=NULL;
    lowIm_p=NULL;
    cwImage_p=NULL;
  }
  
  void Feather::setSDImage(const ImageInterface<Float>& SDImage){
    LogIO os(LogOrigin("Feather", "setSDImage()", WHERE));
    if(highIm_p.null())
      throw(AipsError("High res image has to be defined before SD image"));
    ImageInfo lowInfo=SDImage.imageInfo();
    lBeam_p=lowInfo.restoringBeam();
    if(lBeam_p.isNull())
      throw(AipsError("No Single dish restoring beam info in image"));
    CoordinateSystem csyslow=SDImage.coordinates();
    CountedPtr<ImageInterface<Float> > sdcopy;
    sdcopy=new TempImage<Float>(SDImage.shape(), csyslow);
    (*sdcopy).copyData(SDImage);
    ImageUtilities::copyMiscellaneous(*sdcopy, SDImage);
    if(SDImage.getDefaultMask() != "")
      Imager::copyMask(*sdcopy, SDImage,  SDImage.getDefaultMask());
    PtrHolder<ImageInterface<Float> > copyPtr;
    PtrHolder<ImageInterface<Float> > copyPtr2;
    
      
    Vector<Stokes::StokesTypes> stokesvec;
    if(CoordinateUtil::findStokesAxis(stokesvec, csyslow) <0){
      CoordinateUtil::addIAxis(csyslow);
      
      ImageUtilities::addDegenerateAxes (os, copyPtr, *sdcopy, "",
					 false, false,"I", false, false,
					 true);
      sdcopy=CountedPtr<ImageInterface<Float> >(copyPtr.ptr(), false);
      
    }
    if(CoordinateUtil::findSpectralAxis(csyslow) <0){
      CoordinateUtil::addFreqAxis(csyslow);
      ImageUtilities::addDegenerateAxes (os, copyPtr2, *sdcopy, "",
					 false, true,
					 "", false, false,
					 true);
      sdcopy=CountedPtr<ImageInterface<Float> >(copyPtr2.ptr(), false);
    }
    lowIm_p=new TempImage<Float>(highIm_p->shape(), csysHigh_p);
    // regrid the single dish image
    {
      Vector<Int> dirAxes=CoordinateUtil::findDirectionAxes(csysHigh_p);
      IPosition axes(3,dirAxes(0),dirAxes(1),2);
      Int spectralAxisIndex=CoordinateUtil::findSpectralAxis(csysHigh_p);
      axes(2)=spectralAxisIndex;
      if(sdcopy->getDefaultMask() != "")
	lowIm_p->makeMask(sdcopy->getDefaultMask(), true, true, true, true);

      ImageRegrid<Float> ir;
      ir.regrid(*lowIm_p, Interpolate2D::LINEAR, axes,  *sdcopy);
    }
    /*if(sdcopy->getDefaultMask() != ""){
      //Imager::copyMask(*lowIm_p, *sdcopy, sdcopy->getDefaultMask());
      lowIm_p->makeMask(sdcopy->getDefaultMask(), true, true);
      ImageUtilities::copyMask(*lowIm_p, *sdcopy,sdcopy->getDefaultMask() , sdcopy->getDefaultMask(), AxesSpecifier());
      lowIm_p->setDefaultMask(sdcopy->getDefaultMask());

    }
    */
    
    if(lowImOrig_p.null()){
      lowImOrig_p=new TempImage<Float>(lowIm_p->shape(), lowIm_p->coordinates(),0);
      lowImOrig_p->copyData(*lowIm_p);
      lBeamOrig_p=lBeam_p;
    }   
    cweightCalced_p=false;
  }

  void Feather::convolveINT(const GaussianBeam& newHighBeam){
    GaussianBeam toBeUsed(Quantity(0.0, "arcsec"),Quantity(0.0, "arcsec"), Quantity(0.0, "deg")) ;
    Bool retval=true;
    try {
      //cerr << "highBeam " << hBeam_p.getMajor() << " " << hBeam_p.getMinor() << " " << hBeam_p.getPA() << endl; 
      retval=
      GaussianDeconvolver::deconvolve(toBeUsed, newHighBeam, hBeam_p);
      //cerr << "beam to be used " << toBeUsed.getMajor() << " " << toBeUsed.getMinor() << "  " << toBeUsed.getPA() << endl;
    }
    catch (const AipsError& x) {
      if(toBeUsed.getMajor("arcsec")==0.0)
	throw(AipsError("new Beam may be smaller than the beam of original Interferometer  image"));	
    }
    try{
      StokesImageUtil::Convolve(*highIm_p, toBeUsed, true);
    }
    catch(const AipsError& x){
      throw(AipsError("Could not convolve INT image for some reason; try a lower resolution may be"));
    }
    hBeam_p=newHighBeam; 

    //need to  redo feather  application
    cweightApplied_p=false;
    (void)retval; // avoid compiler warning
  }

  void Feather::setINTImage(const ImageInterface<Float>& INTImage){
    LogIO os(LogOrigin("Feather", "setINTImage()", WHERE));
    ImageInfo highInfo=INTImage.imageInfo();
    hBeam_p=highInfo.restoringBeam();
    if(hBeam_p.isNull())
      throw(AipsError("No high-resolution restoring beam info in image"));
    csysHigh_p=INTImage.coordinates();
    CountedPtr<ImageInterface<Float> > intcopy=new TempImage<Float>(INTImage.shape(), csysHigh_p);
    (*intcopy).copyData(INTImage);
    ImageUtilities::copyMiscellaneous(*intcopy, INTImage);
    if(INTImage.getDefaultMask() != "")
      Imager::copyMask(*intcopy, INTImage,  INTImage.getDefaultMask());
    PtrHolder<ImageInterface<Float> > copyPtr;
    PtrHolder<ImageInterface<Float> > copyPtr2;
    
      
    Vector<Stokes::StokesTypes> stokesvec;
    if(CoordinateUtil::findStokesAxis(stokesvec, csysHigh_p) <0){
      CoordinateUtil::addIAxis(csysHigh_p);
      ImageUtilities::addDegenerateAxes (os, copyPtr, *intcopy, "",
					 false, false,"I", false, false,
					 true);
      intcopy=CountedPtr<ImageInterface<Float> >(copyPtr.ptr(), false);
      
    }
    if(CoordinateUtil::findSpectralAxis(csysHigh_p) <0){
      CoordinateUtil::addFreqAxis(csysHigh_p);
      ImageUtilities::addDegenerateAxes (os, copyPtr2, *intcopy, "",
					 false, true,
					 "", false, false,
					 true);
      intcopy=CountedPtr<ImageInterface<Float> >(copyPtr2.ptr(), false);
    }


    highIm_p=new TempImage<Float>(intcopy->shape(), csysHigh_p);
    
    highIm_p->copyData(*intcopy);
    ImageUtilities::copyMiscellaneous(*highIm_p, *intcopy);
    String maskname=intcopy->getDefaultMask();
    if(maskname != ""){
      Imager::copyMask(*highIm_p, *intcopy, maskname);

    }
    cweightCalced_p=false;

  }

  Bool Feather::setEffectiveDishDiam(const Float xdiam, const Float ydiam){
    dishDiam_p=ydiam >0 ? min(xdiam, ydiam) : xdiam;
    {//reset to the original SD image
      lowIm_p->copyData(*lowImOrig_p);
      lBeam_p=lBeamOrig_p;
    }
    //Change the beam of SD image
    Double freq  = worldFreq(lowIm_p->coordinates(), 0);
    //cerr << "Freq " << freq << endl;
    Quantity halfpb=Quantity(1.22*C::c/freq/dishDiam_p, "rad");
    GaussianBeam newBeam(halfpb, halfpb, Quantity(0.0, "deg"));
    GaussianBeam toBeUsed;
    try {
        GaussianDeconvolver::deconvolve(toBeUsed, newBeam, lBeam_p);
    }
    catch (const AipsError& x) {
      throw(AipsError("Beam due to new effective diameter may be smaller than the beam of original dish image"));
    }
    try{
      //cerr <<"To be used " << toBeUsed.getMajor() << "  " << toBeUsed.getMinor() <<
      //"  " << toBeUsed.getPA() << endl;
      StokesImageUtil::Convolve(*lowIm_p, toBeUsed, true);
    }
    catch(const AipsError& x){
      throw(AipsError("Could not convolve SD image for some reason; try a smaller effective diameter may be"));
    }
    lBeam_p=newBeam; 
    //reset cweight if it was calculated already
    cweightCalced_p=false;
    cweightApplied_p=false;

    return true;
  }
  void Feather::getEffectiveDishDiam(Float& xdiam, Float& ydiam){
    if(dishDiam_p <0){
      if(lBeam_p.isNull())
	throw(AipsError("No Single dish restoring beam info in image"));
      Double maj=lBeam_p.getMajor("rad");
      Double freq  = worldFreq(csysHigh_p, 0);
    //cerr << "Freq " << freq << endl;
      dishDiam_p=1.22*C::c/freq/maj;
    }
    xdiam=dishDiam_p;
    ydiam=dishDiam_p;
  }
  void Feather::setSDScale(Float sdscale){
    sdScale_p=sdscale;
  }
  
  void Feather::getFTCutSDImage(Vector<Float>& ux, Vector<Float>& xamp, Vector<Float>& uy, Vector<Float>& yamp, const Bool radial){
     if(radial){
      uy.resize();
      yamp.resize();
      return getRadialCut(ux, xamp, *lowIm_p);
    }
    
    TempImage<Complex> cimagelow(lowIm_p->shape(), lowIm_p->coordinates() );
    StokesImageUtil::From(cimagelow, *lowIm_p);
    LatticeFFT::cfft2d( cimagelow );
    getCutXY(ux, xamp, uy, yamp, cimagelow);
    
  }
  void Feather::getFTCutIntImage(Vector<Float>& ux, Vector<Float>& xamp, Vector<Float>& uy, Vector<Float>& yamp, const Bool radial){
    if(radial){
      uy.resize();
      yamp.resize();
      return getRadialCut(ux, xamp, *highIm_p);
    }
    TempImage<Complex> cimagehigh(highIm_p->shape(), highIm_p->coordinates() );
    StokesImageUtil::From(cimagehigh, *highIm_p);
    LatticeFFT::cfft2d( cimagehigh );
    getCutXY(ux, xamp, uy, yamp, cimagehigh);

  }
  void Feather::getFeatherINT(Vector<Float>& ux, Vector<Float>& xamp, Vector<Float>& uy, Vector<Float>& yamp, Bool radial){

    calcCWeightImage();
    if(radial){
      uy.resize();
      yamp.resize();
      getRadialCut(xamp, *cwImage_p);
      getRadialUVval(xamp.shape()[0], cwImage_p->shape(), cwImage_p->coordinates(), ux);
      
 
    }
    else{
      getCutXY(ux, xamp, uy, yamp, *cwImage_p);
    }
  }
  void Feather::getFeatherSD(Vector<Float>& ux, Vector<Float>& xamp, Vector<Float>& uy, Vector<Float>& yamp, Bool radial, Bool normalize){
    calcCWeightImage();
    Vector<Float> xampInt, yampInt;
    if(radial){
      uy.resize();
      yamp.resize();
      getRadialCut(xampInt, *cwImage_p);
      getRadialUVval(xampInt.shape()[0], cwImage_p->shape(), cwImage_p->coordinates(), ux);
	
    }
    else{
      getCutXY(ux, xampInt, uy, yampInt, *cwImage_p);
      yamp.resize();
      yamp=(Float(1.0) - yampInt);
      if(!normalize) 
	yamp=yamp*Float(sdScale_p*hBeam_p.getArea("arcsec2")/lBeam_p.getArea("arcsec2"));
    }

      xamp.resize();
      xamp=(Float(1.0) - xampInt);
      if(!normalize)
	xamp=xamp*Float(sdScale_p*hBeam_p.getArea("arcsec2")/lBeam_p.getArea("arcsec2"));
      

  }
  void Feather::getFeatheredCutSD(Vector<Float>& ux, Vector<Float>& xamp, Vector<Float>& uy, Vector<Float>& yamp, Bool radial){

    getFTCutSDImage(ux, xamp, uy,yamp, radial);
    xamp *=Float(sdScale_p*hBeam_p.getArea("arcsec2")/lBeam_p.getArea("arcsec2"));

    if(yamp.nelements() >0)
      yamp *=Float(sdScale_p*hBeam_p.getArea("arcsec2")/lBeam_p.getArea("arcsec2"));
  }
  
  void Feather::getFeatheredCutINT(Vector<Float>& ux, Vector<Float>& xamp, Vector<Float>& uy, Vector<Float>& yamp, Bool radial){
    applyFeather();
    if(radial){
      uy.resize();
      yamp.resize();
      getRadialCut(xamp, *cwHighIm_p);
      getRadialUVval(xamp.shape()[0], cwHighIm_p->shape(), cwHighIm_p->coordinates(), ux);
    }
    else
      getCutXY(ux, xamp, uy, yamp, *cwHighIm_p);
  }

  void Feather::clearWeightFlags(){
    cweightCalced_p=false;
    cweightApplied_p = false;
  }

  void Feather::applyFeather(){
    if(cweightApplied_p)
      return;
    calcCWeightImage();
    if(highIm_p.null())
      throw(AipsError("No high resolution image set"));
    cwHighIm_p=new TempImage<Complex>(highIm_p->shape(), highIm_p->coordinates() );
    StokesImageUtil::From(*cwHighIm_p, *highIm_p);
    LatticeFFT::cfft2d( *cwHighIm_p );
    Vector<Int> extraAxes(cwHighIm_p->shape().nelements()-2);
    if(extraAxes.nelements() > 0){
      
      if(extraAxes.nelements() ==2){
	Int n3=cwHighIm_p->shape()(2);
	Int n4=cwHighIm_p->shape()(3);
	IPosition blc(cwHighIm_p->shape());
	IPosition trc(cwHighIm_p->shape());
	blc(0)=0; blc(1)=0;
	trc(0)=cwHighIm_p->shape()(0)-1;
	trc(1)=cwHighIm_p->shape()(1)-1;
	for (Int j=0; j < n3; ++j){
	  for (Int k=0; k < n4 ; ++k){
	    blc(2)=j; trc(2)=j;
	      blc(3)=k; trc(3)=k;
	      Slicer sl(blc, trc, Slicer::endIsLast);
	      SubImage<Complex> cimagehighSub(*cwHighIm_p, sl, true);
	      cimagehighSub.copyData(  (LatticeExpr<Complex>)((cimagehighSub * (*cwImage_p))));
	    }
	  }
	}
      }
      else{
	cwHighIm_p->copyData(  
			     (LatticeExpr<Complex>)(((*cwHighIm_p) * (*cwImage_p) )));
      }
    cweightApplied_p=true;

  }

  void Feather::calcCWeightImage(){
    if(cweightCalced_p)
      return;
    if(highIm_p.null())
      throw(AipsError("No Interferometer image was defined"));
    IPosition myshap(highIm_p->shape());
    Vector<Int> dirAxes;
    dirAxes=CoordinateUtil::findDirectionAxes(highIm_p->coordinates());
    for( uInt k=0; k< myshap.nelements(); ++k){
      if( (Int(k) != dirAxes[0]) && (Int(k) != dirAxes[1]))
	myshap(k)=1;
    }
      
    cwImage_p=new TempImage<Complex>(myshap, highIm_p->coordinates());
    {
      TempImage<Float> lowpsf(myshap, cwImage_p->coordinates());
      lowpsf.set(0.0);
      IPosition center(4, Int((myshap(0)/4)*2), 
		       Int((myshap(1)/4)*2),0,0);
      lowpsf.putAt(1.0, center);
      StokesImageUtil::Convolve(lowpsf, lBeam_p, false);
      StokesImageUtil::From(*cwImage_p, lowpsf);
    }
    LatticeFFT::cfft2d( *cwImage_p );
    LatticeExprNode node = max( *cwImage_p );
    Float fmax = abs(node.getComplex());
    cwImage_p->copyData(  (LatticeExpr<Complex>)( 1.0f - (*cwImage_p)/fmax ) );
    cweightCalced_p=true;
    cweightApplied_p=false;
  }

  void Feather::getCutXY(Vector<Float>& ux, Vector<Float>& xamp, 
			 Vector<Float>& uy, Vector<Float>& yamp, 
			 const ImageInterface<Float>& image){

   
    TempImage<Complex> cimage(image.shape(), image.coordinates() );
    StokesImageUtil::From(cimage, image);
    if(image.getDefaultMask()!=""){
      ImageRegion elMask=image.getRegion(image.getDefaultMask(),
					 RegionHandler::Masks); 
      LatticeRegion latReg=elMask.toLatticeRegion(image.coordinates(), image.shape());
      ArrayLattice<Bool> pixmask(latReg.get());
      LatticeExpr<Complex> myexpr(iif(pixmask, cimage, Complex(0.0)) );
      cimage.copyData(myexpr);
    
    } 
      
    LatticeFFT::cfft2d( cimage );
    getCutXY(ux, xamp, uy, yamp,  cimage);
    

  }

  void Feather::getRadialCut(Vector<Float>& radius, Vector<Float>& radialAmp, 
			     const ImageInterface<Float>& image){
    
    TempImage<Complex> cimage(image.shape(), image.coordinates() );
    StokesImageUtil::From(cimage, image);
    LatticeFFT::cfft2d( cimage );
    radialAmp.resize();
    getRadialCut(radialAmp, cimage);

    getRadialUVval(radialAmp.shape()[0], cimage.shape(), cimage.coordinates(), radius); 
    ////Get the radial value in uv- units.
    /* Double freq=worldFreq(image.coordinates(), 0);
    Int dirCoordIndex=image.coordinates().findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate dc=image.coordinates().directionCoordinate(dirCoordIndex);
    Vector<Bool> axes(2); axes(0)=true;axes(1)=true;
    Vector<Int> elshape(2); 
    Vector<Int> directionIndex=CoordinateUtil::findDirectionAxes(image.coordinates());
    elshape(0)=image.shape()[directionIndex(0)];
    elshape(1)=image.shape()[directionIndex(1)];
    Coordinate* ftdc=dc.makeFourierCoordinate(axes,elshape);	
    Vector<Double> xpix(radialAmp.nelements());
    indgen(xpix);
    xpix +=Double(image.shape()[directionIndex(0)])/2.0;
    Matrix<Double> pix(2, xpix.nelements());
    Matrix<Double> world(2, xpix.nelements());
    Vector<Bool> failures;
    pix.row(1)=elshape(1)/2;
    pix.row(0)=xpix;
    ftdc->toWorldMany(world, pix, failures);
    xpix=world.row(0);
    xpix=fabs(xpix)*(C::c)/freq;
    radius.resize(xpix.nelements());
    convertArray(radius, xpix);
    */

  }
  void Feather::getRadialUVval(const Int npix, const IPosition& imshape, const CoordinateSystem& csys, Vector<Float>& radius){
    
////Get the radial value in uv- units.
    Double freq=worldFreq(csys, 0);
    Int dirCoordIndex=csys.findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate dc=csys.directionCoordinate(dirCoordIndex);
    Vector<Bool> axes(2); axes(0)=true;axes(1)=true;
    Vector<Int> elshape(2); 
    Vector<Int> directionIndex=CoordinateUtil::findDirectionAxes(csys);
    elshape(0)=imshape[directionIndex(0)];
    elshape(1)=imshape[directionIndex(1)];
    Coordinate* ftdc=dc.makeFourierCoordinate(axes,elshape);	
    Vector<Double> xpix(npix);
    indgen(xpix);
    xpix +=Double(imshape[directionIndex(0)])/2.0;
    Matrix<Double> pix(2, npix);
    Matrix<Double> world(2, npix);
    Vector<Bool> failures;
    pix.row(1)=elshape(1)/2;
    pix.row(0)=xpix;
    ftdc->toWorldMany(world, pix, failures);
    xpix=world.row(0);
    xpix=fabs(xpix)*(C::c)/freq;
    radius.resize(xpix.nelements());
    convertArray(radius, xpix);

  } 
  void Feather::getRadialCut(Vector<Float>& radialAmp, ImageInterface<Complex>& ftimage) {
    CoordinateSystem ftCoords(ftimage.coordinates());
    //////////////////////
    /*{
      PagedImage<Float> thisScreen(ftimage.shape(), ftCoords, "FFTimage");
      LatticeExpr<Float> le(abs(ftimage));
      thisScreen.copyData(le);
      }*/
   /////////////////////
    Vector<Int> directionIndex=CoordinateUtil::findDirectionAxes(ftCoords);
    Int spectralIndex=CoordinateUtil::findSpectralAxis(ftCoords);
    IPosition start=ftimage.shape();
    IPosition shape=ftimage.shape();
    Int centreX=shape[directionIndex(0)]/2;
    Int centreY=shape[directionIndex(1)]/2;
    Int arrLen=min(centreX, centreY);
    
    radialAmp.resize(arrLen);
    radialAmp.set(0.0);
    Array<Complex> tmpval;
     
     for(uInt k=0; k < shape.nelements(); ++k){
       start[k]=0;
       ///value for a single pixel in x, y, and pol
       if((k != uInt(spectralIndex)))
	 shape[k]=1;
     }
     Int counter;
     Float sumval;
     for (Int pix =0 ; pix < arrLen; ++pix){
       sumval=0;
       counter=0;
       for (Int xval=0; xval <= pix; ++xval){
	 Int yval=std::lround(sqrt(Double(pix*pix-xval*xval)));
	 start[directionIndex[0]]=xval+centreX;
	 start[directionIndex[1]]=yval+centreY;
	 tmpval.resize();
	 ftimage.getSlice(tmpval, start, shape, true); 
	 sumval+=fabs(mean(tmpval));
	 counter+=1;
       }
       radialAmp[pix]=sumval/float(counter); 
     }
     

  }

  void Feather::getCutXY(Vector<Float>& ux, Vector<Float>& xamp, 
			 Vector<Float>& uy, Vector<Float>& yamp, ImageInterface<Complex>& ftimage){

    CoordinateSystem ftCoords(ftimage.coordinates());
    Double freq=worldFreq(ftCoords, 0);
	////
    Vector<Int> directionIndex=CoordinateUtil::findDirectionAxes(ftCoords);
    Int spectralIndex=CoordinateUtil::findSpectralAxis(ftCoords);
    Array<Complex> tmpval;
    Vector<Complex> meanval;
    IPosition start=ftimage.shape();
    IPosition shape=ftimage.shape();
    shape[directionIndex(0)]=shape[directionIndex(0)]/2;
    shape[directionIndex(1)]=shape[directionIndex(1)]/2;
    for(uInt k=0; k < shape.nelements(); ++k){
      start[k]=0;
      if((k != uInt(directionIndex(1))) && (k != uInt(spectralIndex)))
	shape[k]=1;
    }
    start[directionIndex(1)]=ftimage.shape()[directionIndex(1)]/2;
    start[directionIndex(0)]=ftimage.shape()[directionIndex(0)]/2;
    ftimage.getSlice(tmpval, start, shape, true);
    if(shape[spectralIndex] >1){
      meanval.resize(shape[directionIndex(1)]);
      Matrix<Complex> retmpval(tmpval);
      Bool colOrRow=spectralIndex > directionIndex(1);
      for (uInt k=0; k < meanval.nelements(); ++k){
	meanval[k]=colOrRow ? mean(retmpval.row(k)) : mean(retmpval.column(k));
      }
    }
    else{
      meanval=tmpval;
    }
    xamp.resize();
    xamp=amplitude(meanval);
    tmpval.resize();
    shape=ftimage.shape();
    shape[directionIndex(0)]=shape[directionIndex(0)]/2;
    shape[directionIndex(1)]=shape[directionIndex(1)]/2;
    for(uInt k=0; k < shape.nelements(); ++k){
      start[k]=0;
      if((k != uInt(directionIndex(0))) && (k != uInt(spectralIndex)))
	shape[k]=1;
    }
    start[directionIndex(1)]=ftimage.shape()[directionIndex(1)]/2;
    start[directionIndex(0)]=ftimage.shape()[directionIndex(0)]/2;
    ftimage.getSlice(tmpval, start, shape, true);
    if(shape[spectralIndex] >1){
      meanval.resize(shape[directionIndex(0)]);
      Bool colOrRow=spectralIndex > directionIndex(0);
      Matrix<Complex> retmpval(tmpval);
      for (uInt k=0; k < meanval.nelements(); ++k){
	meanval[k]=colOrRow ? mean(retmpval.row(k)) : mean(retmpval.column(k));
      }
    }
    else{
    	meanval.resize( tmpval.size());
      meanval=tmpval;
    }
    yamp.resize();
    yamp=amplitude(meanval); 
    Int dirCoordIndex=ftCoords.findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate dc=ftCoords.directionCoordinate(dirCoordIndex);
    Vector<Bool> axes(2); axes(0)=true;axes(1)=true;
    Vector<Int> elshape(2); 
    elshape(0)=ftimage.shape()[directionIndex(0)];
    elshape(1)=ftimage.shape()[directionIndex(1)];
    Coordinate* ftdc=dc.makeFourierCoordinate(axes,elshape);	
    Vector<Double> xpix(xamp.nelements());
    indgen(xpix);
    xpix +=Double(ftimage.shape()[directionIndex(0)])/2.0;
    Matrix<Double> pix(2, xpix.nelements());
    Matrix<Double> world(2, xpix.nelements());
    Vector<Bool> failures;
    pix.row(1)=elshape(0)/2;
    pix.row(0)=xpix;
    ftdc->toWorldMany(world, pix, failures);
    xpix=world.row(0);
	//cerr << "xpix " << xpix << endl;
    xpix=fabs(xpix)*(C::c)/freq;
    Vector<Double> ypix(yamp.nelements());
    indgen(ypix);
    ypix +=Double(ftimage.shape()[directionIndex(1)])/2.0;
    pix.resize(2, ypix.nelements());
    world.resize();
    pix.row(1)=ypix;
    pix.row(0)=elshape(1)/2;
    ftdc->toWorldMany(world, pix, failures);
    ypix=world.row(1);
    ypix=fabs(ypix)*(C::c/freq);
    ux.resize(xpix.nelements());
    uy.resize(ypix.nelements());
    convertArray(ux, xpix);
    convertArray(uy, ypix);
    

  } 
  
	

  Bool Feather::saveFeatheredImage(const String& imagename){
    applyFeather();
    Int stokesAxis, spectralAxis;
    spectralAxis=CoordinateUtil::findSpectralAxis(csysHigh_p);
    Vector<Stokes::StokesTypes> stokesvec;
    stokesAxis=CoordinateUtil::findStokesAxis(stokesvec, csysHigh_p);
    Vector<Int> dirAxes=CoordinateUtil::findDirectionAxes(csysHigh_p);
    Int n3=highIm_p->shape()(stokesAxis);
    Int n4=highIm_p->shape()(spectralAxis);
    IPosition blc(highIm_p->shape());
    IPosition trc(highIm_p->shape());
    blc(dirAxes(0))=0; blc(dirAxes(1))=0;
    trc(dirAxes(0))=highIm_p->shape()(dirAxes(0))-1;
    trc(dirAxes(1))=highIm_p->shape()(dirAxes(1))-1;
    TempImage<Complex>cimagelow(lowIm_p->shape(), lowIm_p->coordinates());
    StokesImageUtil::From(cimagelow, *lowIm_p);
    LatticeFFT::cfft2d( cimagelow);
    Float sdScaling  = sdScale_p*hBeam_p.getArea("arcsec2")/lBeam_p.getArea("arcsec2");
    for (Int j=0; j < n3; ++j){
      for (Int k=0; k < n4 ; ++k){
	blc(stokesAxis)=j; trc(stokesAxis)=j;
	blc(spectralAxis)=k; trc(spectralAxis)=k;
	Slicer sl(blc, trc, Slicer::endIsLast);
	SubImage<Complex> cimagehighSub(*cwHighIm_p, sl, true);
	SubImage<Complex> cimagelowSub(cimagelow, sl, true);
	cimagelowSub.copyData(  (LatticeExpr<Complex>)((cimagehighSub + cimagelowSub * sdScaling)));
      }
    }
    // FT back to image plane
    LatticeFFT::cfft2d( cimagelow, false);
    
    // write to output image
    PagedImage<Float> featherImage(highIm_p->shape(), highIm_p->coordinates(), imagename );
    StokesImageUtil::To(featherImage, cimagelow);
    ImageUtilities::copyMiscellaneous(featherImage, *highIm_p);
    String maskofHigh=highIm_p->getDefaultMask();
    String maskofLow=lowIm_p->getDefaultMask();
    if(maskofHigh != String("")){
      //ImageUtilities::copyMask(featherImage, *highIm_p, "mask0", maskofHigh, AxesSpecifier());
      //featherImage.setDefaultMask("mask0");
      Imager::copyMask(featherImage, *highIm_p, maskofHigh);
      if(maskofLow != String("")){
	featherImage.pixelMask().copyData(LatticeExpr<Bool> (featherImage.pixelMask() && (lowIm_p->pixelMask())));
	
      }
    }
    else if(maskofLow != String("")){
      //ImageUtilities::copyMask(featherImage, *lowIm_p, "mask0", maskofLow, AxesSpecifier());
     Imager::copyMask(featherImage, *lowIm_p, maskofLow); 
    }
    
    return true;
  }

  void Feather::applyDishDiam(ImageInterface<Complex>& image, GaussianBeam& beam, Float effDiam, ImageInterface<Float>& fftim, Vector<Quantity>& extraconv){
    /*
    MathFunc<Float> dd(SPHEROIDAL);
    Vector<Float> valsph(31);
    for(Int k=0; k <31; ++k){
      valsph(k)=dd.value((Float)(k)/10.0);
    }
    Quantity fulrad((52.3/2.0*3.0/1.1117),"arcsec");
    Quantity lefreq(224.0/effDiam*9.0, "GHz");
    
    PBMath1DNumeric elpb(valsph, fulrad, lefreq);*/
    ////////////////////////
    /*{
      PagedImage<Float> thisScreen(image.shape(), image.coordinates(), "Before_apply.image");
      LatticeExpr<Float> le(abs(image));
      thisScreen.copyData(le);
      }*/
    //////////////////////
    
    Double freq  = worldFreq(image.coordinates(), 0);
    //cerr << "Freq " << freq << endl;
    Quantity halfpb=Quantity(1.22*C::c/freq/effDiam, "rad");
    //PBMath1DAiry elpb( Quantity(effDiam,"m"), Quantity(0.01,"m"),
    //				       Quantity(0.8564,"deg"), Quantity(1.0,"GHz"));
    GaussianBeam newBeam(halfpb, halfpb, Quantity(0.0, "deg"));
    try {
      
      GaussianBeam toBeUsed;
      //cerr << "beam " << beam.toVector() << endl;
      //cerr << "newBeam " << newBeam.toVector() << endl;
      GaussianDeconvolver::deconvolve(toBeUsed, newBeam, beam);
      extraconv.resize(3);
      // use the Major difference
      extraconv(0) = toBeUsed.getMajor();
      extraconv(1) = toBeUsed.getMinor();
      extraconv(2) = toBeUsed.getPA();

    }
    catch (const AipsError& x) {
      throw(AipsError("Beam due to new effective diameter may be smaller than the beam of original dish image"));
    }
    //////////////////////
    //1 GHz equivalent   
    //    halfpb=Quantity(1.22*C::c/1.0e9/effDiam, "rad");
    //cerr << "halfpb " << halfpb << endl;
    //PBMath1DGauss elpb(halfpb, Quantity(0.8564,"deg"), Quantity(1.0,"GHz"), true);
    
    fftim.set(0.0);
   
    IPosition center(4, Int((fftim.shape()(0)/4)*2), 
		     Int((fftim.shape()(1)/4)*2),0,0);
    fftim.putAt(1.0, center);
    StokesImageUtil::Convolve(fftim, newBeam, false);
    StokesImageUtil::From(image, fftim);
    /*
    TempImage<Complex> elbeamo(image.shape(), image.coordinates());
    elbeamo.set(1.0);
    MDirection wcenter;  
    {
      Int directionIndex=
	image.coordinates().findCoordinate(Coordinate::DIRECTION);
      DirectionCoordinate
	directionCoord=image.coordinates().directionCoordinate(directionIndex);
      Vector<Double> pcenter(2);
      pcenter(0) = image.shape()(directionIndex)/2;
      pcenter(1) = image.shape()(directionIndex+1)/2;    
      directionCoord.toWorld( wcenter, pcenter );
    }
    elpb.applyPB(elbeamo, elbeamo, wcenter, Quantity(0.0, "deg"), 
			   BeamSquint::NONE);
    
   
    StokesImageUtil::To(fftim, elbeamo);
    */
    ///////////////////
    //StokesImageUtil::FitGaussianPSF(fftim, 
    //    			   beam);
     //cerr << "To be convolved beam 2 " << beam.toVector() << endl;
     ////////////////

    LatticeFFT::cfft2d(image);
    //image.copyData((LatticeExpr<Complex>)(elbeamo) );
    //elbeamo.copyData(image);
    // LatticeFFT::cfft2d(elbeamo, false);
    //StokesImageUtil::To(fftim, elbeamo);
    //StokesImageUtil::FitGaussianPSF(fftim, 
    //				    beam);
    /////////
    /*{
      PagedImage<Float> thisScreen(image.shape(), image.coordinates(), "After_apply.image");
      //LatticeExpr<Float> le(abs(image));
      thisScreen.copyData(fftim);
      }*/ 
    ////////
  }


  /*
  void Feather::feather(const String& image, const ImageInterface<Float>& high, const ImageInterface<Float>& low0, const Float& sdScale, const String& lowPSF, const Bool useDefaultPB, const String& vpTableStr, Float effDiam, const Bool doPlot){

    LogIO os(LogOrigin("Feather", "feather()", WHERE));
   try {

   
      GaussianBeam hBeam , lBeam;
      Vector<Quantity> extraconv;
      ImageInfo highInfo=high.imageInfo();
      hBeam=highInfo.restoringBeam();
      ImageInfo lowInfo=low0.imageInfo();
      lBeam=lowInfo.restoringBeam();
      if(hBeam.isNull()) {
	os << LogIO::WARN 
	   << "High resolution image does not have any resolution information - will be unable to scale correctly.\n" 
	   << LogIO::POST;
      }
      
      PBMath * myPBp = 0;
      if((lowPSF=="") && lBeam.isNull()) {
	// create the low res's PBMath object, needed to apply PB 
	// to make high res Fourier weight image
	if (useDefaultPB) {
	  // look up the telescope in ObsInfo
	  ObsInfo oi = low0.coordinates().obsInfo();
	  String myTelescope = oi.telescope();
	  if (myTelescope == "") {
	    os << LogIO::SEVERE << "No telescope imbedded in low res image" 
	       << LogIO::POST;
	    os << LogIO::SEVERE << "Create a PB description with the vpmanager"
	       << LogIO::EXCEPTION;
	  }
	  Quantity qFreq;
	  {
	    Double freq  = worldFreq(low0.coordinates(), 0);
	    qFreq = Quantity( freq, "Hz" );
	  }
	  String band;
	  PBMath::CommonPB whichPB;
	  String pbName;
	  // get freq from coordinates
	  PBMath::whichCommonPBtoUse (myTelescope, qFreq, band, whichPB, 
				      pbName);
	  if (whichPB  == PBMath::UNKNOWN) {
	    os << LogIO::SEVERE << "Unknown telescope for PB type: " 
	       << myTelescope << LogIO::EXCEPTION;
	  }
	  myPBp = new PBMath(whichPB);
	} else {
	  // get the PB from the vpTable
	  Table vpTable( vpTableStr );
	  ScalarColumn<TableRecord> recCol(vpTable, (String)"pbdescription");
	  myPBp = new PBMath(recCol(0));
	}
	AlwaysAssert((myPBp != 0), AipsError);
      }

      // regrid the single dish image
      TempImage<Float> low(high.shape(), high.coordinates());
      {
	IPosition axes(2,0,1);
	if(high.shape().nelements() >2){
	  Int spectralAxisIndex=high.coordinates().
	    findCoordinate(Coordinate::SPECTRAL);
	  if(spectralAxisIndex > -1){
	    axes.resize(3);
	    axes(0)=0;
	    axes(1)=1;
	    axes(2)=spectralAxisIndex+1;
	  }
	}
	ImageRegrid<Float> ir;
	ir.regrid(low, Interpolate2D::LINEAR, axes, low0);
      }
    
      // get image center direction (needed for SD PB, which is needed for
      // the high res Fourier weight image
      MDirection wcenter;  
      {
	Int directionIndex=
	  high.coordinates().findCoordinate(Coordinate::DIRECTION);
	AlwaysAssert(directionIndex>=0, AipsError);
	DirectionCoordinate
	  directionCoord=high.coordinates().directionCoordinate(directionIndex);
	Vector<Double> pcenter(2);
	pcenter(0) = high.shape()(0)/2;
	pcenter(1) = high.shape()(1)/2;    
	directionCoord.toWorld( wcenter, pcenter );
      }
      
      // make the weight image for high res Fourier plane:  1 - normalized(FT(sd_PB))
      IPosition myshap(high.shape());
      for( uInt k=2; k< myshap.nelements(); ++k){
	myshap(k)=1;
      }
      
      TempImage<Float> lowpsf0;
      TempImage<Complex> cweight(myshap, high.coordinates());
      if(lowPSF=="") {
	os << LogIO::NORMAL // Loglevel INFO
           << "Using primary beam to determine weighting.\n" << LogIO::POST;
	if(lBeam.isNull()) {
	  cweight.set(1.0);
	  if (myPBp != 0) {
	    myPBp->applyPB(cweight, cweight, wcenter, Quantity(0.0, "deg"), 
			   BeamSquint::NONE);
	  
	    lowpsf0=TempImage<Float>(cweight.shape(), cweight.coordinates());
	    
	    os << LogIO::NORMAL // Loglevel INFO
               << "Determining scaling from SD Primary Beam.\n"
	       << LogIO::POST;
	    StokesImageUtil::To(lowpsf0, cweight);
	    StokesImageUtil::FitGaussianPSF(lowpsf0, 
					    lBeam);
	  }
	  delete myPBp;
	}
	else{
	  os << LogIO::NORMAL // Loglevel INFO
             << "Determining scaling from SD restoring beam.\n"
	     << LogIO::POST;
	  lowpsf0=TempImage<Float>(cweight.shape(), cweight.coordinates());
	  lowpsf0.set(0.0);
	  IPosition center(4, Int((cweight.shape()(0)/4)*2), 
			   Int((cweight.shape()(1)/4)*2),0,0);
	  lowpsf0.putAt(1.0, center);
	  StokesImageUtil::Convolve(lowpsf0, lBeam, false);
	  StokesImageUtil::From(cweight, lowpsf0);

	}
      }
      else {
	os << LogIO::NORMAL // Loglevel INFO
           << "Using specified low resolution PSF to determine weighting.\n" 
	   << LogIO::POST;
	// regrid the single dish psf
	PagedImage<Float> lowpsfDisk(lowPSF);
	IPosition lshape(lowpsfDisk.shape());
	lshape.resize(4);
	lshape(2)=1; lshape(3)=1;
	TempImage<Float>lowpsf(lshape,lowpsfDisk.coordinates());
	IPosition blc(lowpsfDisk.shape());
	IPosition trc(lowpsfDisk.shape());
	blc(0)=0; blc(1)=0;
	trc(0)=lowpsfDisk.shape()(0)-1;
	trc(1)=lowpsfDisk.shape()(1)-1;
	for( uInt k=2; k < lowpsfDisk.shape().nelements(); ++k){
	  blc(k)=0; trc(k)=0;	  	  
	}// taking first plane
	Slicer sl(blc, trc, Slicer::endIsLast);
	lowpsf.copyData(SubImage<Float>(lowpsfDisk, sl, false));
	lowpsf0=TempImage<Float> (myshap, high.coordinates());
	{
	  ImageRegrid<Float> ir;
	  IPosition axes(2,0,1);   // if its a cube, regrid the spectral too
	  ir.regrid(lowpsf0, Interpolate2D::LINEAR, axes, lowpsf);
	}
	if(lBeam.isNull()) {
	  os << LogIO::NORMAL // Loglevel INFO
             << "Determining scaling from low resolution PSF.\n" << LogIO::POST;
	  StokesImageUtil::FitGaussianPSF(lowpsf, lBeam);
	}
	StokesImageUtil::From(cweight, lowpsf0);
      }
 
      LatticeFFT::cfft2d( cweight );
      if(effDiam >0.0){
	//cerr << "in effdiam" << effDiam << endl;
	applyDishDiam(cweight, lBeam, effDiam, lowpsf0, extraconv);
	lowpsf0.copyData((LatticeExpr<Float>)(lowpsf0/max(lowpsf0)));
	StokesImageUtil::FitGaussianPSF(lowpsf0, lBeam);
	
	Int directionIndex=
	  cweight.coordinates().findCoordinate(Coordinate::DIRECTION);
	Image2DConvolver<Float>::convolve(
		    os, low, low, VectorKernel::toKernelType("gauss"), IPosition(2, directionIndex, directionIndex+1),
		    extraconv, true, 1.0, true
		    );

      }
      LatticeExprNode node = max( cweight );
      Float fmax = abs(node.getComplex());
      cweight.copyData(  (LatticeExpr<Complex>)( 1.0f - cweight/fmax ) );
      //Plotting part
      if(doPlot){
	CoordinateSystem ftCoords(cweight.coordinates());
	Double freq=worldFreq(ftCoords, 0);
	////
	Int directionIndex=ftCoords.findCoordinate(Coordinate::DIRECTION);
	Array<Complex> tmpval;
	IPosition start=cweight.shape();
	IPosition shape=cweight.shape()/2;
	for(uInt k=0; k < shape.nelements(); ++k){
	  start[k]=0;
	  if(k != uInt(directionIndex+1))
	    shape[k]=1;
	}
	start[directionIndex+1]=cweight.shape()[directionIndex+1]/2;
	start[directionIndex]=cweight.shape()[directionIndex]/2;
	cweight.getSlice(tmpval, start, shape, true);
	Vector<Float> x=amplitude(tmpval);
	Vector<Float> xdish=(Float(1.0) - x)*Float(sdScale);
	tmpval.resize();
	shape=cweight.shape()/2;
	for(uInt k=0; k < shape.nelements(); ++k){
	  start[k]=0;
	  if(k != uInt(directionIndex))
	    shape[k]=1;
	}
	start[directionIndex+1]=cweight.shape()[directionIndex+1]/2;
	start[directionIndex]=cweight.shape()[directionIndex]/2;
	cweight.getSlice(tmpval, start, shape, true);
	Vector<Float> y=amplitude(tmpval);
	Vector<Float> ydish=(Float(1.0)-y)*Float(sdScale);
	DirectionCoordinate dc=ftCoords.directionCoordinate(directionIndex);
	Vector<Bool> axes(2); axes(0)=true;axes(1)=true;
	Vector<Int> elshape(2); 
	elshape(0)=cweight.shape()[directionIndex];
	elshape(1)=cweight.shape()[directionIndex+1];
	Coordinate* ftdc=dc.makeFourierCoordinate(axes,elshape);	
	Vector<Double> xpix(x.nelements());
	indgen(xpix);
	xpix +=Double(cweight.shape()[directionIndex])/2.0;
	Matrix<Double> pix(2, xpix.nelements());
	Matrix<Double> world(2, xpix.nelements());
	Vector<Bool> failures;
	pix.row(1)=elshape(0)/2;
	pix.row(0)=xpix;
	ftdc->toWorldMany(world, pix, failures);
	xpix=world.row(0);
	//cerr << "xpix " << xpix << endl;
	xpix=fabs(xpix)*(C::c)/freq;
	Vector<Double> ypix(y.nelements());
	indgen(ypix);
	ypix +=Double(cweight.shape()[directionIndex+1])/2.0;
	pix.resize(2, ypix.nelements());
	world.resize();
	pix.row(1)=ypix;
	pix.row(0)=elshape(1)/2;
	ftdc->toWorldMany(world, pix, failures);
	ypix=world.row(1);
	ypix=fabs(ypix)*(C::c/freq);
	PlotServerProxy *plotter = dbus::launch<PlotServerProxy>( );
	dbus::variant panel_id = plotter->panel( "Feather function slice cuts", "Distance in m", "Amp", "Feather",
					       std::vector<int>( ), "right");

	if ( panel_id.type( ) != dbus::variant::INT ) {
	  os << LogIO::SEVERE << "failed to start plotter" << LogIO::POST;
	  return ;
	}

      
	plotter->line(dbus::af(xpix),dbus::af(x),"blue","x-interferometer weight",panel_id.getInt( ));
	plotter->line(dbus::af(xpix),dbus::af(xdish),"cyan","x-singledish weight",panel_id.getInt( ));
	
	plotter->line(dbus::af(ypix),dbus::af(y),"green","y-interferometer weight",panel_id.getInt( ));
	plotter->line(dbus::af(ypix),dbus::af(ydish),"purple","y-singledish weight",panel_id.getInt( ));

      }
      //End plotting part
      // FT high res image
      TempImage<Complex> cimagehigh(high.shape(), high.coordinates() );
      StokesImageUtil::From(cimagehigh, high);
      LatticeFFT::cfft2d( cimagehigh );
      
      // FT low res image
      TempImage<Complex> cimagelow(high.shape(), high.coordinates() );
      StokesImageUtil::From(cimagelow, low);
      LatticeFFT::cfft2d( cimagelow );


      // This factor comes from the beam volumes
      if(sdScale != 1.0)
        os << LogIO::NORMAL // Loglevel INFO
           << "Multiplying single dish data by user specified factor "
           << sdScale << ".\n" << LogIO::POST;
      Float sdScaling  = sdScale;
      if (! hBeam.isNull() && ! lBeam.isNull()) {

	Float beamFactor=
	  hBeam.getArea("arcsec2")/lBeam.getArea("arcsec2");
	os << LogIO::NORMAL // Loglevel INFO
           << "Applying additional scaling for ratio of the volumes of the high to the low resolution images : "
	   <<  beamFactor << ".\n" << LogIO::POST;
	sdScaling*=beamFactor;
      }
      else {
	os << LogIO::WARN << "Insufficient information to scale correctly.\n" 
	   << LogIO::POST;
      }

      // combine high and low res, appropriately normalized, in Fourier
      // plane. The vital point to remember is that cimagelow is already
      // multiplied by 1-cweight so we only need adjust for the ratio of beam
      // volumes
      Vector<Int> extraAxes(cimagehigh.shape().nelements()-2);
      if(extraAxes.nelements() > 0){
	
	if(extraAxes.nelements() ==2){
	  Int n3=cimagehigh.shape()(2);
	  Int n4=cimagehigh.shape()(3);
	  IPosition blc(cimagehigh.shape());
	  IPosition trc(cimagehigh.shape());
	  blc(0)=0; blc(1)=0;
	  trc(0)=cimagehigh.shape()(0)-1;
	  trc(1)=cimagehigh.shape()(1)-1;
	  for (Int j=0; j < n3; ++j){
	    for (Int k=0; k < n4 ; ++k){
	      blc(2)=j; trc(2)=j;
	      blc(3)=k; trc(3)=k;
	      Slicer sl(blc, trc, Slicer::endIsLast);
	      SubImage<Complex> cimagehighSub(cimagehigh, sl, true);
	      SubImage<Complex> cimagelowSub(cimagelow, sl, true);
	      cimagehighSub.copyData(  (LatticeExpr<Complex>)((cimagehighSub * cweight + cimagelowSub * sdScaling)));
	    }
	  }
	}
      }
      else{
	cimagehigh.copyData(  
			    (LatticeExpr<Complex>)((cimagehigh * cweight 
						    + cimagelow * sdScaling)));
      }
      // FT back to image plane
      LatticeFFT::cfft2d( cimagehigh, false);
    
      // write to output image
      PagedImage<Float> featherImage(high.shape(), high.coordinates(), image );
      StokesImageUtil::To(featherImage, cimagehigh);
      ImageUtilities::copyMiscellaneous(featherImage, high);

      try{
      { // write data processing history into image logtable
	LoggerHolder imagelog (false);
	LogSink& sink = imagelog.sink();
	LogOrigin lor(String("Feather"), String("feather()"));
	LogMessage msg(lor);
	ostringstream oos;
	oos << endl << "Imager::feather() input paramaters:" << endl
	    << "Feathered image =      '" << image   << "'" << endl
	    << "High resolution image ='" << high.name() << "'" << endl
	    << "Low resolution image = '" << low0.name()  << "'" << endl
	    << "Low resolution PSF =   '" << lowPSF  << "'" << endl << endl;
	String inputs(oos);
	sink.postLocally(msg.message(inputs));
	imagelog.flush();

	LoggerHolder& log = featherImage.logger();
	log.append(imagelog);
	log.flush();
      }
      }catch(exception& x){

	os << LogIO::WARN << "Caught exception: " << x.what()
       << LogIO::POST;
	

      }
      catch(...){
	os << LogIO::SEVERE << "Unknown exception handled" << LogIO::POST;

      }
   







   }catch(exception& x){

	os << LogIO::WARN << "Caught exception: " << x.what()
	   << LogIO::POST;  
   }
    


  }
  */
 void Feather::feather(const String& image, const ImageInterface<Float>& high, const ImageInterface<Float>& low0, const Float& sdScale, const String& lowPSF, const Bool useDefaultPB, const String& vpTableStr, Float effDiam, const Bool doHiPassFilterOnSD){

   Feather plume;
   plume.setINTImage(high);
   ImageInfo lowInfo=low0.imageInfo();
   GaussianBeam lBeam=lowInfo.restoringBeam();
   if(lBeam.isNull()) {
     getLowBeam(low0, lowPSF, useDefaultPB, vpTableStr, lBeam);
     if(lBeam.isNull())
       throw(AipsError("Do not have low resolution beam info "));
     TempImage<Float> newLow(low0.shape(), low0.coordinates());
     newLow.copyData(low0);
     lowInfo.removeRestoringBeam();
     lowInfo.setRestoringBeam(lBeam);
     newLow.setImageInfo(lowInfo);
     plume.setSDImage(newLow);    
   }
   else{
     plume.setSDImage(low0);
   }
   plume.setSDScale(sdScale);
   if(effDiam >0.0){
     plume.setEffectiveDishDiam(effDiam, effDiam);
   }
   else if(doHiPassFilterOnSD){
     Float xdiam, ydiam;
     plume.getEffectiveDishDiam(xdiam, ydiam);
     plume.setEffectiveDishDiam(xdiam, ydiam);
   }
   plume.saveFeatheredImage(image);

 }
  

  void Feather::getLowBeam(const ImageInterface<Float>& low0, const String& lowPSF, const Bool useDefaultPB, const String& vpTableStr, GaussianBeam& lBeam){
    LogIO os(LogOrigin("Feather", "getLowBeam()", WHERE));
    PBMath * myPBp = 0;
    if((lowPSF=="") && lBeam.isNull()) {
      // create the low res's PBMath object, needed to apply PB 
      // to make high res Fourier weight image
      if (useDefaultPB) {
	// look up the telescope in ObsInfo
	ObsInfo oi = low0.coordinates().obsInfo();
	String myTelescope = oi.telescope();
	if (myTelescope == "") {
	  os << LogIO::SEVERE << "No telescope imbedded in low res image" 
	     << LogIO::POST;
	  os << LogIO::SEVERE << "Create a PB description with the vpmanager"
	     << LogIO::EXCEPTION;
	}
	Quantity qFreq;
	{
	  Double freq  = worldFreq(low0.coordinates(), 0);
	  qFreq = Quantity( freq, "Hz" );
	}
	String band;
	PBMath::CommonPB whichPB;
	String pbName;
	// get freq from coordinates
	PBMath::whichCommonPBtoUse (myTelescope, qFreq, band, whichPB, 
				    pbName);
	if (whichPB  == PBMath::UNKNOWN) {
	  os << LogIO::SEVERE << "Unknown telescope for PB type: " 
	     << myTelescope << LogIO::EXCEPTION;
	}
	myPBp = new PBMath(whichPB);
      } else {
	// get the PB from the vpTable
	Table vpTable( vpTableStr );
	ScalarColumn<TableRecord> recCol(vpTable, (String)"pbdescription");
	myPBp = new PBMath(recCol(0));
      }
      AlwaysAssert((myPBp != 0), AipsError);
    }

   
    // get image center direction
    MDirection wcenter;  
    {
      Int directionIndex=
	low0.coordinates().findCoordinate(Coordinate::DIRECTION);
      AlwaysAssert(directionIndex>=0, AipsError);
      DirectionCoordinate
	directionCoord=low0.coordinates().directionCoordinate(directionIndex);
      Vector<Double> pcenter(2);
      pcenter(0) = low0.shape()(0)/2;
      pcenter(1) = low0.shape()(1)/2;    
      directionCoord.toWorld( wcenter, pcenter );
    }
    
    // make the weight image 
      IPosition myshap(low0.shape());
      for( uInt k=2; k< myshap.nelements(); ++k){
	myshap(k)=1;
      }
      
      TempImage<Float> lowpsf0;
      TempImage<Complex> cweight(myshap, low0.coordinates());
      if(lowPSF=="") {
	os << LogIO::NORMAL // Loglevel INFO
           << "Using primary beam to determine weighting.\n" << LogIO::POST;
	cweight.set(1.0);
	if (myPBp != 0) {
	  myPBp->applyPB(cweight, cweight, wcenter, Quantity(0.0, "deg"), 
			 BeamSquint::NONE);
	  
	  lowpsf0=TempImage<Float>(cweight.shape(), cweight.coordinates());
	  
	  os << LogIO::NORMAL // Loglevel INFO
	     << "Determining scaling from SD Primary Beam.\n"
	     << LogIO::POST;
	  StokesImageUtil::To(lowpsf0, cweight);
	  StokesImageUtil::FitGaussianPSF(lowpsf0, 
					  lBeam);
	}
	delete myPBp;	
      }
      else {
	os << LogIO::NORMAL // Loglevel INFO
           << "Using specified low resolution PSF to determine weighting.\n" 
	   << LogIO::POST;
	// regrid the single dish psf
	PagedImage<Float> lowpsfDisk(lowPSF);
	IPosition lshape(lowpsfDisk.shape());
	lshape.resize(4);
	lshape(2)=1; lshape(3)=1;
	TempImage<Float>lowpsf(lshape,lowpsfDisk.coordinates());
	IPosition blc(lowpsfDisk.shape());
	IPosition trc(lowpsfDisk.shape());
	blc(0)=0; blc(1)=0;
	trc(0)=lowpsfDisk.shape()(0)-1;
	trc(1)=lowpsfDisk.shape()(1)-1;
	for( uInt k=2; k < lowpsfDisk.shape().nelements(); ++k){
	  blc(k)=0; trc(k)=0;	  	  
	}// taking first plane
	Slicer sl(blc, trc, Slicer::endIsLast);
	lowpsf.copyData(SubImage<Float>(lowpsfDisk, sl, false));
	os << LogIO::NORMAL // Loglevel INFO
	   << "Determining scaling from low resolution PSF.\n" << LogIO::POST;
	StokesImageUtil::FitGaussianPSF(lowpsf, lBeam);
      }
       
 

  }

  Double Feather::worldFreq(const CoordinateSystem& cs, Int spectralpix){
    ///freq part
    Int spectralIndex=cs.findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate
      spectralCoord=
      cs.spectralCoordinate(spectralIndex);
    Vector<String> units(1); units = "Hz";
    spectralCoord.setWorldAxisUnits(units);	
    Vector<Double> spectralWorld(1);
    Vector<Double> spectralPixel(1);
    spectralPixel(0) = spectralpix;
    spectralCoord.toWorld(spectralWorld, spectralPixel);  
    Double freq  = spectralWorld(0);
    return freq;
  }





}//# NAMESPACE CASA - END
