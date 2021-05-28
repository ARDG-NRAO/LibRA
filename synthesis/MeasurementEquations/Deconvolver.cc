//# DOdeconvolver.cc: this implements the deconvolver DO 
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
//#
//# $Id: DOdeconvolver.cc,v 19.16 2005/12/06 20:18:50 wyoung Exp $

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>

#include <casa/Logging.h>
#include <casa/Logging/LogIO.h>
#include <casa/OS/File.h>
#include <casa/Containers/Record.h>

#include <tables/TaQL/TableParse.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/TableLock.h>
#include <tables/TaQL/ExprNode.h>

#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/Fallible.h>

#include <casa/BasicSL/Constants.h>

#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

#include <casa/Arrays/ArrayMath.h>

#include <measures/Measures/Stokes.h>
#include <casa/Quanta/UnitMap.h>
#include <casa/Quanta/UnitVal.h>
#include <casa/Quanta/MVAngle.h>
#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/GaussianShape.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MPosition.h>
#include <casa/Quanta/MVEpoch.h>
#include <measures/Measures/MEpoch.h>

#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <lattices/LEL/LatticeExpr.h> 
#include <lattices/LatticeMath/LatticeFFT.h> 
#include <lattices/LatticeMath/LatticeCleaner.h> 
#include <lattices/LatticeMath/LatticeCleanProgress.h> 
#include <lattices/LatticeMath/LatticeConvolver.h> 
#include <lattices/Lattices/TiledLineStepper.h> 
#include <lattices/Lattices/LatticeStepper.h> 
#include <lattices/Lattices/LatticeNavigator.h> 
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/LRegions/LCSlicer.h>

#include <imageanalysis/ImageAnalysis/ComponentImager.h>
#include <images/Images/TempImage.h>
#include <images/Images/PagedImage.h>
#include <images/Images/ImageSummary.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <images/Images/ImageRegrid.h>
#include <images/Images/ImageInfo.h>

#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <coordinates/Coordinates/Projection.h>

#include <synthesis/MeasurementComponents/ClarkCleanImageSkyModel.h>
#include <synthesis/MeasurementEquations/CEMemProgress.h>
#include <synthesis/MeasurementEquations/CEMemModel.h>
#include <synthesis/MeasurementEquations/ClarkCleanLatModel.h>
#include <synthesis/MeasurementEquations/ClarkCleanProgress.h>
#include <synthesis/DataSampling/ImageDataSampling.h>
#include <synthesis/MeasurementEquations/LatConvEquation.h>
#include <synthesis/MeasurementEquations/IPLatConvEquation.h>
#include <synthesis/MeasurementEquations/Deconvolver.h>
#include <synthesis/MeasurementEquations/Imager.h>
#include <synthesis/MeasurementEquations/ImageMSCleaner.h>
#include <synthesis/MeasurementEquations/ImageNACleaner.h>


#include <casa/sstream.h>

#include <casa/namespace.h>
// Implementation comment:
// There are two different philosophies active here:
// ClarkCleanLatModel and CEMemModel are evolutionarily related and
// "solve" a LinearEquation (in this case, a LatticeConvolutionEqaution).
// The LatticeCleaner, which performs Hogbom and MultiScale Cleans,
// has no knowledge of the LatticeConvolutionEqaution (LatConvEquation), 
// but carries the PSF and DIRTY around inside itself.
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

Deconvolver::Deconvolver() 
  : dirty_p(0),
    psf_p(0), 
    convolver_p(0), 
    cleaner_p( ), 
    mt_nterms_p(-1), 
    mt_cleaner_p(), 
    mt_valid_p(false)
{

  defaults();
};

void Deconvolver::defaults() 
{
  mode_p="none";
  beamValid_p=false;
  scalesValid_p=false;
  beam_p = GaussianBeam();
  residEqn_p = 0;
  latConvEqn_p = 0;
  cleaner_p = 0;
  dirtyName_p = "";
  psfName_p = "";
  nx_p=0; ny_p=0; npol_p=0; nchan_p=0;
  fullPlane_p=false;
}

Deconvolver::Deconvolver(const String& dirty, const String& psf)
  : dirty_p(0), psf_p(0), convolver_p(0), cleaner_p( ), naCleaner_p(nullptr),
     mt_nterms_p(-1), mt_cleaner_p(), mt_valid_p(false)
{
  LogIO os(LogOrigin("Deconvolver", "Deconvolver(String& dirty, Strong& psf)", WHERE));
  defaults();
  open(dirty, psf);
}

Deconvolver::Deconvolver(const Deconvolver &other)
  : dirty_p(0), psf_p(0), convolver_p(0), cleaner_p( ), naCleaner_p(nullptr),
    mt_nterms_p(-1), mt_cleaner_p(), mt_valid_p(false)
{
  defaults();
  open(other.dirty_p->table().tableName(), other.psf_p->table().tableName());
}

Deconvolver &Deconvolver::operator=(const Deconvolver &other)
{
  if(this != &other){
    if (dirty_p ) {
      *dirty_p = *(other.dirty_p);
    }
    if (psf_p ) {
      *psf_p = *(other.psf_p);
    }
    if (convolver_p && this != &other) {
      *convolver_p = *(other.convolver_p);
    }
    if ((!cleaner_p.null()) ) {
      *cleaner_p = *(other.cleaner_p);
    }
    if(naCleaner_p)
      *naCleaner_p=*(other.naCleaner_p);
  }
  return *this;
}

Deconvolver::~Deconvolver()
{
  if (psf_p) {
    delete psf_p;
  }
  psf_p = 0;
  if (convolver_p) {
    delete convolver_p;
  }
  convolver_p = 0;
  /*if (cleaner_p) {
    delete cleaner_p;
  }
  cleaner_p = 0;
  */
  if (dirty_p) {
    delete dirty_p;
  }
  dirty_p = 0;
}

Bool Deconvolver::open(const String& dirty, const String& psf, Bool warn)
{
  LogIO os(LogOrigin("Deconvolver", "open()", WHERE));
  

  try {
    if (dirty_p) delete dirty_p;  dirty_p = 0;
    dirty_p = new PagedImage<Float>(dirty);
    AlwaysAssert(dirty_p, AipsError);
    nx_p=dirty_p->shape()(0);
    ny_p=dirty_p->shape()(1);
    dirty_p->setMaximumCacheSize(2*nx_p*ny_p);
    dirty_p->setCacheSizeInTiles(10000);

    if(dirty_p->shape().nelements()==3){
      findAxes();
      if (chanAxis_p > 0)
	nchan_p=dirty_p->shape()(chanAxis_p);
      else
	nchan_p=0;
    }
    if(dirty_p->shape().nelements()==4){
      findAxes();
      npol_p=dirty_p->shape()(polAxis_p);
      nchan_p=dirty_p->shape()(chanAxis_p);
    }
    dirtyName_p =  dirty_p->table().tableName();
    
    if (psf_p) delete psf_p;  psf_p = 0;
    if (psf == ""){
    	if(warn) {
    		os << LogIO::WARN
    			<< "No psf given; please define one before deconvolving" << LogIO::POST;
    		os << LogIO::WARN
    			<< "Use the function open with the psf" << LogIO::POST;
    	}
    	return true;
    }
    else{
      psf_p = new PagedImage<Float>(psf);
      AlwaysAssert(psf_p, AipsError);
      psfName_p   =  psf_p->table().tableName();    
      psf_p->setMaximumCacheSize(2*nx_p*ny_p);
      psf_p->setCacheSizeInTiles(10000);
      
      try{
	os << "Fitting PSF" << LogIO::POST;
	fitpsf(psf, beam_p);
	if(! beam_p.isNull()) {
	  os << "  Fitted beam is valid"<< LogIO::POST;
	}
	else {
	  os << LogIO::WARN << "Fitted beam is invalid: please set using setbeam"
	     << LogIO::POST;
	}
	beamValid_p=true;
	
      } catch (AipsError x) {
	os << LogIO::WARN << "Fitted beam is invalid: please set using setbeam"
	   << LogIO::POST;
      } 
      
      if((psf_p->shape()(0) != nx_p) || psf_p->shape()(1) != ny_p){
	
	os << LogIO::SEVERE 
	   << "PSF and Image does not have the same XY shape" << LogIO::POST;
	os << LogIO::SEVERE
	   << "You may wish to regrid the PSF to the same shape as the dirty image" 
	   << LogIO::POST;
      return false;

      } 
      
      try {
	os << "Making Lattice convolver" << LogIO::POST;
	if (convolver_p) {
	  delete convolver_p;
	}
	
	//      convolver_p = new LatticeConvolver<Float>(*psf_p);
	//      AlwaysAssert(convolver_p, AipsError);
	
	if (residEqn_p) {
	  delete residEqn_p;
	}
	residEqn_p = 0;
	
	if (latConvEqn_p) {
	  delete latConvEqn_p;
	}
	latConvEqn_p = 0;
	
	os << "Making Image cleaner" << LogIO::POST;
	//if (cleaner_p) delete cleaner_p;
	cleaner_p= new ImageMSCleaner(*psf_p, *dirty_p);
	naCleaner_p=make_shared<ImageNACleaner>(*psf_p, *dirty_p);
	if(nchan_p<=1){
	  convolver_p = new LatticeConvolver<Float>(*psf_p);
	}
	else{
	  if(npol_p > 0 ){
	    IPosition blc(4, 0, 0, 0, 0);
	    IPosition trc(4, nx_p-1, ny_p-1, 0, 0);
	    trc(polAxis_p)=npol_p-1;
	    Slicer sl(blc, trc, Slicer::endIsLast);
	    SubImage<Float> psfSub(*psf_p, sl, true);  
	    convolver_p = new LatticeConvolver<Float>(psfSub);
	    AlwaysAssert(convolver_p, AipsError);
	  }
	  else{
	    IPosition blc(3, 0, 0, 0);
	    IPosition trc(3, nx_p-1, ny_p-1, 0);
	    Slicer sl(blc, trc, Slicer::endIsLast);
	    //SubImage<Float> dirtySub(*dirty_p, sl, true);
	    SubImage<Float> psfSub(*psf_p, sl, true);
	    convolver_p = new LatticeConvolver<Float>(psfSub);
	    AlwaysAssert(convolver_p, AipsError);
	  }
	  
	}
	AlwaysAssert(!cleaner_p.null(), AipsError);
	
	return true;

      } catch (AipsError x) {
	os << LogIO::SEVERE << "Caught Exception: "<< x.getMesg() << LogIO::POST;
	return false;
      } 
    }
  }
  catch (AipsError y){
    throw(y);
  }
}

Bool Deconvolver::reopen()
{
  LogIO os(LogOrigin("Deconvolver", "reopen()", WHERE));
  try {
    if (dirtyName_p != "" && psfName_p != "") {
      return (open(dirtyName_p, psfName_p));
    } else {
      return false;
    }
  }  catch (AipsError x) {
    dirty_p->table().unlock();
    psf_p->table().unlock();
    os << LogIO::SEVERE << "Caught Exception: "<< x.getMesg() << LogIO::POST;
    return false;
  } 
  return false;
}

