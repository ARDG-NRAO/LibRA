//# Cleaner.h: this defines Cleaner a class for doing convolution
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
//# $Id: LatticeCleaner.h 20739 2009-09-29 01:15:15Z Malte.Marquarding $

#ifndef SYNTHESIS_MATRIXCLEANER_H
#define SYNTHESIS_MATRIXCLEANER_H

//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/lattices/Lattices/TempLattice.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/lattices/LatticeMath/LatticeCleaner.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations
class MatrixNACleaner;
// <summary>A copy of casacore::LatticeCleaner but just using 2-D matrices</summary>
// <synopsis> It is a mimic of the casacore::LatticeCleaner class but avoid a lot of
// of the lattice to array and back copies and uses openmp in the obvious places
// </synopsis>

// <summary>A class for doing multi-dimensional cleaning</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tLatticeCleaner">
// </reviewed>

// <prerequisite>
//  <li> The mathematical concept of deconvolution
// </prerequisite>
//
// <etymology>

// The MatrixCleaner class will deconvolve 2-D arrays of floats.

// </etymology>
//
// <synopsis>
// This class will perform various types of Clean deconvolution
// on Lattices.
//
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// </motivation>
//
// <thrown>
// <li> casacore::AipsError: if psf has more dimensions than the model.
// </thrown>
//
// <todo asof="yyyy/mm/dd">
// </todo>

class MatrixCleaner
{
public:

  // Create a cleaner : default constructor
  MatrixCleaner();

  // Create a cleaner for a specific dirty image and PSF
  MatrixCleaner(const casacore::Matrix<casacore::Float> & psf, const casacore::Matrix<casacore::Float> & dirty);

  // The copy constructor uses reference semantics
  MatrixCleaner(const MatrixCleaner& other);

  // The assignment operator also uses reference semantics
  MatrixCleaner & operator=(const MatrixCleaner & other);

  // The destructor does nothing special.
  ~MatrixCleaner();

  //just define the scales...nothing else is done
  //the user will need to call setPsf+makePsfScales+setDirty+makeDirtyScales
  //to be in a good state to clean.
  void defineScales(const casacore::Vector<casacore::Float>& scales);

  //Set the dirty image without calculating convolutions..
  //can be done by calling  makeDirtyScales or setscales if one want to redo the
  //psfscales too.
  void setDirty(const casacore::Matrix<casacore::Float>& dirty);
  //Calculate the convolutions for the dirt
  //Obviously the
  void makeDirtyScales();
  // Update the dirty image only (equiv of setDirty + makeDirtyScales)
  void update(const casacore::Matrix<casacore::Float> & dirty);

  //change the psf
  //don't forget to redo the setscales or run makePsfScales,
  //followed by makeDirtyScales
  void setPsf(const casacore::Matrix<casacore::Float>& psf);
  //calculate the convolutions of the psf
  void makePsfScales();

  // Set a number of scale sizes. The units of the scale are pixels.
  // The 2 functions below assume you have the dirty image and the psf set
  // the convolutions are calculated automatically and the masks ones too
  // if it is set ....kept so as to be compatible function wise with LatticeCleaner
  casacore::Bool setscales(const casacore::Int nscales, const casacore::Float scaleInc=1.0);

  // Set a specific set of scales
  casacore::Bool setscales(const casacore::Vector<casacore::Float> & scales);



  // Set up control parameters
  // cleanType - type of the cleaning algorithm to use (HOGBOM, MULTISCALE)
  // niter - number of iterations
  // gain - loop gain used in cleaning (a fraction of the maximum
  //        subtracted at every iteration)
  // aThreshold - absolute threshold to stop iterations
  // fThreshold - fractional threshold (i.e. given w.r.t. maximum residual)
  //              to stop iterations. This parameter is specified as
  //              casacore::Quantity so it can be given in per cents.
  // choose - unused at the moment, specify false. Original meaning is
  // to allow interactive decision on whether to continue iterations.
  // This method always returns true.
  casacore::Bool setcontrol(casacore::CleanEnums::CleanType cleanType, const casacore::Int niter,
		  const casacore::Float gain, const casacore::Quantity& aThreshold,
		  const casacore::Quantity& fThreshold);

  // This version of the method disables stopping on fractional threshold
  casacore::Bool setcontrol(casacore::CleanEnums::CleanType cleanType, const casacore::Int niter,
		  const casacore::Float gain, const casacore::Quantity& threshold);

  // return how many iterations we did do
  casacore::Int iteration() const { return itsIteration; }
  casacore::Int numberIterations() const { return itsIteration; }

