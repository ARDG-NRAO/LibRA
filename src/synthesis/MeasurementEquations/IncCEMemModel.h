//# IncCEMemModel.h: this defines IncCEMemModel
//# Copyright (C) 1996,1997,1998,1999,2000
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

#ifndef SYNTHESIS_INCCEMEMMODEL_H
#define SYNTHESIS_INCCEMEMMODEL_H


#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/lattices/Lattices/Lattice.h>
#include <casacore/images/Images/PagedImage.h>
#include <synthesis/MeasurementEquations/IncEntropy.h>
#include <synthesis/MeasurementEquations/LinearEquation.h>
#include <synthesis/MeasurementEquations/LinearModel.h>
#include <casacore/casa/Logging/LogIO.h>


namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declaration
class LatConvEquation;
class CEMemProgress;


// <summary> performs MEM algorithm incrementally </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite> 
// <li> ResidualEquation/ConvolutionEquation 
// <li> LinearModel/LinearEquation Paradigm 
// </prerequisite>
//
// <etymology>
// This class is called CEMemModel because it uses the Cornwell and
// Evans MEM algorithm to deconvolve the model.  The "Inc" is from Incremental,
// as the gradient is calculated from an incremental dirty image, but the
// entropy is calculated from the previous model plus the deltaModel
// </etymology>
//
// <synopsis>
// This class is used to perform the  Cornwell and Evans MEM Algorithm on an
// Array. Only the deconvolved model of the sky are directly stored by this
// class. The point spread function (psf) and convolved (dirty) image are
// stored in a companion class which is must be derived from
// ResidualEquation. 
// 
// The deconvolution works like this. The user constructs a CEMemModel by
// specifying an initial model of the sky. This can by be
// one,two,three... dimensional depending on the dimension of the psf (see
// below). The user then constructs a class which implements the forward
// equation between the model and the dirty image. Typically this will be
// the ConvolutionEquation class, although any class which has a
// ResidualEquation interface will work (but perhaps very slowly, as the
// ConvolutionEquation class has member functions optimised for CLEAN and MEM)
//
// The user then calls the solve() function (with the appropriate equation
// class as an arguement), and this class will perform the MEM deconvolution.
// The various MEM parameters are set (prior to calling solve) using the
// functions derived from the Iterate class, in particular
// setNumberIterations().
// 
// The solve() function does not return either the deconvolved model or the
// residuals. The solved model can be obtained using the getModel() function
// (derived from ArrayModel()) and the residual can be obtained using the
// residual() member function of the Convolution/Residual Equation Class.
// 
// The size and shape of the model used in this class MUST be the same as
// the convolved data (Dirty Image), stored in the companion
// ResidualEquation Class. However the model (and convolved data) can have
// more dimensions than the psf, as well as a different size (either larger
// or smaller). When the dimensionality is different the deconvolution is done
// independendtly in each "plane" of the model. (Note this has not
// been implemented yet but is relatively simple to do if necessary). 
//
// StokesVectors are not yet implemented.
//
// A companion class to this one is MaskedCEMemModel. This provides
// the same functionality but is used with MaskedArrays which indicate which
// regions of the NewtonRaphson (residual) image to apply when forming the
// step image (MaskedCEMemModel is not yet implemented).
//
// </synopsis>
//
// <example>
// <srcblock>
// casacore::Matrix<casacore::Float> psf(12,12), dirty(10,10), initialModel(10,10);
// ...put appropriate values into psf, dirty, & initialModel....
// CEMemModel<casacore::Float> deconvolvedModel(initialModel); 
// ConvolutionEquation convEqn(psf, dirty);
// deconvolvedModel.setSigma(0.001); 
// deconvolvedModel.setTargetFlux(-2.500); 
// deconvolvedModel.setNumberIterations(30);
// casacore::Bool convWorked = deconvolvedModel.solve(convEqn);
// casacore::Array<casacore::Float> finalModel, residuals;
// if (convWorked){
//   finalModel = deconvolvedModel.getModel();
//   ConvEqn.residual(deconvolvedModel, finalResidual);
// }
// </srcblock> 
// </example>
//
// <motivation>
// This class is needed to deconvolve extended images.  
// In time, the MEM algorithm will be a principle player in the 
// mosaicing stuff.
// </motivation>
//
// <templating arg=T>
//    For testing:
//    <li> casacore::Float: lets try it first
//    <li> StokesVector: will require lots more work
// </templating>
//
// <todo asof="1998/12/02">
//   <li> We need to implement soft Masking
// </todo>

