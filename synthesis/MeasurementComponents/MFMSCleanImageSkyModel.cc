//# MFMSCleanImageSkyModel.cc: Implementation of MFMSCleanImageSkyModel class
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

#include <casa/Arrays/ArrayMath.h>
#include <synthesis/MeasurementComponents/MFMSCleanImageSkyModel.h>
#include <images/Images/PagedImage.h>
#include <casa/OS/File.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LEL/LatticeExprNode.h>
#include <images/Images/SubImage.h>
#include <casa/Arrays/IPosition.h>
#include <lattices/LRegions/LCBox.h>
#include <synthesis/MeasurementEquations/ImageMSCleaner.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Quanta/Quantum.h>

#include <casa/sstream.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogIO.h>

#include <synthesis/MeasurementEquations/LatConvEquation.h>

using namespace casacore;
namespace casa {


// Some constructors

MFMSCleanImageSkyModel::MFMSCleanImageSkyModel()
:  method_p(NSCALES), nscales_p(4), progress_p(0), 
   stopLargeNegatives_p(2), stopPointMode_p(-1)
{

  donePSF_p=false;
  modified_p=true;
  getScales();

};

MFMSCleanImageSkyModel::MFMSCleanImageSkyModel(const Int nscales, 
					       const Int sln,
					       const Int spm,
                                               const Float inbias)
: method_p(NSCALES), nscales_p(nscales), progress_p(0),
  stopLargeNegatives_p(sln), stopPointMode_p(spm),smallScaleBias_p(inbias)
{

  donePSF_p=false;
  modified_p=true;
  getScales();

};

MFMSCleanImageSkyModel::MFMSCleanImageSkyModel(const Vector<Float>& userScaleSizes, 
					       const Int sln,
					       const Int spm,
                                               const Float inbias)
: method_p(USERVECTOR), userScaleSizes_p(userScaleSizes), progress_p(0),
  stopLargeNegatives_p(sln), stopPointMode_p(spm), smallScaleBias_p(inbias)
{

  donePSF_p=false;
  modified_p=true;
  getScales();

};

MFMSCleanImageSkyModel::~MFMSCleanImageSkyModel()
{
  if (progress_p) {
    delete progress_p;
  }

  if(componentList_p) delete componentList_p; componentList_p=0;
  for (Int thismodel=0;thismodel<nmodels_p;thismodel++) {
    if(residualImage_p[thismodel]) delete residualImage_p[thismodel]; 
    residualImage_p[thismodel]=0;
    if(cimage_p[thismodel]) delete cimage_p[thismodel]; 
    cimage_p[thismodel]=0;
    for(Int numXFR=0;numXFR<maxNumXFR_p;numXFR++) {
      if(cxfr_p[thismodel*maxNumXFR_p+numXFR])
        delete cxfr_p[thismodel*maxNumXFR_p+numXFR];
      cxfr_p[thismodel*maxNumXFR_p+numXFR]=0;
    }
    if(gS_p[thismodel]) delete gS_p[thismodel]; gS_p[thismodel]=0;
    if(psf_p[thismodel]) delete psf_p[thismodel]; psf_p[thismodel]=0;
    if(ggS_p[thismodel]) delete ggS_p[thismodel]; ggS_p[thismodel]=0;
    if(fluxScale_p[thismodel]) delete fluxScale_p[thismodel]; 
    fluxScale_p[thismodel]=0;
    if(work_p[thismodel]) delete work_p[thismodel]; work_p[thismodel]=0;
    if(deltaimage_p[thismodel]) delete deltaimage_p[thismodel]; 
    deltaimage_p[thismodel]=0;
    if(weight_p[thismodel]) delete weight_p[thismodel]; weight_p[thismodel]=0;
    if(beam_p[thismodel]) delete beam_p[thismodel]; beam_p[thismodel]=0;
  }

};


// Clean solver
Bool MFMSCleanImageSkyModel::solve(SkyEquation& se) {

  LogIO os(LogOrigin("MFMSCleanImageSkyModel","solve"));
  /*backed out for now
  if(modified_p){
    makeNewtonRaphsonStep(se, false);
  }
  
  if(numberIterations() < 1){
    return true;
  }
  */
  //Make the PSFs, one per field
  if(!donePSF_p){
    os << LogIO::NORMAL         // Loglevel PROGRESS
       << "Making approximate PSFs" << LogIO::POST;
    makeApproxPSFs(se);
  }

  Int converged=true;
  Int converging=0;
  
  // Validate PSFs for each field
  Vector<Float> psfmax(numberOfModels()); psfmax=0.0;
  Vector<Float> psfmin(numberOfModels()); psfmin=1.0;
  Vector<Float> resmax(numberOfModels());
  Vector<Float> resmin(numberOfModels());

  Float maxSidelobe=0.0;
  Int model;
  for (model=0;model<numberOfModels();model++) {
    if(isSolveable(model)) {
      IPosition blc(PSF(model).shape().nelements(), 0);
      IPosition trc(PSF(model).shape()-1);
      blc(2) = 0;  trc(2) = 0;
      blc(3) = 0;  trc(3) = 0;

      SubImage<Float> subPSF;
      Int k =0;
      Int numchan= PSF(model).shape()(3);
      //PSF of first non zero plane
      while(psfmax(model) < 0.1 && k< numchan){
        blc(3)= k;
	trc(3)=k;
	LCBox onePlane(blc, trc, PSF(model).shape());

	subPSF=SubImage<Float> ( PSF(model), onePlane, true);
	{
	  LatticeExprNode node = max(subPSF);
	  psfmax(model) = node.getFloat();
	}
	++k;
      }
      {
	LatticeExprNode node = min(subPSF);
	psfmin(model) = node.getFloat();
      }
      os << LogIO::NORMAL    // Loglevel INFO
         << "Model " << model+1 << ": max, min PSF = "
	 << psfmax(model) << ", " << psfmin(model) << endl;
      if(abs(psfmin(model))>maxSidelobe) maxSidelobe=abs(psfmin(model));
    }
  }
  os << LogIO::POST;
	
  Float absmax=threshold();
  Block< Matrix<Int> > iterations(numberOfModels());
  Int maxIterations=0;
  Int oldMaxIterations=-1;
    
  // Loop over major cycles
  /*if (displayProgress_p) {
    if(progress_p) delete progress_p;
    progress_p=0;
    progress_p = new LatticeCleanProgress( pgplotter_p );
    }*/

  Block<CountedPtr<ImageMSCleaner > > cleaner(numberOfModels());
  cleaner=0;

  Int cycle=0;
  Bool stop=false;
  Bool lastCycleWriteModel=false;

  while(absmax>=threshold()&&maxIterations<numberIterations()&&!stop) {

    os << LogIO::NORMAL    // Loglevel PROGRESS
       << "*** Starting major cycle " << cycle+1 << LogIO::POST;
    cycle++;


    // Make the residual images. We do an incremental update
    // for cycles after the first one. If we have only one
    // model then we use convolutions to speed the processing
    Bool incremental(cycle>1);
    if(modified_p){
      if (!incremental||(itsSubAlgorithm == "full")) {
	
	os << LogIO::NORMAL1    // Loglevel INFO
           << "Using visibility-subtraction for residual calculation"
	   << LogIO::POST;
	makeNewtonRaphsonStep(se, false, (numberIterations()<1)?true:False);
	
      }
      else {
	os << LogIO::NORMAL1    // Loglevel INFO
           << "Using XFR-based shortcut for residual calculation"
	   << LogIO::POST;
	makeNewtonRaphsonStep(se, true);
      }
    
    }
    if(numberIterations() < 1){
      // Why waste the time to set up
      return true;
    }

    absmax=maxField(resmax, resmin);

    for (model=0;model<numberOfModels();model++) {
      os << LogIO::NORMAL    // Loglevel INFO
         << "Model " << model+1 << ": max, min residuals (over all pixels) = "
	 << resmax(model) << ", " << resmin(model) << endl;
    }
    os << LogIO::POST;

    // Can we stop?
    if(absmax<threshold()) {
      os << LogIO::NORMAL    // Loglevel PROGRESS?
         << "Reached stopping peak point source residual = "
	 << absmax << LogIO::POST;
      stop=true;
      if(cycle >1)
	lastCycleWriteModel=true;
    }
    else {
      // Calculate the threshold for this cycle. Add a safety factor
      Float fudge = cycleFactor_p*maxSidelobe;
      if (fudge > 0.8) fudge = 0.8;   // painfully slow!
      Quantity fThreshold(fudge*100, "%");
      Quantity aThreshold(threshold(), "Jy");
      os << LogIO::NORMAL    // Loglevel INFO
         << "Maximum point source residual = " << absmax << LogIO::POST;
      os << LogIO::NORMAL    // Loglevel INFO
         << "Cleaning scale flux down to maximum of " << fThreshold.getValue("%")
	 << " % and " << aThreshold.getValue("Jy") << " Jy" << LogIO::POST;
      
      for (model=0;model<numberOfModels();model++) {

	Int nchan=image(model).shape()(3);
	Int npol=image(model).shape()(2);
 
	IPosition blcDirty(image(model).shape().nelements(), 0);
	IPosition trcDirty(image(model).shape()-1);
  
	if(cycle==1) {
	  iterations[model].resize(nchan,npol);
	  iterations[model]=0;
	}
	
	// Initialize delta image
	deltaImage(model).set(0.0);
	  
	// Only process solveable models
	if(isSolveable(model)) {
	  
	  os << LogIO::NORMAL    // Loglevel PROGRESS
             << "Processing model " << model+1 << LogIO::POST;
	  
	  // If mask exists, use it;
	  // If not, use the fluxScale image to figure out
	  Bool doMask = false;
	  Bool mustDeleteMask = false;
	  ImageInterface<Float> *maskPointer = 0;
	  
	  if (hasMask(model)) {
	    doMask = true;
	    maskPointer = &mask(model);
	  } else if (doFluxScale(model)) {
	    doMask = true;
	    mustDeleteMask = true;
	    
	    maskPointer = new TempImage<Float> ( fluxScale(model).shape(),
						 fluxScale(model).coordinates());
	    maskPointer->copyData( (LatticeExpr<Float>)
				   (iif( (fluxScale(model) > 0.0), 1.0, 0.0)) );
	  }
	  
	  // Now clean each channel and each pol
	  for (Int chan=0; chan<nchan; chan++) {
	    if(psfmax(model)>0.0) {
	      // We could keep a cleaner per channel but for the moment
	      // we simply make a new one for each channel
	      if(nchan>1) {
		os << LogIO::NORMAL    // Loglevel PROGRESS
                   <<"Processing channel "<<chan<<" of 0 to "<<nchan-1<<LogIO::POST;
		if(!cleaner[model].null()) cleaner[model]=0;
	      }
	      
	      blcDirty(3) = chan;
	      trcDirty(3) = chan;
	      blcDirty(2) = 0; trcDirty(2) = 0;
	      LCBox firstPlane(blcDirty, trcDirty, image(model).shape());
              SubImage<Float> subPSF( PSF(model), firstPlane);
	      
	      for (Int pol=0; pol<npol; pol++) {
		blcDirty(2) = pol; trcDirty(2) = pol;
		// The PSF should be the same for each polarization so we
		// can use the existing cleaner (unlike the spectral case)
		if(npol>1) {
		  os << LogIO::NORMAL    // Loglevel PROGRESS
                     <<"Processing polarization "<<pol+1<<" of "<< npol
                     <<LogIO::POST;
		}
		LCBox onePlane(blcDirty, trcDirty, image(model).shape());
		
		SubImage<Float> subImage( image(model), onePlane, true);
		SubImage<Float> subResid( residual(model), onePlane);
		SubImage<Float> subDeltaImage( deltaImage(model), onePlane, true);
		SubImage<Float>  subMask;
		Bool skipThisPlane=false;
		if (doMask) {
		  subMask = SubImage<Float> ( *maskPointer, onePlane, true);
		  if(max(subMask).getFloat() <= 0.0){
		    skipThisPlane=true;
		  }
		}
		if(!skipThisPlane){
		  if(!cleaner[model].null()) {
		    os << LogIO::NORMAL2    // Loglevel PROGRESS
                       << "Updating multiscale cleaner with new residual images"
		       << LogIO::POST;
		    cleaner[model]->update(subResid);
		  }
		  else {
		    os << LogIO::NORMAL2    // Loglevel PROGRESS
                       << "Creating multiscale cleaner with psf and residual images"
                       << LogIO::POST;
		    cleaner[model]=new ImageMSCleaner(subPSF, subResid);
		    //setScales(*(cleaner[model]));
		    cleaner[model]->setscales(userScaleSizes_p);
                    cleaner[model]->setSmallScaleBias(smallScaleBias_p);
		    if (doMask) {		  
		      cleaner[model]->setMask(subMask);
		    }
		  }
		  subDeltaImage.set(0.0);
		  
		  cleaner[model]->setcontrol(CleanEnums::MULTISCALE, numberIterations(), gain(), 
				      aThreshold, fThreshold);
		  
		  if (cycleSpeedup_p > 1) {
		    os << LogIO::NORMAL    // Loglevel INFO
                       << "cycleSpeedup is " << cycleSpeedup_p << LogIO::POST;
		    cleaner[model]->speedup(cycleSpeedup_p);
		  }
		  cleaner[model]->startingIteration( iterations[model](chan,pol) );
		  if (cycle <= stopLargeNegatives_p) {
		    cleaner[model]->stopAtLargeScaleNegative();
		  }
		  cleaner[model]->stopPointMode(stopPointMode_p);
		  cleaner[model]->ignoreCenterBox(true);
		  converging=cleaner[model]->clean(subDeltaImage, "fullmsclean", numberIterations(), gain(), aThreshold, fThreshold, displayProgress_p );
		  //diverging
		  if(converging==-3)
		    stop=true;
		  //reduce scales on main field only
		  if(converging==-2 && model==0){
		    if(userScaleSizes_p.nelements() > 1){
		       userScaleSizes_p.resize(userScaleSizes_p.nelements()-1, true);
		       cleaner[model]->setscales(userScaleSizes_p); 
		    }
		    else
		      stop=true;
		  }
		  iterations[model](chan,pol)=cleaner[model]->numberIterations();
		  maxIterations=(iterations[model](chan,pol)>maxIterations) ?
		    iterations[model](chan,pol) : maxIterations;
		  os << LogIO::NORMAL    // Loglevel INFO
                     << "Clean used " << iterations[model](chan,pol)
                     << " iterations" 
		     << LogIO::POST;
		  modified_p=true;
		  
		  subImage.copyData( LatticeExpr<Float>( subImage + subDeltaImage));
		 
		  if (cleaner[model]->queryStopPointMode()) {
		    stop = true;		
		    os << LogIO::NORMAL    // Loglevel INFO
                       << "MSClean terminating because we hit " 
		       << stopPointMode_p
		       << " consecutive compact sources" << LogIO::POST;
		  }
		  //if (doMask) {		  
		  //  delete subMaskPointer;
		  //}
		}
	      }
	    }
	  }
	  if (mustDeleteMask) {
	    delete maskPointer;
	  }
	} // if solveable
      } // for model
      if(maxIterations==oldMaxIterations){
	 os << LogIO::NORMAL    // Loglevel INFO
                       << "MSClean terminating because components search stopped " 
		       << LogIO::POST;
	stop=true;
      }
      else{
	oldMaxIterations=maxIterations; 
      }
    }
  }

  if(modified_p || lastCycleWriteModel) {
    os << LogIO::NORMAL    // Loglevel PROGRESS?
       << "Finalizing residual images for all fields" << LogIO::POST;
    makeNewtonRaphsonStep(se, false, true);
    Float finalabsmax=maxField(resmax, resmin);
    setNumberIterations(numberIterations()-maxIterations);
    converged=(finalabsmax < 1.05 * threshold());
    setThreshold(finalabsmax);
      os << LogIO::NORMAL    // Loglevel INFO
         << "Final maximum residual = " << finalabsmax << LogIO::POST;
      
    for (model=0;model<numberOfModels();model++) {
      os << LogIO::NORMAL    // Loglevel INFO
         << "Model " << model+1 << ": max, min residuals (over all pixels) = "
	 << resmax(model) << ", " << resmin(model) << endl;
    }
    os << LogIO::POST;
  }
  else {
    os << "Residual images for all fields are up-to-date" << LogIO::POST;
  }

  //if(cleaner) delete cleaner; cleaner=0;

  return(converged);
};
  

void
MFMSCleanImageSkyModel::getScales()
{
  LogIO os(LogOrigin("MFCleanImageSkyModel", "getScales"));
  if (method_p == USERVECTOR) {
    if (userScaleSizes_p.nelements() <= 0) {
      os << LogIO::SEVERE 
	 << "Need at least one scale for method uservector"
	 << LogIO::POST;
    }
    os << "Creating scales from uservector method: " << LogIO::POST;
  }
  else {
    if (nscales_p <= 0) nscales_p = 1;
    Vector<Float> scaleSizes(nscales_p);  
    os << "Creating " << nscales_p << 
      " scales from powerlaw nscales method" << LogIO::POST;
    scaleSizes(0) = 0.0;
    Float scaleInc = 2.0;
    for(uInt scale = 1; scale < nscales_p; ++scale)
      scaleSizes[scale] = scaleInc * pow(10.0, (Float(scale) - 2.0)/2.0);
  
    //store the scales as user setscales..in case we need to reduce scales
    userScaleSizes_p.resize();
    userScaleSizes_p=scaleSizes;
    method_p=USERVECTOR;
  }

  for(uInt scale = 0; scale < userScaleSizes_p.nelements(); ++scale)
    os << LogIO::NORMAL << 
      "scale " << scale+1 << " = " << userScaleSizes_p(scale)
       << " pixels"
       << LogIO::POST;
};

 /*
// Inlined here and not in the .h because the .h doesn't #include LatticeCleaner.
inline void MFMSCleanImageSkyModel::setScales(LatticeCleaner<Float>& cleaner)
{ 
  cleaner.setscales(userScaleSizes_p);
}

*/
} //#End casa namespace