// Fit the psf. If psf is blank then make the psf first.
Bool Deconvolver::fitpsf(const String& psf, GaussianBeam& beam)
{
  if(!valid()) return false;
  
  LogIO os(LogOrigin("Deconvolver", "fitpsf()", WHERE));
  
  try {
    
    os << "Fitting to psf" << LogIO::POST;

    if(psf=="") {
      os << LogIO::SEVERE << "Need a psf name" << LogIO::POST;
      return false;
    }
    
    PagedImage<Float> psfImage(psf);
    StokesImageUtil::FitGaussianPSF(psfImage, beam);
    beam_p = beam;
    beamValid_p=true;
    
    os << "  Beam fit: " << beam_p.getMajor("arcsec") << " by "
       << beam_p.getMinor("arcsec") << " (arcsec) at pa "
       << beam_p.getPA(Unit("deg")) << " (deg) " << endl;
    return true;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  return true;
}

Bool Deconvolver::close()
{
  if(!valid()) return false;
  if (detached()) return true;
  LogIO os(LogOrigin("Deconvolver", "close()", WHERE));
  
  os << "Closing images and detaching from Deconvolver" << LogIO::POST;
  if(psf_p) delete psf_p; psf_p = 0;
  if(dirty_p) delete dirty_p; dirty_p = 0;
  if (convolver_p) delete convolver_p; convolver_p = 0;
  if (residEqn_p) delete  residEqn_p;  residEqn_p = 0;
  if (latConvEqn_p) delete latConvEqn_p; latConvEqn_p = 0;

  return true;
}

String Deconvolver::dirtyname() const
{
  if (detached()) {
    return "none";
  }
  return dirty_p->table().tableName();
}

String Deconvolver::psfname() const
{
  if (detached()) {
    return "none";
  }
  return psf_p->table().tableName();
}

Bool Deconvolver::summary() const
{
  if(!valid()) return false;
  LogOrigin OR("Deconvolver", "Deconvolver::summary()",  WHERE);
  
  LogIO los(OR);
  
  try {
    
    los << "Summary of dirty image" << LogIO::POST;
    dirty_p->table().lock();
    {
       ImageSummary<Float> ims(*dirty_p);
       ims.list(los);
    }
    
    los << endl << state() << LogIO::POST;
    dirty_p->table().unlock();

    los << "Summary of PSF" << LogIO::POST;
    psf_p->table().lock();
    {
       ImageSummary<Float> psfs(*psf_p);
       psfs.list(los);
    }
    
    los << "Summary of scales" << LogIO::POST;
    if(scalesValid_p) {
      los << "Scales set" << LogIO::POST;
    }
    else {
      los << "Scales not set" << LogIO::POST;
    }

    los << endl << state() << LogIO::POST;
    psf_p->table().unlock();
    return true;
  } catch (AipsError x) {
    los << LogIO::SEVERE << "Caught Exception: " << x.getMesg()
	<< LogIO::POST;
    dirty_p->table().unlock();
    psf_p->table().unlock();
    return false;
  } 
  
  return true;
}

String Deconvolver::state() const
{
  ostringstream os;
  
  try {
    os << "General: " << endl;
    if(dirty_p != 0){
      os << "  Dirty image is " << dirty_p->table().tableName() << endl; 
      dirty_p->table().unlock();
    }
    if(psf_p !=0){
      os << "  PSF         is " << psf_p->table().tableName() << endl;
      psf_p->table().unlock();
    }
    if(beamValid_p) {
      os << "  Beam fit: " << beam_p.getMajor("arcsec") << " by "
	 << beam_p.getMinor("arcsec") << " (arcsec) at pa "
	 << beam_p.getPA(Unit("deg")) << " (deg) " << endl;
    }
    else {
      os << "  Beam fit is not valid" << endl;
    }
    
  } catch (AipsError x) {
    LogOrigin OR("Deconvolver", "Deconvolver::state()", WHERE); 
    LogIO los(OR);
    los << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::POST;
    dirty_p->table().unlock();
    psf_p->table().unlock();
  } 
  return String(os);
}

// Restore: at least one model must be supplied
Bool Deconvolver::restore(const String& model, const String& image,
			  GaussianBeam& mbeam)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "restore()", WHERE));
  
  dirty_p->table().lock();
  psf_p->table().lock();
  try {
    
    // Validate the names
    if(model=="") {
      os << LogIO::SEVERE << "Need a model"
	 << LogIO::POST;
      return false;
    }
    
    String imagename(image);
    if(imagename=="") imagename=model+".restored";
    removeTable(imagename);
    if(!clone(model, imagename)) return false;
    
    // Smooth all the images and add residuals
    PagedImage<Float> modelImage0(model);

//
    TiledShape tShape(dirty_p->shape());
    ImageInterface<Float>* modelImage_p = new TempImage<Float>(tShape, dirty_p->coordinates());
//
    ImageRegrid<Float> regridder;
    Vector<Double> locate;
    Bool missedIt = regridder.insert(*modelImage_p, locate, modelImage0);
    //cerr << "missedIt " << missedIt << endl;
    if (!missedIt) {
      os << LogIO::SEVERE << "Problem in getting model Image on correct grid " << LogIO::POST;
    }

    PagedImage<Float> imageImage(modelImage_p->shape(),
				 modelImage_p->coordinates(),
				 image);

    TempImage<Float> dirtyModelImage(modelImage_p->shape(),modelImage_p->coordinates());
    //imageImage.copyData(*modelImage_p);
    if(! mbeam.isNull()) {
      os << "  Using specified beam: " << mbeam.getMajor("arcsec") << " by "
	 << mbeam.getMinor("arcsec") << " (arcsec) at pa "
	 << mbeam.getPA(Unit("deg")) << " (deg) " << LogIO::POST;
      //StokesImageUtil::Convolve(imageImage, mbeam, false);
    }
    else {
      if(! beam_p.isNull()) {
	os << "  Using fitted beam: " << beam_p.getMajor("arcsec") << " by "
	   << beam_p.getMinor("arcsec") << " (arcsec) at pa "
	   << beam_p.getPA(Unit("deg")) << " (deg) " << LogIO::POST;
	//StokesImageUtil::Convolve(imageImage, beam_p, false);
	mbeam = beam_p;
      }
      else {
	os << LogIO::SEVERE << "Restoring beam not specified" << LogIO::POST;
	return false;
      }
    }
    //Model * restoring beam 
    {
      IPosition convshp=modelImage_p->shape();
      convshp[0]=nx_p; convshp[1]=ny_p;
      for (uInt k=2; k< convshp.nelements(); ++k) convshp[k]=1;
      TempImage<Float> gaussim(convshp, modelImage_p->coordinates());
      gaussim.set(0.0);
      ImageInfo ii = gaussim.imageInfo();
      ii.setRestoringBeam(mbeam);
      gaussim.setImageInfo(ii);
      gaussim.setUnits(Unit("Jy/beam"));    
      putGaussian(gaussim, mbeam);
      //////////////////////
      //PagedImage<Float>xx(gaussim.shape(), gaussim.coordinates(), "tempGauss");
      //xx.copyData(gaussim);
      //////////////////
      LatticeConvolver<Float> lc(gaussim);
      lc.linear(imageImage, *modelImage_p);
    }
    // PSF * Model    
    convolver_p->circular(dirtyModelImage, *modelImage_p);

    // Smoothed + Dirty - PSF * Model
    imageImage.copyData(LatticeExpr<Float>(imageImage+*dirty_p - dirtyModelImage));
    {
      ImageInfo ii = imageImage.imageInfo();
      ii.setRestoringBeam(mbeam);
      imageImage.setImageInfo(ii);
      imageImage.setUnits(Unit("Jy/beam"));
    }

    dirty_p->table().unlock();
    psf_p->table().unlock();
    if (modelImage_p != & modelImage0) {
      delete modelImage_p;
    }
    return true;
  } catch (AipsError x) {
    dirty_p->table().unlock();
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::POST;
  } 
  dirty_p->table().unlock();
  return true;
}

// Residual
Bool Deconvolver::residual(const String& model, const String& image)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "residual()", WHERE));
  
  dirty_p->table().lock();
  psf_p->table().lock();
  try {
    
    // Validate the names
    if(model=="") {
      os << LogIO::SEVERE << "Need a model"
	 << LogIO::POST;
    }
    
    String imagename(image);
    if(imagename=="") imagename=model+".residual";
    removeTable(imagename);
    if(!clone(dirty_p->table().tableName(), imagename)) return false;
    
    // Smooth all the images and add residuals
    
    // modelImage_p is a pointer to an image with the model data in it, but the
    // shape of the dirty image
    PagedImage<Float> modelImage0(model);

    TiledShape tShape(dirty_p->shape());
    ImageInterface<Float>* modelImage_p = new TempImage<Float>(tShape, dirty_p->coordinates());
//
    ImageRegrid<Float> regridder;
    Vector<Double> locate;
    Bool missedIt = regridder.insert(*modelImage_p, locate, modelImage0);
    if (!missedIt) {
      os << LogIO::SEVERE << "Problem in getting model Image on correct grid " << LogIO::POST;
    }

    PagedImage<Float> imageImage(modelImage_p->shape(),
				 modelImage_p->coordinates(),
				 image);
    // PSF * Model    
    convolver_p->circular(imageImage, *modelImage_p);

    // Dirty - PSF * Model
    imageImage.copyData(LatticeExpr<Float>(*dirty_p-imageImage));
    imageImage.setUnits(Unit("Jy/beam"));
    
    dirty_p->table().unlock();
    psf_p->table().unlock();
    if (modelImage_p != & modelImage0) {
      delete modelImage_p;
    }
    return true;
  } catch (AipsError x) {
    dirty_p->table().unlock();
    psf_p->table().unlock();
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg()
       << LogIO::POST;
  } 
  dirty_p->table().unlock();
  psf_p->table().unlock();
  return true;
}

// Make an empty image
Bool Deconvolver::make(const String& model)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "make()", WHERE));
  
  dirty_p->table().lock();
  try {
    
    // Make an image with the required shape and coordinates
    String modelName(model);
    if(modelName=="") modelName=dirty_p->table().tableName()+".model";
    os << "Making empty image: " << model << LogIO::POST;
    
    removeTable(modelName);
    PagedImage<Float> modelImage(dirty_p->shape(),
				 dirty_p->coordinates(), model);
    modelImage.set(0.0);
    
    modelImage.table().tableInfo().setSubType("GENERIC");
    modelImage.setUnits(Unit("Jy/pixel"));
    dirty_p->table().unlock();
    return true;
  } catch (AipsError x) {
    dirty_p->table().unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  dirty_p->table().unlock();
  return true;
};


// Make an empty image, but with only ONE STOKES pixel
Bool Deconvolver::make1(const String& model)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "make1()", WHERE));
  
  dirty_p->table().lock();
  try {
    
    // Make an image with the required shape and coordinates
    String modelName(model);
    if(modelName=="") modelName=dirty_p->table().tableName()+".model";
    os << "Making empty image: " << model << LogIO::POST;
    
    removeTable(modelName);
    IPosition newshape = dirty_p->shape();
    newshape(2) = 1;
    PagedImage<Float> modelImage(newshape,
				 dirty_p->coordinates(), model);
    modelImage.set(0.0);
    
    modelImage.table().tableInfo().setSubType("GENERIC");
    modelImage.setUnits(Unit("Jy/pixel"));
    dirty_p->table().unlock();
    return true;
  } catch (AipsError x) {
    dirty_p->table().unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  dirty_p->table().unlock();
  return true;
};