class IncCEMemModel: public LinearModel< casacore::Lattice<casacore::Float> > 
{

  // Any new entropies derived from Entropy must sign the friend list:
friend class IncEntropy;
friend class IncEntropyI;
friend class IncEntropyEmptiness;


public:

  // Construct the CEMemModel object and initialise the model.
  IncCEMemModel(IncEntropy &ent, 
		casacore::Lattice<casacore::Float> & model,      // previous model, logically const
		casacore::Lattice<casacore::Float> & deltaModel, // this one changes 
		casacore::uInt nIntegrations = 10,
		casacore::Float sigma = 0.001,
		casacore::Float targetFlux = 1.0,
		casacore::Bool useFluxConstraint = false,
		casacore::Bool initializeModel = true,
		casacore::Bool imagePlane = false);

  // Construct the CEMemModel object, initialise the model and Prior images.
  IncCEMemModel(IncEntropy &ent, 
		casacore::Lattice<casacore::Float> & model,
		casacore::Lattice<casacore::Float> & deltaModel,
		casacore::Lattice<casacore::Float> & prior,
		casacore::uInt nIntegrations = 10,
		casacore::Float sigma = 0.001,
		casacore::Float targetFlux = 1.0,
		casacore::Bool useFluxConstraint = false,
		casacore::Bool initializeModel = true,
		casacore::Bool imagePlane = false);
  
  // Construct the CEMemModel object, initialise the model and 
  // and mask images.
  IncCEMemModel(IncEntropy &ent, 
		casacore::Lattice<casacore::Float> & model, 
		casacore::Lattice<casacore::Float> & deltaModel, 
		casacore::uInt nIntegrations,
		casacore::Lattice<casacore::Float> & mask,
		casacore::Float sigma = 0.001,
		casacore::Float targetFlux = 1.0,
		casacore::Bool useFluxConstraint = false,
		casacore::Bool initializeModel = true,
		casacore::Bool imagePlane = false);

  // Construct the CEMemModel object, initialise the model, Prior,
  // and mask images.
  IncCEMemModel(IncEntropy &ent, 
		casacore::Lattice<casacore::Float> & model, 
		casacore::Lattice<casacore::Float> & deltaModel, 
		casacore::Lattice<casacore::Float> & prior,
		casacore::Lattice<casacore::Float> & mask,
		casacore::uInt nIntegrations = 10,
		casacore::Float sigma = 0.001,
		casacore::Float targetFlux = 1.0,
		casacore::Bool useFluxConstraint = false,
		casacore::Bool initializeModel = true,
		casacore::Bool imagePlane = false);


  // destructor
  virtual ~IncCEMemModel();

  
  // solve the convolution equation
  // returns true if converged

  // Gives information about the state of the CEMem
  // (ie, using mask image, using prior image; more work here!)
  void state();


  //  This needs to be "ResidualEquation", using LatConvEquation as
  //  polymorphism is broken
  casacore::Bool solve(ResidualEquation<casacore::Lattice<casacore::Float> >  & eqn);

  // Set and get various state images and classes
  // <group>
  // set or get the Entropy class
  void setEntropy(IncEntropy &myEntropy ) {itsEntropy_ptr = &myEntropy;}
  void getEntropy(IncEntropy &myEntropy ) {myEntropy = *itsEntropy_ptr;}

  // set or get the Model image
  casacore::Lattice<casacore::Float>& getModel() const 
    { return (*(itsDeltaModel_ptr)); }
  void setModel(const casacore::Lattice<casacore::Float> & model)
    { itsDeltaModel_ptr = model.clone(); }

  // set or get the Prior image
  casacore::Lattice<casacore::Float>& getPrior() const 
    { return (*(itsPrior_ptr->clone())); }
  void setPrior(casacore::Lattice<casacore::Float> & prior);

