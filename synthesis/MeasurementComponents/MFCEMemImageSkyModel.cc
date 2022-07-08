//# MFCEMemImageSkyModel.cc: Implementation of MFCEMemImageSkyModel class
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
#include <casacore/casa/Arrays/Matrix.h>
#include <synthesis/MeasurementComponents/MFCEMemImageSkyModel.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/casa/OS/File.h>
#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/lattices/Lattices/SubLattice.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeUtilities.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogIO.h>

#include <casacore/casa/System/Choice.h>

#include <msvis/MSVis/StokesVector.h>
#include <synthesis/MeasurementEquations/LatConvEquation.h>
#include <synthesis/MeasurementEquations/IncCEMemModel.h>
#include <synthesis/MeasurementEquations/CEMemProgress.h>

using namespace casacore;
namespace casa {

// Constructor

MFCEMemImageSkyModel::
MFCEMemImageSkyModel(Float sigma, 
		     Float targetFlux,
		     Bool constrainFlux,
		     const Vector<String>& priors,
		     const String& entropy) :
  CEMemImageSkyModel(sigma, targetFlux, constrainFlux, priors, entropy)
{

  // Hey, we also need some info which controls the change between major cycles!

}



// Clean solver
Bool MFCEMemImageSkyModel::solve(SkyEquation& se) {

  LogIO os(LogOrigin("MFCEMemImageSkyModel","solve"));
  
  //Make the PSFs, one per field
  makeApproxPSFs(se);
  
  // Validate PSFs for each field
  Vector<Float> psfmax(numberOfModels()); psfmax=0.0;
  Vector<Float> psfmaxouter(numberOfModels()); psfmaxouter=0.0;
  Vector<Float> psfmin(numberOfModels()); psfmin=1.0;
  Vector<Float> resmax(numberOfModels());
  Vector<Float> resmin(numberOfModels());
  
  PagedImage<Float>* priorImagePtr;
  priorImagePtr=0;
  if((itsPrior.nelements()>0)&&(itsPrior(0)!="")) {
    os << "Using prior " << itsPrior(0) << LogIO::POST;
    priorImagePtr=new PagedImage<Float>(itsPrior(0));
  }

  Float maxSidelobe=0.0;
  Int model;
  for (model=0;model<numberOfModels();model++) {
    if(isSolveable(model)) {

      IPosition blc(PSF(model).shape().nelements(), 0);
      IPosition trc(PSF(model).shape()-1);
      blc(2) = 0;  trc(2) = 0;
      blc(3) = 0;  trc(3) = 0;


      SubLattice<Float> subPSF;
      Int k =0;
      Int numchan= PSF(model).shape()(3);
      //PSF of first non zero plane
      while(psfmax(model) < 0.1 && k< numchan){
        blc(3)= k;
	trc(3)=k;
	LCBox onePlane(blc, trc, PSF(model).shape());
	subPSF=SubLattice<Float> ( PSF(model), onePlane, true);
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
      // 32 pixels:  pretty arbitrary, but only look for sidelobes
      // outside the inner (2n+1) * (2n+1) square
      psfmaxouter(model) = maxOuter(subPSF, 32);  
      
      os << "Model " << model+1 << ": max, min, maxOuter PSF = "
         << psfmax(model) << ", " << psfmin(model) << ", " <<
        psfmaxouter(model) << endl;
      if(abs(psfmin(model))>maxSidelobe) maxSidelobe=abs(psfmin(model));
      if(psfmaxouter(model) > maxSidelobe) maxSidelobe= psfmaxouter(model );
    }
  }
  os << LogIO::POST;
  
  Float absmax=threshold();
  Float cycleThreshold=0.0;
  Block< Matrix<Int> > iterations(numberOfModels());
  Int maxIterations=0;
   

  Int mpol=image(0).shape()(2);
  Int mchan=image(0).shape()(3);
 
  Block< Matrix<Float> > alpha(numberOfModels());           
  Block< Matrix<Float> > beta(numberOfModels());            
  Block< Matrix<Float> > Q(numberOfModels());               

  // Loop over major cycles
  Int cycle=0;
  Bool stop=false;
  Bool ask=true;
  Bool modified=false;

  if (displayProgress_p) {
    itsProgress = new CEMemProgress( pgplotter_p );
  }

  while(absmax>=threshold()&&maxIterations<numberIterations()&&!stop) {
    
    os << "*** Starting major cycle " << cycle+1 << LogIO::POST;
    cycle++;
    
    // Make the residual images. We do an incremental update
    // for cycles after the first one. If we have only one
    // model then we use convolutions to speed the processing
    Bool incremental(cycle>1);
    if (!incremental||(itsSubAlgorithm == "full")) {
      os << "Using visibility-subtraction for residual calculation"
	 << LogIO::POST;
      makeNewtonRaphsonStep(se, false);
    }
    else {
      os << "Using XFR-based shortcut for residual calculation"
	 << LogIO::POST;
      makeNewtonRaphsonStep(se, true);
    }
    os << "Finished update of residuals"
       << LogIO::POST;

    absmax=maxField(resmax, resmin);
    
    for (model=0;model<numberOfModels();model++) {
      os << "Model " << model+1 << ": max, min residuals = "
	 << resmax(model) << ", " << resmin(model) << endl;
    }
    os << LogIO::POST;
    
    // Can we stop?
    if(absmax<threshold()) {
      os << "Reached stopping peak residual = " << absmax << LogIO::POST;
      stop=true;
    }
    else {
      
      // Calculate the threshold for this cycle. Add a safety factor
      // This will be fixed someday by an option for an increasing threshold
      Float fudge = cycleFactor_p * maxSidelobe;
      if (fudge > 0.8) fudge = 0.8;   // painfully slow!

      cycleThreshold=max(threshold(), fudge * absmax);
      os << "Maximum residual = " << absmax << ", cleaning down to "
         << cycleThreshold << LogIO::POST;
      
      for (model=0;model<numberOfModels();model++) {
	
	Int npol=image(model).shape()(2);
	Int nchan=image(model).shape()(3);
	
        IPosition blcDirty(image(model).shape().nelements(), 0);
        IPosition trcDirty(image(model).shape()-1);

	if(cycle==1) {
	  iterations[model].resize(npol, nchan);       iterations[model]=0;
	  alpha[model].resize(mpol, mchan);            alpha[model] = 0.0; 
	  beta[model].resize(mpol, mchan);             beta[model] = 0.0;   
	  Q[model].resize(mpol, mchan);                Q[model] = 0.0;         
	}
	
	// Initialize delta image
        deltaImage(model).set(0.0);
	
	// Only process solveable models
	if(isSolveable(model)) {
	  
          if((abs(resmax(model))>cycleThreshold)||
	     (abs(resmin(model))>cycleThreshold)) {
	    
	    os << "Processing model " << model+1 << LogIO::POST;
	    
	    // If mask exists, use it;
            // If not, use the fluxScale image to figure out
            Bool doMask = false;
            Lattice<Float> *maskPointer = 0;
            if (hasMask(model) &&  mask(model).nelements() > 1 ) {
              doMask = true;
	      blcDirty(2)=0; trcDirty(2)=0;
	      blcDirty(3)=0; trcDirty(3)=0;
	      LCBox onePlane(blcDirty, trcDirty, mask(model).shape());
		  
	      maskPointer = new SubLattice<Float>( mask(model), onePlane, false);

            } else if (doFluxScale(model)) {
              doMask = true;
	      blcDirty(2)=0; trcDirty(2)=0;
	      blcDirty(3)=0; trcDirty(3)=0;
	      LCBox onePlane(blcDirty, trcDirty, fluxScale(model).shape());
              maskPointer = new SubLattice<Float> ( fluxScale(model), onePlane,
						    true);
              maskPointer->copyData( (LatticeExpr<Float>)
                                     (iif( (*maskPointer > 0.0), 1.0, 0.0) ));
            }

 	    // Now deconvolve each channel
            for (Int chan=0; chan<nchan; chan++) {
              if(psfmax(model)>0.0) {
                if(nchan>1) {
                  os<<"Processing channel "<<chan+1<<" of "<<nchan<<LogIO::POST;
                }

                blcDirty(3) = chan;
                trcDirty(3) = chan;	      
		for (Int pol=0; pol<npol; pol++) {
		  blcDirty(2) = pol; trcDirty(2) = pol;
		  if(npol>1) {
		    os<<"Processing polarization "<<pol+1<<" of "<<npol<<LogIO::POST;
		  }
 
		  LCBox onePlane(blcDirty, trcDirty, image(model).shape());
		  
		  SubLattice<Float> subImage( image(model), onePlane, true);
		  SubLattice<Float> subResid( residual(model), onePlane);
		  SubLattice<Float> subPSF( PSF(model), onePlane);
		  SubLattice<Float> subDeltaImage( deltaImage(model), onePlane, true);
		  
		  // Now make a convolution equation for this
		  // residual image and psf and then deconvolve it
		  LatConvEquation eqn(subPSF, subResid);
		  
		  // Make entropy
		  IncEntropy * myEnt_p = 0;
		  String entString = entropy();
		  if (entString=="mfemptiness") {
		    if (cycle == 1) {
		      os << "Deconvolving with incremental maximum emptiness algorithm" << LogIO::POST;
		    }
		    myEnt_p = new IncEntropyEmptiness;
		  } else if( entString=="mfentropy") {
		    if (cycle == 1) {
		      os << "Deconvolving with incremental maximum entropy algorithm" << LogIO::POST;
		    }
		    myEnt_p = new IncEntropyI;
		  }
		  else {
		    os << " Known MEM entropies: entropy | emptiness " << LogIO::POST;
		    os << LogIO::SEVERE << "Unknown MEM entropy: " << entString << LogIO::POST;
		    return false;
		  }
		  
		  subDeltaImage.set(0.0);
		  IncCEMemModel *memer;
                  if(priorImagePtr) {
		    if(doMask) {
		      memer= new IncCEMemModel(*myEnt_p, subImage,
					       subDeltaImage,
					       *priorImagePtr, *maskPointer,
					       numberIterations(), itsSigma,
					       abs(itsTargetFlux), false,
					       true,  false );
		    }
		    else {
		      memer=new IncCEMemModel(*myEnt_p, subImage,
					      subDeltaImage,
					      *priorImagePtr, 
					      numberIterations(), itsSigma,
					      abs(itsTargetFlux), false,
					      true,  false );
		    }
		  }
		  else {
		    if(doMask) {
		      memer= new IncCEMemModel(*myEnt_p, subImage,
					       subDeltaImage,
					       numberIterations(),
					       *maskPointer,
					       itsSigma,
					       abs(itsTargetFlux), false,
					       true,  false );
		    }
		    else {
		      memer=new IncCEMemModel(*myEnt_p, subImage,
					      subDeltaImage,
					      numberIterations(), itsSigma,
					      abs(itsTargetFlux), false,
					      true,  false );
		    }
		  }
		  if (displayProgress_p) {
		    memer->setProgress(*itsProgress);
		  }
		  if (alpha[model](pol, chan) != 0.0) {
		    memer->setAlpha(alpha[model](pol, chan));
		    memer->setBeta(beta[model](pol, chan));	
		    memer->setQ(Q[model](pol, chan));	
		  }
		  memer->setGain(Iterate::gain());
		  memer->setThreshold( cycleThreshold );
		  memer->setThresholdSpeedup( cycleSpeedup_p );		  
		  memer->setInitialNumberIterations(iterations[model](pol, chan));
		  
		  memer->solve(eqn);

		  alpha[model](pol, chan) = memer->getAlpha();
		  beta[model](pol, chan) = memer->getBeta();
		  Q[model](pol, chan) = memer->getQ();
		  
		  iterations[model](pol, chan)=memer->numberIterations();
		  maxIterations=(iterations[model](pol, chan)>maxIterations) ?
		    iterations[model](pol, chan) : maxIterations;
		  modified=true;
		  
		  if(memer) delete memer; memer=0;

		  subImage.copyData((LatticeExpr<Float>)
				    (subImage + subDeltaImage ) );
		
		  os << "MEM used " << iterations[model](pol, chan) << " iterations" 
		     << " to approach a threshold of " << cycleThreshold
		     << LogIO::POST;
		}
	      }
	    }
	    if (maskPointer) delete maskPointer;
	  } else {
	    os<<"Skipping model "<<model+1<<" :peak residual below threshold"
	      <<LogIO::POST;
	  }
	}
      }
      // For now if it has converged withing 20% of maxiter we'll stop

      if((maxIterations+Int(numberIterations()/20)) >=numberIterations())
	 stop=true;
      //===
      if(maxIterations<numberIterations()&&ask) {
	Vector<String> choices(3);
	choices(0)="Continue";
	choices(1)="Stop Now";
	choices(2)="Don't ask again";
	String choice =
	  Choice::choice("MEM iteration: do you want to continue or stop?",
					 choices);
	if (choice==choices(1)) {
	  os << "Multi-field MEM stopped at user request" << LogIO::POST;
	  stop=true;
	}
	else if (choice==choices(2)) {
	  os << "Continuing: won't ask again" << LogIO::POST;
	  ask=false;
	}
	
      }
      if(!modified) {
	os << "Nothing happened: stopping" << LogIO::POST;
	stop=true;
      }
    }
  }
  if (itsProgress) {
    delete itsProgress;
  }


  if(modified) {
    os << "Finalizing residual images for all fields" << LogIO::POST;
    makeNewtonRaphsonStep(se, false);
    Float finalabsmax=maxField(resmax, resmin);
    
    os << "Final maximum residual = " << finalabsmax << LogIO::POST;
    
    for (model=0;model<numberOfModels();model++) {
      os << "Model " << model+1 << ": max, min residuals = "
	 << resmax(model) << ", " << resmin(model) << endl;
    }
  }
  else {
    os << "Residual images for all fields are up-to-date" << LogIO::POST;
  }

  os << LogIO::POST;

  return(true);
};
  
  
// Find maximum residual
Float MFCEMemImageSkyModel::maxField(Vector<Float>& imagemax,
				     Vector<Float>& imagemin) {

  LogIO os(LogOrigin("ImageSkyModel","maxField"));
  
  Float absmax=0.0;
  imagemax=-1e20;
  imagemin=1e20;

  // Loop over all models
  for (Int model=0;model<numberOfModels();model++) {

    // Remember that the residual image can be either as specified
    // or created specially.
    ImageInterface<Float>* imagePtr=0;
    if(residual_p[model]) {
      imagePtr=residual_p[model];
    }
    else {
      imagePtr=(ImageInterface<Float> *)residualImage_p[model];
    }
    AlwaysAssert(imagePtr, AipsError);
    AlwaysAssert(imagePtr->shape().nelements()==4, AipsError);
    Int nx=imagePtr->shape()(0);
    Int ny=imagePtr->shape()(1);
    Int npol=imagePtr->shape()(2);
    
    AlwaysAssert((npol==1)||(npol==2)||(npol==4), AipsError);
    
    // Loop over all channels
    IPosition onePlane(4, nx, ny, 1, 1);
    LatticeStepper ls(imagePtr->shape(), onePlane, IPosition(4, 0, 1, 2, 3));
    RO_LatticeIterator<Float> imageli(*imagePtr, ls);
    
    // If we are using a mask then reset the region to be
    // deconvolved
    Array<Float> maskArray;
    
    if(hasMask(model)) {
      LatticeStepper mls(mask(model).shape(), onePlane,
			 IPosition(4, 0, 1, 2, 3));
      
      RO_LatticeIterator<Float> maskli(mask(model), mls);
      maskli.reset();
      if (maskli.cursor().shape().nelements() > 1) maskArray=maskli.cursor();
    }
    
    Int chan=0;
    Float imax, imin;
    imax=-1E20; imagemax(model)=imax;
    imin=+1E20; imagemin(model)=imin;
    imageli.reset();

    for (imageli.reset();!imageli.atEnd();imageli++,chan++) {
      
      IPosition imageposmax(imageli.cursor().ndim());
      IPosition imageposmin(imageli.cursor().ndim());
      
      // If we are using a mask then multiply by it
      if (hasMask(model)) {
	Array<Float> limage=imageli.cursor();
	limage*=maskArray;
	minMax(imin, imax, imageposmin, imageposmax, limage);
      }
      else {
	minMax(imin, imax, imageposmin, imageposmax, imageli.cursor());
      }
      if(abs(imax)>absmax) absmax=abs(imax);
      if(abs(imin)>absmax) absmax=abs(imin);
      if(imin<imagemin(model)) imagemin(model)=imin;
      if(imax>imagemax(model)) imagemax(model)=imax;
    }
  }
  return absmax;
};
    


// Find maximum residual
Float MFCEMemImageSkyModel::maxOuter(Lattice<Float> & lat, const uInt nCenter ) 
{
  Float myMax=0.0;
  Float myMin=0.0;

  /*
  TempLattice<Float>  mask(lat.shape());
  mask.set(1.0);

  IPosition pos(4,0,0,0,0 );
  uInt nxc = lat.shape()(0)/2;
  uInt nyc = lat.shape()(1)/2;
  for (uInt ix = -nCenter; ix < nCenter; ix++) {
    for (uInt iy = -nCenter; iy < nCenter; iy++) {
      pos(0) = nxc + ix;
      pos(1) = nyc + iy;
      mask.putAt( 0.0f, pos );       //   mask out the inner section
    }
  }
  {
    LatticeExpr<Float> exp = (lat * mask);
    LatticeExprNode LEN = max( exp );
    myMax = LEN.getFloat();
  }
  {
    LatticeExpr<Float> exp = (lat * mask);
    LatticeExprNode LEN = min( exp );
    myMin = LEN.getFloat();
  }

  */

  Array<Float> arr = lat.get();
  IPosition pos( arr.shape() );
  uInt nx = lat.shape()(0);
  uInt ny = lat.shape()(1);
  uInt nxc = 0;
  uInt nyc = 0;
  Float amax = 0.0;
  for (uInt ix = 0; ix < nx; ix++) {
    for (uInt iy = 0; iy < ny; iy++) {
      if (arr(IPosition(4, ix, iy, 0, 0)) > amax) {
        nxc = ix;
        nyc = iy;
        amax = arr(IPosition(4, ix, iy, 0, 0));
      }
    }
  }

  uInt nxL = nxc - nCenter;
  uInt nxH = nxc + nCenter;
  uInt nyL = nyc - nCenter;
  uInt nyH = nyc + nCenter;
    
  for (uInt ix = 0; ix < nx; ix++) {
    for (uInt iy = 0; iy < ny; iy++) {
      if ( !(ix >= nxL && ix <= nxH &&  iy >= nyL && iy <= nyH) ) {
        if (arr(IPosition(4, ix, iy, 0, 0)) > myMax) 
          myMax = arr(IPosition(4, ix, iy, 0, 0));
        if (arr(IPosition(4, ix, iy, 0, 0)) < myMin)
          myMin = arr(IPosition(4, ix, iy, 0, 0));
      }
    }
  }

  Float absMax = max( abs(myMin), myMax );
  return absMax;
};

} //#End casa namespace