// Make an empty image, modeled in templateImage
Bool Deconvolver::make(const String& model, ImageInterface<Float>& templateImage)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "make()", WHERE));
  
  try {
    
    // Make an image with the required shape and coordinates
    String modelName(model);

    os << "Making empty image: " << model << LogIO::POST;
        removeTable(modelName);
    PagedImage<Float> modelImage(templateImage.shape(),
				 templateImage.coordinates(), model);
    modelImage.set(0.0);
    
    modelImage.table().tableInfo().setSubType("GENERIC");
    modelImage.setUnits(Unit("Jy/pixel"));
    return true;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  return true;
};


SubImage<Float>* 
Deconvolver::innerQuarter(PagedImage<Float>& in)
{

  IPosition blc(in.shape().nelements(), 0);
  IPosition trc(in.shape()-1);
  for (Int i=0;i<Int(in.shape().nelements());i++) {
    blc(i)=in.shape()(i)/4;
    trc(i)=blc(i)+in.shape()(i)/2-1;
    if(trc(i)<0) trc(i)=1;
  }
  LCSlicer quarter(blc, trc);
  SubImage<Float>* si = new SubImage<Float>(in, quarter, true);
  return si;
};


SubImage<Float>* 
Deconvolver::allQuarters(PagedImage<Float>& in)
{
  SubImage<Float>* si = new SubImage<Float>(in, true);
  return si;
};

Bool Deconvolver::smooth(const String& model, 
			 const String& image,
			 GaussianBeam& mbeam,
			 Bool normalizeVolume)
{
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "smooth()", WHERE));
  
  dirty_p->table().lock();
  try {
    
    os << "Smoothing image" << LogIO::POST;
    
    if(model=="") {
      os << LogIO::SEVERE << "Need a name for model " << LogIO::POST;
      return false;
    }
    
    if(mbeam.getMajor().getValue()==0.0) {
      if(beamValid_p) {
	os << "Using previous beam fit" << LogIO::POST;
	mbeam = beam_p;

      }
      else {
	os << LogIO::SEVERE << "Specified beam is invalid" << LogIO::POST;
      }
    }
    
    // Smooth all the images
    PagedImage<Float> modelImage(model);
    PagedImage<Float> imageImage(modelImage.shape(),
				 modelImage.coordinates(),
				 image);
//
    imageImage.copyData(modelImage);
    StokesImageUtil::Convolve(imageImage, mbeam, normalizeVolume);
    {
      ImageInfo ii = imageImage.imageInfo();
      ii.setRestoringBeam(mbeam);
      imageImage.setImageInfo(ii);
      imageImage.setUnits(Unit("Jy/beam"));
    }

    dirty_p->table().unlock();
    psf_p->table().unlock();
    return true;
  } catch (AipsError x) {
    dirty_p->table().unlock();
    psf_p->table().unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  dirty_p->table().unlock();
  psf_p->table().unlock();
  return true;
}
//will clean casa axes order images 
Bool Deconvolver::clarkclean(const Int niter, 
			     const Float gain, const Quantity& threshold, 
			     const String& model, const String& maskName,
			     Float& maxresidual, Int& iterused,
			     Float cycleFactor){

  Bool retval=false;
  Double thresh=threshold.get("Jy").getValue();
  String imagename(model);
  // Make first image with the required shape and coordinates only if
  // it doesn't exist yet. Otherwise we'll throw an exception later
  if(imagename=="") imagename=dirty_p->table().tableName()+".clarkclean";
  if(!Table::isWritable(imagename)) {
    make(imagename);
  }
  PagedImage<Float> modelImage(imagename);
  Bool hasMask=false;
  if(maskName !="")
    hasMask=Table::isReadable(maskName);
  if(hasMask){
    PagedImage<Float> mask(maskName);
    retval=ClarkCleanImageSkyModel::clean(modelImage, *dirty_p, *psf_p, mask, maxresidual, iterused, gain, niter, thresh, cycleFactor, true, true); 
  }
  else{
    ImageInterface<Float> *tmpMask=0;
    retval=ClarkCleanImageSkyModel::clean(modelImage, *dirty_p, *psf_p, *tmpMask, maxresidual, iterused, gain, niter, thresh, cycleFactor, false, true); 
  }
    
  return retval;
}
// Clean algorithm
Bool Deconvolver::clarkclean(const Int niter, 
			     const Float gain, const Quantity& threshold, 
			     const Bool displayProgress, 
			     const String& model, const String& maskName,
			     const Int histBins, 
			     const Vector<Int>& vi_psfPatchSize, const Float maxExtPsf,
			     const Float speedUp, Int maxNumPix,
			     const Int maxNumMajorCycles,
			     const Int maxNumMinorIterations)
{
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "clarkclean()", WHERE));
  
  IPosition psfPatchSize(vi_psfPatchSize);


  dirty_p->table().lock();
  psf_p->table().lock();

  String imagename(model);
  // Make first image with the required shape and coordinates only if
  // it doesn't exist yet. Otherwise we'll throw an exception later
  if(imagename=="") imagename=dirty_p->table().tableName()+".clarkclean";
  if(!Table::isWritable(imagename)) {
    make(imagename);
  }
  PagedImage<Float> modelImage(imagename);
  ClarkCleanProgress *ccpp = 0;

  {
    ostringstream oos;
    oos << "Clean gain = " <<gain<<", Niter = "<<niter<<", Threshold = "
	<<threshold;                              ;
    os << String(oos) << LogIO::POST;
  }
  {
    ostringstream oos;
    oos << "nHhistBins = "
	<<histBins << ", maxExtPsf = "<<maxExtPsf<<", psfPatchSize = "
	<<psfPatchSize<<", maxNumPix = "<<maxNumPix;
    os << String(oos) << LogIO::POST;
  }
  {
    ostringstream oos;
    oos << "Speedup Factor = "<<speedUp<<", maxMajorCycles = "
	<< maxNumMajorCycles<<", maxMinorIterations = "<<maxNumMinorIterations;
    os << String(oos) << LogIO::POST;
  }
  
  os << "Cleaning image using Clark Clean algorithm" << LogIO::POST;
  

  

  
  try {    
    if(model=="") {
      os << LogIO::SEVERE << "Need a name for model " << LogIO::POST;
      return false;
    }
    PagedImage<Float> *mask = 0;
    Bool isCubeMask=false;
 
    Int xbeg, xend, ybeg, yend;
    //default clean box
    xbeg=nx_p/4; 
    xend=3*nx_p/4-1;
    ybeg=ny_p/4; 
    yend=3*ny_p/4-1;

   // Deal with mask
    if (maskName != "") {
      if( Table::isReadable(maskName)) {
	mask= new PagedImage<Float>(maskName);
	if (chanAxis_p < Int(mask->shape().nelements())){
	  if (mask->shape()(chanAxis_p) > 1) 
	    isCubeMask=true;
	}
	checkMask(*mask, xbeg, xend, ybeg, yend);
	AlwaysAssert(mask, AipsError);

      } else {
	os << LogIO::SEVERE << "Mask "<< mask<<" is not readable" << LogIO::POST;
      }
    }
    SubImage<Float> *maskSub=0;
    IPosition blc(2,xbeg,ybeg);
    IPosition trc(2,xend,yend);
    Bool result=false;
    if(nchan_p >=1){
      for (Int k=0; k<nchan_p; ++k){
	os<< "Cleaning channel " << k+1 << LogIO::POST;
	if(npol_p > 0 ){
	  blc.resize(4);
	  blc(chanAxis_p)=k;
	  blc(polAxis_p)=0;
	  trc.resize(4);
	  trc(polAxis_p)=npol_p-1;
	  trc(chanAxis_p)=k;
	  
	}
	else{
	  blc.resize(3);
	  trc.resize(3);  
	  blc(chanAxis_p)=k;
	  trc(chanAxis_p)=k;
	  
	}
      
	
	
	Slicer sl(blc, trc, Slicer::endIsLast);
	SubImage<Float> psfSub;

	if(mask != 0){
	  if( (isCubeMask) || (!isCubeMask && maskSub == 0 )){
	    if(maskSub !=0 ) delete maskSub;
	    blc(0)=0; blc(1)=0;
	    trc(0)=nx_p-1; trc(1)=ny_p-1;
	    sl=Slicer(blc, trc, Slicer::endIsLast);
	    maskSub=new SubImage<Float> (*mask,sl,false);
	    checkMask(*maskSub, xbeg, xend, ybeg, yend);
	    blc(0)=xbeg; blc(1)=ybeg;
	    trc(0)=xend; trc(1)=yend;
	    sl =Slicer(blc, trc, Slicer::endIsLast);
	    delete maskSub;
	    maskSub=new SubImage<Float> (*mask,sl,false);
	  }
	}

   
	SubImage<Float> dirtySub(*dirty_p, sl, true);
	SubImage<Float> modelSub(modelImage,sl,true);
	IPosition blc_psf=blc; IPosition trc_psf=trc;
	if(psf_p->shape().nelements() != dirty_p->shape().nelements()){
	  blc_psf.resize(psf_p->shape().nelements());
	  trc_psf.resize(psf_p->shape().nelements());
	}
	blc_psf(0)=0; blc_psf(1)=0;
	trc_psf(0)=nx_p-1; trc_psf(1)=ny_p-1;
	sl=Slicer(blc_psf, trc_psf, Slicer::endIsLast);
	psfSub=SubImage<Float>(*psf_p, sl, true);
	
	ClarkCleanLatModel myClarkCleaner(modelSub);
	if(mask !=0 )
	  myClarkCleaner.setMask(*maskSub);
	
	myClarkCleaner.setNumberIterations(niter);
	if (maxNumMajorCycles > 0 ) 
	  myClarkCleaner.setMaxNumberMajorCycles((uInt)maxNumMajorCycles);
	if (maxNumMinorIterations > 0 ) 
	  myClarkCleaner.setMaxNumberMinorIterations((uInt)maxNumMinorIterations); 
	
	myClarkCleaner.setGain(gain);
	Double d_thresh = threshold.getValue("Jy");
	myClarkCleaner.setThreshold((Float)d_thresh);
	
	myClarkCleaner.setPsfPatchSize(psfPatchSize);
	myClarkCleaner.setHistLength((uInt)histBins); 
	myClarkCleaner.setMaxExtPsf(maxExtPsf); 
	myClarkCleaner.setSpeedup(speedUp); 
	
	if (maxNumPix == 0) 
	  maxNumPix = (Int)(modelImage.shape().product()*0.04);
	myClarkCleaner.setMaxNumPix((uInt)maxNumPix);
	
	//Now actually do the clean
	if (displayProgress) {
	  ccpp = new ClarkCleanProgress ();
	  myClarkCleaner.setProgress(*ccpp);
	}
	if(latConvEqn_p !=0) delete latConvEqn_p;
	latConvEqn_p=0;
	latConvEqn_p = new LatConvEquation (psfSub, dirtySub);
	result=myClarkCleaner.solve(*latConvEqn_p);
      }
    }
    else{
      IPosition blc(modelImage.shape().nelements(),0);
      Int elem= npol_p >0 ? npol_p:0;
      IPosition trc(modelImage.shape().nelements(),elem);
      blc(0)=xbeg; blc(1)=ybeg;
      trc(0)=xend; trc(1)=yend;
      Slicer sl(blc, trc, Slicer::endIsLast);
      SubImage<Float> maskSub;
      SubImage<Float> dirtySub(*dirty_p, sl, true);
      SubImage<Float> modelSub(modelImage,sl,true);
      if(psf_p->shape().nelements() != dirty_p->shape().nelements()){
	blc.resize(psf_p->shape().nelements());
	trc.resize(psf_p->shape().nelements());
      }
      blc(0)=0; blc(1)=0;
      trc(0)=nx_p-1; trc(1)=ny_p-1;
      sl=Slicer(blc, trc, Slicer::endIsLast);
      SubImage<Float> psfSub(*psf_p, sl, true);
      
      ClarkCleanLatModel myClarkCleaner(modelSub);
      if(mask !=0 ){
	maskSub= SubImage<Float>(*mask, sl, false);
	myClarkCleaner.setMask(maskSub);
      }

      myClarkCleaner.setNumberIterations(niter);
      if (maxNumMajorCycles > 0 ) 
	myClarkCleaner.setMaxNumberMajorCycles((uInt)maxNumMajorCycles);
      if (maxNumMinorIterations > 0 ) 
	myClarkCleaner.setMaxNumberMinorIterations((uInt)maxNumMinorIterations); 
    
      myClarkCleaner.setGain(gain);
      Double d_thresh = threshold.getValue("Jy");
      myClarkCleaner.setThreshold((Float)d_thresh);
  
      myClarkCleaner.setPsfPatchSize(psfPatchSize);
      myClarkCleaner.setHistLength((uInt)histBins); 
      myClarkCleaner.setMaxExtPsf(maxExtPsf); 
      myClarkCleaner.setSpeedup(speedUp); 
  
      if (maxNumPix == 0) 
	maxNumPix = (Int)(modelImage.shape().product()*0.04);
      myClarkCleaner.setMaxNumPix((uInt)maxNumPix);

      //Now actually do the clean
      if (displayProgress) {
	ccpp = new ClarkCleanProgress ();
	myClarkCleaner.setProgress(*ccpp);
      }
      latConvEqn_p = new LatConvEquation (psfSub, dirtySub);
      result=myClarkCleaner.solve(*latConvEqn_p);
    }
    dirty_p->table().unlock();
    psf_p->table().unlock();
    if (ccpp != 0) { delete ccpp; ccpp = 0;}
    delete latConvEqn_p;  latConvEqn_p = 0;
    if (mask) { delete  mask;}

    return result;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  dirty_p->table().unlock();
  psf_p->table().unlock();
  if (ccpp != 0) {delete ccpp;  ccpp = 0; }
  delete latConvEqn_p;  latConvEqn_p = 0;

  
  return true;
};



