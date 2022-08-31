//# ImageSkyModel.cc: Implementation of ImageSkyModel classes
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/OS/HostInfo.h>
#include <synthesis/MeasurementComponents/ImageSkyModel.h>
#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/ComponentList.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/casa/OS/File.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/Lattices/TiledLineStepper.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/tables/Tables/TableLock.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

#define MEMFACTOR 8.0
#if !(defined (AIPS_64B))
#  undef  MEMFACTOR
#  define MEMFACTOR 18.0
#endif

ImageSkyModel::ImageSkyModel(const Int maxNumModels) :
  maxnmodels_p(maxNumModels), 
  nmodels_p(0), 
  componentList_p(0), 
  pgplotter_p(0),
  displayProgress_p(false),
  cycleFactor_p(4.0),
  cycleSpeedup_p(-1.0),
  cycleMaxPsfFraction_p(0.8),
  donePSF_p(false),
  modified_p(true),
  dataPolRep_p(StokesImageUtil::CIRCULAR),
  workDirOnNFS_p(false), useMem_p(false), tileVol_p(1000000)
 {
   
 }

void ImageSkyModel::setMaxNumberModels(const Int maxNumModels) {
  maxnmodels_p = maxNumModels;
}

Bool ImageSkyModel::add(ComponentList& compList)
{
  if(componentList_p==0) {
    componentList_p=new ComponentList(compList);
    return true;
  }
  return false;
}

Bool ImageSkyModel::updatemodel(ComponentList& compList)
{
  if(componentList_p!=0) {
    delete componentList_p;
    componentList_p=0;
  }
  
  componentList_p=new ComponentList(compList);
  modified_p=true; 
  return true;
}
  

Bool ImageSkyModel::updatemodel(const Int thismodel, ImageInterface<Float>& image){
  if(nmodels_p < thismodel)
    throw(AipsError("Programming error " + String::toString(thismodel) + " is larger than the number of models"));
  image_p[thismodel]=&image;
  AlwaysAssert(image_p[thismodel], AipsError);
  image_p[thismodel]->setUnits(Unit("Jy/pixel"));
  modified_p=true;
  return true;
}

Int ImageSkyModel::add(ImageInterface<Float>& image, const Int maxNumXfr)
{
  Int thismodel=nmodels_p;
  nmodels_p++;
  /////Avoid using nfs file if memory is available
  try{
    Directory eldir(image.name());
    workDirOnNFS_p=eldir.isNFSMounted();
    //cerr << "Using NFS " << workDirOnNFS_p << endl;
  }
  catch(...){
    workDirOnNFS_p=false;
  }

  if(nmodels_p>maxnmodels_p) maxnmodels_p=nmodels_p;

  maxNumXFR_p=maxNumXfr;

  image_p.resize(nmodels_p); 
  cimage_p.resize(nmodels_p);
  cxfr_p.resize(nmodels_p*maxNumXFR_p);
  residual_p.resize(nmodels_p);
  residualImage_p.resize(nmodels_p);
  gS_p.resize(nmodels_p);
  psf_p.resize(nmodels_p);
  ggS_p.resize(nmodels_p);
  fluxScale_p.resize(nmodels_p);
  work_p.resize(nmodels_p);
  deltaimage_p.resize(nmodels_p);
  solve_p.resize(nmodels_p);
  doFluxScale_p.resize(nmodels_p);
  weight_p.resize(nmodels_p);
  beam_p.resize(nmodels_p);
  
  image_p[thismodel]=0;
  cimage_p[thismodel]=0;
  residual_p[thismodel]=0;

  for (Int numXFR=0;numXFR<maxNumXFR_p;numXFR++) {
    cxfr_p[thismodel*maxNumXFR_p+numXFR]=0;
  }
  residualImage_p[thismodel]=0;
  gS_p[thismodel]=0;
  psf_p[thismodel]=0;
  ggS_p[thismodel]=0;
  fluxScale_p[thismodel]=0;
  work_p[thismodel]=0;
  deltaimage_p[thismodel]=0;
  solve_p[thismodel]=true;
  doFluxScale_p[thismodel]=false;
  weight_p[thismodel]=0;
  beam_p[thismodel]=0;
  
  // Initialize image
  image_p[thismodel]=&image;
  AlwaysAssert(image_p[thismodel], AipsError);
  image_p[thismodel]->setUnits(Unit("Jy/pixel"));
  donePSF_p=false;
  return thismodel;
}