  // what iteration number to start on
  void startingIteration(const casacore::Int starting = 0) {itsStartingIter = starting; }

 //Total flux accumulated so far
  casacore::Float totalFlux() const {return itsTotalFlux;}


  // Clean an image.
  //return value gives you a hint of what's happening
  //  1 = converged
  //  0 = not converged but behaving normally
  // -1 = not converged and stopped on cleaning consecutive smallest scale
  // -2 = not converged and either large scale hit negative or diverging
  // -3 = clean is diverging rather than converging
  casacore::Int clean(casacore::Matrix<casacore::Float> & model, casacore::Bool doPlotProgress=false);

  // Set the mask
  // mask - input mask lattice
  // maskThreshold - if positive, the value is treated as a threshold value to determine
  // whether a pixel is good (mask value is greater than the threshold) or has to be
  // masked (mask value is below the threshold). Negative threshold switches mask clipping
  // off. The mask value is used to weight the flux during cleaning. This mode is used
  // to implement cleaning based on the signal-to-noise as opposed to the standard cleaning
  // based on the flux. The default threshold value is 0.9, which ensures the behavior of the
  // code is exactly the same as before this parameter has been introduced.
  void setMask(casacore::Matrix<casacore::Float> & mask, const casacore::Float& maskThreshold = 0.9);
  // Call the function below if the psf is changed ..no need to setMask again
  casacore::Bool makeScaleMasks();

  // remove the mask;
  // useful when keeping object and sending a new dirty image to clean
  // one can set another mask then
  void unsetMask();

  // Tell the algorithm to NOT clean just the inner quarter
  // (This is useful when multiscale clean is being used
  // inside a major cycle for MF or WF algorithms)
  // if true, the full image deconvolution will be attempted
  void ignoreCenterBox(casacore::Bool huh) { itsIgnoreCenterBox = huh; }

  // Consider the case of a point source:
  // the flux on all scales is the same, and the first scale will be chosen.
  // Now, consider the case of a point source with a *little* bit of extended structure:
  // thats right, the largest scale will be chosen.  In this case, we should provide some
  // bias towards the small scales, or against the large scales.  We do this in
  // an ad hoc manner, multiplying the maxima found at each scale by
  // 1.0 - itsSmallScaleBias * itsScaleSizes(scale)/itsScaleSizes(nScalesToClean-1);
  // Typical bias values range from 0.2 to 1.0.
  void setSmallScaleBias(const casacore::Float x=0.5) { itsSmallScaleBias = x; }

  // During early iterations of a cycled casacore::MS Clean in mosaicing, it common
  // to come across an ocsilatory pattern going between positive and
  // negative in the large scale.  If this is set, we stop at the first
  // negative in the largest scale.
  void stopAtLargeScaleNegative() {itsStopAtLargeScaleNegative = true; }

  // Some algorithms require that the cycles be terminated when the image
  // is dominated by point sources; if we get nStopPointMode of the
  // smallest scale components in a row, we terminate the cycles
  void stopPointMode(casacore::Int nStopPointMode) {itsStopPointMode = nStopPointMode; }

  // After completion of cycle, querry this to find out if we stopped because
  // of stopPointMode
  casacore::Bool queryStopPointMode() const {return itsDidStopPointMode; }

  // speedup() will speed the clean iteration by raising the
  // threshold.  This may be required if the threshold is
  // accidentally set too low (ie, lower than can be achieved
  // given errors in the approximate PSF).
  //
  // threshold(iteration) = threshold(0)
  //                        * ( exp( (iteration - startingiteration)/Ndouble )/ 2.718 )
  // If speedup() is NOT invoked, no effect on threshold
  void speedup(const casacore::Float Ndouble);

  // Look at what WE think the residuals look like
  // Assumes the first scale is zero-sized
  casacore::Matrix<casacore::Float>  residual() { return itsDirtyConvScales[0]; }
  //slightly better approximation of the residual: it convolves the given model
  //with the psf and remove it from the dirty image put in setdirty
  casacore::Matrix<casacore::Float>  residual(const casacore::Matrix<casacore::Float>& model);
  // Method to return threshold, including any speedup factors
  casacore::Float threshold() const;

  // Method to return the strength optimum achieved at the last clean iteration
  // The output of this method makes sense only if it is called after clean
  casacore::Float strengthOptimum() const { return itsStrengthOptimum; }

  // Helper function to optimize adding
  //static void addTo(casacore::Matrix<casacore::Float>& to, const casacore::Matrix<casacore::Float> & add);

protected:
  friend class MatrixNACleaner;
  // Make sure that the peak of the Psf is within the image
  casacore::Bool validatePsf(const casacore::Matrix<casacore::Float> & psf);

