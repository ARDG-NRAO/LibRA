//# ClarkCleanModel.cc:  this defines ClarkCleanModel
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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

#include <synthesis/MeasurementEquations/ClarkCleanModel.h>
#include <synthesis/MeasurementEquations/ClarkCleanProgress.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/ArrayError.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/VectorIter.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/iostream.h> 
#include <casacore/casa/System/Choice.h>

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
  void getbigf(Float * pixVal, Int * pixPos, Int * maxPix, 
	       const Float * fluxLimit, const Float * arr, 
	       const Int * nx, const Int * ny);
  void getbig2f(Float * pixVal, Int * pixPos, Int * maxPix, 
		const Float * fluxLimit, const Float * arr, 
		const Int * nx, const Int * ny);
  void getbig4f(Float * pixVal, Int * pixPos, Int * maxPix, 
		const Float * fluxLimit, const Float * arr, 
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
  void getbimf(Float * pixVal, Int * pixPos, Int * maxPix, 
	       const Float * fluxLimit, const Float * arr, const Float * mask, 
	       const Int * nx, const Int * ny);
  void getbim2f(Float * pixVal, Int * pixPos, Int * maxPix, 
		const Float * fluxLimit, const Float * arr, const Float * mask,  
		const Int * nx, const Int * ny);
  void getbim4f(Float * pixVal, Int * pixPos, Int * maxPix, 
		const Float * fluxLimit, const Float * arr, const Float * mask,  
		const Int * nx, const Int * ny);
};


//----------------------------------------------------------------------
ClarkCleanModel::ClarkCleanModel()
  :ArrayModel<Float>(),
   theHistBins(1024), 
   theMaxExtPsf(0.0),
   theMaxNumberMinorIterations(10000),
   theInitialNumberIterations(0),
   theMaxNumberMajorCycles(-1),
   theMaxNumPix(32*1024),
   thePsfPatchSize(2,51,51),
   theSpeedup(0.0),
   theCycleSpeedup(-1.0),
   theChoose(false),
   theMask(),
   theLog(LogOrigin("ClarkCleanModel", "ClarkCleanModel()")),
   theIterCounter(0),
   itsProgressPtr(0),
   itsJustStarting(true)
{
};
//----------------------------------------------------------------------
ClarkCleanModel::ClarkCleanModel(Array<Float> & model)
  :ArrayModel<Float>(model),
   theHistBins(1024), 
   theMaxExtPsf(0.0),
   theMaxNumberMinorIterations(10000),
   theInitialNumberIterations(0),
   theMaxNumberMajorCycles(-1),
   theMaxNumPix(32*1024),
   thePsfPatchSize(2, 51, 51),
   theSpeedup(0.0), 
   theCycleSpeedup(-1.0), 
   theChoose(false),
   theMask(),
   theLog(LogOrigin("ClarkCleanModel", 
		    "ClarkCleanModel(const Array<Float> & model)")),
   theIterCounter(0),
   itsProgressPtr(0),
   itsJustStarting(true)
{
  AlwaysAssert(theModel.ndim() >= 2, AipsError);
  if (theModel.ndim() >= 3)
    AlwaysAssert(theModel.shape()(2) == 1 || theModel.shape()(2) == 2 || 
		 theModel.shape()(2) == 4, AipsError);
  if (theModel.ndim() >= 4)
    for (uInt i = 3; i < theModel.ndim(); i++)
      AlwaysAssert(theModel.shape()(i) == 1, AipsError);
//      theLog << LogOrigin("ClarkCleanModel", "ClarkCleanModel") 
// 	    << "Model shape is:" << theModel.shape() << endl;
};
//----------------------------------------------------------------------
ClarkCleanModel::ClarkCleanModel(Array<Float> & model, 
				 Array<Float> & mask)
  :ArrayModel<Float>(model),
   theHistBins(1024), 
   theMaxExtPsf(0.0),
   theMaxNumberMinorIterations(10000),
   theInitialNumberIterations(0),
   theMaxNumberMajorCycles(-1),
   theMaxNumPix(32*1024),
   thePsfPatchSize(2, 51, 51),
   theSpeedup(0.0), 
   theCycleSpeedup(-1.0), 
   theChoose(false),
   theMask(mask),
   theLog(LogOrigin("ClarkCleanModel", 
		    "ClarkCleanModel(Array<Float> & model"
		    ", Array<Float> & mask)")),
   theIterCounter(0),
   itsProgressPtr(0),
   itsJustStarting(true)
{
     AlwaysAssert(theModel.ndim() >= 2, AipsError);
     if (theModel.ndim() >= 3)
       AlwaysAssert(theModel.shape()(2) == 1 || theModel.shape()(2) == 2 || 
		    theModel.shape()(2) == 4, AipsError);
     if (theModel.ndim() >= 4)
       for (uInt i = 3; i < theModel.ndim(); i++)
	 AlwaysAssert(theModel.shape()(i) == 1, AipsError);

     AlwaysAssert(theMask.ndim() >= 2, AipsError);
     AlwaysAssert(theMask.shape()(0) == theModel.shape()(0), AipsError);
     AlwaysAssert(theMask.shape()(1) == theModel.shape()(1), AipsError);
     if (theMask.ndim() >= 3)
       for (uInt i = 2; i < theMask.ndim(); i++)
	 AlwaysAssert(theMask.shape()(i) == 1, AipsError);
};

