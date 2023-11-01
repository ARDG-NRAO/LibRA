//# ClarkCleanLatModel.h: this defines ClarkCleanLatModel
//# Copyright (C) 1996,1997,1998,1999,2000,2003
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
//# $Id$

#ifndef SYNTHESIS_CLARKCLEANLATMODEL_H
#define SYNTHESIS_CLARKCLEANLATMODEL_H

#include <casacore/casa/aips.h>
#include <synthesis/MeasurementEquations/LinearModel.h>
#include <synthesis/MeasurementEquations/LinearEquation.h>
#include <casacore/lattices/Lattices/Lattice.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <synthesis/MeasurementEquations/Iterate.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class ClarkCleanProgress;
class LatConvEquation;
class CCList;

// <summary>
// A Class for performing the Clark Clean Algorithm on Arrays
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite> 
// <li> ResidualEquation/LatConvEquation 
// <li> LinearModel/LinearEquation Paradigm 
// </prerequisite>
//
// <etymology>
// This class is called ClarkCleanLatModel because thats the algorithm it uses
// deconvolve the lattice-based model. 
// </etymology>
//
// <synopsis>
// This class is used to perform the Clark Clean Algorithm on an
// Array. Only the deconvolved model of the sky are directly stored by this
// class. The point spread function (psf) and convolved (dirty) image are
// stored in a companion class which is must be derived from
// ResidualEquation. 
// 
// The cleaning works like this. The user constructs a ClarkCleanLatModel by
// specifying an initial model of the sky. This can by be
// one,two,three... dimensional depending on the dimension of the psf (see
// below). The user then constructs a class which implements the forward
// equation between the model and the dirty image. Typically this will be
// the ConvolutionEquation class, although any class which has a
// ResidualEquation interface will be work (but perhaps very slowly, as the
// ConvolutionEquation class has member functions optimised for cleaning)
//
// The user then calls the solve() function (with the appropriate equation
// class as an arguement), and this class will perform the Clark clean.
// The various clean parameters are set (prior to calling solve) using the
// functions derived from the Iterate class, in particular setGain(),
// setNumberIterations() & setThreshold() (to set a flux limit). 
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
// or smaller). When the dimensionality is different the cleaning is done
// independendtly in each "plane" of the model. (Note this has not
// been implemented yet but is relatively simple to do if necessary). 
//
// This multi-dimensionalty is exploited when cleaning arrays of
// StokesVectors. Here the casacore::Array of StokesVectors is decomposed into a stack
// of 4 Floating point arrays and the cleaning is done on all the the arrays
// simultaneosly. The criterion for choosing the brightest pixel has been
// generalised by using the "length" of the Stokesvector in 4 dimensional
// space. 
//
// A companion class to this one is MaskedClarkCleanLatModel. This provides
// the same functionality but is used with MaskedArrays which indicate which
// regions of the model to search for clean components. 
//
// </synopsis>
//
// <example>
// <srcblock>
// casacore::Matrix<casacore::Float> psf(12,12), dirty(10,10), initialModel(10,10);
// ...put appropriate values into psf, dirty, & initialModel....
// ClarkCleanLatModel<casacore::Float> deconvolvedModel(initialModel); 
// ConvolutionEquation convEqn(psf, dirty);
// deconvolvedModel.setGain(0.2); 
// deconvolvedModel.setNumberIterations(1000);
// casacore::Bool convWorked = deconvolvedModel.solve(convEqn);
// if (convWorked)
//   ConvEqn.residual(deconvolvedModel, finalResidual);
// </srcblock> 
// </example>
//
// <motivation>
// This class is needed to deconvolve images.
// </motivation>
//
// <templating arg=T>
// I have tested this class with Arrays of
//    <li> Float
//    <li> StokesVector
// </templating>
//
// <todo asof="1996/05/02">
//   <li> Make changes so that multidimensions work as advertised
//   <li> compare timing with other clean implementations (ie, Mark's
//   CleanTools, SDE, AIPS & miriad) 
// </todo>