  // Make an array of the specified scale
  void makeScale(casacore::Matrix<casacore::Float>& scale, const casacore::Float& scaleSize);

  // Make Spheroidal function for scale images
  casacore::Float spheroidal(casacore::Float nu);

  // Find the Peak of the matrix
  static casacore::Bool findMaxAbs(const casacore::Matrix<casacore::Float>& lattice,
                         casacore::Float& maxAbs, casacore::IPosition& posMax);

  // This is made static since findMaxAbs is static(!).
  // Why is findMaxAbs static???
  //                       --SB
  static casacore::Bool findPSFMaxAbs(const casacore::Matrix<casacore::Float>& lattice,
			    casacore::Float& maxAbs, casacore::IPosition& posMax,
			    const casacore::Int& supportSize=100);

  casacore::Int findBeamPatch(const casacore::Float maxScaleSize, const casacore::Int& nx, const casacore::Int& ny,
		    const casacore::Float psfBeam=4.0, const casacore::Float nBeams=20.0);
  // Find the Peak of the lattice, applying a mask
  casacore::Bool findMaxAbsMask(const casacore::Matrix<casacore::Float>& lattice, const casacore::Matrix<casacore::Float>& mask,
                             casacore::Float& maxAbs, casacore::IPosition& posMax);

  // Helper function to reduce the box sizes until the have the same
  // size keeping the centers intact
  static void makeBoxesSameSize(casacore::IPosition& blc1, casacore::IPosition& trc1,
     casacore::IPosition &blc2, casacore::IPosition& trc2);


  casacore::CleanEnums::CleanType itsCleanType;
  casacore::Float itsGain;
  casacore::Int itsMaxNiter;      // maximum possible number of iterations
  casacore::Quantum<casacore::Double> itsThreshold;
  casacore::CountedPtr<casacore::Matrix<casacore::Float> > itsMask;
  casacore::IPosition itsPositionPeakPsf;
  casacore::Float itsSmallScaleBias;
  casacore::Block<casacore::Matrix<casacore::Float> > itsScaleMasks;
  casacore::Block<casacore::Matrix<casacore::Complex> > itsScaleXfrs;
  casacore::Bool itsScalesValid;
  casacore::Int itsNscales;
  casacore::Float itsMaskThreshold;

  //# The following functions are used in various places in the code and are
  //# documented in the .cc file. Static functions are used when the functions
  //# do not modify the object state. They ensure that implicit assumptions
  //# about the current state and implicit side-effects are not possible
  //# because all information must be supplied in the input arguments


  casacore::CountedPtr<casacore::Matrix<casacore::Float> > itsDirty;
  casacore::CountedPtr<casacore::Matrix<casacore::Complex> >itsXfr;

  casacore::Vector<casacore::Float> itsScaleSizes;

  casacore::Block<casacore::Matrix<casacore::Float> > itsScales;
  casacore::Block<casacore::Matrix<casacore::Float> > itsPsfConvScales;
  casacore::Block<casacore::Matrix<casacore::Float> > itsDirtyConvScales;


  casacore::Int itsIteration;	// what iteration did we get to?
  casacore::Int itsStartingIter;	// what iteration did we get to?
  casacore::Quantum<casacore::Double> itsFracThreshold;

  casacore::Float itsMaximumResidual;
  casacore::Float itsStrengthOptimum;


  casacore::Vector<casacore::Float> itsTotalFluxScale;
  casacore::Float itsTotalFlux;

  // Calculate index into PsfConvScales
  casacore::Int index(const casacore::Int scale, const casacore::Int otherscale);

  casacore::Bool destroyScales();
  casacore::Bool destroyMasks();

  casacore::Bool itsIgnoreCenterBox;
  casacore::Bool itsStopAtLargeScaleNegative;
  casacore::Int itsStopPointMode;
  casacore::Bool itsDidStopPointMode;

  casacore::IPosition psfShape_p;
  casacore::Bool noClean_p;

private:

  // casacore::Memory to be allocated per TempLattice
  casacore::Double itsMemoryMB;

  // Let the user choose whether to stop
  casacore::Bool itsChoose;

  // Threshold speedup factors:
  casacore::Bool  itsDoSpeedup;  // if false, threshold does not change with iteration
  casacore::Float itsNDouble;

  //# Stop now?
  //#//  casacore::Bool stopnow();   Removed on 8-Apr-2004 by GvD

  casacore::Bool itsJustStarting;

  // threshold for masks. If negative, mask values are used as weights and no pixels are
  // discarded (although effectively they would be discarded if the mask value is 0.)
};

} //# NAMESPACE CASA - END

#endif