void ClarkCleanModel::getModel(Array<Float>& model) const{
  model = theModel;
};
void ClarkCleanModel::setModel(const Array<Float>& model){
  AlwaysAssert(model.ndim() >= 2, AipsError);
  if (model.ndim() >= 3)
    AlwaysAssert(model.shape()(2) == 1 || model.shape()(2) == 2 || 
		 model.shape()(2) == 4, AipsError);
  if (model.ndim() >= 4)
    for (uInt i = 3; i < model.ndim(); i++)
      AlwaysAssert(model.shape()(i) == 1, AipsError);
  theModel = model;
};
void ClarkCleanModel::setModel(Array<Float> & model){
  AlwaysAssert(model.ndim() >= 2, AipsError);
  if (model.ndim() >= 3)
    AlwaysAssert(model.shape()(2) == 1 || model.shape()(2) == 2 || 
		 model.shape()(2) == 4, AipsError);
  if (model.ndim() >= 4)
    for (uInt i = 3; i < model.ndim(); i++)
      AlwaysAssert(model.shape()(i) == 1, AipsError);
  theModel.reference(model);
};

void ClarkCleanModel::getMask(Array<Float>& mask) const{
  mask = theMask;
};
void ClarkCleanModel::setMask(const Array<Float>& mask){
  AlwaysAssert(mask.ndim() >= 2, AipsError);
  AlwaysAssert(mask.shape()(0) == theModel.shape()(0), AipsError);
  AlwaysAssert(mask.shape()(1) == theModel.shape()(1), AipsError);
  if (mask.ndim() >= 3)
    for (uInt i = 2; i < mask.ndim(); i++)
      AlwaysAssert(mask.shape()(i) == 1, AipsError);
  theMask = mask;
};
void ClarkCleanModel::setMask(Array<Float> & mask){
  AlwaysAssert(mask.ndim() >= 2, AipsError);
  AlwaysAssert(mask.shape()(0) == theModel.shape()(0), AipsError);
  AlwaysAssert(mask.shape()(1) == theModel.shape()(1), AipsError);
  if (mask.ndim() >= 3)
    for (uInt i = 2; i < mask.ndim(); i++)
      AlwaysAssert(mask.shape()(i) == 1, AipsError);
  theMask.reference(mask);
};


//----------------------------------------------------------------------
Bool ClarkCleanModel::solve(ConvolutionEquation & eqn){
  theLog << LogOrigin("ClarkCleanModel", "solve");
  AlwaysAssert(theModel.ndim() >= 2, AipsError);
  const IPosition dataShape = theModel.shape();
  Int npol = 1;
  if (theModel.ndim() >= 3)
    npol = dataShape(2);
  AlwaysAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  
  // Determine the number of polarisations
//   theLog << "Data has " << npol << " polarisations" << LogIO::POST;
    
  // compute the current residual image (using an FFT)
  Array<Float> residual;
  eqn.residual(residual, *this);
  
  // Determine the psf patch to use 
  Matrix<Float> psfPatch;
  Float maxExtPsf;
  maxExtPsf = getPsfPatch(psfPatch, eqn);
//   theLog << "PsfPatch shape is: " << psfPatch.shape() 
//  	 << " and has a maximum exterior sidelobe of " 
//  	 << maxExtPsf << LogIO::POST;
  
  // Declare variables needed inside the following while loop
  Float minLimit;                 // the min flux limit when using the
				  // maximum number of active pixels
  Int numPix;                     // the number of Active pixels
  Int maxNumPix = 0;              // The max. number of active pixels ever used
  uInt numIterations = theInitialNumberIterations;
                                  // Number of Iterations done so far
  uInt numMinorIterations;        // The number of minor iterations done/todo
  uInt numMajorCycles = 0;        // The number of major cycles done
  uInt maxNumberMinorIterations = 0;// The max. number of min. iterations
				   // ever used
  Matrix<Float> pixelValue;       // cache of "active" pixel values
  Matrix<Int> pixelPos;           // cache of "active" pixel positions
  Float Fmn=1;                    // The "uncertainty" factor 
  Float fluxLimit;                // The fluxlimit for the current major cycle
  Float totalFlux = 0;

  // Note that a Matrix is used rather than say a Vector of IPositions as
  // it allows the inner loop (in doMinorIterations()) to be more highly
  // optimised (using pointers)

  // find its maximum value of the residual
  Float maxRes = maxResidual(residual);

  theLog << "Initial maximum residual: " << maxRes << LogIO::POST;
  // if flux limit or iteration limit reached then bail out. 
  Bool userHalt = false;
  while ((Int(numIterations) < numberIterations()) && 
	 (maxRes > threshold()) &&
	 ((theMaxNumberMajorCycles<0)||(numMajorCycles<(uInt)theMaxNumberMajorCycles)) &&
	 userHalt == false){

    // determine the fluxlimit for this major cycle
    // choose fluxlimit for residuals to be considered in minor iterations
    // don't consider residuals below maxRes*(value of maxPsf at outside the 
    // patch)
    fluxLimit = maxRes * maxExtPsf;
//     theLog << "Fluxlimit determined using the Maximum exterior Psf: " 
//  	   << fluxLimit << LogIO::POST;
    // See if this flux limit needs to be modified because it selects too
    // many pixels.
    minLimit = biggestResiduals(maxRes, theMaxNumPix, fluxLimit, residual);
//     theLog << "Fluxlimit determined using the maximum number active pixels: " 
//  	   << minLimit << endl;
    fluxLimit = max(fluxLimit, minLimit);
    fluxLimit /=3.0; //This factor was found empirically as
                     // as it it was too conservative in the minor loop        
//     theLog << "Final Fluxlimit: " << fluxLimit << LogIO::POST;
    
    // Copy all the active pixels into separate areas for better memory
    // management and quicker indexing.
    numPix = cacheActivePixels(pixelValue, pixelPos, residual,
		      max(fluxLimit,threshold()));
    // The numpix calculated here frequently differs 
    // from the number calculated using the histogram, because of the
    // quantisation of the flux levels in the histogram, and the imposition
    // of an external fluxlevel.
    if (numPix > 0) {
//       theLog <<"Major cycle has "<< numPix << " active residuals, "
// 	     << "a Fluxlimit of " << max(fluxLimit,threshold()) << endl;
      // Start of minor cycles
      numMinorIterations = min(theMaxNumberMinorIterations,
			       numberIterations()-numIterations);
      doMinorIterations(theModel, pixelValue, pixelPos, numPix, 
  			psfPatch, fluxLimit, numMinorIterations, 
			Fmn, numIterations, totalFlux);
      numIterations += numMinorIterations;
//       theLog << "Clean has used " << numIterations << " Iterations" ;
      maxNumberMinorIterations = max(maxNumberMinorIterations,
				     numMinorIterations);
      maxNumPix = max(maxNumPix, numPix);
      // Now do a  major cycle
      eqn.residual(residual, *this);

      // find the new maximum residual 
      maxRes = maxResidual(residual);
      theLog << "Iteration: " << numIterations
	     << ", Maximum residual=" << maxRes << LogIO::POST;
// 	     << " Flux limit=" << max(fluxLimit,threshold()) 
// 	     << ", " << numPix << " Active pixels" << LogIO::POST;
      
//       theLog << " to get to a maximum residual of " << maxRes << LogIO::POST;

      // Count the number of major cycles
      numMajorCycles++;
    }
    else{
      theLog << LogIO::WARN 
      	     << "Zero Pixels selected with a Fluxlimit of " << fluxLimit
      	     << " and a maximum Residual of " << maxRes << LogIO::POST;
      userHalt=true;
    }
    userHalt = userHalt || stopnow();
  }
  setThreshold(maxRes);
  setNumberIterations(numIterations);
  theMaxNumPix = maxNumPix;
  theMaxNumberMinorIterations = maxNumberMinorIterations;
  return true;
};