  // set or get the Mask image
  casacore::Lattice<casacore::Float>& getMask() const 
    { return (*(itsMask_ptr->clone())); }
  void setMask(casacore::Lattice<casacore::Float> & mask);
  // </group>


  // set and get alpha, beta, and Q
  // <group>
  casacore::Float getAlpha() const { return itsAlpha; }
  casacore::Float getBeta() const { return itsBeta; }
  void setAlpha(casacore::Float alpha) {itsAlpha = alpha; }
  void setBeta(casacore::Float beta) {itsBeta = beta; }
  // </group>

  // Set various controlling parameters
  // (The most popular controlling parameters are 
  // set in the constructor)
  // <group>
  casacore::Float getTolerance() {return itsTolerance; }
  void  setTolerance(casacore::Float x) { itsTolerance = x; }
  casacore::Float getQ() {return itsQ; }
  void  setQ(casacore::Float x) { itsQ= x; }
  casacore::Float getGain() {return itsGain; }
  void  setGain(casacore::Float x) { itsGain = x; }
  casacore::Float getMaxNormGrad() {return itsMaxNormGrad; }
  void  setMaxNormGrad(casacore::Float x) { itsMaxNormGrad = x; }
  casacore::Int getInitialNumberIterations() {return itsFirstIteration; }
  void  setInitialNumberIterations(casacore::Int x) { itsFirstIteration = x; }
  // </group>

  // The convergence can also be in terms of the maximum residual
  // (ie, for use in stopping in a major cycle).
  void setThreshold (const casacore::Float x ) { itsThreshold0 = x; }
  // Thresh doubles in iter iterations
  void setThresholdSpeedup (const casacore::Float iter) {itsThresholdSpeedup = iter; }
  casacore::Float getThreshold();

  // Set/get the progress display 
  // <group>
  virtual void setProgress(CEMemProgress& ccp) { itsProgressPtr = &ccp; }
  virtual CEMemProgress& getProgress() { return *itsProgressPtr; }
  // </group>

  // return the number of iterations completed
  casacore::Int numberIterations () { return itsIteration; }

protected:


  // Perform One Iteration
  void oneIteration();

  // apply mask to a lattice; returns true if mask is available, 
  // false if not
  casacore::Bool applyMask( casacore::Lattice<casacore::Float> & lat );

  // Helper functions that interface with Entropy routines
  // Access to the entropy should be through this interface; the
  // points at which the Entropy is mentioned is then limited to
  // these lines right here, and to the constructors which set the
  // Entropy.  The entropy should not ever change the private
  // 
  // <group>
  void formEntropy() { itsEntropy = itsEntropy_ptr->formEntropy(); }

  void  formGDG() { itsEntropy_ptr->formGDG( itsGDG ); }

  void  formGDGStep() { itsEntropy_ptr->formGDGStep( itsGDG ); }

  void formGDS() { itsGradDotStep1=itsEntropy_ptr->formGDS(); }

  void entropyType(casacore::String &str) { itsEntropy_ptr->entropyType(str); }

  void relaxMin() { itsRequiredModelMin = itsEntropy_ptr->relaxMin(); }

  casacore::Bool testConvergence() { return itsEntropy_ptr->testConvergence(); }

  // </group>


  // protected generic constrcutor: DON'T USE IT!
  IncCEMemModel();

  // Set entropy pointer to zero: called by Entropy's destructor
  void letEntropyDie() { itsEntropy_ptr = 0; }

  // initialize itsStep and itsResidual and other stuff
  casacore::Bool initStuff();


  // controls how to change Alpha and Beta
  // <group>
  void changeAlphaBeta ();
  // initialize Alpha-Beta (called by changeAlphaBeta)
  void initializeAlphaBeta();
  // update Alpha-Beta (called by changeAlphaBeta)
  void updateAlphaBeta();
  // </group>




  // Generic utility functions
  // <group>
  // check that a single image is onf plausible shape
  casacore::Bool checkImage(const casacore::Lattice<casacore::Float> *);
  // check that the lattices and the underlying tile sizes are consistent
  casacore::Bool checkImages(const casacore::Lattice<casacore::Float> *one, const casacore::Lattice<casacore::Float> *other);
  // check that all is well in Denmark:
  //     ensure all images are the same size,
  //     ensure we have an entropy,
  //     ensure state variables have reasonable values
  casacore::Bool ok();
  // </group>
  



