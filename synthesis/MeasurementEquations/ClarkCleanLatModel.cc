//# ClarkCleanLatModel.cc:  this defines ClarkCleanLatModel
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

#include <synthesis/MeasurementEquations/ClarkCleanLatModel.h>
#include <synthesis/MeasurementEquations/ClarkCleanProgress.h>
#include <casa/Arrays/Slice.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayError.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/VectorIter.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>
#include <casa/iostream.h> 
#include <casa/System/Choice.h>
#include <synthesis/MeasurementEquations/LatConvEquation.h>
#include <synthesis/MeasurementEquations/CCList.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LEL/LatticeExprNode.h>
#include <casa/BasicSL/Constants.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//# These are the definitions of the fortran functions

#define NEED_FORTRAN_UNDERSCORES

#if defined(NEED_FORTRAN_UNDERSCORES)
#define getbigf getbigf_
#define getbig2f getbig2f_
#define getbig4f getbig4f_
#define abshisf abshisf_
#define abshis2f abshis2f_
#define abshis4f abshis4f_
#define absmaxf absmaxf_
#define absmax2f absmax2f_
#define absmax4f absmax4f_
#define subcomf subcomf_
#define subcom2f subcom2f_
#define subcom4f subcom4f_
#define maxabsf maxabsf_
#define maxabs2f maxabs2f_
#define maxabs4f maxabs4f_
#define maxabmf maxabmf_
#define maxabm2f maxabm2f_
#define maxabm4f maxabm4f_
#define abshimf abshimf_
#define abshim2f abshim2f_
#define abshim4f abshim4f_
#define getbimf getbimf_
#define getbim2f getbim2f_
#define getbim4f getbim4f_
#endif

extern "C" {
  void getbigf(const Float * const pixVal, const Int * const pixPos, 
	       Int * maxPix, const Float * fluxLimit, const Float * arr, 
	       const Int * nx, const Int * ny);
  void getbig2f(const Float * const pixVal, const Int * const pixPos,
		Int * maxPix, const Float * fluxLimit, const Float * arr, 
		const Int * nx, const Int * ny);
  void getbig4f(const Float * const pixVal, const Int * const pixPos, 
		Int * maxPix, const Float * fluxLimit, const Float * arr, 
		const Int * nx, const Int * ny);
  void abshisf(Int * hist, Float * minval, Float * maxval, 
	       const Int * nbins, const Float * arr, const Int * npix);
  void abshis2f(Int * hist, Float * minval, Float * maxval, 
		const Int * nbins, const Float * arr, const Int * npix);
  void abshis4f(Int * hist, Float * minval, Float * maxval, 
		const Int * nbins, const Float * arr, const Int * npix);
  void absmaxf(Float * maxelem, Float * maxval, Int * maxpos, 
	       const Float * arr, const Int * npix);
  void absmax2f(Float * maxelem, Float * maxval, Int * maxpos, 
		const Float * arr, const Int * npix);
  void absmax4f(Float * maxelem, Float * maxval, Int * maxpos, 
		const Float * arr, const Int * npix);
  void subcomf(Float * pixval, const Int * pixpos, const Int * npix, 
	       const Float * maxpix, const Int * maxpos, 
	       const Float * psf, const Int * nx, const Int * ny);
  void subcom2f(Float * pixval, const Int * pixpos, const Int * npix, 
	       const Float * maxpix, const Int * maxpos, 
	       const Float * psf, const Int * nx, const Int * ny);
  void subcom4f(Float * pixval, const Int * pixpos, const Int * npix, 
	       const Float * maxpix, const Int * maxpos, 
	       const Float * psf, const Int * nx, const Int * ny);
  void maxabsf(Float * maxval, const Float * arr, const Int * npix);
  void maxabs2f(Float * maxval, const Float * arr, const Int * npix);
  void maxabs4f(Float * maxval, const Float * arr, const Int * npix);
  void maxabmf(Float * maxval, const Float * arr, const Float * mask,
	       const Int * npix);
  void maxabm2f(Float * maxval, const Float * arr, const Float * mask,
		const Int * npix);
  void maxabm4f(Float * maxval, const Float * arr, const Float * mask,
		const Int * npix);
  void abshimf(Int * hist, Float * minval, Float * maxval, const Int * nbins,
	       const Float * arr, const Float * mask, const Int * npix);
  void abshim2f(Int * hist, Float * minval, Float * maxval, const Int * nbins,
		const Float * arr, const Float * mask, const Int * npix);
  void abshim4f(Int * hist, Float * minval, Float * maxval, const Int * nbins,
		const Float * arr, const Float * mask, const Int * npix);
  void getbimf(const Float * const pixVal, const Int * const pixPos,
	       Int * maxPix, const Float * fluxLimit, const Float * arr, 
	       const Float * mask, const Int * nx, const Int * ny);
  void getbim2f(const Float * const pixVal, const Int * const pixPos,
		Int * maxPix, const Float * fluxLimit, const Float * arr, 
		const Float * mask, const Int * nx, const Int * ny);
  void getbim4f(const Float * const pixVal, const Int * const pixPos,
		Int * maxPix, const Float * fluxLimit, const Float * arr,
		const Float * mask, const Int * nx, const Int * ny);
};

//----------------------------------------------------------------------
ClarkCleanLatModel::ClarkCleanLatModel()
  :itsModelPtr(0),
   itsResidualPtr(0),
   itsSoftMaskPtr(0),
   itsMaxNumPix(32*1024),
   itsHistBins(1024), 
   itsMaxNumberMinorIterations(10000),
   itsInitialNumberIterations(0),
   itsMaxNumberMajorCycles(-1),
   itsMaxExtPsf(0.0),
   itsPsfPatchSize(2,51,51),
   itsChoose(false),
   itsSpeedup(0.0),
   itsCycleFactor(1.5),
   itsLog(LogOrigin("ClarkCleanLatModel", "ClarkCleanLatModel()")),
   itsProgressPtr(0),
   itsJustStarting(true),
   itsWarnFlag(false),
   itsLocalResTL(false)

