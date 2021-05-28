//# MFCleanImageSkyModel.cc: Implementation of MFCleanImageSkyModel class
//# Copyrighogt (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/Matrix.h>
#include <synthesis/MeasurementComponents/MFCleanImageSkyModel.h>
#include <images/Images/PagedImage.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <images/Regions/WCBox.h>
#include <casa/OS/File.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LEL/LatticeExprNode.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/LRegions/RegionType.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/LRegions/LCBox.h>

#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/CountedPtr.h>

#include <casa/sstream.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogIO.h>
#include <casa/BasicSL/Constants.h>

#include <msvis/MSVis/StokesVector.h>
#include <synthesis/MeasurementEquations/ConvolutionEquation.h>
#include <synthesis/MeasurementEquations/ClarkCleanModel.h>



using namespace casacore;
namespace casa {

#define NEED_UNDERSCORES
#if defined(NEED_UNDERSCORES)
#define hclean hclean_
#endif
extern "C" {
  void hclean(Float*, Float*, Float*, int*, Float*, int*, int*, int*,
              int*, int*, int*, int*, int*, int*, int*, Float*, Float*,
              Float*, void *, void *);
};


// This is defined in HogbomCleanImageSkyModel.cc
void HogbomCleanImageSkyModelstopnow (Int *yes);

// This is defined in HogbomCleanImageSkyModel.cc
void HogbomCleanImageSkyModelmsgput(Int* npol, Int* pol, Int* iter, Int* px, Int* py, Float* fMaxVal);


Int MFCleanImageSkyModel::add(ImageInterface<Float>& image,
			      const Int maxNumXfr)
{
  return CleanImageSkyModel::add(image, maxNumXfr);
};

Bool MFCleanImageSkyModel::addMask(Int image,
				   ImageInterface<Float>& mask)
{
  return CleanImageSkyModel::addMask(image, mask);
};

Bool MFCleanImageSkyModel::addResidual(Int image,
				       ImageInterface<Float>& residual) 
{
  return ImageSkyModel::addResidual(image, residual);
};

// Clean solver
Bool MFCleanImageSkyModel::solve(SkyEquation& se) {


  LogIO os(LogOrigin("MFCleanImageSkyModel","solve"));
  Bool converged=true;
  //Make the PSFs, one per field
  /*back out for now
  if(modified_p){ 
      blankOverlappingModels();
      makeNewtonRaphsonStep(se, false);
  }
  if(numberIterations() < 1){
      // Why waste the time to set up
      return true;
  }
  */

  if(!donePSF_p) {
    makeApproxPSFs(se);
  }

  // Validate PSFs for each field
  Vector<Float> psfmax(numberOfModels()); psfmax=0.0;
  Vector<Float> psfmaxouter(numberOfModels()); psfmaxouter=0.0;
  Vector<Float> psfmin(numberOfModels()); psfmin=1.0;
  Vector<Float> resmax(numberOfModels());
  Vector<Float> resmin(numberOfModels());
  Int psfpatch=51;
  Float maxSidelobe=0.0;
  Int model;
  os << LogIO::NORMAL1;   // Loglevel INFO
  for (model=0;model<numberOfModels();model++) {
    if(isSolveable(model)) {
      
      IPosition blc(PSF(model).shape().nelements(), 0);
      IPosition trc(PSF(model).shape()-1);
      blc(2) = 0;  trc(2) = 0;
      blc(3) = 0;  trc(3) = 0;

      Float maxMaxPSF=max(PSF(model)).getFloat();
      SubLattice<Float> subPSF;
      Int k =0;
      Int numchan= PSF(model).shape()(3);
      //PSF of first non zero plane
      while(psfmax(model) < (0.8*maxMaxPSF) && k< numchan){
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
      // 4 pixels:  pretty arbitrary, but only look for sidelobes
      // outside the inner (2n+1) * (2n+1) square
      Int ncent=4;
      {//locate peak size
	CoordinateSystem cs=PSF(model).coordinates();
	Vector<String> unitas=cs.worldAxisUnits();
	unitas(0)="arcsec";
	unitas(1)="arcsec";
	cs.setWorldAxisUnits(unitas);
	//Get the minimum increment in arcsec
	Double incr=abs(min(cs.increment()(0), cs.increment()(1)));
	if(incr > 0.0){
	  GaussianBeam beamModel=beam(model)(0,0);
	  ncent=max(ncent, Int(ceil(beamModel.getMajor("arcsec")/incr)));
	  ncent=max(ncent, Int(ceil(beamModel.getMinor("arcsec")/incr)));
	}
      }
      psfpatch=3*ncent+1;
   
      psfmaxouter(model) = maxOuter(subPSF, ncent);  
      os << "Model " << model << ": max, min, maxOuter PSF = "
	 << psfmax(model) << ", " << psfmin(model) << ", " <<
	psfmaxouter(model) << endl;
      if(abs(psfmin(model))>maxSidelobe) maxSidelobe=abs(psfmin(model));
      if(psfmaxouter(model) > maxSidelobe) maxSidelobe= psfmaxouter(model );
      //      os << "     : PSFPatch = " << psfpatch << LogIO::POST;
    }
  }
  os << LogIO::POST;
	
  Float absmax=threshold();
  Float oldabsmax=C::flt_max;
  Float cycleThreshold=0.0;
  Block< Vector<Int> > iterations(numberOfModels());
  Int maxIterations=0;
  Int oldMaxIterations=0;
    
  // Loop over major cycles
  Int cycle=0;
  Bool stop=false;
  Bool diverging=false;

  if (displayProgress_p) {
    progress_p = new ClarkCleanProgress( pgplotter_p );
  } else {
    progress_p = 0;
  }
  
  //Making the mask matrices
  Int nmodels=numberOfModels();
  Block< Vector<Int> > xbeg(nmodels), ybeg(nmodels), xend(nmodels), 
                       yend(nmodels);
  Vector<Bool> isCubeMask(nmodels);
  isCubeMask.set(false);
  Int nx, ny;
  Int npol=image(0).shape()(2);
  Int nchan=image(0).shape()(3);
  //Int polloop=1;
  Vector<String> stokesID(npol, "");
  if (!doPolJoint_p) {
    //polloop=npol;
    CoordinateSystem cs=image(0).coordinates();
    Int stokesindex=cs.findCoordinate(Coordinate::STOKES);
    StokesCoordinate stcoord=cs.stokesCoordinate(stokesindex);
    for (Int jj=0; jj < npol ; ++jj){
      stokesID(jj)=Stokes::name(Stokes::type(stcoord.stokes()(jj)));      
    }
  }

  //merge the overlapping part of the masks if any
  mergeOverlappingMasks();
  

  PtrBlock<Matrix<Float> *>  lmaskCube(nmodels*nchan);

  for (model=0;model<numberOfModels();model++) {
    
    nx=image(model).shape()(0);
    ny=image(model).shape()(1);
    npol=image(model).shape()(2);
    nchan=image(model).shape()(3);
    xbeg[model].resize(nchan);
    xend[model].resize(nchan);
    ybeg[model].resize(nchan);
    yend[model].resize(nchan);
    for (Int chan=0; chan < nchan; ++chan){
      xbeg[model][chan]=0;
      ybeg[model][chan]=0;
      xend[model][chan]=nx-1;
      yend[model][chan]=ny-1;
      lmaskCube[chan*nmodels+model]=0;
    }
    if(hasMask(model)){
      Int mx=mask(model).shape()(0);
      Int my=mask(model).shape()(1);
      Int mpol=mask(model).shape()(2);
      if((mx != nx) || (my != ny) || (mpol != npol)){
	throw(AipsError("Mask image shape is not the same as dirty image"));
      }
      LatticeStepper mls(mask(model).shape(),
		       IPosition(4, nx, ny, 1, 1),
		       IPosition(4, 0, 1, 2, 3));
      RO_LatticeIterator<Float>maskli(mask(model), mls);
      maskli.reset();
      if(nchan > 1){
	if(nchan==mask(model).shape()(3)){
	  isCubeMask[model]=true;
	  for (Int chan=0; chan < nchan; ++chan){
	    if(lmaskCube[chan*nmodels+model]) 
	      delete lmaskCube[chan*nmodels+model];
	    lmaskCube[chan*nmodels+model]=0;
	    lmaskCube[chan*nmodels+model]=makeMaskMatrix(nx,ny, maskli, 
							   xbeg[model][chan],
							   xend[model][chan],
							   ybeg[model][chan],
							   yend[model][chan]);
	    maskli++;
	  }
	}
	else{
	  //Using first plane mask
	  if(lmaskCube[model]) 
	    delete lmaskCube[model];
	  lmaskCube[model]=0;
	  lmaskCube[model]=makeMaskMatrix(nx,ny, maskli, 
					  xbeg[model][0],
					  xend[model][0],
					  ybeg[model][0],
					  yend[model][0]);
	  for (Int chan=1; chan < nchan; ++chan){
	    xbeg[model][chan]=xbeg[model][0];
	    ybeg[model][chan]=ybeg[model][0];
	    xend[model][chan]=xend[model][0];
	    yend[model][chan]=yend[model][0];
	  }

	}
      }
      else{
	if(lmaskCube[model]) 
	  delete lmaskCube[model];
	lmaskCube[model]=0;
	lmaskCube[model]=makeMaskMatrix(nx,ny, maskli, 
					xbeg[model][0],
					xend[model][0],
					ybeg[model][0],
					yend[model][0]);


      }
    }



  }

    
  Vector<Float> maxggS(nmodels,0.0);
  Bool lastCycleWriteModel=false;

  while(absmax>=threshold()&&maxIterations<numberIterations()&&!stop && !diverging) {
    os << LogIO::NORMAL
       << "*** Starting major cycle " << cycle
       << LogIO::POST; // Loglevel PROGRESS
    cycle++;

    // Make the residual images. We do an incremental update
    // for cycles after the first one. If we have only one
    // model then we use convolutions to speed the processing
    os << LogIO::NORMAL2 << "Making residual images for all fields" << LogIO::POST; // Loglevel PROGRESS
    if(modified_p){ 
      blankOverlappingModels();
      makeNewtonRaphsonStep(se, false, (numberIterations()<1)?true:False);
      //makeNewtonRaphsonStep(se, (cycle>1));
    }

    if(numberIterations() < 1){
      // Why waste the time to set up
      return true;
    }
    

    if(0) {
      ostringstream name;
      name << "Residual" << cycle;
      PagedImage<Float> thisCycleImage(residual(0).shape(),
				       residual(0).coordinates(),
				       name.str());
      thisCycleImage.copyData(residual(0));
    }

    if(cycle==1) {
      for (Int model=0;model<numberOfModels();model++) {
	LatticeExprNode LEN = max(LatticeExpr<Float>(ggS(model)));
	maxggS[model] = LEN.getFloat();
 
	if(0) {
	  ostringstream name;
	  name << "ggS";
	  PagedImage<Float> thisImage(residual(0).shape(),
				      residual(0).coordinates(),
				      name.str());
	  thisImage.copyData(ggS(0));
	}
      }
      os << LogIO::NORMAL1 // Loglevel INFO
         << "Maximum sensitivity = " << 1.0/sqrt(max(maxggS))
	 << " Jy/beam" << LogIO::POST;
    }

    if(0) {
      ostringstream name;
      name << "ggSxResidual" << cycle;
      PagedImage<Float> thisCycleImage(residual(0).shape(),
				       residual(0).coordinates(),
				       name.str());
      thisCycleImage.copyData(LatticeExpr<Float>(sqrt(ggS(0)/maxggS[0])*residual(0)));
    }

    absmax=maxField(resmax, resmin);
    if(cycle >1){
      //check if its 5% above previous value 
      if(absmax < 1.000005*oldabsmax)
	oldabsmax=absmax;
      else{
	diverging=true;
	os << LogIO::WARN 
         << "Clean not converging   "  << LogIO::POST;
      }
    }

    os << LogIO::NORMAL1; // Loglevel INFO
    for (model=0;model<numberOfModels();model++) {
      os << "Model " << model << ": max, min (weighted) residuals (over all pixels) = "
	 << resmax(model) << ", " << resmin(model) << endl;
    }
    os << LogIO::POST;

    // Can we stop?
    if(absmax<threshold()) {
      os << LogIO::NORMAL // Loglevel PROGRESS
         << "Reached stopping peak residual = " << absmax << LogIO::POST;
      stop=true;
      if(cycle >1)
	lastCycleWriteModel=true;
    }
    else if(!diverging) {
      
    
      // Calculate the threshold for this cycle. Add a safety factor

      // fractionOfPsf controls how deep the cleaning should go. 
      // There arse two user-controls.
      //  cycleFactor_p : scale factor for the PSF sidelobe level. 
      //                         1 : clean down to the psf sidelobe level
      //                         <1 : go deeper
      //                         >1 : shallower : stop sooner.
      //                          Default : 1.5
      //  cycleMaxPsfFraction_p : scale factor as a fraction of the PSF peak
      //                                     must be 0.0 < xx < 1.0 (obviously)
      //                                     Default : 0.8
      Float fractionOfPsf = min(cycleMaxPsfFraction_p, cycleFactor_p * maxSidelobe);
      if (fractionOfPsf > (Float)0.8) 
	{
	  //          os << LogIO::WARN << "PSF fraction for threshold computation is too high : " << fractionOfPsf << ".  Forcing to 0.8 to ensure that the threshold is smaller than the peak residual !" << LogIO::POST;
          os << LogIO::NORMAL << "Current values of max-PSF-fraction, cycle-factor and max PSF sidelobe level result in a stopping threshold more than 80% of the peak residual. Forcing the maximum threshold to 80% of the peak." << LogIO::POST;
          fractionOfPsf = 0.8;   // painfully slow!
	}

      os << LogIO::NORMAL << "The minor-cycle threshold is MAX[ 0.95 x " << threshold()  << "  ,   peak residual x " << fractionOfPsf  <<  " ] " << LogIO::POST;

      cycleThreshold=max(0.95*threshold(), fractionOfPsf * absmax);
      os << LogIO::NORMAL << "Maximum residual = " << absmax // Loglevel INFO
         << ", cleaning down to " << cycleThreshold << LogIO::POST;
      
      for (model=0;model<numberOfModels();model++) {
	
	nx=image(model).shape()(0);
	ny=image(model).shape()(1);
	npol=image(model).shape()(2);
	nchan=image(model).shape()(3);
	
	Int npolcube=npol;
	if(!doPolJoint_p){
	  npolcube=1;
	}
	if(cycle==1) {
	  if(doPolJoint_p)
	    iterations[model].resize(nchan);
	  else
	    iterations[model].resize(nchan*npol);
	  iterations[model].resize(nchan*npol);
	  iterations[model]=0;
	}
	  
	// Initialize delta image
        deltaImage(model).set(0.0);

	// Only process solveable models
	if(isSolveable(model)) {
	  

          if((abs(resmax(model))>cycleThreshold)||
	     (abs(resmin(model))>cycleThreshold)) {

	    os << LogIO::NORMAL
               << "Processing model " << model
               << LogIO::POST; // Loglevel PROGRESS
	    
	    IPosition onePlane(4, nx, ny, 1, 1);
	    IPosition oneCube(4, nx, ny, npolcube, 1);
	    
	    //AlwaysAssert((npol==1)||(npol==2)||(npol==4), AipsError);
	    
	    // Loop over all channels. We only want the PSF for the first
	    // polarization so we iterate over polarization LAST

	    LatticeStepper psfls(PSF(model).shape(), onePlane,
				 IPosition(4,0,1,3,2));
	    RO_LatticeIterator<Float> psfli(PSF(model),psfls);

	    LatticeStepper fsls(ggS(model).shape(), oneCube,
				IPosition(4,0,1,3,2));
	    RO_LatticeIterator<Float> ggSli(ggS(model),fsls);

	    LatticeStepper ls(image(model).shape(), oneCube,
			      IPosition(4, 0, 1, 3, 2));
	    LatticeIterator<Float> imageStepli(residual(model), ls);
	    LatticeIterator<Float> imageli(image(model), ls);
	    LatticeIterator<Float> deltaimageli(deltaImage(model), ls);
	    
	      // If we are using a mask: assign the first ones
	    Matrix<Float> maskArray;
	    Bool domask=false;
	    if(hasMask(model)) {
	      maskArray.resize();
	      //maskArray.resize(lmaskCube[model]->shape());
	      maskArray.reference(*(lmaskCube[model]));
	      domask=true;
	    }

	    // Now clean each channel
	    Int chan=0;
	    Int ipol=0;
	    for (imageStepli.reset(),imageli.reset(),psfli.reset(),
		   deltaimageli.reset(),ggSli.reset();
		 !imageStepli.atEnd();
		 imageStepli++,imageli++,psfli++,deltaimageli++,
		   ggSli++,chan++) {
	      if(!doPolJoint_p){
		if(chan==0){
		  os << LogIO::NORMAL // Loglevel PROGRESS
                     << "Doing stokes "<< stokesID(ipol) << " image" <<LogIO::POST;
		}
		if(chan==nchan){
		  chan=0;
		  psfli.reset();
		  ++ipol;
		  os << LogIO::NORMAL2 // Loglevel PROGRESS
                     << "Doing stokes "<< stokesID(ipol) << " image" <<LogIO::POST;
		}
	      }
	      if(hasMask(model) && isCubeMask[model] && chan >0) {
		maskArray.reference(*(lmaskCube[chan*nmodels+model])); 
	      }
	      
	      if(psfmax(model)>0.0) {

		if(nchan>1) {
		  os << LogIO::NORMAL // Loglevel PROGRESS
                     << "Processing channel "<<chan<<" of "<<nchan<<LogIO::POST;
		}

		// Renormalize by the weights 
		Matrix<Float> wmask(nx, ny, 0.0);
		//Trouble..
		Array<Float>  residu(imageStepli.cursor());
		Cube<Float> resid(residu.nonDegenerate(3));
		Cube<Float> weight(sqrt(ggSli.cursor().nonDegenerate(3)/maxggS[model]));


		for (Int iy=0;iy<ny;iy++) {
		  for (Int ix=0;ix<nx;ix++) {
		    for (Int pol=0;pol<npolcube;pol++) {
		      //resid(ix,iy,pol)*=weight(ix,iy,pol);
		      if(weight(ix,iy,pol)>0.01) {
			wmask(ix,iy)=1.0;
		      }
		    }
		  }
		}
		if (domask) {
		  wmask*=maskArray;
		}
		Bool delete_its;
		Float* limageStep_data=resid.getStorage(delete_its);

		if (itsSubAlgorithm == "hogbom") {
		  Bool delete_itdi, delete_itp, delete_itm;
		  Float *ldeltaimage_data;
		  ldeltaimage_data=deltaimageli.rwCursor().getStorage(delete_itdi);
		  const Float *lpsf_data(0), *lmask_data(0);
		  lmask_data=wmask.getStorage(delete_itm);
		  lpsf_data=psfli.cursor().getStorage(delete_itp);
		  Int niter=numberIterations();
		  Int starting_iteration = iterations[model](chan*npolcube+ipol);
		  Int ending_iteration;

		  Float g=gain();
		  Float thres=cycleThreshold;
		  Int fxbeg=xbeg[model][chan]+1;
		  Int fxend=xend[model][chan]+1;
		  Int fybeg=ybeg[model][chan]+1;
		  Int fyend=yend[model][chan]+1;

		  Int domaskI=1;
		  hclean(ldeltaimage_data, limageStep_data, (Float*)lpsf_data,
			 &domaskI, (Float*)lmask_data,
			 &nx, &ny, &npolcube,
			 &fxbeg, &fxend, &fybeg, &fyend, &niter,
			 &starting_iteration, &ending_iteration,
			 &g, &thres, &cycleSpeedup_p,
			 (void*) &HogbomCleanImageSkyModelmsgput,
			 (void*) &HogbomCleanImageSkyModelstopnow);

		  os << LogIO::NORMAL // Loglevel PROGRESS
                     << "Finished Hogbom clean inner cycle " << LogIO::POST;
		  
		  imageStepli.rwCursor().putStorage (limageStep_data, delete_its);
		  deltaimageli.rwCursor().putStorage (ldeltaimage_data, delete_itdi);
		  
		  Cube<Float> step(deltaimageli.rwCursor().nonDegenerate(3));
		  ///Need to go
		  /*
		  for (Int iy=0;iy<ny;iy++) {
		    for (Int ix=0;ix<nx;ix++) {
		      for (Int pol=0;pol<npol;pol++) {
			if(weight(ix,iy,pol)>0.0) {
			  step(ix,iy,pol)/=weight(ix,iy,pol);
			} 
		      }
		    }
		  }
		  deltaimageli.rwCursor()=step.addDegenerate(1);    
		  */ 	  
		  /////
		  //imageli.rwCursor()+=deltaimageli.cursor();
		  psfli.cursor().freeStorage (lpsf_data, delete_itp);
		  if (domask) {
		    maskArray.freeStorage (lmask_data, delete_itm);
		  }		  
		  iterations[model](chan*npolcube+ipol) = ending_iteration;
		  maxIterations=(iterations[model](chan*npolcube+ipol)>maxIterations) ?
		    iterations[model](chan*npolcube+ipol) : maxIterations;
		  modified_p=true;
		} else {  // clark is the default for now
		  
		  // Now make a convolution equation for this
		  // residual image and psf and then clean it
		  ConvolutionEquation eqn(psfli.matrixCursor(),
					  residu);
		  deltaimageli.rwCursor().set(Float(0.0));
		  ClarkCleanModel cleaner(deltaimageli.rwCursor());
		  cleaner.setMask(wmask);
		  cleaner.setGain(gain());
		  cleaner.setNumberIterations(numberIterations());
		  cleaner.setInitialNumberIterations(iterations[model](chan*npolcube+ipol));
		  cleaner.setThreshold(cycleThreshold);
		  cleaner.setPsfPatchSize(IPosition(2,psfpatch)); 
		  cleaner.setMaxNumberMajorCycles(10);
		  cleaner.setHistLength(1024);
		  cleaner.setMaxNumPix(32*1024);
		  cleaner.setChoose(false);
		  cleaner.setCycleSpeedup(cycleSpeedup_p);
		  cleaner.setSpeedup(0.0);
		  if ( displayProgress_p ) {
		    cleaner.setProgress( *progress_p );
		  }
		  cleaner.singleSolve(eqn, residu);
		  iterations[model](chan*npolcube+ipol)=cleaner.numberIterations();
		  maxIterations=(iterations[model](chan*npolcube+ipol)>maxIterations) ?
		    iterations[model](chan*npolcube+ipol) : maxIterations;
		  modified_p=true;
		  
		  cleaner.getModel(deltaimageli.rwCursor());
		  //imageli.rwCursor()+=deltaimageli.cursor();
		  //eqn.residual(imageStepli.rwCursor(), cleaner);
		  
		  os << LogIO::NORMAL // Loglevel PROGRESS
                     <<"Finished Clark clean inner cycle " << LogIO::POST;
		}
		if(maxIterations==0) {
		  stop=true;
		}
		else{
		  stop=false;
		}
                // Upped to NORMAL per CAS-2017.
		// cerr << model << " " << chan << " " << npolcube << " " << ipol 
		//      << " " << iterations.nelements() 
		//      << " " << iterations[0].shape()
		//      << endl;
		if (iterations[model](chan*npolcube+ipol) > 0) {
		  os << LogIO::NORMAL << "Clean used " // Loglevel PROGRESS
                     << iterations[model](chan*npolcube+ipol) << " iterations" 
		     << " to approach a threshhold of " << cycleThreshold
		     << LogIO::POST; 
                  }
	      }
	    }
	  }
	  else {
	    os << LogIO::NORMAL // Loglevel INFO
               << "No need to clean model " << model
               << ": peak residual below threshhold"
               << LogIO::POST;
	  }
	}
      }

      if(maxIterations != oldMaxIterations){
	oldMaxIterations=maxIterations;
	for(Int model=0; model < numberOfModels(); ++model){
	  image(model).copyData( LatticeExpr<Float>((image(model))+(deltaImage(model))));
	  os << LogIO::NORMAL << LatticeExprNode(sum(deltaImage(model))).getFloat()  // Loglevel INFO
	     << " Jy <- cleaned in this cycle for  model " 
	     << model 
	     << " (Total flux : " << LatticeExprNode(sum(image(model))).getFloat() << "Jy)"
	     << LogIO::POST;

	}

      }
      else {
	os << LogIO::NORMAL
           << "No more iterations left in this major cycle - stopping now"
           << LogIO::POST;
	stop=true;
	converged=true;
      }
    }
    if(0) {
      ostringstream name;
      name << "Delta" << cycle;
      PagedImage<Float> thisCycleDeltaImage(residual(1).shape(),
					    residual(1).coordinates(),
					    name.str());
      thisCycleDeltaImage.copyData(deltaImage(1));
    }
  }
  if (progress_p) delete progress_p;
  
  for(model=0; model < nmodels; ++model){
    image(model).clearCache();
    PSF(model).clearCache();
    ggS(model).clearCache();
    residual(model).clearCache();
    deltaImage(model).clearCache();
    for(Int chan=0; chan < nchan; ++chan){
      if(lmaskCube[chan*nmodels+model] !=0) 
	delete lmaskCube[chan*nmodels+model];
    }
  }
  
  
  setNumberIterations(maxIterations);
  if(modified_p || lastCycleWriteModel) {
    os << LogIO::NORMAL2 // Loglevel PROGRESS
       << "Finalizing residual images for all fields" << LogIO::POST;
    blankOverlappingModels();
    makeNewtonRaphsonStep(se, false, true); //committing model to MS
    restoreOverlappingModels();
    Float finalabsmax=maxField(resmax, resmin); 
    converged=(finalabsmax < 1.05 * threshold());
    setThreshold(finalabsmax);
    os << LogIO::NORMAL << "Final maximum residual = " << finalabsmax << LogIO::POST; // Loglevel INFO
    
    os << LogIO::NORMAL; // Loglevel INFO
    for (model=0;model<numberOfModels();model++) {
      os << "Model " << model << ": max, min residuals = "
	 << resmax(model) << ", " << resmin(model) << " clean flux " << LatticeExprNode(sum(image(model))).getFloat() << endl;
    }
  }
  else {
    os << LogIO::NORMAL2 // Loglevel PROGRESS
       << "Residual images for all fields are up-to-date" << LogIO::POST;
  }

  os << LogIO::POST;
  if(stop)
    converged=true;

  return(converged);
};
  

void MFCleanImageSkyModel::blankOverlappingModels(){
  if(numberOfModels() == 1) 
    return;
  /////////////////////
  /*
   for (Int model=0;model<(numberOfModels()); ++model) {

    
      LatticeExprNode maxIm=max(deltaImage(model));
      cout << "MAX model " << model << "    " << maxIm.getFloat() << endl;
  }
  */
  //////////////
  for (Int model=0;model<(numberOfModels()-1); ++model) {
    CoordinateSystem cs0=image(model).coordinates();
    IPosition iblc0(image(model).shape().nelements(),0);
      
      IPosition itrc0(image(model).shape());
      itrc0=itrc0-Int(1);
      LCBox lbox0(iblc0, itrc0, image(model).shape());

      ImageRegion imagreg0(WCBox(lbox0, cs0));
    for (Int nextmodel=model+1; nextmodel < numberOfModels(); ++nextmodel){
      CoordinateSystem cs=image(nextmodel).coordinates();
      IPosition iblc(image(nextmodel).shape().nelements(),0);
      
      IPosition itrc(image(nextmodel).shape());
      itrc=itrc-Int(1);
      
      LCBox lbox(iblc, itrc, image(nextmodel).shape());

      ImageRegion imagreg(WCBox(lbox, cs));


      try{
	LatticeRegion latReg=imagreg.toLatticeRegion(image(model).coordinates(), image(model).shape());

	SubImage<Float> partToMask(image(model), imagreg, true);
	ArrayLattice<Bool> pixmask(latReg.get());
	LatticeExpr<Float> myexpr(iif(pixmask, 0.0, partToMask) );
	partToMask.copyData(myexpr);

      }
      catch(...){
	//no overlap you think ?
	/*
           os << LogIO::WARN
              << "no overlap or failure of copying the clean components"
              << x.getMesg()
              << LogIO::POST;
	*/
	continue;
      }
    
    
      }
    
    
    
  }
}

// test method - reversing blanking of overlapped regions
void MFCleanImageSkyModel::restoreOverlappingModels(){
  LogIO os(LogOrigin("MFCleanImageSkyModel","restoreOverlappingModels"));
  if(numberOfModels() == 1)
    return;
  for (Int model=0;model<(numberOfModels()-1); ++model) {
    CoordinateSystem cs0=image(model).coordinates();
    IPosition iblc0(image(model).shape().nelements(),0);
    IPosition itrc0(image(model).shape());
    itrc0=itrc0-Int(1);
    LCBox lbox0(iblc0, itrc0, image(model).shape());
    ImageRegion imagreg0(WCBox(lbox0, cs0));

    for (Int nextmodel=model+1; nextmodel < numberOfModels(); ++nextmodel){
      CoordinateSystem cs=image(nextmodel).coordinates();
      IPosition iblc(image(nextmodel).shape().nelements(),0);

      IPosition itrc(image(nextmodel).shape());
      itrc=itrc-Int(1);

      LCBox lbox(iblc, itrc, image(nextmodel).shape());

      ImageRegion imagreg(WCBox(lbox, cs));
      try{
        LatticeRegion latReg0=imagreg0.toLatticeRegion(image(nextmodel).coordinates(), image(nextmodel).shape());
        LatticeRegion latReg=imagreg.toLatticeRegion(image(model).coordinates(), image(model).shape());


        SubImage<Float> partToMerge(image(nextmodel), imagreg0, true);
        SubImage<Float> partToUnmask(image(model), imagreg, true);
        
        ArrayLattice<Bool> pixmask(latReg.get());
        LatticeExpr<Float> myexpr0(iif(pixmask,partToMerge,partToUnmask));
        partToUnmask.copyData(myexpr0);

      }
      catch(AipsError x){
	/*
           os << LogIO::WARN
              << "no overlap or failure of copying the clean components"
              << x.getMesg()
              << LogIO::POST;
	*/
        continue;
      }
    }
  }
}

void MFCleanImageSkyModel::mergeOverlappingMasks(){

    using casacore::operator<;
    using casacore::max;

  LogIO os(LogOrigin("MFCleanImageSkyModel","restoreOverlappingModels"));
  if(numberOfModels() == 1)
    return;
  for (Int model=0;model<(numberOfModels()-1); ++model) {
    if(hasMask(model)){
	CoordinateSystem cs0=mask(model).coordinates();
	IPosition iblc0(mask(model).shape().nelements(),0);
	
	IPosition itrc0(mask(model).shape());
	itrc0=itrc0-Int(1);
	LCBox lbox0(iblc0, itrc0, mask(model).shape());
	
	ImageRegion imagreg0(WCBox(lbox0, cs0));
	for (Int nextmodel=model+1; nextmodel < numberOfModels(); ++nextmodel){
	  if(hasMask(nextmodel)){
	    CoordinateSystem cs=mask(nextmodel).coordinates();
	    IPosition iblc(mask(nextmodel).shape().nelements(),0);
	    
	    IPosition itrc(mask(nextmodel).shape());
	    itrc=itrc-Int(1);
	    
	    LCBox lbox(iblc, itrc, image(nextmodel).shape());
	    
	    ImageRegion imagreg(WCBox(lbox, cs));

	    try{

	      LatticeRegion latRef1 = imagreg0.toLatticeRegion(
							      (mask(nextmodel)).coordinates(), 
							      (mask(nextmodel)).shape() );
	      LatticeRegion latRef2 = imagreg.toLatticeRegion(
							      (mask(model)).coordinates(), 
							      (mask(model)).shape() );

	      SubImage<Float> partToMerge(mask(nextmodel), imagreg0, true);
	      SubImage<Float> partToMask(mask(model), imagreg, true);
	      LatticeExpr<Float> myexpr0(max(partToMerge,partToMask));	      
	      partToMask.copyData(myexpr0);
	      partToMerge.copyData(myexpr0);

	    }
	    catch(AipsError x){
	      //most probably no overlap
	      /*
		os << LogIO::WARN
		<< "no overlap or failure of copying the clean components"
		<< x.getMesg()
		<< LogIO::POST;
	      */

	      continue;
	    }
	  }
	}// for model
    }//hasmask(model)
  }
}

    

Float MFCleanImageSkyModel::maxOuter(Lattice<Float> & lat, const uInt nCenter ) 
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

Matrix<Float>* MFCleanImageSkyModel::makeMaskMatrix(const Int& nx, 
							const Int& ny, 
							RO_LatticeIterator<Float>& maskIter,
							Int& xbeg,
							Int& xend,
							Int& ybeg,
							Int& yend) {

  LogIO os(LogOrigin("MFCleanImageSkyModel","makeMaskMatrix",WHERE)); 

    xbeg=0;
    ybeg=0;
    xend=nx-1;
    yend=ny-1;
  
  Matrix<Float>* mask= new Matrix<Float>(maskIter.matrixCursor().shape());
  (*mask)=maskIter.matrixCursor();
  // ignore mask if none exists

  if(max(*mask) < 0.5) {
    //If empty mask respect it for MF.... till we break WF from it
    return mask;
  }
  // Now read the mask and determine the bounding box
  xbeg=xend;
  ybeg=yend;
  yend=0;
  xend=0;
  
  for (Int iy=0;iy<ny;iy++) {
    for (Int ix=0;ix<nx;ix++) {
      if((*mask)(ix,iy)>0.5) {
	xbeg=min(xbeg,ix);
	ybeg=min(ybeg,iy);
	xend=max(xend,ix);
	yend=max(yend,iy);
      }
    }
  }

  return mask;
}

} //#End casa namespace