Bool ImageSkyModel::addResidual(Int thismodel, ImageInterface<Float>& residual)
{
  LogIO os(LogOrigin("ImageSkyModel", "addResidual"));
  if(thismodel>=nmodels_p||thismodel<0) {
    os << LogIO::SEVERE << "Illegal model slot" << thismodel << LogIO::POST;
    return false;
  }
  residual_p[thismodel] = &residual;
  AlwaysAssert(residual_p[thismodel], AipsError);
  if(residualImage_p[thismodel]) delete residualImage_p[thismodel];
  residualImage_p[thismodel]=0;
  return true;
}
  
  ImageSkyModel::ImageSkyModel(const ImageSkyModel& other) : SkyModel() {
  operator=(other);
};

ImageSkyModel::~ImageSkyModel() {
  if(componentList_p) delete componentList_p; componentList_p=0;
  for (Int thismodel=0;thismodel<nmodels_p;thismodel++) {
    if(residualImage_p[thismodel]) {    
      //(residualImage_p[thismodel])->clearCache();
      //(residualImage_p[thismodel])->tempClose();
      delete residualImage_p[thismodel];       
    }
    residualImage_p[thismodel]=0;
    if(cimage_p[thismodel]){
      //(cimage_p[thismodel])->clearCache();
      //(cimage_p[thismodel])->tempClose();
      delete cimage_p[thismodel]; 
    }
    cimage_p[thismodel]=0;
    for(Int numXFR=0;numXFR<maxNumXFR_p;numXFR++) {
      if(cxfr_p[thismodel*maxNumXFR_p+numXFR])
        delete cxfr_p[thismodel*maxNumXFR_p+numXFR];
      cxfr_p[thismodel*maxNumXFR_p+numXFR]=0;
    }
    if(gS_p[thismodel]) delete gS_p[thismodel]; gS_p[thismodel]=0;
    if(psf_p[thismodel]) delete psf_p[thismodel]; psf_p[thismodel]=0;
    if(ggS_p[thismodel]) delete ggS_p[thismodel]; ggS_p[thismodel]=0;
    if(fluxScale_p[thismodel]) delete fluxScale_p[thismodel]; fluxScale_p[thismodel]=0;
    if(work_p[thismodel]) delete work_p[thismodel]; work_p[thismodel]=0;
    if(deltaimage_p[thismodel]) delete deltaimage_p[thismodel]; deltaimage_p[thismodel]=0;
    if(weight_p[thismodel]) delete weight_p[thismodel]; weight_p[thismodel]=0;
    if(beam_p[thismodel]) delete beam_p[thismodel]; beam_p[thismodel]=0;
  }
};

ImageSkyModel& ImageSkyModel::operator=(const ImageSkyModel& other) {
  if(this!=&other) {
    componentList_p=other.componentList_p;
    pgplotter_p=other.pgplotter_p;
    donePSF_p=other.donePSF_p;
    nmodels_p=other.nmodels_p;
    for (Int thismodel=0;thismodel<nmodels_p;thismodel++) {
      image_p[thismodel]=other.image_p[thismodel];
      cimage_p[thismodel]=other.cimage_p[thismodel];
      for(Int numXFR=0;numXFR<maxNumXFR_p;numXFR++) {
	cxfr_p[thismodel*maxNumXFR_p+numXFR]=
	  other.cxfr_p[thismodel*maxNumXFR_p+numXFR];
      }
      residual_p[thismodel]=other.residual_p[thismodel];
      residualImage_p[thismodel]=other.residualImage_p[thismodel];
      gS_p[thismodel]=other.gS_p[thismodel];
      ggS_p[thismodel]=other.ggS_p[thismodel];
      fluxScale_p[thismodel]=other.fluxScale_p[thismodel];
      work_p[thismodel]=other.work_p[thismodel];
      deltaimage_p[thismodel]=other.deltaimage_p[thismodel];
      psf_p[thismodel]=other.psf_p[thismodel];
      weight_p[thismodel]=other.weight_p[thismodel];
      beam_p[thismodel]=other.beam_p[thismodel];
    }
    sumwt_p=other.sumwt_p;
    chisq_p=other.chisq_p;
  };
  return *this;
}