{
};
//----------------------------------------------------------------------
ClarkCleanLatModel::ClarkCleanLatModel(Lattice<Float> & model)
  :itsModelPtr(&model),
   itsResidualPtr(0),
   itsSoftMaskPtr(0),
   itsMaxNumPix(32*1024),
   itsHistBins(1024), 
   itsMaxNumberMinorIterations(10000),
   itsInitialNumberIterations(0),
   itsMaxNumberMajorCycles(-1),
   itsMaxExtPsf(0.0),
   itsPsfPatchSize(2, 51, 51),
   itsChoose(false),
   itsSpeedup(0.0), 
   itsCycleFactor(1.5),
   itsLog(LogOrigin("ClarkCleanLatModel", 
		    "ClarkCleanLatModel(const Lattice<Float> & model)")),
   itsProgressPtr(0),
   itsWarnFlag(false),
   itsLocalResTL(false)
{
  AlwaysAssert(getModel().ndim() >= 2, AipsError);
  if (getModel().ndim() >= 3)
    AlwaysAssert(getModel().shape()(2) == 1 || getModel().shape()(2) == 2 || 
		 getModel().shape()(2) == 4, AipsError);
  if (getModel().ndim() >= 4)
    for (uInt i = 3; i < getModel().ndim(); i++)
      AlwaysAssert(getModel().shape()(i) == 1, AipsError);
//      itsLog << LogOrigin("ClarkCleanLatModel", "ClarkCleanLatModel") 
// 	    << "Model shape is:" << getModel().shape() << endl;
};
//----------------------------------------------------------------------
ClarkCleanLatModel::ClarkCleanLatModel(Lattice<Float> & model, 
				 Lattice<Float> & mask)
  :itsModelPtr(&model),
   itsResidualPtr(0),
   itsSoftMaskPtr(&mask),
   itsMaxNumPix(32*1024),
   itsHistBins(1024), 
   itsMaxNumberMinorIterations(10000),
   itsInitialNumberIterations(0),
   itsMaxNumberMajorCycles(-1),
   itsMaxExtPsf(0.0),
   itsPsfPatchSize(2, 51, 51),
   itsChoose(false),
   itsSpeedup(0.0), 
   itsCycleFactor(1.5),
   itsLog(LogOrigin("ClarkCleanLatModel", 
		    "ClarkCleanLatModel(Lattice<Float> & model"
		    ", Lattice<Float> & mask)")),
   itsProgressPtr(0),
   itsJustStarting(true),
   itsWarnFlag(false),
   itsLocalResTL(false)

{
     AlwaysAssert(getModel().ndim() >= 2, AipsError);
     if (getModel().ndim() >= 3)
       AlwaysAssert(getModel().shape()(2) == 1 || getModel().shape()(2) == 2 || 
		    getModel().shape()(2) == 4, AipsError);
     if (getModel().ndim() >= 4)
       for (uInt i = 3; i < getModel().ndim(); i++)
	 AlwaysAssert(getModel().shape()(i) == 1, AipsError);

     AlwaysAssert(itsSoftMaskPtr->ndim() >= 2, AipsError);
     AlwaysAssert(itsSoftMaskPtr->shape()(0) == getModel().shape()(0), AipsError);
     AlwaysAssert(itsSoftMaskPtr->shape()(1) == getModel().shape()(1), AipsError);
     if (itsSoftMaskPtr->ndim() >= 3)
       for (uInt i = 2; i < itsSoftMaskPtr->ndim(); i++)
	 AlwaysAssert(itsSoftMaskPtr->shape()(i) == 1, AipsError);
};
ClarkCleanLatModel::ClarkCleanLatModel(Lattice<Float> & model,
				       Lattice<Float> & residual,
				 Lattice<Float> & mask)
  :itsModelPtr(&model),
   itsResidualPtr(&residual),
   itsSoftMaskPtr(&mask),
   itsMaxNumPix(32*1024),
   itsHistBins(1024), 
   itsMaxNumberMinorIterations(10000),
   itsInitialNumberIterations(0),
   itsMaxNumberMajorCycles(-1),
   itsMaxExtPsf(0.0),
   itsPsfPatchSize(2, 51, 51),
   itsChoose(false),
   itsSpeedup(0.0), 
   itsCycleFactor(1.5),
   itsLog(LogOrigin("ClarkCleanLatModel", 
		    "ClarkCleanLatModel(Lattice<Float> & model"
		    ", Lattice<Float> & mask)")),
   itsProgressPtr(0),
   itsJustStarting(true),
   itsWarnFlag(false),
   itsLocalResTL(false)

{


};


ClarkCleanLatModel::~ClarkCleanLatModel() {
  // CAS-9268 needs this for tclean, where itsResidualPtr is not constructed outside the cleaner
  if(itsLocalResTL==True && itsResidualPtr != NULL){delete itsResidualPtr; itsResidualPtr=NULL;}
}


void ClarkCleanLatModel::setModel(const Lattice<Float>& model){
  AlwaysAssert(model.ndim() >= 2, AipsError);
  if (model.ndim() >= 3)
    AlwaysAssert(model.shape()(2) == 1 || model.shape()(2) == 2 || 
		 model.shape()(2) == 4, AipsError);
  if (model.ndim() >= 4)
    for (uInt i = 3; i < model.ndim(); i++)
      AlwaysAssert(model.shape()(i) == 1, AipsError);
  //  itsModelPtr = &model;
  throw(AipsError(
"setModel(const Lattice<Float>& ) : only non-const version works!"));
};

void ClarkCleanLatModel::setModel(Lattice<Float>& model){
  AlwaysAssert(model.ndim() >= 2, AipsError);
  if (model.ndim() >= 3)
    AlwaysAssert(model.shape()(2) == 1 || model.shape()(2) == 2 || 
		 model.shape()(2) == 4, AipsError);
  if (model.ndim() >= 4)
    for (uInt i = 3; i < model.ndim(); i++)
      AlwaysAssert(model.shape()(i) == 1, AipsError);
  itsModelPtr = &model;
};

const Lattice<Float> & ClarkCleanLatModel::getMask() const{
  return (*itsSoftMaskPtr);
};

void ClarkCleanLatModel::setMask(const Lattice<Float> & mask){
  AlwaysAssert(mask.ndim() >= 2, AipsError);
  AlwaysAssert(mask.shape()(0) == getModel().shape()(0), AipsError);
  AlwaysAssert(mask.shape()(1) == getModel().shape()(1), AipsError);
  if (mask.ndim() >= 3)
    for (uInt i = 2; i < mask.ndim(); i++)
      AlwaysAssert(mask.shape()(i) == 1, AipsError);
  itsSoftMaskPtr = &mask;
};