  // Helper functions for oneIteration:
  // <group>
  // calculate size of step
  void calculateStep();

  // take one step: clipped addition of
  // wt1*itsModel + wt2*itsStep
  void takeStep(casacore::Float wt1, casacore::Float wt2);

  // Calculate the flux, itsModMin, and itsModMax
  casacore::Float formFlux();
  // </group>

  // Determine if the peak residual is less than the getThreshold()
  casacore::Bool testConvergenceThreshold();

  // ------------Private Member casacore::Data---------------------
  // functional form of the entropy
  IncEntropy  *itsEntropy_ptr;   

  // form of the Residual method
  ResidualEquation< casacore::Lattice<casacore::Float> > * itsResidualEquation_ptr;

  // Images:
  casacore::Lattice<casacore::Float> * itsModel_ptr;
  casacore::Lattice<casacore::Float> * itsDeltaModel_ptr;
  casacore::Lattice<casacore::Float> * itsPrior_ptr;
  casacore::Lattice<casacore::Float> * itsMask_ptr;
  // Our OWN internal temp images; delete these upon destruction
  casacore::Lattice<casacore::Float> * itsStep_ptr;
  casacore::Lattice<casacore::Float> * itsResidual_ptr;


  // Controlling parameters
  // <group>
  casacore::Bool itsInitializeModel;
  casacore::uInt itsNumberIterations;
  casacore::Bool  itsDoInit;
  casacore::Float itsSigma;
  casacore::Float itsTargetFlux;  
  casacore::Float itsDefaultLevel;
  casacore::Float itsTargetChisq;
  // tolerance for convergence
  casacore::Float itsTolerance;	
  // N points per beam
  casacore::Float itsQ;		
  // gain for adding step image
  casacore::Float itsGain;	
  casacore::Float itsMaxNormGrad;
  // constrain flux or not?
  casacore::Bool  itsUseFluxConstraint;  
  // is this an image plane problem (like Single Dish or Optical?)
  casacore::Bool  itsDoImagePlane;
  casacore::Float itsThreshold0;
  casacore::Float itsThresholdSpeedup;
  // </group>

  // State variables
  // <group>  
  casacore::Float itsAlpha;
  casacore::Float itsBeta;
  casacore::Float itsNormGrad;
  // note that            itsModelFlux + itsDeltaFlux = itsFlux
  casacore::Float itsFlux;       // this is the total Flux
  casacore::Float itsModelFlux;  // this is the flux of image itsModel_ptr
  casacore::Float itsDeltaFlux;  // this is the flux of image itsDeltaModel_ptr
  casacore::Float itsChisq;
  // sqrt( chi^2/target_chi^2 )
  casacore::Float itsFit;	       
  // sqrt( chi^2/ Npixels )
  casacore::Float itsAFit;       
  // numerical value of entropy
  casacore::Float itsEntropy;    
  // Model is constrained to be >= itsNewMin
  casacore::Float itsRequiredModelMin; 
  // maximum pixel value in model
  casacore::Float itsModelMax;   
  // minimum pixel value n model
  casacore::Float itsModelMin;   
  casacore::Float itsLength;
  casacore::Double itsGradDotStep0;
  casacore::Double itsGradDotStep1;
  casacore::uInt   itsIteration;
  casacore::uInt   itsFirstIteration;
  // matrix of gradient dot products
  casacore::Matrix<casacore::Double> itsGDG;  
  casacore::Float itsCurrentPeakResidual;
  // </group>
  casacore::Float itsNumberPixels;


  // Accesories
  // <group>  
  casacore::Bool itsChoose;
  casacore::LogIO itsLog;
  // </group>  

  // Enumerate the different Gradient subscript types
  enum GradientType {
    // Entropy
    H=0,
    // Chi_sq
    C=1,
    // Flux
    F=2,
    // Objective function J
    J=3
  };

  CEMemProgress* itsProgressPtr;
 

};



} //# NAMESPACE CASA - END

#endif