Bool Deconvolver::setupLatCleaner(const String& /*algorithm*/, const Int /*niter*/,
				  const Float /*gain*/, const Quantity& /*threshold*/, 
				  const Bool /*displayProgress*/){

  LogIO os(LogOrigin("Deconvolver", "clean()", WHERE));
  
  /*
  if((algorithm=="msclean")||(algorithm=="fullmsclean" || algorithm=="multiscale" || algorithm=="fullmultiscale")) {
    os << "Cleaning image using multi-scale algorithm" << LogIO::POST;
    if(!scalesValid_p) {
      os << LogIO::SEVERE << "Scales not yet set" << LogIO::POST;
      return false;
    }
    cleaner_p->setscales(scaleSizes_p);
    cleaner_p->setcontrol(CleanEnums::MULTISCALE, niter, gain, threshold);
  }
  else if (algorithm=="hogbom") {
    if(!scalesValid_p) {
      Vector<Float> dummy;
      setscales("nscales", 1, dummy);
    }
    cleaner_p->setscales(scaleSizes_p);
    cleaner_p->setcontrol(CleanEnums::HOGBOM, niter, gain, threshold);
  } else {
    os << LogIO::SEVERE << "Unknown algorithm: " << algorithm << LogIO::POST;
    return false;
  }

  


  if(algorithm=="fullmsclean" || algorithm=="fullmultiscale") {
    os << "Cleaning full image using multi-scale algorithm" << LogIO::POST;
    cleaner_p->ignoreCenterBox(true);
  }
  */
  return true;

}
// Clean algorithm
Bool Deconvolver::clean(const String& algorithm, const Int niter,
			const Float gain, const Quantity& threshold, 
			const Bool displayProgress,
			const String& model, const String& mask, Float& maxResidual, Int& iterationsDone)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "clean()", WHERE));
  
  dirty_p->table().lock();
  psf_p->table().lock();
  try {
    
    if(model=="") {
      os << LogIO::SEVERE << "Need a name for model " << LogIO::POST;
      return false;
    }
    //Int psfnchan=psf_p->shape()(chanAxis_p);
    //Int masknchan=0;
  
    String imagename(model);
    // Make first image with the required shape and coordinates only if
    // it doesn't exist yet. Otherwise we'll throw an exception later
    if(imagename=="") imagename=dirty_p->table().tableName()+"."+algorithm;
    if(!Table::isWritable(imagename)) {
      make(imagename);
    }
    
    {
      ostringstream oos;
      oos << "Clean gain = " <<gain<<", Niter = "<<niter<<", Threshold = "
	  <<threshold << ", Algorithm " << algorithm ;
      os << String(oos) << LogIO::POST;
    }

    PagedImage<Float> modelImage(imagename);

    AlwaysAssert(!cleaner_p.null(), AipsError);
    PagedImage<Float> *maskim = 0;
    // Deal with mask
    if (mask != "") {
      if( Table::isReadable(mask)) {
	maskim = new PagedImage<Float>(mask);
	AlwaysAssert(maskim, AipsError);
	cleaner_p->setMask(*maskim);
	 } else {

	os << LogIO::SEVERE << "Mask "<< mask<<" is not readable" << LogIO::POST;
      }
    }

    
    Bool result=false;

    result=cleaner_p->clean(modelImage, algorithm, niter, gain, threshold, displayProgress);
    maxResidual=cleaner_p->maxResidual();
    iterationsDone=cleaner_p->numberIterations();
    dirty_p->table().relinquishAutoLocks(true);
    dirty_p->table().unlock();
    psf_p->table().relinquishAutoLocks(true);
    psf_p->table().unlock();
    if (maskim) delete maskim;    

    return result;
  } catch (AipsError x) {
    dirty_p->table().unlock();
    psf_p->table().unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  
  return true;
}

Bool Deconvolver::naclean(const Int niter,
			const Float gain, const Quantity& threshold, 
			  const String& model, const String& maskname, const Int masksupp, const Int memType, const Float numSigma, Float& maxResidual, Int& iterationsDone)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "clean()", WHERE));
  
  dirty_p->table().lock();
  psf_p->table().lock();
  try {
    
    if(model=="") {
      os << LogIO::SEVERE << "Need a name for model " << LogIO::POST;
      return false;
    }
    //Int psfnchan=psf_p->shape()(chanAxis_p);
    //Int masknchan=0;
  
    String imagename(model);
    // Make first image with the required shape and coordinates only if
    // it doesn't exist yet. Otherwise we'll throw an exception later
    if(imagename=="") imagename=dirty_p->table().tableName()+".naclean";
    if(!Table::isWritable(imagename)) {
      make(imagename);
    }
    
    {
      ostringstream oos;
      oos << "Clean gain = " <<gain<<", Niter = "<<niter<<", Threshold = "
	  <<threshold ;
      os << String(oos) << LogIO::POST;
    }

    PagedImage<Float> modelImage(imagename);

    AlwaysAssert(naCleaner_p != nullptr, AipsError);
    PagedImage<Float> *maskim = 0;
    // Deal with mask
    String mask=maskname;
    if (mask == "") {
      mask=dirty_p->table().tableName()+".mask";
    }
    if(!Table::isWritable(mask))
       make(mask);
    
    maskim = new PagedImage<Float>(mask);
       
    
    AlwaysAssert(maskim, AipsError);
    naCleaner_p->setMask(*maskim);

    

    
    Bool result=false;

    result=naCleaner_p->clean(modelImage, niter, gain, threshold, masksupp, memType, numSigma);
    maxResidual=cleaner_p->maxResidual();
    iterationsDone=cleaner_p->iteration();
    dirty_p->table().relinquishAutoLocks(true);
    dirty_p->table().unlock();
    psf_p->table().relinquishAutoLocks(true);
    psf_p->table().unlock();
    if (maskim) delete maskim;    

    return result;
  } catch (AipsError x) {
    dirty_p->table().unlock();
    psf_p->table().unlock();
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  
  return true;
}



