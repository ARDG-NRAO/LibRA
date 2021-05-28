//# Copyright (C) 2015
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU  General Public License as published by
//# the Free Software Foundation; either version 3 of the License, or (at your
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
#include <synthesis/MeasurementEquations/MatrixNACleaner.h>
#include <synthesis/MeasurementEquations/ImageNACleaner.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/CoordinateUtil.h>
using namespace std;
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  ImageNACleaner::ImageNACleaner(): psf_p(nullptr), dirty_p(nullptr), mask_p(nullptr), nPsfChan_p(0), 
				    nImChan_p(0), nPsfPol_p(0), nImPol_p(0), chanAxis_p(-1), 
				    polAxis_p(-1), nMaskChan_p(0), nMaskPol_p(0), maxResidual_p(0.0)
 {


  }
  ImageNACleaner::ImageNACleaner(ImageInterface<Float>& psf, 
				 ImageInterface<Float>& dirty):  mask_p(nullptr), 
								      nMaskChan_p(0), nMaskPol_p(0),maxResidual_p(0.0){
    psf_p=CountedPtr<ImageInterface<Float> >(&psf, false); 
    dirty_p=CountedPtr<ImageInterface<Float> >(&dirty, false); 
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


  ImageNACleaner::ImageNACleaner(const ImageNACleaner& other){
    operator=(other);
  }

  ImageNACleaner::~ImageNACleaner(){

  }

  ImageNACleaner& ImageNACleaner::operator=(const ImageNACleaner& other){
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
      maxResidual_p=other.maxResidual_p;
    }
    return *this;
  }
  void ImageNACleaner::setDirty(ImageInterface<Float>& dirty){
    dirty_p=CountedPtr<ImageInterface<Float> >(&dirty, false);
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

  void ImageNACleaner::setPsf(ImageInterface<Float> & psf){
    psf_p=CountedPtr<ImageInterface<Float> >(&psf,false);
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
  
  
  void ImageNACleaner::setcontrol(const Int niter,
		  const Float gain, const Quantity& aThreshold,
				  const Int supp, const Int memtype, const Float numsigma){
    matClean_p.setcontrol(niter, gain, aThreshold, supp, memtype, numsigma);
  }
  
  Int ImageNACleaner::iteration() const{
    return matClean_p.iteration();
  }
 
  /* void ImageMSCleaner::startingIteration(const Int starting){
    matClean_p.startingIteration(starting);
  }
  */
  void ImageNACleaner::setMask(ImageInterface<Float> & mask){
  
   
    mask_p=CountedPtr<ImageInterface<Float> >(&mask, false);
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

  

  Bool ImageNACleaner::setupMatCleaner(const Int niter,
				       const Float gain, const Quantity& threshold, const Int masksupp, const Int memType, const Float numsigma){

    LogIO os(LogOrigin("ImageNACleaner", "setupMatCleaner()", WHERE));
    
    matClean_p.setcontrol(niter, gain, threshold, masksupp, memType, numsigma);
    
   
    return true;
  }

  Int ImageNACleaner::clean(ImageInterface<Float> & modelimage, 
			    const Int niter,
			    const Float gain, const Quantity& threshold,  const Int masksupp, const Int memType, const Float numSigma){


    
    
    
    Int result=0;
    ///case of single plane mask
    //now may be a time to set stuff  scales will be done later
    if(!setupMatCleaner(niter, gain, threshold, masksupp, memType, numSigma))
      return false;
    //cerr << "nPol " << nMaskPol_p << " " << nPsfPol_p << " " << nImPol_p << endl;
    //cerr << "nChan " << nMaskChan_p << " " << nPsfChan_p << " " << nImChan_p << endl;
    if( (nMaskPol_p >1) && (nMaskPol_p != nImPol_p))
      throw(AipsError("Donot know how to deal with mask that has different pol axis as Image")); 
    CountedPtr<ImageInterface<Float> > somemask;
    if(!mask_p){
      //make one
      somemask=CountedPtr<TempImage<Float> > (new TempImage<Float>((*dirty_p).shape(), (*dirty_p).coordinates())) ;
      setMask(*somemask);
    }
    
    /////TEST
    //PagedImage<Float> someResid((*dirty_p).shape(), (*dirty_p).coordinates(), "decon.resid");
    /////
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
      
    }

    

    //has cube axis
    if(chanAxis_p > -1 && nImChan_p > 0){
      Int npol=1;
      if(polAxis_p > -1 && nImPol_p >0){
	npol=nImPol_p;
      }
      Matrix<Float> subModel;
      Bool getModel=false;
      Bool getMask=false;
      IPosition blc(dirty_p->ndim(), 0);
      IPosition trc=dirty_p->shape() -1;
      for (Int k=0; k < nImChan_p ; ++k){
	for (Int j=0; j < npol; ++j){
	  setupMatCleaner(niter, gain, threshold, masksupp, memType, numSigma);
	  if(npol > 1 ){
	    blc(polAxis_p)=j;
	    trc(polAxis_p)=j;
	  }
	  blc(chanAxis_p)=k;
	  trc(chanAxis_p)=k;
	  //cerr << "blc " << blc << " trc " << trc << endl;
	  Slicer sl(blc, trc, Slicer::endIsLast);
	  Matrix<Float> dirtySub ;
	  Array<Float> buf;
	  dirty_p->getSlice(buf, sl, true);
	  if(dirty_p->isMasked()){
	    cerr << "zeroing masked dirty" << endl;
	    Array<Bool> bitmask;
	    dirty_p->pixelMask().getSlice(bitmask, sl, true);
	    buf(!bitmask)=0.0;
	    matClean_p.setPixFlag(bitmask);
	  }
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
	  }
	  Array<Float> buf2;
	  if(mask_p){
	    // if((nMaskChan_p >1) && (k<(nMaskChan_p))){
	      Matrix<Float> maskSub;
	      getMask=mask_p->getSlice(buf2, sl, true);
	      //cerr << "getmask " << getMask << " buf2 " << buf2.shape() << endl;
	      maskSub.reference(buf2);
	      matClean_p.setMask(maskSub);
	      //Set mask above does the makeScaleMasks as psf-scale-Xfr are valid	    	
	      // }
       
	    
	      //    else{
	      //matClean_p.unsetMask();
	      //}
	  }
	  result=matClean_p.clean(subModel);
	  //Update the private flux and residuals here
	  cerr << "maxResidual " << matClean_p.maxResidual() << " iteration " << matClean_p.iteration() << endl;
	  maxResidual_p=max(maxResidual_p, matClean_p.maxResidual()); 
	  if(!getModel)
	    modelimage.putSlice(bufMod, sl.start());
	  /////TESTING

	  //someResid.putSlice(matClean_p.getResidual().reform(bufMod.shape()), sl.start());

	  ///////
	  //  cerr << "mask " << max(buf2) << " buf2 " << buf2.shape() << " start " << sl.start() << endl;
	  if(!getMask)
	    mask_p->putSlice(buf2, sl.start());
	}
      }
    }
    //has no channel but has pol
    else if(polAxis_p > -1 && nImPol_p > 1){
      //The psf should have been set above before the if loop
      //for this case
      Matrix<Float> subModel;
      Bool getModel=false;
      Bool getMask=false;
      Array<Float> buf2;
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
	Array<Float> bufMod;
	getModel=modelimage.getSlice(bufMod, sl, true);
	subModel.reference(bufMod);
	if(!mask_p){
	  if((nMaskPol_p >1)){
	    Matrix<Float> maskSub;
	   
	    getMask=mask_p->getSlice(buf2, sl, true);
	    maskSub.reference(buf2);
	    matClean_p.setMask(maskSub);
	  }
	
	}
	result=matClean_p.clean(subModel);
	//Update the private flux and residuals here
	maxResidual_p=max(maxResidual_p, matClean_p.maxResidual()); 
	if(!getModel)
	  modelimage.putSlice(bufMod, sl.start());
	if(!getMask)
	  mask_p->putSlice(buf2, sl.start());
      }      
    }
    //1 pol or less
    else{
      ////Will look at this later
      //psf and mask should have been set above if 
      Matrix<Float> dirtySub ;
      Array<Float> buf;
      dirty_p->get(buf, true);
      if(buf.shape().nelements() != 2){
	throw(AipsError("Non-expected axes in this image"));
      } 
      dirtySub.reference(buf);
      matClean_p.setDirty(dirtySub);
      Matrix<Float> subModel;
      Array<Float> buf0;
      Bool getModel=false;
      getModel=modelimage.get(buf0, true);
      subModel.reference(buf0);
      result=matClean_p.clean(subModel);
      //Update the private flux and residuals here
      maxResidual_p=max(maxResidual_p, matClean_p.maxResidual()); 
      if(!getModel)
	modelimage.put(subModel);
    }
    return result;
    
  }


} //# NAMESPACE CASA - END