// Make the PSF image for each model
void ImageSkyModel::makeApproxPSFs(SkyEquation& se) {
  LogIO os(LogOrigin("ImageSkyModel", "makeApproxPSFs"));

  if(!donePSF_p){
    for (Int thismodel=0;thismodel<nmodels_p;thismodel++) {
      //make sure the psf images are made
      PSF(thismodel);
    }
    se.makeApproxPSF(psf_p);
    for (Int thismodel=0;thismodel<nmodels_p;thismodel++) {
      //beam(thismodel) = 0.0;
      if(!StokesImageUtil::FitGaussianPSF(PSF(thismodel),
					  beam(thismodel))) {
	os << "Beam fit failed: using default" << LogIO::POST;
      }
      if(nmodels_p > 1)
        os  << LogIO::NORMAL << "Model " << thismodel+1 << ": ";  // Loglevel INFO
      os << LogIO::NORMAL                     // Loglevel INFO
	 << "bmaj: " << abs((beam(thismodel)(0,0)).getMajor("arcsec"))
	 << "\", bmin: " << abs((beam(thismodel)(0,0)).getMinor("arcsec"))
	 << "\", bpa: " << ((beam(thismodel)(0,0)).getPA(Unit("deg")))
	 << " deg" << LogIO::POST;
    }
  }
  donePSF_p=true;
}

void ImageSkyModel::initializeGradients() {
  sumwt_p=0.0;
  chisq_p=0.0;
  for (Int thismodel=0;thismodel<nmodels_p;thismodel++) {
    cImage(thismodel).set(Complex(0.0));
    gS(thismodel).set(0.0);
    ggS(thismodel).set(0.0);
  }
}

Bool ImageSkyModel::makeNewtonRaphsonStep(SkyEquation& se, Bool incremental, Bool modelToMS) 
{
  LogIO os(LogOrigin("ImageSkyModel", "makeNewtonRaphsonStep"));
  se.gradientsChiSquared(incremental, modelToMS);
  // os << "Image normalization moved to CSE!!" << LogIO::WARN << LogIO::POST;

  // Now for each model, we find the recommended step
  LatticeExpr<Float> le;
  if(numberOfModels()>0) {
    for(Int thismodel=0;thismodel<nmodels_p;thismodel++) {
      if(isSolveable(thismodel)) {
	// SB
	// Use the following code without the CSE::tmpNormalizeImage()
	if (isImageNormalized()) le = LatticeExpr<Float>(gS(thismodel));
	else                     le = (iif(ggS(thismodel)>(0.0), -gS(thismodel)/ggS(thismodel), 0.0));

	// Use the following code when using CSE::tmpNormalizeImage()
	//	le = LatticeExpr<Float>(gS(thismodel));
	residual(thismodel).copyData(le);
      }
    }
  }
  modified_p=false;
  return true;
}

// Simply finds residual image: i.e. Dirty Image if started with
// zero'ed image. We work from corrected visibilities only!
Bool ImageSkyModel::solve(SkyEquation& se) {
  return solveResiduals(se);
}

// Simply finds residual image: i.e. Dirty Image if started with
// zero'ed image. We work from corrected visibilities only!
Bool ImageSkyModel::solveResiduals(SkyEquation& se, Bool modelToMS) {
  makeNewtonRaphsonStep(se, false, modelToMS);
  return true;
}

// Return residual image: to be used by callers when it is known that the
// current residual image is correct
ImageInterface<Float>& ImageSkyModel::getResidual(Int model) {
  return ImageSkyModel::residual(model);
}

Bool ImageSkyModel::isEmpty(Int model) 
{
  const IPosition tileShape = image(model).niceCursorShape();
  TiledLineStepper ls(image(model).shape(), tileShape, 0);
  RO_LatticeIterator<Float> li(image(model), ls);
  
  for(li.reset();!li.atEnd();li++) {
    if(max(abs(li.cursor()))>0.0) return false;
  }
  return true;
}

ImageInterface<Float>& ImageSkyModel::image(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  AlwaysAssert(image_p[model], AipsError);
  return *image_p[model];
};