//----------------------------------------------------------------------
Bool ClarkCleanLatModel::solve(LatConvEquation & eqn){
  itsLog << LogOrigin("ClarkCleanLatModel", "solve");
  AlwaysAssert(getModel().ndim() >= 2, AipsError);
  const IPosition dataShape = getModel().shape();

  Int npol = 1;
  if (getModel().ndim() >= 3)
    npol = dataShape(2);
  AlwaysAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  
  // Determine the number of polarisations
  //   itsLog << "Data has " << npol << " polarisations" << LogIO::POST;
    
  // compute the current residual image (using an FFT)
  if(itsResidualPtr==0){
    itsResidualPtr=new TempLattice<Float> (dataShape);
    itsLocalResTL=True;
    eqn.residual(*itsResidualPtr, *this);
  }
  // Determine the psf patch to use 
  Matrix<Float> psfPatch;
  Float maxExtPsf;  

  maxExtPsf = getPsfPatch(psfPatch, eqn);
//   itsLog << "PsfPatch shape is: " << psfPatch.shape() 
//  	 << " and has a maximum exterior sidelobe of " 
//  	 << maxExtPsf << LogIO::POST;
  
  // Declare variables needed inside the following while loop
  Int numPix;                     // the number of Active pixels
  Int maxNumPix = 0;		  // unclear
  uInt numIterations = itsInitialNumberIterations;
                                  // Number of Iterations done so far
  uInt numMinorIterations;        // The number of minor iterations done/todo
  uInt numMajorCycles = 0;        // The number of major cycles done
  uInt maxNumberMinorIterations = 0;// The max. number of min. iterations
				   // ever used
  Float Fmn= 0;   //1;                    // The "uncertainty" factor 
  Float fluxLimit;                // The fluxlimit for the current major cycle
  Float totalFlux = 0;
  Float factor=itsCycleFactor/4.5; // This factor is empirical...found to be
                                   // good without being too conservative 
  // Note that a Matrix is used rather than say a Vector of IPositions as
  // it allows the inner loop (in doMinorIterations()) to be more highly
  // optimised (using pointers)

  // find its maximum value of the residual
  Float maxRes = maxResidual(*itsResidualPtr);
  Float maxResPrevious=maxRes;
  if (numIterations > 0)
    itsLog << LogIO::NORMAL << "Initial maximum residual: " << maxRes << LogIO::POST;
  // if flux limit or iteration limit reached then bail out. 
  Bool userHalt = false;
  Int numIt=numberIterations();
  //Pathological PSFs
  if(maxExtPsf > 0.5)
    itsMaxNumberMinorIterations=5;
  else if(maxExtPsf > 0.35)
    itsMaxNumberMinorIterations=50;

  while ((Int(numIterations) < numIt) && 
	 (maxRes > threshold()) &&
	 ((itsMaxNumberMajorCycles<0)||
	  (Int(numMajorCycles)<itsMaxNumberMajorCycles)) &&
	 userHalt == false){

    CCList activePixels(npol, 2, 0)  ; // cache of active pixel values and positions;

    // determine the fluxlimit for this major cycle
    // choose fluxlimit for residuals to be considered in minor iterations
    // don't consider residuals below maxRes*(value of maxPsf at outside the 
    // patch)
    fluxLimit = maxRes * maxExtPsf * factor;

    if(factor > 1.0) 
      fluxLimit = min(0.95*maxRes, fluxLimit);
   

//     itsLog << "Fluxlimit determined using the Maximum exterior Psf: " 
//  	   << fluxLimit << LogIO::POST;
    // See if this flux limit needs to be modified because it selects too
    // many pixels.

    //*** COMMENTED below off as its giving extremely conservative limit
    // and making the loop very slow
    //       minLimit = biggestResiduals(maxRes, itsMaxNumPix, fluxLimit, residual);


//     itsLog << "Fluxlimit determined using the maximum number active pixels: " 
//  	   << minLimit << endl;
    //
       //        fluxLimit = std::max(fluxLimit, minLimit);


//     itsLog << "Final Fluxlimit: " << fluxLimit << LogIO::POST;
    
    // Copy all the active pixels into separate areas for better memory
    // management and quicker indexing.

    cacheActivePixels(activePixels, *itsResidualPtr,
		      std::max(fluxLimit,threshold()));
    // The numpix calculated here frequently differs 
    // from the number calculated using the histogram, because of the
    // quantisation of the flux levels in the histogram, and the imposition
    // of an external fluxlevel.
    numPix = activePixels.nComp();
    if (numPix > 0) {
//       itsLog <<"Major cycle has "<< numPix << " active residuals, "
// 	     << "a Fluxlimit of " << std::max(fluxLimit,threshold()) << endl;
      // Start of minor cycles

      numMinorIterations = std::min(itsMaxNumberMinorIterations,
			       numIt-numIterations);
      doMinorIterations(activePixels, 
  			psfPatch, fluxLimit, numMinorIterations, 
			Fmn, numIterations, totalFlux);
      numIterations += numMinorIterations;
      maxNumberMinorIterations = std::max(maxNumberMinorIterations,
				     numMinorIterations);
      maxNumPix = std::max((Int)itsMaxNumPix, numPix);
      // Now do a  major cycle
      eqn.residual(*itsResidualPtr, *this);


      // find the new maximum residual
      //      maxRes = maxResidual(*itsResidualPtr);
      maxRes = itsMaxRes;
      if(maxRes > maxResPrevious){
	if(!itsWarnFlag){
	  itsLog << LogIO::WARN 
		 << "Slowing down in the minor cycle loop; " 
		 << "Could be a PSF with bad sidelobes; " 
		 << "Suggest using CS or Hogbom instead" << LogIO::POST;
	}
	factor=factor*3; //pathological PSF's go very slowly in minorcycles
	itsMaxNumberMinorIterations=10;
	itsWarnFlag=true;
      }
      maxResPrevious=maxRes;
	
      itsLog << LogIO::NORMAL
             << "Iteration: " << numIterations
             << ", Maximum residual=" << maxRes
             << " Flux limit=" << std::max(fluxLimit,threshold()) 
 	     << ", " << numPix << " Active pixels" << LogIO::POST;

      // Count the number of major cycles
      numMajorCycles++;
    }
    else{
      itsLog << LogIO::WARN 
	     << "Zero Pixels selected with a Fluxlimit of " << fluxLimit
	     << " and a maximum Residual of " << maxRes << LogIO::POST;
      if(itsWarnFlag){
	userHalt=true;
	itsLog << LogIO::WARN 
	       << "Bailing out prior to reaching threshold as residual value is   not converging " 
	       << LogIO::POST;
      }
      else{
	//lets try to increase the depth  a little bit
	factor=factor*1.2;
      }
      itsWarnFlag=true;
//    userHalt = stopnow();
// The above is commented off as users do not seem to find this 
// useful. If nobody ask for it again the function stopnow() 
// along with userHalt will be obliterated before the release 
    }
    
  }

  // Is this a problem?
  if (itsProgressPtr) {
    itsProgressPtr->finalize();
  }

  setThreshold(maxRes);
  setNumberIterations(numIterations);
  itsMaxNumPix = maxNumPix;
  itsMaxNumberMinorIterations = maxNumberMinorIterations;
  return true;
};
//----------------------------------------------------------------------
//----------------------------------------------------------------------
Bool ClarkCleanLatModel::singleSolve(LatConvEquation & eqn, Lattice<Float>& residual){
  itsLog << LogOrigin("ClarkCleanLatModel", "singleSolve");
  AlwaysAssert(getModel().ndim() >= 2, AipsError);
  const IPosition dataShape = getModel().shape();
  //  itsLog << "Model    shape " << getModel().shape() << LogIO::POST;
  //  itsLog << "Residual shape " << residual.shape() << LogIO::POST;
  Int npol = 1;
  if (getModel().ndim() >= 3)
    npol = dataShape(2);
  AlwaysAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  
  // Determine the number of polarisations
  //   itsLog << "Data has " << npol << " polarisations" << LogIO::POST;
    
  // Determine the psf patch to use 
  Matrix<Float> psfPatch;
  Float maxExtPsf;  

  maxExtPsf = getPsfPatch(psfPatch, eqn);
  //  itsLog << "PsfPatch shape is: " << psfPatch.shape() 
  //     	 << " and has a maximum exterior sidelobe of " 
  //     	 << maxExtPsf << LogIO::POST;
  
  // Declare variables needed inside the following while loop
  Int numPix;                     // the number of Active pixels
  Int maxNumPix = 0;		  // unclear
  uInt numIterations = itsInitialNumberIterations;
  // Number of Iterations done so far
  uInt numMinorIterations;        // The number of minor iterations done/todo
  uInt maxNumberMinorIterations = 0;// The max. number of min. iterations
  // ever used
  Float Fmn= 0;   //1;                    // The "uncertainty" factor 
  Float fluxLimit;                // The fluxlimit for the current major cycle
  Float totalFlux = 0;
  Float factor=1.0/3.0;          // This factor is empirical...found to be 
                                 // good without being too conservative 
  // Note that a Matrix is used rather than say a Vector of IPositions as
  // it allows the inner loop (in doMinorIterations()) to be more highly
  // optimised (using pointers)
  
  // find its maximum value of the residual
  Float maxRes = maxResidual(residual);
  itsLog << "Initial maximum residual: " << maxRes << LogIO::POST;
  
  CCList activePixels(npol, 2, 0)  ; // cache of active pixel values and positions;
  
  // determine the fluxlimit for this major cycle
  // choose fluxlimit for residuals to be considered in minor iterations
  // don't consider residuals below maxRes*(value of maxPsf at outside the 
  // patch)
  fluxLimit = maxRes * maxExtPsf * factor;
  
  if(factor > 1.0) 
    fluxLimit = min(0.95*maxRes, fluxLimit);
  
  
  //     itsLog << "Fluxlimit determined using the Maximum exterior Psf: " 
  //  	   << fluxLimit << LogIO::POST;
  // See if this flux limit needs to be modified because it selects too
  // many pixels.
  
  //*** COMMENTED below off as its giving extremely conservative limit
  // and making the loop very slow
  //       minLimit = biggestResiduals(maxRes, itsMaxNumPix, fluxLimit, residual);
  
  
  //     itsLog << "Fluxlimit determined using the maximum number active pixels: " 
  //  	   << minLimit << endl;
  //
  //        fluxLimit = std::max(fluxLimit, minLimit);
  
  
  //     itsLog << "Final Fluxlimit: " << fluxLimit << LogIO::POST;
  
  // Copy all the active pixels into separate areas for better memory
  // management and quicker indexing.
  
  cacheActivePixels(activePixels, residual, std::max(fluxLimit,threshold()));
  // The numpix calculated here frequently differs 
  // from the number calculated using the histogram, because of the
  // quantisation of the flux levels in the histogram, and the imposition
  // of an external fluxlevel.
  numPix = activePixels.nComp();
  if (numPix > 0) {
    //       itsLog <<"Major cycle has "<< numPix << " active residuals, "
    // 	     << "a Fluxlimit of " << std::max(fluxLimit,threshold()) << endl;
    // Start of minor cycles
    
    
    numMinorIterations = std::min(itsMaxNumberMinorIterations,
			     numberIterations()-numIterations);
    doMinorIterations(activePixels, 
		      psfPatch, fluxLimit, numMinorIterations, 
		      Fmn, numIterations, totalFlux);
    numIterations += numMinorIterations;
    //       itsLog << "Clean has used " << numIterations << " Iterations" ;
    maxNumberMinorIterations = std::max(maxNumberMinorIterations,
				   numMinorIterations);
    maxNumPix = std::max((Int)itsMaxNumPix, numPix);
    
  }
  else {
    itsLog << LogIO::WARN 
	    << "Zero Pixels selected with a Fluxlimit of " << fluxLimit
	    << " and a maximum Residual of " << maxRes << endl;
    return false;
  }
  
  setThreshold(maxRes);
  setNumberIterations(numIterations);
  itsMaxNumPix = maxNumPix;
  itsMaxNumberMinorIterations = maxNumberMinorIterations;
  return true;
};
//----------------------------------------------------------------------
void ClarkCleanLatModel::setResidual(Lattice<Float>& residual){
  
  itsResidualPtr= &residual;
  
}; 
//---------------------------------------------------------------------
void ClarkCleanLatModel::setPsfPatchSize(const IPosition & psfPatchSize){
  if (psfPatchSize.nelements() < 2) {
    throw(AipsError("ClarkCleanLatModel::setPsfPatchSize - assumption of 2-D (or greater) not met"));
  } else {
    // only 2 dimensions used here
    itsPsfPatchSize(0)=psfPatchSize(0);
    itsPsfPatchSize(1)=psfPatchSize(1);
  }
}; 
//----------------------------------------------------------------------
IPosition ClarkCleanLatModel::getPsfPatchSize(){
  return itsPsfPatchSize;
}; 
//----------------------------------------------------------------------
void ClarkCleanLatModel::setHistLength(const uInt HistBins ){
  itsHistBins=HistBins;
}; 
//----------------------------------------------------------------------
uInt ClarkCleanLatModel::getHistLength(){
  return itsHistBins;
}; 
//----------------------------------------------------------------------
void ClarkCleanLatModel::setMaxNumberMinorIterations(const uInt maxNumMinorIterations){
  itsMaxNumberMinorIterations=maxNumMinorIterations;
}; 
//----------------------------------------------------------------------
uInt ClarkCleanLatModel::getMaxNumberMinorIterations(){
  return itsMaxNumberMinorIterations;
};
//----------------------------------------------------------------------
void ClarkCleanLatModel::setInitialNumberIterations(const uInt initialNumberIterations){
  itsInitialNumberIterations=initialNumberIterations;
}; 
//----------------------------------------------------------------------
uInt ClarkCleanLatModel::getInitialNumberIterations(){
  return itsInitialNumberIterations;
};
//----------------------------------------------------------------------
void ClarkCleanLatModel::setMaxNumberMajorCycles(const uInt maxNumMajorCycles){
  itsMaxNumberMajorCycles=maxNumMajorCycles;
}; 
//----------------------------------------------------------------------
uInt ClarkCleanLatModel::getMaxNumberMajorCycles(){
  return itsMaxNumberMajorCycles;
};
//----------------------------------------------------------------------
void ClarkCleanLatModel::setMaxNumPix(const uInt maxNumPix ){
  itsMaxNumPix=maxNumPix;
}; 
//----------------------------------------------------------------------
uInt ClarkCleanLatModel::getMaxNumPix(){
  return itsMaxNumPix;
}; 
//----------------------------------------------------------------------
void ClarkCleanLatModel::setMaxExtPsf(const Float maxExtPsf ){
  itsMaxExtPsf=maxExtPsf;
};
//----------------------------------------------------------------------
Float ClarkCleanLatModel::getMaxExtPsf(){
  return itsMaxExtPsf;
}; 
//----------------------------------------------------------------------
void ClarkCleanLatModel::setSpeedup(const Float speedup ){
  itsSpeedup=speedup;
}; 
//----------------------------------------------------------------------
Float ClarkCleanLatModel::getSpeedup(){
  return itsSpeedup;
}; 
void ClarkCleanLatModel::setCycleFactor(const Float factor){
  itsCycleFactor=factor;
}