// MEM algorithm
Bool Deconvolver::mem(const String& entropy, const Int niter,
		      const Quantity& sigma, const Quantity& targetFlux, 
		      Bool constrainTargetFlux, Bool displayProgress,
		      const String& model, const String& priorImage,
		      const String& maskImage,
		      const Bool imagePlane)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "mem()", WHERE));
  
  //  SubImage<Float>* dirtyQ_p =0; 
  //  SubImage<Float>* modelImageQ_p =0;
  //  SubImage<Float>* priorImageQ_p =0;
  //  SubImage<Float>* maskImageQ_p =0;

  Entropy* myEnt_p =0;
  CEMemProgress * memProgress_p = 0;
  residEqn_p=0;

  try {

    //    if(model=="") {
    //     os << LogIO::SEVERE << "Need a name for model " << LogIO::POST;
    //      return false;
    //   }

    Bool initializeModel = false;
    Int xbeg, xend, ybeg, yend;
    if (imagePlane) {
      xbeg=0;
      xend=nx_p-1;
      ybeg=0;
      yend=ny_p-1;
      fullPlane_p=true;
    } else {
      xbeg=nx_p/4; 
      xend=3*nx_p/4-1;
      ybeg=ny_p/4; 
      yend=3*ny_p/4-1;
    }

    String imagename(model);
    // Make first image with the required shape and coordinates only if
    // it doesn't exist yet. Otherwise we'll throw an exception later
    if(imagename=="") {
      imagename=dirty_p->table().tableName()+"."+entropy;
      os << LogIO::WARN << "No model name given, model will be " 
	 << imagename  << LogIO::POST;
    }
    if(!Table::isWritable(imagename)) {
      initializeModel = true;
      make(imagename);
      dirty_p->table().lock();
      psf_p->table().lock();
    }

    PagedImage<Float> modelImage(imagename);
    
    //   if (imagePlane) {
    //      modelImageQ_p = allQuarters(modelImage);
    //   } else {
    //     modelImageQ_p = innerQuarter(modelImage);
    //  }

    {
      ostringstream oos;
      oos << "MEM Niter = "<<niter<<", Sigma = "<<sigma << 
	", TargetFlux = "  <<targetFlux << 
	", ConstrainTargetFlux = "  <<constrainTargetFlux << 
	", Entropy = " << entropy;
      os << String(oos) << LogIO::POST;
    }

    if(entropy=="entropy") {
      os << "Deconvolving image using maximum entropy algorithm" << LogIO::POST;
      myEnt_p = new EntropyI;
    }
    else if (entropy=="emptiness") {
      myEnt_p = new EntropyEmptiness;
    }
    else {
      os << " Known MEM entropies: entropy | emptiness " << LogIO::POST;
      os << LogIO::SEVERE << "Unknown MEM entropy: " << entropy << LogIO::POST;
      return false;
    }


    PagedImage<Float> *mask = 0;
    Bool isCubeMask=false;
    PagedImage<Float> *prior =0;
    
    // Deal with mask
    if (maskImage != "") {
      if( Table::isReadable(maskImage)) {
	mask= new PagedImage<Float>(maskImage);
	if (chanAxis_p < Int(mask->shape().nelements())){
	  if (mask->shape()(chanAxis_p) > 1) 
	    isCubeMask=true;
	}
	checkMask(*mask, xbeg, xend, ybeg, yend);
	AlwaysAssert(mask, AipsError);
	
      } else {
	os << LogIO::SEVERE << "Mask "<< mask<<" is not readable" << LogIO::POST;
      }
    }

    if (priorImage!="") {
      if( Table::isReadable(priorImage)) {
        prior= new PagedImage<Float>(priorImage);
      } else {
        os << LogIO::SEVERE << "Prior "<< prior<<" is not readable" << LogIO::POST;
      }
    }

    Bool result=false;
    SubImage<Float> *maskSub=0;
    IPosition blc(2,xbeg,ybeg);
    IPosition trc(2,xend,yend);
    if(nchan_p >= 1){
      for (Int k=0; k< nchan_p; ++k){	
	os<< "Processing channel " << k+1 << LogIO::POST;
	if(npol_p > 0 ){
	  blc.resize(4);
	  blc(chanAxis_p)=k;
	  blc(polAxis_p)=0;
	  trc.resize(4);
	  trc(polAxis_p)=npol_p-1;
	  trc(chanAxis_p)=k;
	
	}
	else{
	  blc.resize(3);
	  trc.resize(3);  
	  blc(chanAxis_p)=k;
	  trc(chanAxis_p)=k;
	
	}

	Slicer sl(blc, trc, Slicer::endIsLast);
	SubImage<Float> psfSub;
	SubImage<Float> priorSub;

	if(mask != 0){
	  if( (isCubeMask) || (!isCubeMask && maskSub == 0 )){
	    if(maskSub !=0 ) delete maskSub;
	    blc(0)=0; blc(1)=0;
	    trc(0)=nx_p-1; trc(1)=ny_p-1;
	    sl=Slicer(blc, trc, Slicer::endIsLast);
	    maskSub=new SubImage<Float> (*mask,sl,false);
	    checkMask(*maskSub, xbeg, xend, ybeg, yend);
	    blc(0)=xbeg; blc(1)=ybeg;
	    trc(0)=xend; trc(1)=yend;
	    sl =Slicer(blc, trc, Slicer::endIsLast);
	    delete maskSub;
	    maskSub=new SubImage<Float> (*mask,sl,false);
	  }
	}


	if(prior !=0 ){	  
	  priorSub= SubImage<Float>(*prior, false);
	}

	SubImage<Float> dirtySub(*dirty_p, sl, true);
	SubImage<Float> modelSub(modelImage,sl,true);
	IPosition blc_psf=blc; IPosition trc_psf=trc;
	if(psf_p->shape().nelements() != dirty_p->shape().nelements()){
	  blc_psf.resize(psf_p->shape().nelements());
	  trc_psf.resize(psf_p->shape().nelements());
	}
	blc_psf(0)=0; blc_psf(1)=0;
	trc_psf(0)=nx_p-1; trc_psf(1)=ny_p-1;
	sl=Slicer(blc_psf, trc_psf, Slicer::endIsLast);
	psfSub=SubImage<Float>(*psf_p, sl, true);



	CEMemModel myMemer( *myEnt_p, modelSub, niter, sigma.getValue("Jy"),
			    targetFlux.getValue("Jy"),  constrainTargetFlux,
			    initializeModel, imagePlane);
    
	if (!initializeModel) {
	  Record info=modelImage.miscInfo();
	  try {
	    Float alpha = 0.0;
	    Float beta = 0.0;
	    info.get("ALPHA", alpha);
	    myMemer.setAlpha(alpha);
	    info.get("BETA", beta);
	    myMemer.setBeta(beta); 
	  } catch  (AipsError x) {
	    // could not get Alpha and Beta for initialization
	    // continue
	    os << "Could not retrieve Alpha and Beta from previously initialized model" 
	       << LogIO::POST;
	  } 
	} 



	if(prior != 0){
	  myMemer.setPrior(priorSub);
	}
	if (mask != 0) {
	  myMemer.setMask(*maskSub);
	}
      
    
	// Now actually do the MEM deconvolution
	if (displayProgress) {
	  memProgress_p = new  CEMemProgress ();
	  myMemer.setProgress(*memProgress_p);
	}

	if(residEqn_p !=0) delete residEqn_p;
	residEqn_p=0;
	if (imagePlane) {
	  residEqn_p = new IPLatConvEquation (psfSub, dirtySub);
	} else {
	  residEqn_p = new LatConvEquation (psfSub, dirtySub);
	}    

	result=myMemer.solve(*residEqn_p);
    
	Record info=modelImage.miscInfo();
	info.define("ALPHA", myMemer.getBeta());
	info.define("BETA",  myMemer.getAlpha());
	modelImage.setMiscInfo(info);
      }
    }

    else{
      SubImage<Float>* dirtyQ =0; 
      SubImage<Float>* modelQ =0; 
      Bool initializeModel = false;

      if (imagePlane) {
	dirtyQ = allQuarters(*dirty_p);
      } else {
	dirtyQ = innerQuarter(*dirty_p);
      }
      if (imagePlane) {
	modelQ = allQuarters(modelImage);
      } else {
	modelQ = innerQuarter(modelImage);
      }
      CEMemModel myMemer( *myEnt_p, *modelQ, niter, 
			  sigma.getValue("Jy"),
			  targetFlux.getValue("Jy"),  constrainTargetFlux,
			  initializeModel, imagePlane);
      if (!initializeModel) {
	Record info=modelImage.miscInfo();
	try {
	  Float alpha = 0.0;
	  Float beta = 0.0;
	  info.get("ALPHA", alpha);
	  myMemer.setAlpha(alpha);
	  info.get("BETA", beta);
	  myMemer.setBeta(beta); 
	} catch  (AipsError x) {
	  // could not get Alpha and Beta for initialization
	  // continue
	  os << "Could not retrieve Alpha and Beta from previously initialized model" 
	     << LogIO::POST;
	} 
      } 
      SubImage<Float> * priorQ=NULL;
      if(prior !=0){	

	if (imagePlane) {
	  priorQ = allQuarters(*prior);
	} else {
	  priorQ = innerQuarter(*prior);
	
	}
	 myMemer.setPrior(*priorQ);
      }
      SubImage<Float> *maskQ=NULL;
      if(mask !=0){	

	if (imagePlane) {
	  maskQ = allQuarters(*mask);
	} else {
	  maskQ = innerQuarter(*mask);
	
	}
	 myMemer.setMask(*maskQ);
      }
      if (displayProgress) {
	memProgress_p = new  CEMemProgress ();
	myMemer.setProgress(*memProgress_p);
      }

      if (imagePlane) {
	residEqn_p = new IPLatConvEquation (*psf_p, *dirtyQ);
      } else {
	residEqn_p = new LatConvEquation (*psf_p, *dirtyQ);
      }    
      
      result=myMemer.solve(*residEqn_p);
      
      Record info=modelImage.miscInfo();
      info.define("ALPHA", myMemer.getBeta());
      info.define("BETA",  myMemer.getAlpha());
      modelImage.setMiscInfo(info);

      if (dirtyQ != 0) {delete dirtyQ; dirtyQ = 0;}
      if (modelQ != 0) {delete modelQ; modelQ = 0;}
      if (residEqn_p != 0) {delete residEqn_p;    residEqn_p = 0;}
      if (priorQ != 0) {delete priorQ; priorQ = 0;}
      if (maskQ != 0) {delete maskQ; maskQ = 0;}

    }

    modelImage.setUnits(Unit("Jy/pixel"));

    dirty_p->table().unlock();
    psf_p->table().unlock();
    if (myEnt_p != 0) {delete myEnt_p;  myEnt_p = 0;}
    if (memProgress_p!=0) {delete memProgress_p;  memProgress_p = 0; }    

    return result;

  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  
  dirty_p->table().unlock();
  psf_p->table().unlock();
  if (myEnt_p != 0) {delete myEnt_p;  myEnt_p = 0;}
  if (residEqn_p != 0) {delete residEqn_p;    residEqn_p = 0;}
  if (memProgress_p!=0) {delete memProgress_p;  memProgress_p = 0; }    
  
  return true;
}

// makeprior, for MEM
Bool Deconvolver::makeprior(const String& prior, const String& templatename, 
			    const Quantity& lowClipFrom, const Quantity& lowClipTo, 
			    const Quantity& highClipFrom, const Quantity& highClipTo, 
			    const Vector<Int>& blc, const Vector<Int>& trc)
{
  
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "makeprior()", WHERE));
  
  try {
    if(templatename=="") {
      os << LogIO::SEVERE << "Need a name for template image " << endl
	 << "May I suggest you make a clean or mem image and " << endl
	 << "smooth it for the template?" << LogIO::POST;
      return false;
    }
    if(prior=="") {
      os << LogIO::SEVERE << "Need a name for output prior image " << LogIO::POST;
      return false;
    }

    PagedImage<Float> templateImage(templatename);
    String priorname(prior);
    if(priorname=="") priorname=templateImage.table().tableName()+".prior";
    if(!Table::isWritable(priorname)) {
      make(priorname);
    }
//
    PagedImage<Float> priorImage(priorname);
    ImageInterface<Float>* templateImage2_p = 0;
//
    if (priorImage.shape() != templateImage.shape()) {
       TiledShape tShape(priorImage.shape());
       templateImage2_p = new TempImage<Float>(tShape, priorImage.coordinates());
//
       ImageRegrid<Float> regridder;
       Vector<Double> locate;
       Bool missedIt = regridder.insert(*templateImage2_p, locate, templateImage);
       if (!missedIt) {
	os << LogIO::SEVERE << "Problem in getting template Image on correct grid " << LogIO::POST;
       }
    } else {
      templateImage2_p = &templateImage;
    }

    {
      ostringstream oos;
      oos << "Prior = "<<priorname<<", template = "<<templatename << endl;
      oos <<"   Clip Below = "  << lowClipFrom.getValue("Jy") << 
	", Replace with = "  << lowClipTo.getValue("Jy") << endl;
      
      oos <<"   Clip Above = "  << highClipFrom.getValue("Jy") << 
	", Replace with = "  << highClipTo.getValue("Jy") << endl; 
      // oos <<"   blc = " << blc <<", trc = " << trc << endl;
      os << String(oos) << LogIO::POST;
    }

    priorImage.set(lowClipTo.getValue("Jy"));

    IPosition iblc(blc);
    IPosition itrc(trc);
    IPosition imshape(priorImage.shape());  
    //    Index::convertIPosition(iblc, blc);
    //   Index::convertIPosition(itrc, trc);
    IPosition iinc(imshape.nelements(),1);
    LCBox::verify(iblc, itrc, iinc, imshape);
    LCSlicer box(iblc, itrc);

    SubImage<Float> templateBox(*templateImage2_p, ImageRegion(box), false);
    SubImage<Float> priorBox(priorImage, ImageRegion(box), true);

    // do Low clipping
    priorBox.copyData( (LatticeExpr<Float>) 
		       (iif(templateBox<lowClipFrom.getValue("Jy"), 
			    lowClipTo.getValue("Jy"), templateBox)) );  
    // do High clipping
    priorBox.copyData( (LatticeExpr<Float>) 
		       (iif(priorBox > highClipFrom.getValue("Jy"), 
			    highClipTo.getValue("Jy"), priorBox)) );  


    return true;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  
  return true;
}