ImageInterface<Complex>& ImageSkyModel::cImage(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);

  //if(model>0&&(cimage_p[model-1])) cimage_p[model-1]->tempClose();

  Double memoryMB=HostInfo::memoryFree()/1024/(MEMFACTOR*maxnmodels_p);
  if(cimage_p[model]==0) {
    Vector<Int> whichStokes(0);
    IPosition cimageShape;
    cimageShape=image_p[model]->shape();

    CoordinateSystem cimageCoord =
      StokesImageUtil::CStokesCoord(//cimageShape,
				    image_p[model]->coordinates(),
				    whichStokes,
				    dataPolRep_p);
    //				    StokesImageUtil::CIRCULAR);

    /* STOKESDBG */ //cout << "ImageSkyModel::CImage : Correlation Planes 'whichStokes' : " << whichStokes << endl;
    cimageShape(2)=whichStokes.nelements();
    
    // Now set up the tile size, here we guess only
    //    IPosition tileShape(4, min(32, cimageShape(0)), min(32, cimageShape(1)),
    //			min(4, cimageShape(2)), min(32, cimageShape(3)));

    //If tempImage is going to use disk based image ...try to respect the tile shape of 
    //of original model image

      TempImage<Complex>* cimagePtr = 
	new TempImage<Complex> (TiledShape(cimageShape, tileShape(model)),
			      //IPosition(4, min(image_p[model]->shape()(0), 1000), min(image_p[model]->shape()(1), 1000), 1, 1)), 
			      cimageCoord, (workDirOnNFS_p|| useMem_p) ? memoryMB : 0);
      cimagePtr->set(0.0);
      cimagePtr->setMaximumCacheSize(min(cimagePtr->shape().product()/10, 1000000));
    // cimagePtr->setMaximumCacheSize(cacheSize(model));
    AlwaysAssert(cimagePtr, AipsError);
    cimage_p[model] = cimagePtr;
    cimage_p[model]->setMiscInfo(image_p[model]->miscInfo());
    cimagePtr->clearCache();
  }
  return *cimage_p[model];
};


Bool ImageSkyModel::hasXFR(Int model) 
{
  return (cxfr_p[model]);
}

ImageInterface<Complex>& ImageSkyModel::XFR(Int model, Int numXFR) 
{

  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  if(numXFR==maxNumXFR_p){
    ++maxNumXFR_p;
    cxfr_p.resize(nmodels_p*maxNumXFR_p, true);
    //initialize the extra pointers to 0
    for (Int k = 0; k <nmodels_p; ++k){
      cxfr_p[nmodels_p*(maxNumXFR_p-1)+k]=0;
    }
  }
  AlwaysAssert(numXFR<maxNumXFR_p, AipsError);
  Double memoryMB=HostInfo::memoryTotal(true)/1024/(MEMFACTOR*maxnmodels_p);
  if(cxfr_p[model*maxNumXFR_p+numXFR]==0) {

    TempImage<Complex>* cxfrPtr = 0;
    if (imageRegion_p) {
      // use the imageRegion_p to define the image size;
      // this will usually be related to the primary beam size,
      // but is specified outside of the realm of the ImageSkyModel

      //      cout << "ISM::XFR() Using ImageRegion to make smaller XFR" << endl;

      SubImage<Complex>  sub( cImage(model), *imageRegion_p );
      cxfrPtr = 
	new TempImage<Complex> (IPosition(sub.ndim(),
					  sub.shape()(0),
					  sub.shape()(1),
					  sub.shape()(2),
					  sub.shape()(3)),
				sub.coordinates(),
			          workDirOnNFS_p ? memoryMB : 0);
      //      cout << "ISM::XFR() shape = " << cxfrPtr->shape() << endl;

    } else {
      // use default (ie, full) image size
      cxfrPtr = 
	new TempImage<Complex> (IPosition(cImage(model).ndim(),
					  cImage(model).shape()(0),
					  cImage(model).shape()(1),
					  cImage(model).shape()(2),
					  cImage(model).shape()(3)),
				cImage(model).coordinates(),
				 workDirOnNFS_p ? memoryMB : 0);
    }
    AlwaysAssert(cxfrPtr, AipsError);
    //cxfrPtr->setMaximumCacheSize(cxfrPtr->shape().product()/2);
    cxfrPtr->setMaximumCacheSize(cacheSize(model));
    cxfrPtr->clearCache();
    cxfr_p[model*maxNumXFR_p+numXFR] = cxfrPtr;
  }
  AlwaysAssert(cxfr_p[model*maxNumXFR_p+numXFR], AipsError);
  return *cxfr_p[model*maxNumXFR_p+numXFR];
};


ImageInterface<Float>& ImageSkyModel::PSF(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);

  //if(model>0&&(psf_p[model-1])) psf_p[model-1]->tempClose();

  Double memoryMB=HostInfo::memoryTotal(true)/1024/(MEMFACTOR*maxnmodels_p);
  if(psf_p[model]==0) {
        TempImage<Float>* psfPtr = 
	  new TempImage<Float> (TiledShape(image_p[model]->shape(), tileShape(model)), 
			    image_p[model]->coordinates(),
				(workDirOnNFS_p || useMem_p) ? memoryMB : 0);
    
    AlwaysAssert(psfPtr, AipsError);
    psfPtr->set(0.0);
    //psfPtr->setMaximumCacheSize(psfPtr->shape().product()/10);
    psfPtr->setMaximumCacheSize(cacheSize(model));
    psfPtr->clearCache();
    psf_p[model] = psfPtr;
  }
  return *psf_p[model];
};