//----------------------------------------------------------------------
void ClarkCleanLatModel::setChoose(const Bool choose ){
  itsChoose=choose;
}; 
//----------------------------------------------------------------------
Bool ClarkCleanLatModel::getChoose(){
  return itsChoose;
}; 

//----------------------------------------------------------------------
void ClarkCleanLatModel::
doMinorIterations(CCList & activePixels, Matrix<Float> & psfPatch, 
		  Float fluxLimit, uInt & numberIterations, Float Fmn, 
		  const uInt totalIterations, Float& totalFlux) {
  const uInt ndim = itsModelPtr->ndim();
  DebugAssert(ndim >= 2, AipsError);
  const IPosition modelShape = itsModelPtr->shape();
  DebugAssert(modelShape(0) > 0, AipsError);
  DebugAssert(modelShape(1) > 0, AipsError);
  uInt npol = 1;
  if (ndim > 2) npol = modelShape(2);
  DebugAssert(activePixels.nPol() == npol , AipsError);
  DebugAssert(activePixels.nComp() > 0 , AipsError);
  DebugAssert( modelShape(0)*modelShape(1)*npol == uInt(modelShape.product()), AipsError);
  DebugAssert(psfPatch.nrow() > 0, AipsError);
  DebugAssert(psfPatch.ncolumn() > 0, AipsError);
  
//   itsLog << LogOrigin("ClarkCleanLatModel", "doMinorIterations");
  CCList cleanComponents(npol, 2, totalIterations);

  // Find the largest residual and its position.
  Block<Float> maxRes(npol);
  Float absRes;
  Float signedAbsRes;
  Int offRes;
  maxVect(maxRes, absRes, offRes, activePixels);
  Int *  maxPosPtr = activePixels.pixelPosition(offRes);
  Block<Int> maxPos(2, maxPosPtr, false);
  // declare variables used inside the main loop
  Int curIter = 0;
  Float iterFluxLimit = std::max(fluxLimit, threshold());
  Float Fac = pow(fluxLimit/absRes, itsSpeedup);
//   itsLog << "Initial maximum residual:" << maxRes 
// 	 << " (" << absRes << ") "
// 	 << " @ " << maxPos << endl;
  
  // Do the minor Iterations
  while ((curIter < Int(numberIterations)) && (absRes > iterFluxLimit)){
    // scale the maximum residual by the gain factor
    for (uInt i = 0; i < npol; i++) maxRes[i] *= gain();
    totalFlux += maxRes[0];
    // Add the new component to the clean components list
    cleanComponents.addComp(maxRes, maxPos);
    // Subtract the component from the current list of active pixels
    subtractComponent(activePixels, maxRes, maxPos, psfPatch);
    // We have now done an iteration
    curIter++;
    // find the next residual
    maxVect(maxRes, absRes, offRes, activePixels);
    maxPosPtr =  activePixels.pixelPosition(offRes);
    maxPos.replaceStorage(2, maxPosPtr, false);
    // Update the uncertainty factors and fluxlimits
    Fmn += Fac/Float(totalIterations+curIter);
    iterFluxLimit = std::max(fluxLimit * Fmn, threshold());

    if (itsProgressPtr) {
	signedAbsRes = absRes * maxRes[0]/abs( maxRes[0] );
	itsProgressPtr->
	  info(false, (Int)(totalIterations+curIter),  (Int)numberIterations,
	       signedAbsRes, IPosition(2,maxPos[0],maxPos[1]),
	       totalFlux, false, itsJustStarting);
        itsJustStarting = false;
    }
  }

  itsMaxRes= absRes;
  // Now copy the clean components into the image. 
  updateModel(cleanComponents);
  // Data returned to the main routine
  numberIterations = curIter;
  fluxLimit = absRes;
}
//----------------------------------------------------------------------
// Find all the pixels in the residual that are greater than fluxlimit, and
// store the values in the activePixels list. Increases the size of the list if
// necessary, but does not decrease it. This function will weight the residual
// using the mask (if set).
void ClarkCleanLatModel::
cacheActivePixels(CCList & activePixels,
		  const Lattice<Float> & residual, Float fluxLimit) {
  const uInt ndim = residual.ndim();
  DebugAssert(ndim >= 2, AipsError);
  const IPosition residualShape = residual.shape();
  DebugAssert(residualShape == itsModelPtr->shape(), AipsError);
  if (itsSoftMaskPtr != 0) {
    DebugAssert(residualShape(0) == itsSoftMaskPtr->shape()(0), AipsError);
    DebugAssert(residualShape(1) == itsSoftMaskPtr->shape()(1), AipsError);
  }
  DebugAssert(residualShape(0) > 0, AipsError);
  DebugAssert(residualShape(1) > 0, AipsError);
  Int npol = 1;
  if (ndim > 2) npol = residualShape(2);
  DebugAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  DebugAssert(npol == Int(activePixels.nPol()), AipsError);
  DebugAssert(residualShape(0)*residualShape(1)*npol == residualShape.product(), AipsError);
  
  IPosition cursorShape(ndim,1);
  Int tx, ty;
  {
    IPosition tileShape(residual.niceCursorShape());
    tx = cursorShape(0) = tileShape(0);
    ty = cursorShape(1) = tileShape(1);
    if (ndim > 2) cursorShape(2) = npol;
  }

  RO_LatticeIterator<Float> residualIter(residual, cursorShape);
  RO_LatticeIterator<Float> maskIter(residualIter); // There is no default ctr
  if (itsSoftMaskPtr != 0) {
    IPosition mCursorShape = cursorShape;
    mCursorShape(2) = 1;
    LatticeStepper mNav(itsSoftMaskPtr->shape(), mCursorShape, LatticeStepper::RESIZE);
    maskIter = RO_LatticeIterator<Float>(*itsSoftMaskPtr, mNav);
    maskIter.reset();
  }
  
  for (residualIter.reset(); !residualIter.atEnd(); residualIter++) {
    Bool residualCopy, maskCopy;
    const Float * residualPtr = residualIter.cursor().getStorage(residualCopy);
    const Float * maskPtr = 0;
    if (itsSoftMaskPtr != 0) {
      maskPtr = maskIter.cursor().getStorage(maskCopy);
    }

    Int nUsedPix = getbig(activePixels.freeFluxPtr(), 
			  activePixels.freePositionPtr(), 
			  activePixels.freeComp(), fluxLimit, 
			  residualPtr, maskPtr, npol, tx, ty);

    uInt lastLen=activePixels.nComp();

    const uInt reqLen = nUsedPix + activePixels.nComp();


    if (reqLen > activePixels.maxComp()) {
      // Need to resize the Pixel lists
      activePixels.resize(reqLen);
      // Now rescan the residual to get all the big pixels (in this iter.)
      nUsedPix = getbig(activePixels.freeFluxPtr(), 
			activePixels.freePositionPtr(), 
			activePixels.freeComp(), fluxLimit, 
			residualPtr, maskPtr, npol, tx, ty);
      
      activePixels.nComp() = reqLen;

      AlwaysAssert(nUsedPix + activePixels.nComp() == activePixels.maxComp(),
		   AipsError);
    }

    // Need to add the corner of the tile for the last set of components found!!!!
    Int offx=residualIter.position()(0);
    Int offy=residualIter.position()(1);
    //    itsLog << "Adding offset " << offx << " " << offy << LogIO::POST;
    for (uInt c=lastLen;c<activePixels.nComp();c++) {
      Int* locPtr=activePixels.pixelPosition(c);
      *locPtr+=offx;
      *(locPtr+1)+=offy;
    }

    residualIter.cursor().freeStorage(residualPtr, residualCopy);
    if (itsSoftMaskPtr != 0) {
      maskIter.cursor().freeStorage(maskPtr, maskCopy);
      maskIter++;
    }
  }
  //  AlwaysAssert(activePixels.nComp() > 0, AipsError);
}
//----------------------------------------------------------------------
// make histogram of absolute values in array
void ClarkCleanLatModel::absHistogram(Vector<Int> & hist,
				   Float & minVal, 
				   Float & maxVal, 
				   const Lattice<Float> & residual) {
  const IPosition residualShape = residual.shape();
  const uInt ndim = residualShape.nelements();
  Int npol = 1;
  if (residual.ndim() > 2) npol = residualShape(2);
  
  IPosition cursorShape(ndim,1);
  {
    IPosition tileShape(residual.niceCursorShape());
    cursorShape(0) = tileShape(0);
    cursorShape(1) = tileShape(1);
    if (ndim > 2) cursorShape(2) = npol;
  }
  LatticeStepper nav(residualShape, cursorShape, LatticeStepper::RESIZE);
  RO_LatticeIterator<Float> residualIter(residual, nav);
  RO_LatticeIterator<Float> maskIter(residualIter); // There is no default ctr
  if (itsSoftMaskPtr != 0) {
    IPosition mCursorShape = cursorShape;
    mCursorShape(2) = 1;
    LatticeStepper mNav(itsSoftMaskPtr->shape(), mCursorShape, LatticeStepper::RESIZE);
    maskIter = RO_LatticeIterator<Float>(*itsSoftMaskPtr, mNav);
    maskIter.reset();
  }

  hist = 0;
  const Int nbins = hist.nelements();
  Bool histIsACopy;
  Int * histPtr = hist.getStorage(histIsACopy);
  
  Bool residualIsACopy = false;
  const Float * residualPtr = 0;
  Bool maskIsACopy = false;
  const Float * maskPtr = 0;
  for (residualIter.reset(); !residualIter.atEnd(); residualIter++) {
    residualPtr = residualIter.cursor().getStorage(residualIsACopy);
    if (itsSoftMaskPtr != 0) {
      maskPtr = maskIter.cursor().getStorage(maskIsACopy);
    }
    const IPosition thisCursorShape = residualIter.cursorShape();
    const Int npix = thisCursorShape(0) * thisCursorShape(1);
    switch (npol) {
    case 1:
      if (itsSoftMaskPtr == 0) {
	abshisf(histPtr, &minVal, &maxVal, &nbins, residualPtr, &npix);
      } else {
	abshimf(histPtr, &minVal, &maxVal, &nbins, residualPtr, maskPtr, 
		&npix);
      }
      break;
    case 2:
      if (itsSoftMaskPtr == 0) {
	abshis2f(histPtr, &minVal, &maxVal, &nbins, residualPtr, &npix);
      } else {
	abshim2f(histPtr, &minVal, &maxVal, &nbins, residualPtr, maskPtr,
		 &npix);
      }
      break;
    case 4:
      if (itsSoftMaskPtr == 0) {
	abshis4f(histPtr, &minVal, &maxVal, &nbins, residualPtr, &npix);
      } else {
	abshim4f(histPtr, &minVal, &maxVal, &nbins, residualPtr, maskPtr,
		 &npix);
      }
      break;
    }
    if (itsSoftMaskPtr != 0) {
      maskIter.cursor().freeStorage(maskPtr, maskIsACopy);
      maskIter++;
    }
    residualIter.cursor().freeStorage(residualPtr, residualIsACopy);
  }
  hist.putStorage(histPtr, histIsACopy);
}