// clipimage
Bool Deconvolver::clipimage(const String& clippedImageName, const String& inputImageName, 
			    const Quantity& threshold)
{
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "clipimage()", WHERE));
  
  try {
    if(inputImageName=="") {
      os << LogIO::SEVERE << "Need a name for the image to clip" <<  LogIO::POST;
      return false;
    }
    if(clippedImageName=="") {
      os << LogIO::SEVERE << "Need a name for output clipped image " << LogIO::POST;
      return false;
    }

    PagedImage<Float> inputImage(inputImageName);
    String clippedImageName2(clippedImageName);
    if(clippedImageName2=="") clippedImageName2 = inputImage.table().tableName()+".clipped";
    if(!Table::isWritable(clippedImageName2)) {
      make (clippedImageName2);
    }
    PagedImage<Float> clippedImage(clippedImageName2);
    if  (clippedImage.shape() != inputImage.shape() ) {
      os << LogIO::SEVERE << "Input and clipped image sizes disagree " << LogIO::POST;
      return false;
    }
    {
      ostringstream oos;
      oos << "Clipped Image = "<<clippedImageName2<<", Input Image = "<< inputImageName << endl;
      oos << "Clip with Stokes I below = "  << threshold.getValue("Jy");
      os << String(oos) << LogIO::POST;
    }

    IPosition trc = inputImage.shape() - 1;
    IPosition blc(4,0);
    
    trc(2) = 0;
    blc(2) = 0;
    LCSlicer boxI(blc, trc);
    SubImage<Float> stokesISub(inputImage, ImageRegion(boxI), false);
    Int iStokes;
    for (iStokes=0; iStokes < inputImage.shape()(2); iStokes++) {
      trc(2) = iStokes;
      blc(2) = iStokes;
      LCSlicer box(blc, trc);
      SubImage<Float> stokesClippedSub(clippedImage, ImageRegion(box), true);
      SubImage<Float>   stokesInputSub(inputImage, ImageRegion(box), false);
      if (stokesISub.shape() != stokesClippedSub.shape() ) {
	os << LogIO::SEVERE << "Input and clipped image sizes disagree " << LogIO::POST;
	return false;
      }
      stokesClippedSub.copyData( (LatticeExpr<Float>) 
				 (iif(stokesISub < threshold.getValue("Jy"), 
				      0.0, stokesInputSub)) );  
    }
    return true;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  
  return true;
}

// boxmask
Bool Deconvolver::boxmask(const String& boxmask, 
			  const Vector<Int> blc, const Vector<Int> trc,
			  const Quantity& fillValue, const Quantity& externalValue)
{
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "boxmask()", WHERE));
  
  try {
    if(boxmask=="") {
      os << LogIO::SEVERE << "Need a name for output boxmask image " << LogIO::POST;
      return false;
    }
    String boxname(boxmask);
    if(boxname=="") boxname="boxmask";
    if(!Table::isWritable(boxname)) {
      make(boxname);
    }
    PagedImage<Float> boxImage(boxname);

    {
      ostringstream oos;
      oos << "BoxMask = "<<boxname<<
	", blc = " << blc(0) << " " << blc(1)<<
	", trc = " << trc(0) << " " << trc(1);
      os << String(oos) << LogIO::POST;
    }

    boxImage.set(externalValue.getValue("Jy"));

    // This only makes a 2-d box; will need to fix this for other
    // image sorts
    uInt dim = boxImage.ndim();
    IPosition pshape = boxImage.shape();
    IPosition blc0(dim, 0);
    IPosition trc0(dim, 0);
    blc0(0) = max(0, blc(0));
    blc0(1) = max(0, blc(1));
    if (trc0(0) == 0) trc0(0) = pshape(0)-1;
    if (trc0(1) == 0) trc0(1) = pshape(1)-1;    
    trc0(0) = min(trc(0), pshape(0)-1);
    trc0(1) = min(trc(1), pshape(1)-1);
    LCSlicer box(blc0, trc0);
    SubImage<Float> innerSub(boxImage, ImageRegion(box), true);
    innerSub.set(fillValue.getValue("Jy"));
    return true;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  }   
  return true;
}

//regionmask
Bool Deconvolver::regionmask(const String& maskimage, Record* imageRegRec, Matrix<Quantity>& blctrcs, const Float& value){
 
  LogIO os(LogOrigin("deconvolver", "regionmask()", WHERE));
  if(!dirty_p) {
    os << LogIO::SEVERE << "Program logic error: Dirty image pointer dirty_p not yet set"
       << LogIO::POST;
    return false;
  }
  if(!Table::isWritable(maskimage)) {
    if (!clone(dirty_p->name(),maskimage)) return false;
    PagedImage<Float> mim(maskimage);
    mim.set(0.0);
    mim.table().relinquishAutoLocks();
  }
  Matrix<Float> circles;
  return Imager::regionToImageMask(maskimage, imageRegRec, blctrcs, circles, value);

}