ImageInterface<Float>& ImageSkyModel::residual(Int model) {
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  if(residual_p[model]) {
    if(residualImage_p[model]) delete residualImage_p[model];
    residualImage_p[model]=0;
    return *residual_p[model];
  }
  else {
    if(residualImage_p[model]==0) {
      Double memoryMB=HostInfo::memoryFree()/1024.0/(MEMFACTOR*maxnmodels_p);
      
      TempImage<Float>* tempImagePtr =
	new TempImage<Float> (TiledShape(image_p[model]->shape(), 
			      image_p[model]->niceCursorShape()),
			      //IPosition(4, min(image_p[model]->shape()(0), 1000), min(image_p[model]->shape()(1), 1000), 1, 1)), 
			       image_p[model]->coordinates(), memoryMB);
      
      AlwaysAssert(tempImagePtr, AipsError);
      
      //tempImagePtr->setMaximumCacheSize(tempImagePtr->shape().product()/10);
      tempImagePtr->setMaximumCacheSize(cacheSize(model));
      tempImagePtr->set(0.0);
      tempImagePtr->clearCache();
      residualImage_p[model] = tempImagePtr;
    }
    return *residualImage_p[model];
  }
}

ImageInterface<Float>& ImageSkyModel::gS(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);

  //if(model>0&&(gS_p[model-1])) gS_p[model-1]->tempClose();

  if(gS_p[model]==0) {
    Double memoryMB=HostInfo::memoryTotal(true)/1024/(MEMFACTOR*maxnmodels_p);
    
      TempImage<Float>* gSPtr = 
	new TempImage<Float> (TiledShape(image_p[model]->shape(), tileShape(model)), 
			      image_p[model]->coordinates(), (workDirOnNFS_p || useMem_p) ? memoryMB : 0);
    
      //gSPtr->setMaximumCacheSize(gSPtr->shape().product()/10);
      gSPtr->setMaximumCacheSize(cacheSize(model));
    gSPtr->set(0.0);
    gSPtr->clearCache();
    AlwaysAssert(gSPtr, AipsError);
    gS_p[model] = gSPtr;
  }
  return *gS_p[model];
};

ImageInterface<Float>& ImageSkyModel::ggS(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);

  //if(model>0&&(ggS_p[model-1])) ggS_p[model-1]->tempClose();

  if(ggS_p[model]==0) {
    Double memoryMB=HostInfo::memoryTotal(true)/1024/(MEMFACTOR*maxnmodels_p);
    TempImage<Float>* ggSPtr = 
      new TempImage<Float> (TiledShape(image_p[model]->shape(), tileShape(model)),
			    image_p[model]->coordinates(),
			    (workDirOnNFS_p || useMem_p) ? memoryMB : 0);
    
    AlwaysAssert(ggSPtr, AipsError);
    //ggSPtr->setMaximumCacheSize(ggSPtr->shape().product()/10);
    ggSPtr->setMaximumCacheSize(cacheSize(model));
    ggSPtr->set(0.0);
    ggSPtr->clearCache();
    ggS_p[model] = ggSPtr;
  }
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  return *ggS_p[model];
};

ImageInterface<Float>& ImageSkyModel::fluxScale(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);

  //  if(model>0&&(fluxScale_p[model-1])) fluxScale_p[model-1]->tempClose();

  if(fluxScale_p[model]==0) {
    Double memoryMB=HostInfo::memoryTotal(true)/1024/(MEMFACTOR*maxnmodels_p);
    
      TempImage<Float>* fluxScalePtr = 
	new TempImage<Float> (TiledShape(image_p[model]->shape(), tileShape(model)),       
			    image_p[model]->coordinates(),
			      (workDirOnNFS_p || useMem_p) ? memoryMB : 0);
    
    AlwaysAssert(fluxScalePtr, AipsError);
    //fluxScalePtr->setMaximumCacheSize(fluxScalePtr->shape().product()/10);
    fluxScalePtr->setMaximumCacheSize(cacheSize(model));
    fluxScalePtr->set(0.0);
    fluxScalePtr->clearCache();
    fluxScale_p[model] = fluxScalePtr;
    // Set default value to avoid a nasty side effect elsewhere
    fluxScale_p[model]->set(1.0);
  }
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  if( isImageNormalized() == false ) 
    {
      mandateFluxScale(model);
    }
  return *fluxScale_p[model];
};