//----------------------------------------------------------------------
// Determine the flux limit if we only select the maxNumPix biggest
// residuals. Flux limit is not exact due to quantising by the histogram
Float ClarkCleanLatModel::biggestResiduals(Float & maxRes,
					const uInt maxNumPix, 
					const Float fluxLimit,
					const Lattice<Float> & residual) {
//   itsLog << LogOrigin("ClarkCleanLatModel", "biggestResiduals");
  // Calculate the histogram of the absolute value of the residuals
  Vector<Int> resHist(itsHistBins); 
//   itsLog << "Created a histogram with " << resHist.nelements() 
// 	 << " bins" << endl;;
  Float minRes;
  absHistogram(resHist, minRes, maxRes, residual);
//    itsLog << "Min/Max residuals are: " << minRes << " -> " << maxRes<< endl;
  
  // Deteremine how far we need to scan the histogram, before we reach the
  // flux cutoff imposed by the maximum exterior psf.
  Int lowbin=0;
  if (fluxLimit <= minRes)
    lowbin = 0;
  else if (fluxLimit < maxRes)
    lowbin=Int(itsHistBins*(fluxLimit-minRes)/(maxRes-minRes));

//   itsLog << "Select at most " << maxNumPix 
// 	 << " pixels with the lowest bin being " << lowbin << endl;

  Int numPix = 0;
  Int curBin = itsHistBins - 1;
  while (curBin >= lowbin && numPix <= Int(maxNumPix)){
    numPix += resHist(curBin);
    curBin--;
  }
  curBin++;
  
  // Try to ensure we have maxNumPix or fewer residuals selected UNLESS
  // the topmost bin contains more than maxNumPix pixels. Then use all the
  // pixels in the topmost bin.
  if (numPix > Int(maxNumPix) && curBin != Int(itsHistBins) - 1){
    numPix -= resHist(curBin); 
    curBin++;
  }
//   itsLog << "Selected " << numPix << " pixels from the top " 
// 	 << itsHistBins - curBin << " bins" << LogIO::POST;
  
  return minRes+curBin*(maxRes-minRes)/Float(itsHistBins);
}