// Fourier transform the model
Bool Deconvolver::ft(const String& model, const String& transform)
{
  if(!valid()) return false;
  
  LogIO os(LogOrigin("Deconvolver", "ft()", WHERE));
  
  try {
    
    if(model=="") {
      os << LogIO::SEVERE << "Need a name for model " << LogIO::POST;
      return false;
    }
    
    os << "Fourier transforming model" << LogIO::POST;

    String transformname(transform);
    if(transformname=="") transformname=model+".ft";
    removeTable(transformname);
    
    PagedImage<Float> modelImage(model);
    PagedImage<Complex> transformImage(modelImage.shape(),
				       modelImage.coordinates(),
				       transformname);
    transformImage.copyData(LatticeExpr<Complex>(toComplex(modelImage)));

    LatticeFFT::cfft2d(transformImage);

    return true;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  return true;
}

Bool Deconvolver::setscales(const String& scaleMethod,
			    const Int inscales,
			    const Vector<Float>& userScaleSizes)
{
  LogIO os(LogOrigin("Deconvolver", "setscales()", WHERE));

  AlwaysAssert(!cleaner_p.null(), AipsError);

  Vector<Double> cells = psf_p->coordinates().increment();
  os << "Cell size = " << abs(cells(0)/C::arcsec) << LogIO::POST;
  AlwaysAssert (cells(0)!=0.0, AipsError);
  scaleSizes_p.resize();
  if (scaleMethod == "nscales") {
    Int nscales=inscales;

    if(nscales<1) {
      os << "Using default of 5 scales" << LogIO::POST;
      nscales=5;
    }
  
    // Validate scales
    Float scaleInc=beam_p.getMinor().get("arcsec").getValue()/abs(cells(0)/C::arcsec);

    Vector<Float> scaleSizes(nscales);  
    os << "Creating " << nscales << 
      " scales from powerlaw nscales method" << LogIO::POST;
    scaleSizes(0) = 0.0;
    os << "scale 1 = 0.0 pixels " << LogIO::POST;
    for (Int scale=1; scale<nscales;scale++) {
      scaleSizes(scale) =
	scaleInc * pow(10.0, (Float(scale)-2.0)/2.0);
      os << "scale " << scale+1 << " = " << scaleSizes(scale)
	 << " pixels" << LogIO::POST;
    }  
    scaleSizes_p=scaleSizes;
    cleaner_p->setscales(scaleSizes);   
    scalesValid_p=true;

  } else if (scaleMethod == "uservector") {
    if (userScaleSizes.nelements() <= 0) {
       os << LogIO::SEVERE 
	  << "Need at least one scale for method uservector"
	  << LogIO::POST;
    }
    os << "Creating scales from uservector method: " << LogIO::POST;
    for(uInt scale=0; scale < userScaleSizes.nelements(); scale++) {
      os << "scale " << scale+1 << " = " << userScaleSizes(scale)
	 << " pixels" << LogIO::POST;
    }
    scaleSizes_p=userScaleSizes;
    cleaner_p->setscales(userScaleSizes);   
    scalesValid_p=true;

  } else {
    os << LogIO::SEVERE << "Unknown scale setting algorithm: " 
       << scaleMethod << LogIO::POST;
    return false;
  }

  return true;
}
  
Bool Deconvolver::clone(const String& imageName, const String& newImageName)
{
  if(!valid()) return false;
  LogIO os(LogOrigin("Deconvolver", "clone()", WHERE));
  try {
    PagedImage<Float> oldImage(imageName);
    PagedImage<Float> newImage(oldImage.shape(), oldImage.coordinates(),
			       newImageName);
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
  return true;
}



Bool Deconvolver::convolve(const String& convolvedName, 
			   const String& modelName)
{
   PagedImage<Float> model(modelName);
   PagedImage<Float> convolved(model.shape(),
			     model.coordinates(),
			     convolvedName);
   convolver_p->linear( convolved, model );
   return true;
};

Bool Deconvolver::makegaussian(const String& gaussianName, GaussianBeam& mbeam, Bool normalizeVolume)
{
  LogIO os(LogOrigin("Deconvolver", "makegaussian()", WHERE));
  if(!dirty_p) {
    os << LogIO::SEVERE << "Program logic error: Dirty image pointer dirty_p not yet set"
       << LogIO::POST;
    return false;
  }
  PagedImage<Float> gaussian(dirty_p->shape(),
			     dirty_p->coordinates(),
			     gaussianName);
  gaussian.set(0.0);
  gaussian.setUnits(Unit("Jy/pixel"));
  putGaussian(gaussian, mbeam);
  if(!normalizeVolume){
    Float maxpsf=max(gaussian).getFloat();
    gaussian.copyData((LatticeExpr<Float>)(gaussian/maxpsf));
  }
  //uInt naxis=gaussian.shape().nelements();
  // StokesImageUnil::Convolve requires an image with four axes
  /* 
  if(naxis==2){
    IPosition center = gaussian.shape()/2;
    gaussian.putAt(1.0, center);
  }
  else if(naxis==3){
    IPosition center(3, Int((nx_p/4)*2), Int((ny_p/4)*2),0);
    for (Int k=0; k < gaussian.shape()(2); ++k){
      center(2) = k;
      gaussian.putAt(1.0, center);
    }
  }
  */
  /*  if(naxis<4){
    os << LogIO::SEVERE << "Input dirty image: naxis=" <<naxis
       << ". Four axes are required."<< LogIO::POST;
    return false;

  }
  else if(naxis==4){
    IPosition center(4, Int((nx_p/4)*2), Int((ny_p/4)*2),0,0);
    for (Int k=0; k < gaussian.shape()(2); ++k){
      
      center(2) = k;
      for(Int j=0; j < gaussian.shape()(3); ++j){
	center(3)=j;
	gaussian.putAt(1.0, center);
      }
    }


  }
  StokesImageUtil::Convolve(gaussian, mbeam, normalizeVolume);
  */
  return true;
};

Bool Deconvolver::putGaussian(ImageInterface<Float>& im, const GaussianBeam& beam){
  CoordinateSystem cs=im.coordinates();
  
  Vector<Int> dirAxes=CoordinateUtil::findDirectionAxes(cs);
  DirectionCoordinate dirCoord=cs.directionCoordinate(cs.findCoordinate(Coordinate::DIRECTION));
  Vector<Double> cenpix(2, Double(nx_p)/2.0);
  cenpix(1)=Double(ny_p)/2.0;
  MDirection centre;
  dirCoord.toWorld(centre, cenpix);
  MVAngle mvRA=centre.getAngle().getValue()(0);
  MVAngle mvDEC=centre.getAngle().getValue()(1);
  //cerr << "centre " << cenpix <<  "   " << centre.getRefString() << " " << mvRA(0.0).string(MVAngle::TIME,8)  << " " <<  mvDEC(0.0).string(MVAngle::ANGLE_CLEAN,8) << endl;
  //cerr << "maj min pa " << beam.getMajor() << "   " <<  beam.getMinor() << "   " <<  beam.getPA() << endl;
  GaussianShape gshp(centre, beam.getMajor(), beam.getMinor(), beam.getPA());
  SkyComponent gcomp(Flux<Double>(1.0), gshp, ConstantSpectrum());
  ComponentList cl;
  cl.add(gcomp);
  ComponentImager::project(im, cl);
  return true;
  


}


Bool Deconvolver::detached() const
{
  if (dirty_p == 0) {
    LogIO os(LogOrigin("Deconvolver", "detached()", WHERE));
    os << LogIO::SEVERE << 
      "Deconvolver is detached - cannot perform operation." << endl <<
      "Call Deconvolver.open('dirtyname', 'psfname') to reattach." << LogIO::POST;
    return true;
  }
  return false;
}

Bool Deconvolver::removeTable(const String& tablename) {
  
  LogIO os(LogOrigin("Deconvolver", "removeTable()", WHERE));
  
  if(Table::isReadable(tablename)) {
    if (! Table::isWritable(tablename)) {
      os << LogIO::SEVERE << "Table " << tablename
	 << " is not writable!: cannot alter it" << LogIO::POST;
      return false;
    }
    else {
      if (Table::isOpened(tablename)) {
	os << LogIO::SEVERE << "Table " << tablename
	   << " is already open in the process. It needs to be closed first"
	   << LogIO::POST;
	  return false;
      } else {
	Table table(tablename, Table::Update);
	if (table.isMultiUsed()) {
	  os << LogIO::SEVERE << "Table " << tablename
	     << " is already open in another process. It needs to be closed first"
	     << LogIO::POST;
	    return false;
	} else {
	  Table table(tablename, Table::Delete);
	}
      }
    }
  }
  return true;
}

Bool Deconvolver::valid() const {
  LogIO os(LogOrigin("Deconvolver", "if(!valid()) return false", WHERE));
  if(!dirty_p) {
    os << LogIO::SEVERE << "Program logic error: Dirty image pointer dirty_p not yet set"
       << LogIO::POST;
    return false;
  }
  if(!psf_p) {
    os << LogIO::SEVERE << "Program logic error: PSF  pointer psf_p not yet set"
       << LogIO::POST;
    return false;
  }
  return true;
}

void Deconvolver::findAxes(){

  CoordinateSystem coordsys= dirty_p->coordinates();
  polAxis_p=coordsys.findCoordinate(Coordinate::STOKES);
  if(polAxis_p > coordsys.findCoordinate(Coordinate::DIRECTION))
    polAxis_p+=1;
  chanAxis_p=coordsys.findCoordinate(Coordinate::SPECTRAL);
  if(chanAxis_p > coordsys.findCoordinate(Coordinate::DIRECTION))
    chanAxis_p+=1;
  
}

void Deconvolver::checkMask(ImageInterface<Float>& maskImage, Int& xbeg, Int& xend, 
			    Int& ybeg, Int& yend){
 

  LogIO os(LogOrigin("Deconvolver","checkMask",WHERE)); 

  xbeg=nx_p/4;
  ybeg=ny_p/4;
  
  xend=xbeg+nx_p/2-1;
  yend=ybeg+ny_p/2-1;  
  Slicer sl;
  if(nchan_p >= 1){
    
    if(npol_p > 0 ){
      IPosition blc(4,0,0,0,0);
      blc(chanAxis_p)=0;
      blc(polAxis_p)=0;
      IPosition trc(4, nx_p-1, ny_p-1, 0, 0);
      trc(chanAxis_p)=0;
      trc(polAxis_p)=0;
      sl=Slicer(blc, trc, Slicer::endIsLast);
    }
    else{
      IPosition blc(3, 0, 0, 0);
      IPosition trc(3, nx_p-1, ny_p-1, 0);
      sl=Slicer(blc, trc, Slicer::endIsLast);
    }
  }
  else{
    if(npol_p > 0 ){
      IPosition blc(3, 0, 0, 0);
      IPosition trc(3, nx_p-1, ny_p-1, 0);
      sl=Slicer(blc, trc, Slicer::endIsLast);
    }
    else{
       IPosition blc(2, 0, 0);
       IPosition trc(2, nx_p-1, ny_p-1);
    }
  }


  Matrix<Float> mask= maskImage.getSlice(sl, true);
  // ignore mask if none exists
  if(max(mask) < 0.000001) {
    os << "Mask seems to be empty; will CLEAN inner quarter" 
       << LogIO::WARN;
    return;
  }
  // Now read the mask and determine the bounding box

  xbeg=nx_p-1;
  ybeg=ny_p-1;
  xend=0;
  yend=0;

  
  for (Int iy=0;iy<ny_p;iy++) {
    for (Int ix=0;ix<nx_p;ix++) {
      if(mask(ix,iy)>0.000001) {
	xbeg=min(xbeg,ix);
	ybeg=min(ybeg,iy);
	xend=max(xend,ix);
	yend=max(yend,iy);

      }
    }
  }
  // Now have possible BLC. Make sure that we don't go over the
  // edge later
  if(((xend - xbeg)>nx_p/2) && (!fullPlane_p)) {
    xbeg=nx_p/4-1; //if larger than quarter take inner of mask
    os << LogIO::WARN << "Mask span over more than half the x-axis: Considering inner half of the x-axis"  << LogIO::POST;
  } 
  if(((yend - ybeg)>ny_p/2) && (!fullPlane_p)) { 
    ybeg=ny_p/4-1;
    os << LogIO::WARN << "Mask span over more than half the y-axis: Considering inner half of the y-axis" << LogIO::POST;
  }  

  // Just making sure we are within limits...
  if(fullPlane_p){
    xend=min(xend,nx_p-1);
    yend=min(yend,ny_p-1);
  }
  else{
    xend=min(xend,xbeg+nx_p/2-1);
    yend=min(yend,ybeg+ny_p/2-1); 
  }



}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Multi-Term Clean algorithm with Taylor-Polynomial basis functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Bool Deconvolver::mtopen(const Int nTaylor,
			 const Vector<Float>& userScaleSizes,
			 const Vector<String>& psfs)
{
  
  //  if(!valid()) return false; //make MT version
  LogIO os(LogOrigin("Deconvolver", "mtopen()", WHERE));

  // Check for already-open mt-deconvolver
  
  if(mt_nterms_p != -1)
    {
      os << LogIO::WARN << "Multi-term Deconvolver is already open, and set-up for " << mt_nterms_p << " Taylor-terms. Please close the deconvolver and reopen, or continue with cleaning." << LogIO::POST;
      return false;
    }

  // Check for valid ntaylor
  if( nTaylor <=0 ) 
    {
      os << LogIO::SEVERE << "nTaylor must be at-least 1" << LogIO::POST; 
      return false;
    }

  mt_nterms_p = nTaylor;
  uInt mt_npsftaylor = 2*nTaylor-1;

  // Check if the correct number of PSFs exist.
  if( psfs.nelements() != mt_npsftaylor )
    {
      os << LogIO::SEVERE << "For " << mt_nterms_p << " Taylor terms, " << mt_npsftaylor << " PSFs are needed to populate the Hessian matrix. " << LogIO::POST;
      return false;
    }

  os << "Initializing MT-Clean with " << mt_nterms_p << " Taylor terms, " << userScaleSizes.nelements() << " scales and " << psfs.nelements() << " unique Hessian elements (psfs)" << LogIO::POST;

  // Open all the PSFs...
  Block<CountedPtr<PagedImage<Float> > > mt_psfs(mt_npsftaylor);

  // Open the first PSF and extract shape info.
  mt_psfs[0] = new PagedImage<Float>(psfs[0]);
  AlwaysAssert(&*mt_psfs[0], AipsError);
  nx_p=mt_psfs[0]->shape()(0);
  ny_p=mt_psfs[0]->shape()(1);
  //mt_psfs_p[0].setMaximumCacheSize(2*nx_p*ny_p);
  //mt_psfs_p[0].setCacheSizeInTiles(10000);

  // Open the other PSFs and verify shape consistency
  for(uInt i=1; i<mt_npsftaylor;i++)
    {
      mt_psfs[i] = new PagedImage<Float>(psfs[i]);
      AlwaysAssert(&*mt_psfs[i], AipsError);
      if( mt_psfs[i]->shape()(0) != nx_p || mt_psfs[i]->shape()(1) != ny_p )
	{
	  os << LogIO::SEVERE << "Supplied PSFs are of different shapes. Please try again." << LogIO::POST;
	  mt_psfs.resize(0); // Not sure if this is safe, with CountedPtrs. WARN.
	  return false;
	}
    }

  // Initialize the Multi-Term Cleaner
  try
    {
      mt_cleaner_p.setntaylorterms(mt_nterms_p);
      mt_cleaner_p.setscales(userScaleSizes);
      mt_cleaner_p.initialise(nx_p,ny_p); // allocates memory once....
    }
  catch (AipsError x) 
    {
      os << LogIO::WARN << "Cannot allocate required memory for Multi-Term minor cycle" 
       << LogIO::POST;
      return false;
    } 

  // Send the PSFs into the Multi-Term Matrix Cleaner
  for (uInt order=0;order<mt_npsftaylor;order++)
    {
      Matrix<Float> tempMat;
      Array<Float> tempArr;
      (mt_psfs[order])->get(tempArr,true); // by reference.
      tempMat.reference(tempArr);

      mt_cleaner_p.setpsf( order , tempMat ); 
    }

  // Compute Hessian elements for Taylor terms and Scales ( convolutions ), take the Hessian block-diagonal approximation and inverse, Check for invertability.
  Int ret = mt_cleaner_p.computeHessianPeak();
  if (ret != 0)
    {
      os << LogIO::SEVERE << "Cannot Invert Hessian matrix. Please close the deconvolver and supply different PSFs." << LogIO::POST; 
      return false;
    }

  // mt_psfs goes out of scope and CountedPtrs delete themselves automatically.

  mt_valid_p=true;
  
  return true;
}

Bool Deconvolver::mtclean(const Vector<String>& residuals,
			  const Vector<String>& models,
			  const Int niter,
			  const Float gain, 
			  const Quantity& threshold, 
			  const Bool /*displayProgress*/,
			  const String& mask, 
			  Float& maxResidual, Int& iterationsDone)
{
  
  LogIO os(LogOrigin("Deconvolver", "mtclean()", WHERE));

  if(mt_valid_p==false)
    {
      os << LogIO::WARN << "Multi-Term Deconvolver is not initialized yet. Please call 'mtopen()' first" << LogIO::POST;
      return false;
    }

  os << "Running MT-Clean with " << mt_nterms_p << " Taylor terms " << LogIO::POST;

  // Send in the mask.
  if( mask != String("") )
    {
      PagedImage<Float> mt_mask(mask);
      Matrix<Float> tempMat;
      Array<Float> tempArr;
      (mt_mask).get(tempArr,true);
      tempMat.reference(tempArr);
      
      mt_cleaner_p.setmask( tempMat );
    }

  // Send in the residuals and model images.
  Block<CountedPtr<PagedImage<Float> > > mt_residuals(mt_nterms_p);
  Block<CountedPtr<PagedImage<Float> > > mt_models(mt_nterms_p);
  for (Int i=0;i<mt_nterms_p;i++)
    {
      mt_residuals[i] = new PagedImage<Float>(residuals[i]);
      AlwaysAssert(&*mt_residuals[i], AipsError);
      if( mt_residuals[i]->shape()(0) != nx_p || mt_residuals[i]->shape()(1) != ny_p )
	{
	  os << LogIO::SEVERE << "Supplied Residual images don't match PSF shapes." << LogIO::POST;
	  mt_residuals.resize(0); // Not sure if this is safe, with CountedPtrs. WARN.
	  return false;
	}

      mt_models[i] = new PagedImage<Float>(models[i]);
      AlwaysAssert(&*mt_models[i], AipsError);
      if( mt_models[i]->shape()(0) != nx_p || mt_models[i]->shape()(1) != ny_p )
	{
	  os << LogIO::SEVERE << "Supplied Model images don't match PSF shapes." << LogIO::POST;
	  mt_models.resize(0); // Not sure if this is safe, with CountedPtrs. WARN.
	  return false;
	}


      Matrix<Float> tempMat;
      Array<Float> tempArr;

      (mt_residuals[i])->get(tempArr,true); // by reference.
      tempMat.reference(tempArr);
      mt_cleaner_p.setresidual( i , tempMat ); 

      (mt_models[i])->get(tempArr,true); // by reference.
      tempMat.reference(tempArr);
      mt_cleaner_p.setmodel( i , tempMat ); 

    } // end of for i 


  // Fill in  maxResidual and iterationsDone

  Float fractionOfPsf=0.05;
  
  // Call the cleaner
  iterationsDone = mt_cleaner_p.mtclean(niter, fractionOfPsf, gain, threshold.getValue(String("Jy")));

  // Get back the model (this is not held in MTMC by reference, because of 'incremental=T/F' logic....
  for (Int order=0;order<mt_nterms_p;order++)
    {
      Matrix<Float> tempMod;
      mt_cleaner_p.getmodel(order,tempMod);
      mt_models[order]->put(tempMod);

      // Also get the updated residuals. Not really needed, but good to have.
      mt_cleaner_p.getresidual(order,tempMod);
      mt_residuals[order]->put(tempMod);

      if(order==0) maxResidual = max(fabs(tempMod));

    }           

  return maxResidual <= threshold.getValue(String("Jy"));
}


Bool Deconvolver::mtrestore(const Vector<String>& models,
			  const Vector<String>& residuals,
			  const Vector<String>& images,
			  GaussianBeam& mbeam)
{

  LogIO os(LogOrigin("Deconvolver", "mtrestore()", WHERE));

  // check that the mt_cleaner_p is alive..
  if(mt_valid_p==false)
    {
      os << LogIO::WARN << "Multi-Term Deconvolver is not initialized yet. Please call 'mtopen()' first" << LogIO::POST;
      return false;
    }
  
  os << "Restoring Taylor-coefficient images" << LogIO::POST;

  Block<CountedPtr<PagedImage<Float> > > mt_residuals(mt_nterms_p);
  for (Int i=0;i<mt_nterms_p;i++)
    {
      mt_residuals[i] = new PagedImage<Float>(residuals[i]);
      AlwaysAssert(&*mt_residuals[i], AipsError);
      if( mt_residuals[i]->shape()(0) != nx_p || mt_residuals[i]->shape()(1) != ny_p )
	{
	  os << LogIO::SEVERE << "Supplied Residual images don't match PSF shapes." << LogIO::POST;
	  mt_residuals.resize(0); // Not sure if this is safe, with CountedPtrs. WARN.
	  return false;
	}
    }

  // Calculate principal solution on the residuals.
  //// Send in new residuals
  for (Int order=0;order<mt_nterms_p;order++)
    {
      Matrix<Float> tempMat;
      Array<Float> tempArr;
      (mt_residuals[order])->get(tempArr,true); // by reference.
      tempMat.reference(tempArr);
      mt_cleaner_p.setresidual( order , tempMat ); 
    }           

  //// Compute p-soln
  mt_cleaner_p.computeprincipalsolution();

  //// Get residuals out
  for (Int order=0;order<mt_nterms_p;order++)
    {
      Matrix<Float> tempMat;
      Matrix<Float> tempMod;
      mt_cleaner_p.getresidual(order,tempMod);
      mt_residuals[order]->put(tempMod);
    }

  // Convolve models with the clean beam and add new residuals.... per term
  for (Int order=0;order<mt_nterms_p;order++)
    {
      PagedImage<Float> thismodel(models[order]);
      PagedImage<Float> thisimage( thismodel.shape(), thismodel.coordinates(), images[order]);

      LatticeExpr<Float> cop(thismodel);
      thisimage.copyData(cop);

      StokesImageUtil::Convolve(thisimage, mbeam);

      LatticeExpr<Float> le(thisimage+( *mt_residuals[order] )); 
      thisimage.copyData(le);

      ImageInfo ii = thisimage.imageInfo();
      ii.setRestoringBeam(mbeam);
      thisimage.setImageInfo(ii);
      thisimage.setUnits(Unit("Jy/beam"));
      thisimage.table().unmarkForDelete();

    }

  return true;
}

// This code duplicates WBCleanImageSkyModel::calculateAlphaBeta
// Eventually, This Deconvolver code will replace what's in WBCleanImageSkyModel.cc
//  This function must be callable stand-alone, and must not use private vars.
//      This is to allow easy recalculation of alpha with different thresholds
Bool Deconvolver::mtcalcpowerlaw(const Vector<String>& images,
				 const Vector<String>& residuals,
				 const String& alphaname,
				 const String& betaname,
				 const Quantity& threshold,
				 const Bool calcerror)
{
  LogIO os(LogOrigin("Deconvolver", "mtcalcpowerlaw()", WHERE));

  uInt ntaylor = images.nelements();

  if(ntaylor<2)
    {
      os << LogIO::SEVERE << "Please enter at-least two Taylor-coefficient images" << LogIO::POST;
      return false;
    }
  else
    {

      // Check that the images exist on disk

      os << "Calculating spectral index";
      if(ntaylor>3) os << " and spectral curvature";
      os << " from " << ntaylor << " restored images, using a mask defined by a threshold of " << threshold.getValue(String("Jy")) << " Jy " << LogIO::POST;
    }
  
  // Open restored images 
  PagedImage<Float> imtaylor0(images[0]);
  PagedImage<Float> imtaylor1(images[1]);

  // Check shapes
  if( imtaylor0.shape() != imtaylor1.shape() )
    {
      os << LogIO::SEVERE << "Taylor-coefficient image shapes must match." << LogIO::POST;
      return false;
    }

  // Create empty alpha image
  PagedImage<Float> imalpha(imtaylor0.shape(),imtaylor0.coordinates(),alphaname); 
  imalpha.set(0.0);

  Float specthreshold = threshold.getValue(String("Jy"));
  
  // Create a mask - make this adapt to the signal-to-noise 
  LatticeExpr<Float> mask1(iif((imtaylor0)>(specthreshold),1.0,0.0));
  LatticeExpr<Float> mask0(iif((imtaylor0)>(specthreshold),0.0,1.0));

  /////// Calculate alpha
  LatticeExpr<Float> alphacalc( ((imtaylor1)*mask1)/((imtaylor0)+(mask0)) );
  imalpha.copyData(alphacalc);

  // Set the restoring beam for alpha
  ImageInfo ii = imalpha.imageInfo();
  ii.setRestoringBeam( (imtaylor0.imageInfo()).restoringBeam() );
  imalpha.setImageInfo(ii);
  //imalpha.setUnits(Unit("Spectral Index"));
  imalpha.table().unmarkForDelete();

  // Make a mask for the alpha image
  LatticeExpr<Bool> lemask(iif((imtaylor0 > specthreshold) , true, false));

  createMask(lemask, imalpha);
  os << "Written Spectral Index Image : " << alphaname << LogIO::POST;


  ////// Calculate error on alpha, if requested.
  if(calcerror)
    {
      if( residuals.nelements() != ntaylor )
	{
	  os << LogIO::WARN << "Number of residual images is different from number of restored images. Not calculating alpha-error map." << LogIO::POST;
	}
      else
	{
	  PagedImage<Float> imalphaerror(imtaylor0.shape(),imtaylor0.coordinates(),alphaname+String(".error")); 
	  imalphaerror.set(0.0);
	  
	  /* Open residual images */
	  PagedImage<Float> residual0(residuals[0]);
	  PagedImage<Float> residual1(residuals[1]);

	  if( residual0.shape() != residual1.shape() || residual0.shape() != imtaylor0.shape() )
	    {
	      os << LogIO::WARN << "Shapes of residual images (and/or restored images) do not match. Not calculating alpha-error map." << LogIO::POST;
	    }
	  else
	    {
	      LatticeExpr<Float> alphacalcerror( abs(alphacalc) * sqrt( ( (residual0*mask1)/(imtaylor0+mask0) )*( (residual0*mask1)/(imtaylor0+mask0) ) + ( (residual1*mask1)/(imtaylor1+mask0) )*( (residual1*mask1)/(imtaylor1+mask0) )  ) );
	      imalphaerror.copyData(alphacalcerror);
	      imalphaerror.setImageInfo(ii);
	      createMask(lemask, imalphaerror);
	      imalphaerror.table().unmarkForDelete();      
	      os << "Written Spectral Index Error Image : " << alphaname << ".error" << LogIO::POST;
	      
	      //          mergeDataError( imalpha, imalphaerror, alphaerrorname+".new" );
	    }

	}

    }// end if calcerror

  ////// Calculate beta, if enough images are given.
  if(ntaylor>2)
    {
      PagedImage<Float> imbeta(imtaylor0.shape(),imtaylor0.coordinates(),betaname); 
      imbeta.set(0.0);
      PagedImage<Float> imtaylor2(images[2]);
      if( imtaylor2.shape() != imtaylor0.shape() )
	{
	  os << LogIO::WARN << "Restored image shapes do not match. Not calculating 'beta'" << LogIO::POST;
	}
      else
	{
	  LatticeExpr<Float> betacalc( ((imtaylor2)*mask1)/((imtaylor0)+(mask0))-0.5*(imalpha)*(imalpha-1.0) );
	  imbeta.copyData(betacalc);
	  imbeta.setImageInfo(ii);
	  //imbeta.setUnits(Unit("Spectral Curvature"));
	  createMask(lemask, imbeta);
	  imbeta.table().unmarkForDelete();
	  
	  os << "Written Spectral Curvature Image : " << betaname << LogIO::POST;
	}
    }
  
  return true;
}

// This is also a copy of WBCleanImageSkyModel::createMask
// Eventually, WBCleanImageSkyModel must use this Deconvolver version.
Bool Deconvolver::createMask(LatticeExpr<Bool> &lemask, ImageInterface<Float> &outimage)
{
      ImageRegion outreg = outimage.makeMask("mask0",false,true);
      LCRegion& outmask=outreg.asMask();
      outmask.copyData(lemask);
      outimage.defineRegion("mask0",outreg, RegionHandler::Masks, true);
      outimage.setDefaultMask("mask0");
      return true;
}


} //# NAMESPACE CASA - END