ImageInterface<Float>& ImageSkyModel::work(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);

  //  if(model>0&&(work_p[model-1])) work_p[model-1]->tempClose();

  if(work_p[model]==0) {
    Double memoryMB=HostInfo::memoryTotal(true)/1024/(MEMFACTOR*maxnmodels_p);
    
    TempImage<Float>* workPtr = 
      new TempImage<Float> (TiledShape(image_p[model]->shape(),tileShape(model)),
			    image_p[model]->coordinates(),
			    (workDirOnNFS_p || useMem_p) ? memoryMB : 0);
    
    AlwaysAssert(workPtr, AipsError);
    //workPtr->setMaximumCacheSize(workPtr->shape().product()/10);
    workPtr->setMaximumCacheSize(cacheSize(model));
    workPtr->set(0.0);
    workPtr->clearCache();
    work_p[model] = workPtr;
  }
  return *work_p[model];
};

ImageInterface<Float>& ImageSkyModel::deltaImage(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);

  // if(model>0&&(deltaimage_p[model-1])) deltaimage_p[model-1]->tempClose();

  if(deltaimage_p[model]==0) {
    Double memoryMB=HostInfo::memoryTotal(true)/1024/(MEMFACTOR*maxnmodels_p);
        TempImage<Float>* deltaimagePtr = 
	  new TempImage<Float> (TiledShape(image_p[model]->shape(),tileShape(model)), 
			    image_p[model]->coordinates(),
				(workDirOnNFS_p || useMem_p) ? memoryMB : 0);
    
    AlwaysAssert(deltaimagePtr, AipsError);
    //deltaimagePtr->setMaximumCacheSize(deltaimagePtr->shape().product()/10);
    deltaimagePtr->setMaximumCacheSize(cacheSize(model));
    deltaimagePtr->set(0.0);
    deltaimagePtr->clearCache();
    deltaimage_p[model] = deltaimagePtr;
  }
  return *deltaimage_p[model];
};

Matrix<Float>& ImageSkyModel::weight(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  if(weight_p[model]==0) {
    weight_p[model] = new Matrix<Float>(1,1);
    AlwaysAssert(weight_p[model], AipsError);
    *weight_p[model]=Float(0.0);
  }
  return *weight_p[model];
};

ImageBeamSet& ImageSkyModel::beam(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  if(beam_p[model]==0) {
    beam_p[model] = new ImageBeamSet();

    AlwaysAssert(beam_p[model], AipsError);
    //*beam_p[model] = Float(-1.0);
  }
  return *beam_p[model];
};


Bool ImageSkyModel::free(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  Bool previous=solve_p[model];
  solve_p[model]=true;
  return previous;
};

Bool ImageSkyModel::fix(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  Bool previous=solve_p[model];
  solve_p[model]=false;
  return previous;
};


Bool ImageSkyModel::isSolveable(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  return solve_p[model];
};


Bool ImageSkyModel::doFluxScale(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  return doFluxScale_p[model];
};

void ImageSkyModel::mandateFluxScale(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  doFluxScale_p[model]=true;
};

Bool ImageSkyModel::hasComponentList()
{
  return (componentList_p);
}

ComponentList& ImageSkyModel::componentList() 
{
  AlwaysAssert(componentList_p, AipsError);
  return *componentList_p;
}

Long ImageSkyModel::cacheSize(Int model){
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  //  Long cachesize=(image_p[model]->shape()[0])*(image_p[model]->shape()[1])/4;
  Long cachesize=(image_p[model]->shape().product());
  // return Long((HostInfo::memoryFree()/(sizeof(Float)*16)*1024));

  return cachesize;
}

IPosition ImageSkyModel::tileShape(Int model){
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  Int x=Int(sqrt(Double(tileVol_p)));
  IPosition shp(4, min(image_p[model]->shape()(0), x), min(image_p[model]->shape()(1), x), 1, 1);
  
  return shp;
}

void ImageSkyModel::setMemoryUse(Bool useMem){
  useMem_p=useMem;
}
void ImageSkyModel::setTileVol(Int  tileVol){
  tileVol_p=tileVol;
}

} //# NAMESPACE CASA - END