//----------------------------------------------------------------------
// Work out the size of the Psf patch to use. 
Float ClarkCleanLatModel::getPsfPatch(Matrix<Float> & psfPatch,
				      LatConvEquation & eqn) {

  // Determine the maximum possible size that should be used. Sizes greater
  // than the maximum size cannot affect the cleaning and will not be used,
  // even if the user requests it!
  IPosition psfShape(eqn.psfSize());
  uInt ndim = psfShape.nelements();
  AlwaysAssert(ndim >= 2, AipsError);
  IPosition modelShape(itsModelPtr->shape());
  AlwaysAssert(modelShape.nelements() >= 2, AipsError);
  IPosition maxShape(2, 1);
  maxShape(0) = std::min(2*modelShape(0), psfShape(0));
  maxShape(1) = std::min(2*modelShape(1), psfShape(1));


  // See if the user has set a psfPatch size, and if it is less than the
  // maximum size use it.
  if (itsPsfPatchSize.nelements() != 0) {
    IPosition psfPatchSize(2, 1, 1);
    psfPatchSize(0) = std::min(maxShape(0), itsPsfPatchSize(0));
    psfPatchSize(1) = std::min(maxShape(1), itsPsfPatchSize(1));
    itsPsfPatchSize = psfPatchSize;
  } else {
    itsPsfPatchSize = maxShape;
  }

  // Now calculate the maximum exterior psf value
  // This is calculated where possible, otherwise a user supplied value is
  // used.

  // Check if Psf is big enough to do a proper calculation
  // This is the full psf, not the patch
  Lattice<Float> *psf_p = 0;
  Float maxExtPsf(0);
  IPosition beyond(psfShape.nelements(), 0);
  beyond(0) = itsPsfPatchSize(0)/2;
  beyond(1) = itsPsfPatchSize(1)/2;
  if (max((2*modelShape-psfShape).asVector()) <= 0){
    if ( (itsPsfPatchSize(0) == (2*modelShape(0))) && 
	 (itsPsfPatchSize(1) == (2*modelShape(1))) ) {
      maxExtPsf = Float(0); // Here the PsfPatch is used is big enough so
      // that exterior sidelobes are irrelevant
    }
    else { // Calculate the exterior sidelobes
      psf_p = eqn.evaluate(psfShape/2, Float(1), psfShape);
      maxExtPsf = absMaxBeyondDist(beyond, psfShape/2, *psf_p);
    }
  }
  else { // psf is not big enough so try and estimate something sensible

    if ((itsPsfPatchSize(0) == psfShape(0)) &&
	(itsPsfPatchSize(1) == psfShape(1)) ) 
      {
	maxExtPsf = itsMaxExtPsf; // must use the user supplied value as it is
				// impossible to estimate anything
      }  else { // try and estimate the ext. Psf and use the maximum of this
	// value and the user supplied value
	psf_p = eqn.evaluate(psfShape/2, Float(1), psfShape);
	Float tempMax = absMaxBeyondDist(beyond, psfShape/2, *psf_p);
	maxExtPsf = std::max(tempMax, itsMaxExtPsf);	

      }
  }
  if(psf_p) delete psf_p; psf_p=0;
  // set the max external psf Value to what is actually used. 
  // So the user can find out.
  itsMaxExtPsf = maxExtPsf;

  // Now get a psf of the required size


  {
    Array<Float> a_psfPatch;
    IPosition a_PsfPatchSize(psfShape.nelements(), 1);
    a_PsfPatchSize(0) = itsPsfPatchSize(0);
    a_PsfPatchSize(1) = itsPsfPatchSize(1);
    AlwaysAssert(a_PsfPatchSize.product() == itsPsfPatchSize.product(), AipsError);
    eqn.evaluate(a_psfPatch, beyond, Float(1), a_PsfPatchSize);
    psfPatch = a_psfPatch.reform(itsPsfPatchSize);
  }
  return maxExtPsf;
}
//----------------------------------------------------------------------
// The maximum residual is the absolute maximum.
Float ClarkCleanLatModel::maxResidual(const Lattice<Float> & residual) {
  const IPosition residualShape = residual.shape();
  const uInt ndim = residualShape.nelements();
  Int npol = 1;
  if (ndim > 2) npol = residualShape(2);

  IPosition cursorShape(ndim, 1);
  {
    IPosition tileShape(residual.niceCursorShape());
    cursorShape(0) = tileShape(0);
    cursorShape(1) = tileShape(1);
    if (ndim > 2) cursorShape(2) = npol;
  }
  LatticeStepper nav(residualShape, cursorShape, LatticeStepper::RESIZE);
  RO_LatticeIterator<Float> residualIter(residual, nav);
  RO_LatticeIterator<Float> maskIter(residualIter); // There is no default ctr
  if (itsSoftMaskPtr != 0) {
    IPosition mCursorShape = cursorShape;
    mCursorShape(2) = 1;
    LatticeStepper mNav(itsSoftMaskPtr->shape(), mCursorShape, LatticeStepper::RESIZE);
    maskIter = RO_LatticeIterator<Float>(*itsSoftMaskPtr, mNav);
    maskIter.reset();
  }

  Float maxVal = 0, iterMaxVal = 0;
  Bool residualIsACopy = false;
  const Float * residualPtr = 0;
  Bool maskIsACopy = false;
  const Float * maskPtr = 0;
  for (residualIter.reset(); !residualIter.atEnd(); residualIter++) {
    residualPtr = residualIter.cursor().getStorage(residualIsACopy);
    if (itsSoftMaskPtr != 0) {
      maskPtr = maskIter.cursor().getStorage(maskIsACopy);
    } 
    const IPosition thisCursorShape = residualIter.cursorShape();
    const Int npix = thisCursorShape(0) * thisCursorShape(1);
    switch (npol) {
    case 1:
      if (itsSoftMaskPtr == 0) {
	maxabsf(&iterMaxVal, residualPtr, &npix);
      } else {
	maxabmf(&iterMaxVal, residualPtr, maskPtr, &npix);
      }
      break;
    case 2:
      if (itsSoftMaskPtr == 0) {
	maxabs2f(&iterMaxVal, residualPtr, &npix);
      } else {
	maxabm2f(&iterMaxVal, residualPtr, maskPtr, &npix);
      }
      break;
    case 4:
      if (itsSoftMaskPtr == 0) {
	maxabs4f(&iterMaxVal, residualPtr, &npix);
      } else {
	maxabm4f(&iterMaxVal, residualPtr, maskPtr, &npix);
      }
    }
    if (iterMaxVal > maxVal) maxVal = iterMaxVal;
    if (itsSoftMaskPtr == 0) {
      maskIter.cursor().freeStorage(maskPtr, maskIsACopy);
      maskIter++;
    }
    residualIter.cursor().freeStorage(residualPtr, residualIsACopy);
  }
  return maxVal;
}
//----------------------------------------------------------------------
void ClarkCleanLatModel::
maxVect(Block<Float> & maxVal, Float & absVal, Int & offset, 
	const CCList & activePixels) {
  const Int npol = activePixels.nPol();
  const Int numComp = activePixels.nComp();
  DebugAssert(numComp > 0 , AipsError);
  DebugAssert((maxVal.nelements() == 1 || maxVal.nelements() == 2
	      || maxVal.nelements() == 4) , AipsError);

  const Float * dataPtr = activePixels.fluxPtr();
  Float * maxPtr = maxVal.storage();
  switch (npol) {
  case 1:
    absmaxf(maxPtr, &absVal, &offset, dataPtr, &numComp);
    break;
  case 2:
    absmax2f(maxPtr, &absVal, &offset, dataPtr, &numComp);
    break;
  case 4:
    absmax4f(maxPtr, &absVal, &offset, dataPtr, &numComp);
  }
}

