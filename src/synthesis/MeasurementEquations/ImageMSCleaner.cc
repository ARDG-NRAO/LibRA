//# Copyright (C) 1997-2010
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
//# $Id:  $
#include <synthesis/MeasurementEquations/MatrixCleaner.h>
#include <synthesis/MeasurementEquations/ImageMSCleaner.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  ImageMSCleaner::ImageMSCleaner(): psf_p(0), dirty_p(0), mask_p(0), nPsfChan_p(0), 
				    nImChan_p(0), nPsfPol_p(0), nImPol_p(0), chanAxis_p(-1), 
				    polAxis_p(-1), nMaskChan_p(0), nMaskPol_p(0), maskThresh_p(0.9), maxResidual_p(0.0)
 {


  }
  ImageMSCleaner::ImageMSCleaner(ImageInterface<Float>& psf, 
				 ImageInterface<Float>& dirty): psf_p(&psf), 
								      dirty_p(&dirty), mask_p(0), 
								      nMaskChan_p(0), nMaskPol_p(0),
								maskThresh_p(0.9), maxResidual_p(0.0){
    
    chanAxis_p=CoordinateUtil::findSpectralAxis(dirty_p->coordinates());
    Vector<Stokes::StokesTypes> whichPols;
    polAxis_p=CoordinateUtil::findStokesAxis(whichPols, dirty_p->coordinates());
    if(chanAxis_p > -1){
      nPsfChan_p=psf_p->shape()(chanAxis_p);
      nImChan_p=dirty_p->shape()(chanAxis_p);
    }
    if(polAxis_p > -1){
      nPsfPol_p=psf_p->shape()(polAxis_p);
      nImPol_p=dirty_p->shape()(polAxis_p);
    }
  }


  ImageMSCleaner::ImageMSCleaner(const ImageMSCleaner& other){
    operator=(other);
  }

  ImageMSCleaner::~ImageMSCleaner(){

  }

  ImageMSCleaner& ImageMSCleaner::operator=(const ImageMSCleaner& other){
    if (this != &other) {
      matClean_p=other.matClean_p;
      psf_p=other.psf_p;
      dirty_p=other.dirty_p;
      mask_p=other.mask_p;
      nPsfChan_p=other.nPsfChan_p;
      nImChan_p=other.nImChan_p;
      nPsfPol_p=other.nPsfPol_p;
      nImPol_p=other.nImPol_p;
      chanAxis_p=other.chanAxis_p;
      nMaskChan_p=other.nMaskChan_p;
      nMaskPol_p=other.nMaskPol_p;
      polAxis_p=other.polAxis_p;
      scales_p=other.scales_p;
      maskThresh_p=other.maskThresh_p;
      maxResidual_p=other.maxResidual_p;
    }
    return *this;
  }
  void ImageMSCleaner::update(ImageInterface<Float>& dirty){
    dirty_p=&dirty;
    chanAxis_p=CoordinateUtil::findSpectralAxis(dirty_p->coordinates());
    Vector<Stokes::StokesTypes> whichPols;
    polAxis_p=CoordinateUtil::findStokesAxis(whichPols, dirty_p->coordinates());
    if(chanAxis_p > -1){
      nImChan_p=dirty_p->shape()(chanAxis_p);
    }
    else
      nImChan_p=0;
    if(polAxis_p > -1){
      nImPol_p=dirty_p->shape()(polAxis_p);
    }
    else
      nImPol_p=0;
  }

  void ImageMSCleaner::setPsf(ImageInterface<Float> & psf){
    psf_p=&psf;
    Int chanAxis=CoordinateUtil::findSpectralAxis(psf_p->coordinates());
    Vector<Stokes::StokesTypes> whichPols;
    Int polAxis=CoordinateUtil::findStokesAxis(whichPols, psf_p->coordinates());
    if(chanAxis > -1){
      nPsfChan_p=psf_p->shape()(chanAxis);
    }
    else
      nPsfChan_p=0;
    if(polAxis > -1){
      nPsfPol_p=psf_p->shape()(polAxis);
    }
    else
      nPsfPol_p=0;
  }
  
  void ImageMSCleaner::setscales(const Int nscs, const Float scaleInc){
    LogIO os(LogOrigin("ImageMSCleaner", "setscales()", WHERE));
    Int nscales=nscs;

    if(nscales<1) {
      os << "Using default of 5 scales" << LogIO::POST;
      nscales=5;
    }
  
    scales_p.resize(nscales);
  
    // Validate scales
    os << "Creating " << nscales << " scales" << LogIO::POST;
    scales_p(0) = 0.00001 * scaleInc;
    os << "scale 0 = 0.0 arcsec" << LogIO::POST;
    for (Int scale=1; scale<nscales;scale++) {
    scales_p(scale) =
      scaleInc * pow(10.0, (Float(scale)-2.0)/2.0);
    os << "scale " << scale << " = " << scales_p(scale)
       << " arcsec" << LogIO::POST;
    }

  }
  void  ImageMSCleaner::setscales(const Vector<Float> & scales){
    scales_p.resize(scales.nelements());
    scales_p=scales;
  }

  Bool ImageMSCleaner::setcontrol(CleanEnums::CleanType cleanType, const Int niter,
		  const Float gain, const Quantity& aThreshold,
				  const Quantity& fThreshold){
    return matClean_p.setcontrol(cleanType, niter, gain, aThreshold, fThreshold);
  }
  Bool ImageMSCleaner::setcontrol(CleanEnums::CleanType cleanType, const Int niter,
				  const Float gain, const Quantity& threshold){

    return matClean_p.setcontrol(cleanType, niter, gain, threshold);

  }
  Int ImageMSCleaner::iteration() const{
    return matClean_p.iteration();
  }
  Int ImageMSCleaner::numberIterations() const{
    return matClean_p.numberIterations();
  }
  void ImageMSCleaner::startingIteration(const Int starting){
    matClean_p.startingIteration(starting);
  }

  void ImageMSCleaner::setMask(ImageInterface<Float> & mask, 
			       const Float& maskThreshold){
  
    maskThresh_p=maskThreshold;
    mask_p=&mask;
    Int chanAxis=CoordinateUtil::findSpectralAxis(mask_p->coordinates());
    Vector<Stokes::StokesTypes> whichPols;
    Int polAxis=CoordinateUtil::findStokesAxis(whichPols, mask_p->coordinates());
    if(chanAxis > -1){
      nMaskChan_p=mask_p->shape()(chanAxis);
    }
    else
      nMaskChan_p=0;
    if(polAxis > -1){
      nMaskPol_p=mask_p->shape()(polAxis);
    }
    else
      nMaskPol_p=0;


  }

  void ImageMSCleaner::ignoreCenterBox(Bool ign){
    matClean_p.ignoreCenterBox(ign);
  }
  void ImageMSCleaner::setSmallScaleBias(const Float x){
    matClean_p.setSmallScaleBias(x);
  }
  void ImageMSCleaner::stopAtLargeScaleNegative(){
    matClean_p.stopAtLargeScaleNegative();
  }
  void ImageMSCleaner::stopPointMode(Int nStopPointMode) {
    matClean_p.stopPointMode(nStopPointMode);
  }
  Bool ImageMSCleaner::queryStopPointMode() const{
    return matClean_p.queryStopPointMode();
  }
  void ImageMSCleaner::speedup(const Float Ndouble){
    matClean_p.speedup(Ndouble);
  }

  Bool ImageMSCleaner::setupMatCleaner(const String& alg, const Int niter,
				       const Float gain, const Quantity& threshold, const Quantity& fthresh){

    LogIO os(LogOrigin("ImageMSCleaner", "setupclean()", WHERE));
    
    String algorithm=alg;
    algorithm.downcase();
    if((algorithm=="msclean")||(algorithm=="fullmsclean" )|| (algorithm=="multiscale") 
       || (algorithm=="fullmultiscale")) {
      //os << "Cleaning image using multi-scale algorithm" << LogIO::POST;
      if(scales_p.nelements()==0) {
	os << LogIO::SEVERE << "Scales not yet set" << LogIO::POST;
	return false;
      }
      //matClean_p.setscales(scaleSizes_p);
      matClean_p.setcontrol(CleanEnums::MULTISCALE, niter, gain, threshold, fthresh);
    }
    else if (algorithm=="hogbom") {
      scales_p=Vector<Float>(1,0.0);
      matClean_p.defineScales(scales_p);
      matClean_p.setcontrol(CleanEnums::HOGBOM, niter, gain, threshold, fthresh);
    } else {
      os << LogIO::SEVERE << "Unknown algorithm: " << algorithm << LogIO::POST;
      return false; 
    }

    if(algorithm=="fullmsclean" || algorithm=="fullmultiscale") {
      matClean_p.ignoreCenterBox(true);
    }
    return true;
  }

  Int ImageMSCleaner::clean(ImageInterface<Float> & modelimage, const String& algorithm, 
			    const Int niter,
			    const Float gain, const Quantity& threshold, const Quantity& fthresh, Bool /*doPlotProgress*/){


    
    
    
    Int result=0;
    ///case of single plane mask
    //now may be a time to set stuff  scales will be done later
    if(!setupMatCleaner(algorithm, niter, gain, threshold, fthresh))
      return false;
    matClean_p.defineScales(scales_p);
    //cerr << "nPol " << nMaskPol_p << " " << nPsfPol_p << " " << nImPol_p << endl;
    //cerr << "nChan " << nMaskChan_p << " " << nPsfChan_p << " " << nImChan_p << endl;
    if( (nMaskPol_p >1) && (nMaskPol_p != nImPol_p))
      throw(AipsError("Donot know how to deal with mask that has different pol axis as Image")); 
    if(mask_p && nMaskChan_p < 2 && nMaskPol_p < 2){
      Matrix<Float> maskMat;
      Array<Float> mbuf;
      mask_p->get(mbuf, true);
      maskMat.reference(mbuf);
     
      matClean_p.setMask(maskMat, maskThresh_p);
    }
    if(!psf_p)
      throw(AipsError("No PSF defined "));
    ///case of single plane psf
    if(nPsfChan_p < 2){
      Matrix<Float> psfMat;
      Array<Float> mbuf;
      if(nPsfPol_p > 1){
	///First stokes psf is good enough
	IPosition blc(dirty_p->ndim(),0);
	IPosition trc=(dirty_p->shape()) -1;
	trc(polAxis_p)=0;
	Slicer sl(blc, trc, Slicer::endIsLast);
	psf_p->getSlice(mbuf, sl, true);
      }
      else{
	psf_p->get(mbuf, true);
      }
      psfMat.reference(mbuf);
      matClean_p.setPsf(psfMat);
      matClean_p.makePsfScales();
      //matClean_p.makeScaleMasks();
      
    }

    

    //has cube axis
    if(chanAxis_p > -1 && nImChan_p > 0){
      Int npol=1;
      if(polAxis_p > -1 && nImPol_p >0){
	npol=nImPol_p;
      }
      Matrix<Float> subModel;
      Bool getModel=false;
      IPosition blc(dirty_p->ndim(), 0);
      IPosition trc=dirty_p->shape() -1;
      for (Int k=0; k < nImChan_p ; ++k){
	for (Int j=0; j < npol; ++j){
	  if(npol > 1 ){
	    blc(polAxis_p)=j;
	    trc(polAxis_p)=j;
	  }
	  blc(chanAxis_p)=k;
	  trc(chanAxis_p)=k;
	  Slicer sl(blc, trc, Slicer::endIsLast);
	  Matrix<Float> dirtySub ;
	  Array<Float> buf;
	  dirty_p->getSlice(buf, sl, true);
	  dirtySub.reference(buf);
	  matClean_p.setDirty(dirtySub);
	  Array<Float> bufMod;
	  getModel=modelimage.getSlice(bufMod, sl, true);
	  subModel.reference(bufMod);
	  
	  if((nPsfChan_p>1) && (k<(nPsfChan_p-1))){
	    Matrix<Float> psfSub;
	    Array<Float> buf1;
	    psf_p->getSlice(buf1, sl, true);
	    psfSub.reference(buf1);
	    matClean_p.setPsf(psfSub);
	    matClean_p.makePsfScales();
	  }
	  matClean_p.makeDirtyScales();
	  if(mask_p !=0){
	    if((nMaskChan_p >1) && (k<(nMaskChan_p-1))){
	      Matrix<Float> maskSub;
	      Array<Float> buf2;
	      mask_p->getSlice(buf2, sl, true);
	      maskSub.reference(buf2);
	      matClean_p.setMask(maskSub, maskThresh_p);
	      //Set mask above does the makeScaleMasks as psf-scale-Xfr are valid	    	
	    }
	    //use one plane mask to all planes
	    else if(nMaskChan_p==1){
	      if((nMaskPol_p >1) && (mask_p !=0) ){
		Matrix<Float> maskSub;
		Array<Float> buf2;
		mask_p->getSlice(buf2, sl, true);
		maskSub.reference(buf2);
		matClean_p.setMask(maskSub, maskThresh_p);
	      }
	      matClean_p.makeScaleMasks();
	    }
	    else{
	      matClean_p.unsetMask();
	    }
	  }
	  result=matClean_p.clean(subModel, true);
	  //Update the private flux and residuals here
	  maxResidual_p=max(maxResidual_p, matClean_p.strengthOptimum()); 
	  if(!getModel)
	    modelimage.putSlice(bufMod, sl.start());
	  
	}
      }
    }
    //has no channel but has pol
    else if(polAxis_p > -1 && nImPol_p > 1){
      //The psf should have been set above before the if loop
      //for this case
      Matrix<Float> subModel;
      Bool getModel=false;
      IPosition blc(dirty_p->ndim(), 0);
      IPosition trc=dirty_p->shape() -1;
      for (Int j=0; j < nImPol_p; ++j){
	blc(polAxis_p)=j;
	trc(polAxis_p)=j;
	Slicer sl(blc, trc, Slicer::endIsLast);
	Matrix<Float> dirtySub ;
	Array<Float> buf;
	dirty_p->getSlice(buf, sl, true);
	dirtySub.reference(buf);
	matClean_p.setDirty(dirtySub);
	matClean_p.makeDirtyScales();
	Array<Float> bufMod;
	getModel=modelimage.getSlice(bufMod, sl, true);
	subModel.reference(bufMod);
	if(mask_p !=0){
	  if((nMaskPol_p >1)){
	    Matrix<Float> maskSub;
	    Array<Float> buf2;
	    mask_p->getSlice(buf2, sl, true);
	    maskSub.reference(buf2);
	    matClean_p.setMask(maskSub, maskThresh_p);
	  }
	  matClean_p.makeScaleMasks();
	}
	result=matClean_p.clean(subModel, true);
	//Update the private flux and residuals here
	maxResidual_p=max(maxResidual_p, matClean_p.strengthOptimum()); 
	if(!getModel)
	  modelimage.putSlice(bufMod, sl.start());
      }      
    }
    //1 pol or less
    else{
      //psf and mask should have been set above if 
      Matrix<Float> dirtySub ;
      Array<Float> buf;
      dirty_p->get(buf, true);
      if(buf.shape().nelements() != 2){
	throw(AipsError("Non-expected axes in this image"));
      } 
      dirtySub.reference(buf);
      matClean_p.setDirty(dirtySub);
      matClean_p.makeDirtyScales();
      Matrix<Float> subModel;
      Array<Float> buf0;
      Bool getModel=false;
      getModel=modelimage.get(buf0, true);
      subModel.reference(buf0);
      result=matClean_p.clean(subModel, true);
      //Update the private flux and residuals here
      maxResidual_p=max(maxResidual_p, matClean_p.strengthOptimum()); 
      if(!getModel)
	modelimage.put(subModel);
    }
    return result;
    
  }


} //# NAMESPACE CASA - END