class ClarkCleanLatModel: 
  public LinearModel< casacore::Lattice<casacore::Float> >,
  public Iterate
{
public:
  // The default constructor does nothing more than initialise a zero length
  // array to hold the deconvolved model. If this constructor is used then 
  // the actual model must be set using the setModel() function of the
  // LatticeModel class.
  ClarkCleanLatModel();

  // Construct the ClarkCleanLatModel object and initialise the model.
  ClarkCleanLatModel(casacore::Lattice<casacore::Float> & model);

  // Construct the ClarkCleanLatModel object and initialise the model ans mask
  ClarkCleanLatModel(casacore::Lattice<casacore::Float> & model, casacore::Lattice<casacore::Float> & mask);

  // Construct the ClarkCleanLatModel object and initialise the model ans mask
  ClarkCleanLatModel(casacore::Lattice<casacore::Float> & model, casacore::Lattice<casacore::Float> & residual, 
		     casacore::Lattice<casacore::Float> & mask);
  // Destroy!
  virtual ~ClarkCleanLatModel();

  virtual const casacore::Lattice<casacore::Float> & getModel() const { return *itsModelPtr; }
  virtual void setModel(const casacore::Lattice<casacore::Float> & model);
  virtual void setModel(casacore::Lattice<casacore::Float> & model);

  const casacore::Lattice<casacore::Float> & getMask() const;
  void setMask(const casacore::Lattice<casacore::Float> & mask);

  void setResidual( casacore::Lattice<casacore::Float> & residual);
  virtual const casacore::Lattice<casacore::Float> & getResidual() const { return *itsResidualPtr; }

  casacore::Int getNumberIterations(){ return numberIterations(); }

  casacore::Float getMaxResidual() { return itsMaxRes;};
  // Using a Clark clean deconvolution proceedure solve for an improved
  // estimate of the deconvolved object. The convolution/residual equation
  // contains the psf and dirty image. When called with a ResidualEquation
  // arguement a quite general interface is used that is slow. The
  // convolution equation contains functions that speed things up. The
  // functions return false if the deconvolution could not be done.
  // <group>
  casacore::Bool solve(LatConvEquation & eqn);
  casacore::Bool singleSolve(LatConvEquation & eqn, casacore::Lattice<casacore::Float> & residual);
  // </group>

  // The user can be asked whether to stop after every minor cycle
  // <group>
  virtual void setChoose(const casacore::Bool askForChoice);
  virtual casacore::Bool getChoose();
  // </group>

  // These remaining functions set various "knobs" that the user can tweak and
  // are specific to the Clark clean algorithm. The more generic parameters
  // ie. clean gain, and maximum residual fluxlimit, are set using functions in
  // the Iterate base class. The get functions return the value that was
  // actually used after the cleaning was done.

  // set the size of the PSF used in the minor iterations. If not set it
  // defaults to the largest useful Psf (ie. min(modelSize*2, psfSize))
  // <group>
  virtual void setPsfPatchSize(const casacore::IPosition & psfPatchSize); 
  virtual casacore::IPosition getPsfPatchSize(); 
  // </group>

  // Set the size of the histogram used to determine how many pixels are
  // "active" in a minor iteration. Default value of 1000 is OK for
  // everything except very small cleans.
  // <group>
  virtual void setHistLength(const casacore::uInt histBins); 
  virtual casacore::uInt getHistLength();
  // </group>
 
  // Set the maximum number of minor iterations to perform for each major
  // cycle. 
  // <group>
  virtual void setMaxNumberMinorIterations(const casacore::uInt maxNumMinorIterations); 
  virtual casacore::uInt getMaxNumberMinorIterations();
  // </group>

  // Set and get the initial number of iterations
  // <group>
  virtual void setInitialNumberIterations(const casacore::uInt initialNumberIterations); 
  virtual casacore::uInt getInitialNumberIterations();
  // </group>

  // Set the maximum number of major cycles to perform
  // <group>
  virtual void setMaxNumberMajorCycles(const casacore::uInt maxNumMajorCycles); 
  virtual casacore::uInt getMaxNumberMajorCycles();
  // </group>

  // Set the maximum number of active pixels to use in the minor
  // iterations. The specified number can be exceeded if the topmost bin of
  // the histogram contains more pixels than specified here. The default is
  // 10,000 which is suitable for images of 512by512 pixels. Reduce this for
  // smaller images and increase it for larger ones. 
  // <group>
  virtual void setMaxNumPix(const casacore::uInt maxNumPix ); 
  virtual casacore::uInt getMaxNumPix(); 
  // </group>


  // Set the maximum exterior psf value. This is used to determine when to
  // stop the minor itartions. This is normally determined from the psf and
  // the number set here is only used if this cannot be determined. The
  // default is zero.
  // <group>
  virtual void setMaxExtPsf(const casacore::Float maxExtPsf ); 
  virtual casacore::Float getMaxExtPsf(); 
  // </group>

  // The total flux density in the model.
  casacore::Float modelFlux();

  // An exponent on the F(m,n) factor (see Clark[1980]) which influences how
  // quickly active pixels are treated as unreliable. Larger values mean
  // more major iterations. The default is zero. I have no experience on
  // when to use this factor.
  // <group>
  virtual void setSpeedup(const casacore::Float speedup); 
  virtual casacore::Float getSpeedup();
  // </group>
  //Set the cycle factor....the larger this is the shallower is the minor
  //cycle
  virtual void setCycleFactor(const casacore::Float factor);


  // Set/get the progress display 
  // <group>
  virtual void setProgress(ClarkCleanProgress& ccp) { itsProgressPtr = &ccp; }
  virtual ClarkCleanProgress& getProgress() { return *itsProgressPtr; }
  // </group>

private:
// Do all the minor iterations for one major cycle. Cleaning stops
// when the flux or iteration limit is reached.
  void doMinorIterations(CCList & activePixels,
			 casacore::Matrix<casacore::Float> & psfPatch,
			 casacore::Float fluxLimit, 
			 casacore::uInt & numberIterations, 
			 casacore::Float Fmn, 
			 const casacore::uInt totalIterations,
			 casacore::Float& totalFlux);

  void cacheActivePixels(CCList & activePixels, 
			const casacore::Lattice<casacore::Float> & residual, casacore::Float fluxLimit);

  // make histogram of absolute values in array
  void absHistogram(casacore::Vector<casacore::Int> & hist, casacore::Float & minVal, 
		    casacore::Float & maxVal, const casacore::Lattice<casacore::Float> & data);

  // Determine the flux limit if we only select the maxNumPix biggest
  // residuals. Flux limit is not exact due to quantising by the histogram
  casacore::Float biggestResiduals(casacore::Float & maxRes, const casacore::uInt maxNumPix, 
			 const casacore::Float fluxLimit, 
			 const casacore::Lattice<casacore::Float> & residual);

// Work out the size of the Psf patch to use. 
  casacore::Float getPsfPatch(casacore::Matrix<casacore::Float> & psfPatch, LatConvEquation & eqn);

// The maximum residual is the absolute maximum.
  casacore::Float maxResidual(const casacore::Lattice<casacore::Float> & residual);
  void maxVect(casacore::Block<casacore::Float> & maxVal, casacore::Float & absVal, casacore::Int & offset,
	       const CCList & activePixels);
  void subtractComponent(CCList & activePixels, const casacore::Block<casacore::Float> & maxVal,
			 const casacore::Block<casacore::Int> & maxPos, const casacore::Matrix<casacore::Float> & psf);
  casacore::Float absMaxBeyondDist(const casacore::IPosition & maxDist, const casacore::IPosition & centre,
			 const casacore::Lattice<casacore::Float> & psf);
  casacore::Bool stopnow();
  casacore::Int  getbig(casacore::Float const * pixValPtr, casacore::Int const * pixPosPtr, const casacore::Int nPix, 
	      const casacore::Float fluxLimit, 
	      const casacore::Float * const residualPtr, const casacore::Float * const maskPtr, 
	      const casacore::uInt npol, const casacore::Int nx, const casacore::Int ny);

  void updateModel(CCList & cleanComponents);

  casacore::Lattice<casacore::Float> * itsModelPtr;
  casacore::Lattice<casacore::Float> * itsResidualPtr;
  const casacore::Lattice<casacore::Float> * itsSoftMaskPtr;
  casacore::uInt itsMaxNumPix;
  casacore::uInt itsHistBins;
  casacore::uInt itsMaxNumberMinorIterations;
  casacore::uInt itsInitialNumberIterations;
  casacore::Int itsMaxNumberMajorCycles;
  casacore::Float itsMaxExtPsf;
  casacore::Float itsMaxRes;
  casacore::IPosition itsPsfPatchSize;
  casacore::Bool itsChoose;
  casacore::Float itsSpeedup;
  casacore::Float itsCycleFactor;
  casacore::LogIO itsLog;
  ClarkCleanProgress* itsProgressPtr;
  casacore::Bool itsJustStarting;
  casacore::Bool itsWarnFlag;

  casacore::Bool itsLocalResTL;

};


} //# NAMESPACE CASA - END

#endif