//----------------------------------------------------------------------
void ClarkCleanLatModel::
subtractComponent(CCList & activePixels, const Block<Float> & maxVal,
		  const Block<Int> & maxPos, const Matrix<Float> & psfPatch) {
  const Int nx = psfPatch.nrow();
  const Int ny = psfPatch.ncolumn();
  
  const Int numComp = activePixels.nComp();
  Float * pixFluxPtr = activePixels.fluxPtr();
  const Int * pixPosPtr = activePixels.positionPtr();
  const Float * maxValPtr = maxVal.storage();
  const Int * maxPosPtr = maxPos.storage();
  Bool psfIsACopy;
  const Float * psfPtr = psfPatch.getStorage(psfIsACopy);
  switch (activePixels.nPol()){
  case 1:
    subcomf(pixFluxPtr, pixPosPtr, &numComp, maxValPtr, maxPosPtr, 
	    psfPtr, &nx, &ny);
    break;
  case 2:
    subcom2f(pixFluxPtr, pixPosPtr, &numComp, maxValPtr, maxPosPtr, 
	     psfPtr, &nx, &ny);
    break;
  case 4:
    subcom4f(pixFluxPtr, pixPosPtr, &numComp, maxValPtr, maxPosPtr, 
	     psfPtr, &nx, &ny);
    break;
  }
  psfPatch.freeStorage(psfPtr, psfIsACopy);
}