//----------------------------------------------------------------------
Bool ClarkCleanModel::singleSolve(ConvolutionEquation & eqn,
				  Array<Float>& residual){
  theLog << LogOrigin("ClarkCleanModel", "singleSolve");
  AlwaysAssert(theModel.ndim() >= 2, AipsError);
  const IPosition dataShape = theModel.shape();
  Int npol = 1;
  if (theModel.ndim() >= 3)
    npol = dataShape(2);
  AlwaysAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  
  // Determine the number of polarisations
//   theLog << "Data has " << npol << " polarisations" << LogIO::POST;
    
  // Determine the psf patch to use 
  Matrix<Float> psfPatch;
  Float maxExtPsf;
  maxExtPsf = getPsfPatch(psfPatch, eqn);
//   theLog << "PsfPatch shape is: " << psfPatch.shape() 
//  	 << " and has a maximum exterior sidelobe of " 
//  	 << maxExtPsf << LogIO::POST;
  
  // Declare variables needed inside the following while loop
  Float minLimit;                 // the min flux limit when using the
				  // maximum number of active pixels
  Int numPix;                     // the number of Active pixels
  Int maxNumPix = 0;              // The max. number of active pixels ever used
  uInt numIterations = theInitialNumberIterations;
                                  // Number of Iterations done so far
  uInt numMinorIterations;        // The number of minor iterations done/todo
  uInt maxNumberMinorIterations = 0;// The max. number of min. iterations
				   // ever used
  Matrix<Float> pixelValue;       // cache of "active" pixel values
  Matrix<Int> pixelPos;           // cache of "active" pixel positions
  Float Fmn=1;                    // The "uncertainty" factor 
  Float fluxLimit;                // The fluxlimit for the current major cycle
  Float totalFlux = 0;

  // Note that a Matrix is used rather than say a Vector of IPositions as
  // it allows the inner loop (in doMinorIterations()) to be more highly
  // optimised (using pointers)

  // find its maximum value of the residual
  Float maxRes = maxResidual(residual);

  // move this to later TT
  //theLog << "Initial maximum residual: " << maxRes << LogIO::POST;
  //
  // if flux limit or iteration limit reached then bail out. 
  
  // determine the fluxlimit for this major cycle
  // choose fluxlimit for residuals to be considered in minor iterations
  // don't consider residuals below maxRes*(value of maxPsf at outside the 
  // patch)
  fluxLimit = maxRes * maxExtPsf;
  //     theLog << "Fluxlimit determined using the Maximum exterior Psf: " 
  //  	   << fluxLimit << LogIO::POST;
  // See if this flux limit needs to be modified because it selects too
  // many pixels.
  minLimit = biggestResiduals(maxRes, theMaxNumPix, fluxLimit, residual);
  //     theLog << "Fluxlimit determined using the maximum number active pixels: " 
  //  	   << minLimit << endl;
  fluxLimit = max(fluxLimit, minLimit);
  fluxLimit /= 8.0; //emepirically found that fluxlimit was too
                    // too conservative  
  //     theLog << "Final Fluxlimit: " << fluxLimit << LogIO::POST;
  
  // Copy all the active pixels into separate areas for better memory
  // management and quicker indexing.
  numPix = cacheActivePixels(pixelValue, pixelPos, residual,
			     max(fluxLimit,threshold()));
  // The numpix calculated here frequently differs 
  // from the number calculated using the histogram, because of the
  // quantisation of the flux levels in the histogram, and the imposition
  // of an external fluxlevel.
  if (numPix > 0) {
    //       theLog <<"Major cycle has "<< numPix << " active residuals, "
    // 	     << "a Fluxlimit of " << max(fluxLimit,threshold()) << endl;
    // Start of minor cycles
    theLog << "Initial maximum residual: " << maxRes << LogIO::POST;
    numMinorIterations = min(theMaxNumberMinorIterations,
			     numberIterations()-numIterations);
    doMinorIterations(theModel, pixelValue, pixelPos, numPix, 
		      psfPatch, fluxLimit, numMinorIterations, 
		      Fmn, numIterations, totalFlux);
    numIterations += numMinorIterations;
    //       theLog << "Clean has used " << numIterations << " Iterations" ;
    maxNumberMinorIterations = max(maxNumberMinorIterations,
				   numMinorIterations);
    maxNumPix = max(maxNumPix, numPix);
  }
  /*
  else
    theLog << LogIO::WARN 
	   << "Zero Pixels selected with a Fluxlimit of " << fluxLimit
	   << " and a maximum Residual of " << maxRes << LogIO::POST;
  */
  setNumberIterations(numIterations);
  theMaxNumPix = maxNumPix;
  theMaxNumberMinorIterations = maxNumberMinorIterations;

  return true;
};
//----------------------------------------------------------------------
void ClarkCleanModel::setPsfPatchSize(const IPosition & psfPatchSize){
  thePsfPatchSize=psfPatchSize;
}; 
//----------------------------------------------------------------------
IPosition ClarkCleanModel::getPsfPatchSize(){
  return thePsfPatchSize;
}; 
//----------------------------------------------------------------------
void ClarkCleanModel::setHistLength(const uInt HistBins ){
  theHistBins=HistBins;
}; 
//----------------------------------------------------------------------
uInt ClarkCleanModel::getHistLength(){
  return theHistBins;
}; 
//----------------------------------------------------------------------
void ClarkCleanModel::setMaxNumberMinorIterations(const uInt maxNumMinorIterations){
  theMaxNumberMinorIterations=maxNumMinorIterations;
}; 
//----------------------------------------------------------------------
uInt ClarkCleanModel::getMaxNumberMinorIterations(){
  return theMaxNumberMinorIterations;
};
//----------------------------------------------------------------------
void ClarkCleanModel::setInitialNumberIterations(const uInt initialNumberIterations){
  theInitialNumberIterations=initialNumberIterations;
}; 
//----------------------------------------------------------------------
uInt ClarkCleanModel::getInitialNumberIterations(){
  return theInitialNumberIterations;
};
//----------------------------------------------------------------------
void ClarkCleanModel::setMaxNumberMajorCycles(const uInt maxNumMajorCycles){
  theMaxNumberMajorCycles=maxNumMajorCycles;
}; 
//----------------------------------------------------------------------
uInt ClarkCleanModel::getMaxNumberMajorCycles(){
  return theMaxNumberMajorCycles;
};
//----------------------------------------------------------------------
void ClarkCleanModel::setMaxNumPix(const uInt maxNumPix ){
  theMaxNumPix=maxNumPix;
}; 
//----------------------------------------------------------------------
uInt ClarkCleanModel::getMaxNumPix(){
  return theMaxNumPix;
}; 
//----------------------------------------------------------------------
void ClarkCleanModel::setMaxExtPsf(const Float maxExtPsf ){
  theMaxExtPsf=maxExtPsf;
};
//----------------------------------------------------------------------
Float ClarkCleanModel::getMaxExtPsf(){
  return theMaxExtPsf;
}; 
//----------------------------------------------------------------------
void ClarkCleanModel::setSpeedup(const Float speedup ){
  theSpeedup=speedup;
}; 
//----------------------------------------------------------------------
Float ClarkCleanModel::getSpeedup(){
  return theSpeedup;
}; 
//----------------------------------------------------------------------
void ClarkCleanModel::setCycleSpeedup(const Float speedup ){
  theCycleSpeedup=speedup;
}; 
//----------------------------------------------------------------------
Float ClarkCleanModel::getCycleSpeedup(){
  return theCycleSpeedup;
}; 
//----------------------------------------------------------------------
void ClarkCleanModel::setChoose(const Bool choose ){
  theChoose=choose;
}; 
//----------------------------------------------------------------------
Bool ClarkCleanModel::getChoose(){
  return theChoose;
}; 
//----------------------------------------------------------------------
void ClarkCleanModel::doMinorIterations(Array<Float> & model, 
					Matrix<Float> & pixVal, 
					const Matrix<Int> & pixPos, 
					const Int numPix,
					Matrix<Float> & psfPatch,
					Float fluxLimit, 
					uInt & numberIterations, 
					Float Fmn, 
					const uInt totalIterations,
					Float &totalFlux){
  DebugAssert(model.ndim() >= 2, AipsError);
  DebugAssert(model.shape()(0) > 0, AipsError);
  DebugAssert(model.shape()(1) > 0, AipsError);
  Int npol = 1;
  if (model.ndim() >= 3)
    npol = model.shape()(2);
  DebugAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  DebugAssert(model.shape()(3) == 1, AipsError);
  DebugAssert(Int(pixVal.nrow()) == npol, AipsError);
  DebugAssert(numPix <= Int(pixVal.ncolumn()), AipsError);
  DebugAssert(0 < numPix , AipsError);
  DebugAssert(pixPos.nrow() == 2, AipsError);
  DebugAssert(pixPos.ncolumn() == pixVal.ncolumn(), AipsError);
  DebugAssert(psfPatch.nrow() > 0, AipsError);
  DebugAssert(psfPatch.ncolumn() > 0, AipsError);
  
//   theLog << LogOrigin("ClarkCleanModel", "doMinorIterations");
  // Find the largest residual and its position.
  Vector<Float> maxRes(npol);
  Vector<Int> maxPos(2);
  Float absRes;
  Float signedAbsRes;
  Int offRes;
  maxVect(maxRes, absRes, offRes, pixVal, numPix);
  maxPos = pixPos.column(offRes);
  // declare variables used inside the main loop
  Int curIter = 0;
  Float iterFluxLimit = max(fluxLimit, threshold());
  Float Fac = pow(fluxLimit/absRes, theSpeedup);
  IPosition position(model.ndim(), 0);
//   theLog << "Initial maximum residual:" << maxRes 
// 	 << " (" << absRes << ") "
// 	 << " @ " << maxPos << endl;
  
  // Do the minor Iterations
  while ((curIter < Int(numberIterations)) && (absRes > iterFluxLimit)){
    iterFluxLimit = max(fluxLimit, threshold());  // threshold() changes now!
    maxRes *= gain();
    totalFlux += maxRes(0);
    // Add the new component to the current model
    position(0) = maxPos(0);
    position(1) = maxPos(1);
    if (model.ndim() >= 3)
      for (Int p = 0; p < npol; p++){
	position(2) = p;
// 	theLog << "Model " << model(position) << " @ " << position;
	model(position) += maxRes(p);
// 	theLog << " -> " << model(position);
      }
    else {
//       theLog << "Model " << model(position) << " @ " << position;
      model(position) += maxRes(0);
//       theLog << " -> " << model(position);
    }
    
//     theLog << " Subtracting:" << maxRes 
//   	   << " @ " << position;
    // Subtract the component from the current list of active pixels
    subtractComponent(pixVal, pixPos, numPix, 
		      maxRes, maxPos, psfPatch);
    // We have now done an iteration
    curIter++;
    theIterCounter++;
    // find the next residual
    maxVect(maxRes, absRes, offRes, pixVal, numPix);
    maxPos = pixPos.column(offRes);
//     theLog << " After Iteration: " << curIter 
//  	   << " the Maximum residual is:" << maxRes 
// 	   << " (" << absRes << ") "
//   	   << " @ " << maxPos << LogIO::POST;
    // Update the uncertainty factors and fluxlimits
    Fmn += Fac/Float(totalIterations+curIter);
    iterFluxLimit = max(fluxLimit * Fmn, threshold());

    if (itsProgressPtr) {
      try {
        // if this does not throw an exception, we are in business
        itsProgressPtr->hasPGPlotter();  
        signedAbsRes = absRes * maxRes(0)/abs( maxRes(0) );
        itsProgressPtr->
          info(false, (Int)(totalIterations+curIter),  (Int)numberIterations,
               signedAbsRes, IPosition(2,maxPos(0),maxPos(1)),
               totalFlux, false, itsJustStarting );
	itsJustStarting = false;
      } catch (AipsError x) {
        // if it throw an exception, do nothing
      } 
    }
  }
  // Data returned to the main routine
  numberIterations = curIter;
  fluxLimit = absRes;
  itsMaxRes = maxRes(0);
};
//----------------------------------------------------------------------
Int ClarkCleanModel::
cacheActivePixels(Matrix<Float> & pixVal, Matrix<Int> & pixPos, 
		  const Array<Float> & data, const Float fluxLimit){
  DebugAssert(data.ndim() >= 2, AipsError);
  const IPosition dataShape = data.shape();
  const Int nx = dataShape(0);
  const Int ny = dataShape(1);
  Int npol = 1;
  if (data.ndim() >= 3)
    npol = dataShape(2);
  DebugAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  DebugAssert(nx > 0, AipsError);
  DebugAssert(ny > 0, AipsError);
  DebugAssert(pixVal.ncolumn() == pixPos.ncolumn(), AipsError);

  Int nBigPix = pixVal.ncolumn();
  Bool dataCopy, valCopy, posCopy;
  const Float * dataPtr = data.getStorage(dataCopy);
  Float * valPtr = pixVal.getStorage(valCopy);
  Int * posPtr = pixPos.getStorage(posCopy);

  if (theMask.nelements() == 0) {
    switch (npol){
    case 1:
      getbigf(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, &nx, &ny);
      break;
    case 2:
      getbig2f(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, &nx, &ny);
      break;
    case 4:
      getbig4f(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, &nx, &ny);
    }
    if (nBigPix > 0){ // I could be more efficient about this
      nBigPix += pixVal.ncolumn();
      pixVal.putStorage(valPtr, valCopy); pixPos.putStorage(posPtr, posCopy);
      pixVal.resize(npol, nBigPix);
      pixPos.resize(2, nBigPix);
      valPtr = pixVal.getStorage(valCopy); posPtr = pixPos.getStorage(posCopy);
      switch (npol){
      case 1:
	getbigf(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, &nx, &ny);
	break;
      case 2:
	getbig2f(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, &nx, &ny);
	break;
      case 4:
	getbig4f(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, &nx, &ny);
      }
      AlwaysAssert(nBigPix == 0, AipsError);
    }
  }
  else {
    Bool maskCopy;
    const Float * maskPtr = theMask.getStorage(maskCopy);
    switch (npol){
    case 1:
      getbimf(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, maskPtr, &nx, &ny);
      break;
    case 2:
      getbim2f(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, maskPtr, &nx, &ny);
      break;
    case 4:
      getbim4f(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, maskPtr, &nx, &ny);
    }
    if (nBigPix > 0){ // I could be more effecient about this
      nBigPix += pixVal.ncolumn();
      pixVal.putStorage(valPtr, valCopy); pixPos.putStorage(posPtr, posCopy);
      pixVal.resize(npol, nBigPix);
      pixPos.resize(2, nBigPix);
      valPtr = pixVal.getStorage(valCopy); posPtr = pixPos.getStorage(posCopy);
      switch (npol){
      case 1:
	getbimf(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, maskPtr, &nx, &ny);
	break;
      case 2:
	getbim2f(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, maskPtr, &nx, &ny);
	break;
      case 4:
	getbim4f(valPtr, posPtr, &nBigPix, &fluxLimit, dataPtr, maskPtr, &nx, &ny);
      }
      AlwaysAssert(nBigPix == 0, AipsError);
    }
    theMask.freeStorage(maskPtr, dataCopy);
  }
  pixVal.putStorage(valPtr, valCopy);
  pixPos.putStorage(posPtr, posCopy);
  data.freeStorage(dataPtr, dataCopy);
  DebugAssert(nBigPix <= 0 && (nBigPix + Int(pixVal.ncolumn())) >= Int(0), AipsError);
  return pixVal.ncolumn() + nBigPix;
};
//----------------------------------------------------------------------
// make histogram of absolute values in array
void ClarkCleanModel::absHistogram(Vector<Int> & hist,
				   Float & minVal, 
				   Float & maxVal, 
				   const Array<Float> & data) {
  DebugAssert(data.ndim() >= 2, AipsError);
  const IPosition dataShape = data.shape();
  const Int nx = dataShape(0);
  const Int ny = dataShape(1);
  Int npol = 1;
  if (data.ndim() >= 3)
    npol = dataShape(2);
  const Int npix = nx*ny;
  DebugAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  DebugAssert(nx > 0, AipsError);
  DebugAssert(ny > 0, AipsError);
  Bool dataCopy, histCopy;
  const Int nbins = hist.nelements();
  const Float * dataPtr = data.getStorage(dataCopy);
  Int * histPtr = hist.getStorage(histCopy);
  hist = 0;
  if (theMask.nelements() == 0)
    switch (npol){
    case 1:
      abshisf(histPtr, &minVal, &maxVal, &nbins, dataPtr, &npix);
      break;
    case 2:
      abshis2f(histPtr, &minVal, &maxVal, &nbins, dataPtr, &npix);
      break;
    case 4:
      abshis4f(histPtr, &minVal, &maxVal, &nbins, dataPtr, &npix);
    }
  else{
    Bool maskCopy;
    const Float * maskPtr = theMask.getStorage(maskCopy);
    switch (npol){
    case 1:
      abshimf(histPtr, &minVal, &maxVal, &nbins, dataPtr, maskPtr, &npix);
      break;
    case 2:
      abshim2f(histPtr, &minVal, &maxVal, &nbins, dataPtr, maskPtr, &npix);
      break;
    case 4:
      abshim4f(histPtr, &minVal, &maxVal, &nbins, dataPtr, maskPtr, &npix);
    }
    theMask.freeStorage(maskPtr, dataCopy);
  }
  data.freeStorage(dataPtr, dataCopy);
  hist.putStorage(histPtr, histCopy);
  
};
//----------------------------------------------------------------------
// Determine the flux limit if we only select the maxNumPix biggest
// residuals. Flux limit is not exact due to quantising by the histogram
Float ClarkCleanModel::biggestResiduals(Float & maxRes,
					const uInt maxNumPix, 
					const Float fluxLimit,
					const Array<Float> & residual) {
//   theLog << LogOrigin("ClarkCleanModel", "biggestResiduals");
  // Calculate the histogram of the absolute value of the residuals
  Vector<Int> resHist(theHistBins); 
//   theLog << "Created a histogram with " << resHist.nelements() 
// 	 << " bins" << endl;;
  Float minRes;
  absHistogram(resHist, minRes, maxRes, residual);
//    theLog << "Min/Max residuals are: " << minRes << " -> " << maxRes<< endl;
  
  // Deteremine how far we need to scan the histogram, before we reach the
  // flux cutoff imposed by the maximum exterior psf.
  Int lowbin;
  if (fluxLimit <= minRes)
    lowbin = 0;
  else if (fluxLimit >= maxRes)
    lowbin = theHistBins - 1;
  else
    lowbin=Int(theHistBins*(fluxLimit-minRes)/(maxRes-minRes));

//   theLog << "Select at most " << maxNumPix 
// 	 << " pixels with the lowest bin being " << lowbin << endl;

  Int numPix = 0;
  Int curBin = theHistBins - 1;
  while (curBin >= lowbin && numPix <= Int(maxNumPix)){
    numPix += resHist(curBin);
    curBin--;
  }
  curBin++;
  
  // Try to ensure we have maxNumPix or fewer residuals selected UNLESS
  // the topmost bin contains more than maxNumPix pixels. Then use all the
  // pixels in the topmost bin.
  if (numPix > Int(maxNumPix) && curBin != Int(theHistBins - 1)){
    numPix -= resHist(curBin); 
    curBin++;
  }
//   theLog << "Selected " << numPix << " pixels from the top " 
// 	 << theHistBins - curBin << " bins" << LogIO::POST;
  
  return minRes+curBin*(maxRes-minRes)/Float(theHistBins);
}
//----------------------------------------------------------------------
// Work out the size of the Psf patch to use. 
Float ClarkCleanModel::getPsfPatch(Array<Float>& psfPatch, 
				   ConvolutionEquation& eqn) {

  // Determine the maximum possible size that should be used. Sizes greater
  // than the maximum size cannot affect the cleaning and will not be used,
  // even if the user requests it!
  IPosition psfSize(eqn.psfSize());
  uInt ndim = psfSize.nelements();
  IPosition modelSize(theModel.shape().getFirst(ndim));
  IPosition maxSize(min(2*modelSize.asVector(), 
			psfSize.asVector()));
  // See if the user has set a psfPatch size, and if it is less than the
  // maximum size use it.
  IPosition psfPatchSize;
  if (thePsfPatchSize.nelements() != 0) {
    psfPatchSize = casacore::min(maxSize.asVector(),
		       thePsfPatchSize.asVector());
  }
  else {
    psfPatchSize = maxSize;
  }
  // set the psf Patch size to what is actually used. So the user can find out.
  thePsfPatchSize = psfPatchSize;

  // Now calculate the maximum exterior psf value
  
  // This is calculated where possible, otherwise a user supplied value is
  // used.

  // Check if Psf is big enough to do a proper calculation
  Array<Float> psf;
  Float maxExtPsf(0);
  if (max((2*modelSize-psfSize).asVector()) <= 0){
    if (psfPatchSize.isEqual(2*modelSize)) {
      maxExtPsf = Float(0); // Here the PsfPatch is used is big enough so
			    // that exterior sidelobes are irrelevant
    }
    else { // Calculate the exterior sidelobes
      eqn.evaluate(psf, psfSize/2, Float(1), psfSize);
      maxExtPsf = absMaxBeyondDist(psfPatchSize/2, psfSize/2, psf);
    }
  }
  else { // psf is not big enough so try and estimate something sensible
    if (psfPatchSize.isEqual(psfSize)) {
      maxExtPsf = theMaxExtPsf; // must use the user supplied value as it is
				// impossible to estimate anything
    }
    else { // try and estimate the ext. Psf and use the maximum of this
	   // value and the user supplied value
      eqn.evaluate(psf, psfSize/2, Float(1), psfSize);
      maxExtPsf = max(absMaxBeyondDist(psfPatchSize/2, psfSize/2, psf),
		      theMaxExtPsf);
    }
  }
  eqn.flushPsf(); // Tell the convolution equation to release the cached psf
  // set the max external psf Value to what is actually used. 
  // So the user can find out.
  theMaxExtPsf = maxExtPsf;
  // Now get a psf of the required size
  eqn.evaluate(psfPatch, psfPatchSize/2, Float(1), psfPatchSize);
  return maxExtPsf;
};
//----------------------------------------------------------------------
// The maximum residual is the absolute maximum.
Float ClarkCleanModel::maxResidual(const Array<Float> & residual) {
  DebugAssert(residual.ndim() >= 2, AipsError);
  const IPosition dataShape = residual.shape();
  const Int nx = dataShape(0);
  const Int ny = dataShape(1);
  Int npol = 1;
  if (residual.ndim() >= 3)
    npol = dataShape(2);
  const Int npix = nx*ny;
  DebugAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  DebugAssert(nx > 0, AipsError);
  DebugAssert(ny > 0, AipsError);

  Float maxVal;
  Bool dataCopy;
  const Float * dataPtr = residual.getStorage(dataCopy);
  if (theMask.nelements() == 0)
    switch (npol){
    case 1:
      maxabsf(&maxVal, dataPtr, &npix);
      break;
    case 2:
      maxabs2f(&maxVal, dataPtr, &npix);
      break;
    case 4:
      maxabs4f(&maxVal, dataPtr, &npix);
    }
  else {
    Bool maskCopy;
    const Float * maskPtr = theMask.getStorage(maskCopy);
    switch (npol){
    case 1:
      maxabmf(&maxVal, dataPtr, maskPtr, &npix);
      break;
    case 2:
      maxabm2f(&maxVal, dataPtr, maskPtr, &npix);
      break;
    case 4:
      maxabm4f(&maxVal, dataPtr, maskPtr, &npix);
    }
    theMask.freeStorage(maskPtr, dataCopy);
  }
  residual.freeStorage(dataPtr, dataCopy);
  return maxVal;
};
//----------------------------------------------------------------------
void ClarkCleanModel::maxVect(Vector<Float> & maxVal, 
			      Float & absVal, 
			      Int & offset, 
			      const Matrix<Float> & pixVal, 
			      const Int numPix) {
  const Int npol = pixVal.nrow();
  DebugAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  DebugAssert(numPix <= Int(pixVal.ncolumn()), AipsError);
  DebugAssert(0 < numPix , AipsError);

  Bool dataCopy, maxCopy;
  const Float * dataPtr = pixVal.getStorage(dataCopy);
  Float * maxPtr = maxVal.getStorage(maxCopy);
  switch (npol){
  case 1:
    absmaxf(maxPtr, &absVal, &offset, dataPtr, &numPix);
    break;
  case 2:
    absmax2f(maxPtr, &absVal, &offset, dataPtr, &numPix);
    break;
  case 4:
    absmax4f(maxPtr, &absVal, &offset, dataPtr, &numPix);
  }
  pixVal.freeStorage(dataPtr, dataCopy);
  maxVal.putStorage(maxPtr, dataCopy);
};
//----------------------------------------------------------------------
void ClarkCleanModel::subtractComponent(Matrix<Float> & pixVal, 
					const Matrix<Int> & pixPos,
					const Int numPix,
					const Vector<Float> & maxVal,
					const Vector<Int> & maxPos, 
					const Matrix<Float> & psf){
  const Int npol = pixVal.nrow();
  DebugAssert(npol == 1 || npol == 2 || npol == 4, AipsError);
  DebugAssert(numPix <= Int(pixVal.ncolumn()), AipsError);
  DebugAssert(0 < numPix , AipsError);
  DebugAssert(pixPos.nrow() == 2, AipsError);
  DebugAssert(pixPos.ncolumn() == pixVal.ncolumn(), AipsError);
  DebugAssert(Int(maxVal.nelements()) == npol, AipsError);
  DebugAssert(maxPos.nelements() == 2, AipsError);
  const Int nx = psf.nrow();
  const Int ny = psf.ncolumn();
  DebugAssert(nx > 0, AipsError);
  DebugAssert(ny > 0, AipsError);
  
  Bool pixValCopy, pixPosCopy, maxValCopy, maxPosCopy, psfCopy;
  Float * pixValPtr = pixVal.getStorage(pixValCopy);
  const Int * pixPosPtr = pixPos.getStorage(pixPosCopy);
  const Float * maxValPtr = maxVal.getStorage(maxValCopy);
  const Int * maxPosPtr = maxPos.getStorage(maxPosCopy);
  const Float * psfPtr = psf.getStorage(psfCopy);
  switch (npol){
  case 1:
    subcomf(pixValPtr, pixPosPtr, &numPix, maxValPtr, maxPosPtr, 
	    psfPtr, &nx, &ny);
    break;
  case 2:
    subcom2f(pixValPtr, pixPosPtr, &numPix, maxValPtr, maxPosPtr, 
	     psfPtr, &nx, &ny);
    break;
  case 4:
    subcom4f(pixValPtr, pixPosPtr, &numPix, maxValPtr, maxPosPtr, 
	     psfPtr, &nx, &ny);
  }
  psf.freeStorage(psfPtr, psfCopy);
  maxPos.freeStorage(maxPosPtr, maxPosCopy);
  maxVal.freeStorage(maxValPtr, maxValCopy);
  pixPos.freeStorage(pixPosPtr, pixPosCopy);
  pixVal.putStorage(pixValPtr, pixValCopy);
};
//----------------------------------------------------------------------
// For an Array make a vector which gives the peak beyond distance n, p(n):
// p(0)= central value, p(n)=max value outside hypercube with side 2n-1
// Distance is measured from the point centre in the array
Float ClarkCleanModel::absMaxBeyondDist(const IPosition &maxDist, 
					const IPosition &centre,
					const Array<Float> &array){
  if (maxDist.nelements() != array.ndim()) {
    throw(ArrayError("Vector<Float> absMaxBeyondDist("
		     "const IPosition &maxDist,const IPosition &centre,"
		     "const Array<Float> &array) - "
		     "maxDist dimensionality inconsistent with array"));
  }
  if (array.nelements() == 0) {
    throw(ArrayError("Vector<Float> absMaxBeyondDist("
		     "const IPosition &maxDist,const IPosition &centre,"
		     "const Array<Float> &array) - "
		     "Array has no elements"));     
  }
  { 
    Vector<Int> tmp1 = (centre-maxDist).asVector();
    Vector<Int> tmp2 = (centre+maxDist-array.endPosition()).asVector();
    if (min(tmp1)<0 || max(tmp2)>0) {
      throw(ArrayError("Vector<Float> absMaxBeyondDist("
		       "const IPosition &maxDist,const IPosition &centre,"
		       "const Array<Float> &array) - "
		       "maxDist too large for Array"));       
    }
  }
  // Initialize
  ReadOnlyVectorIterator<Float> ai(array);
  Float maxVal(0);
  uInt n = ai.vector().nelements();
  uInt start = centre(0) - maxDist(0);
  uInt end = centre(0) + maxDist(0) + 1;
  IPosition vecPos(array.ndim());
  IPosition vecDist(array.ndim());
  uInt i;

  // loop though array accumulating maxima for each distance
  while (! ai.pastEnd()) {
    // find the distance of the current vector to the midpoint of the array
    vecPos=ai.pos(); vecPos(0)=centre(0);
    vecDist=abs((vecPos-centre).asVector());
    // skip if current vector too far from midpoint in any dimension
    if (max((vecDist-maxDist).asVector()) > 0)
      for (i=0; i<n; i++) {
	maxVal=max(maxVal, Float(abs(ai.vector()(i))));
      }
    else {
      for (i=0; i<start; i++) {
	maxVal=max(maxVal, Float(abs(ai.vector()(i))));
      }
      for (i=end; i<n; i++) {
	maxVal=max(maxVal, Float(abs(ai.vector()(i))));
      }
    }
    ai.next();
  }
  return maxVal;
};

Bool ClarkCleanModel::stopnow() {
  if(theChoose) {
    Vector<String> choices(2);
    choices(0)="Continue";
    choices(1)="Stop Now";
    choices(2)="Don't ask again";
    String choice = Choice::choice("Do you want to continue or stop?",
				   choices);
    if (choice==choices(0)) {
      return false;
    }
    else if (choice==choices(2)) {
      setChoose(false);
      theLog << "Continuing: won't ask again" << LogIO::POST;
      return false;
    }
    else {
      theLog << "Clark clean stopped at user request" << LogIO::POST;
      return true;
    }
  }
  else {
    return false;
  }
}

Float ClarkCleanModel::threshold()
{
  Float thresh = Iterate::threshold();
  if (theCycleSpeedup > 0.0) {
    thresh = thresh * pow(2.0, ((Double)(theIterCounter)/theCycleSpeedup) );
  }
  return thresh;
};

} //# NAMESPACE CASA - END