//----------------------------------------------------------------------
// For an Array make a vector which gives the peak beyond distance n, p(n):
// p(0)= central value, p(n)=max value outside hypercube with side 2n-1
// Distance is measured from the point centre in the array
Float ClarkCleanLatModel::absMaxBeyondDist(const IPosition & maxDist,
					const IPosition & centre,
					const Lattice<Float> & psf) {
  const IPosition psfShape = psf.shape();
  const uInt ndim = psfShape.nelements();
  const Int nx = psfShape(0);
  const Int ny = psfShape(1);
  AlwaysAssert(psfShape.product() == Int(nx*ny), AipsError);
  IPosition cursorShape(ndim, 1);
  cursorShape(0) = nx;
  LatticeStepper nav(psfShape, cursorShape);
  RO_LatticeIterator<Float> iter(psf, nav);
  Int miny = centre(1) - maxDist(1);
  Int maxy = centre(1) + maxDist(1);
  Int nleft = centre(0) - maxDist(0);
  Int nright = nx - (centre(0) + maxDist(0));
  Int rightoffset = centre(0) + maxDist(0);
  Float psfMax = 0, rowMax = 0;
  Bool psfIsACopy = false;
  const Float * psfPtr = 0;
  const Float * endPtr = 0;
  for (iter.reset(); !iter.atEnd(); iter++) {
    psfPtr = iter.cursor().getStorage(psfIsACopy);
    const IPosition cursorPos = iter.position();
    if (cursorPos(1) < miny || cursorPos(1) > maxy) { // do the whole row
      maxabsf(&rowMax, psfPtr, &nx);
    } else { // just the ends of the row
      maxabsf(&rowMax, psfPtr, &nleft);
      if (rowMax > psfMax) psfMax = rowMax;
      endPtr = psfPtr + rightoffset;
      maxabsf(&rowMax, endPtr, &nright);
      endPtr = 0;
    }
    iter.cursor().freeStorage(psfPtr, psfIsACopy);
    if (rowMax > psfMax) psfMax = rowMax;
  }
  return psfMax;
}

Bool ClarkCleanLatModel::stopnow() {
  if (itsChoose == true) {
    Vector<String> choices(2);
    choices(0) = "Continue";
    choices(1) = "Stop Now";
    choices(2) = "Don't ask again";
    String choice = Choice::choice("Do you want to continue or stop?",
				   choices);
    if (choice == choices(1)) {
      itsLog << "Clark clean stopped at user request" << LogIO::POST;
      return true;
    }
    if (choice == choices(2)) {
      setChoose(false);
      itsLog << "Continuing: won't ask again" << LogIO::POST;
    }
  }
  return false;
}

Int ClarkCleanLatModel::
getbig(Float const * pixValPtr, Int const * pixPosPtr, const Int nPix, 
       const Float fluxLimit, 
       const Float * const residualPtr, const Float * const maskPtr, 
       const uInt npol, const Int nx, const Int ny) 
{
  Int nUsedPix = nPix;
  switch (npol) {
  case 1:
    if (maskPtr == 0) {
      getbigf(pixValPtr, pixPosPtr, &nUsedPix, &fluxLimit, residualPtr,
	      &nx, &ny);
    } else {
      getbimf(pixValPtr, pixPosPtr, &nUsedPix, &fluxLimit, 
	      residualPtr, maskPtr, &nx, &ny);
    }
    break;
  case 2:
    if (maskPtr == 0) {
      getbig2f(pixValPtr, pixPosPtr, &nUsedPix, &fluxLimit, residualPtr,
	       &nx, &ny);
    } else {
      getbim2f(pixValPtr, pixPosPtr, &nUsedPix, &fluxLimit, 
	       residualPtr, maskPtr, &nx, &ny);
    }
    break;
  case 4:
    if (maskPtr == 0) {
      getbig4f(pixValPtr, pixPosPtr, &nUsedPix, &fluxLimit, residualPtr,
	       &nx, &ny);
    } else {
      getbim4f(pixValPtr, pixPosPtr, &nUsedPix, &fluxLimit, 
	       residualPtr, maskPtr, &nx, &ny);
    }
    break;
  }
  return nUsedPix;
}

void ClarkCleanLatModel::
updateModel(CCList & cleanComponents) {
  const IPosition modelShape(itsModelPtr->shape());
  const uInt ndim = modelShape.nelements();
  IPosition cursorShape(ndim,1);
  if (ndim > 2) cursorShape(2) = modelShape(2);
  IPosition cs = itsModelPtr->niceCursorShape();
  cleanComponents.tiledSort(cs);
  const Int * compPositionPtr;
  IPosition compPosition(ndim,0);
  Array<Float> compFlux(cursorShape), modelFlux(cursorShape);
  for(uInt c = 0; c < cleanComponents.nComp(); c++) {
    compPositionPtr = cleanComponents.pixelPosition(c);
    compPosition(0) = *compPositionPtr;
    compPosition(1) = *(compPositionPtr+1);
    compFlux.takeStorage(cursorShape, cleanComponents.pixelFlux(c), SHARE);
    itsModelPtr->getSlice(modelFlux, compPosition, cursorShape);
    //    itsLog << "Model " << modelFlux << " @ " << compPosition;
    modelFlux += compFlux;
    //    itsLog << " -> " << modelFlux;
    //    itsLog << " Subtracting:" << compFlux << " @ " << compPosition;
    itsModelPtr->putSlice(modelFlux, compPosition);
  }
}
// Local Variables: 
// compile-command: "gmake OPTLIB=1 ClarkCleanLatModel"
// End: 

} //# NAMESPACE CASA - END

