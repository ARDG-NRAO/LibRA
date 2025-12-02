
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

#ifndef SYNTHESIS_AUTOCLEANER_H
#define SYNTHESIS_AUTOCLEANER_H

//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/lattices/Lattices/TempLattice.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/lattices/LatticeMath/LatticeCleaner.h>
#include <casacore/casa/Arrays/ArrayFwd.h>
#include <casacore/casa/BasicMath/Functors.h>
#include <casacore/casa/Utilities/CountedPtr.h>

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

class AutoCleaner
{
public:

  // Create a cleaner : default constructor
  AutoCleaner();

  // Create a cleaner for a specific dirty image and PSF
  AutoCleaner(const casacore::Matrix<casacore::Float> & psf, const casacore::Matrix<casacore::Float> & dirty);

  // The copy constructor uses reference semantics
  AutoCleaner(const AutoCleaner& other);

  // The assignment operator also uses reference semantics
  AutoCleaner & operator=(const AutoCleaner & other);

  // The destructor does nothing special.
  ~AutoCleaner();

  //Set the dirty image without calculating convolutions..
  //can be done by calling  makeDirtyScales or setscales if one want to redo the
  //psfscales too.
  void setDirty(const casacore::Matrix<casacore::Float>& dirty);
  // Update the dirty image only (equiv of setDirty + makeDirtyScales)
  void update(const casacore::Matrix<casacore::Float> & dirty);

  casacore::Bool setscales();
  void initializeCorrProducts();
  void reinitializeCorrProducts();
  void recomputeCorrProducts();
  void approximateBasisFunction();
  void updateBasisFunction();
  void subtractBeam(casacore::Matrix<casacore::Float> &map, casacore::Matrix<casacore::Float> &beam, casacore::IPosition blc, casacore::IPosition trc, casacore::IPosition blcbeam, casacore::IPosition trcbeam, casacore::Float factor, casacore::Bool reverse, casacore::Bool add);


  //change the psf
  //don't forget to redo the setscales or run makePsfScales,
  //followed by makeDirtyScales
  void setPsf(const casacore::Matrix<casacore::Float>& psf);

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

  // remove the mask;
  // useful when keeping object and sending a new dirty image to clean
  // one can set another mask then
  void unsetMask();

  // Tell the algorithm to NOT clean just the inner quarter
  // (This is useful when multiscale clean is being used
  // inside a major cycle for MF or WF algorithms)
  // if true, the full image deconvolution will be attempted
  void ignoreCenterBox(casacore::Bool huh) { itsIgnoreCenterBox = huh; }

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
  casacore::Matrix<casacore::Float>  residual() { return itsDirty; }
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

  casacore::Matrix<casacore::Float> cmap;
  casacore::Matrix<casacore::Float> mod;
  casacore::Matrix<casacore::Float> delta;

  casacore::Matrix<casacore::Float> shifted;
  casacore::Matrix<casacore::Float> clean_map;

  casacore::Matrix<casacore::Float> BB;
  casacore::Matrix<casacore::Float> tildeII;
  casacore::Matrix<casacore::Float> BI;
  casacore::Matrix<casacore::Float> basis_function;
  casacore::Matrix<casacore::Float> tildeMB;
  casacore::Matrix<casacore::Float> mspsf;
  casacore::Matrix<casacore::Float> tildeMI;
  casacore::Matrix<casacore::Float> tildeMBB;
  casacore::Matrix<casacore::Float> MtildeMBB;
  casacore::Matrix<casacore::Float> tildeMBI;
  casacore::Matrix<casacore::Float> MtildeMB;
  casacore::Matrix<casacore::Float> window_basis;

  casacore::Float normcmap;
  casacore::Float normcmapsq;
  casacore::Sqrt<casacore::Float> sqrtop;
  casacore::Float sumcmap;
  casacore::Float triggerhogbom;
  casacore::Float itsmaxbeam;

  casacore::IPosition blcautomask;
  casacore::IPosition trcautomask;

  void setAutoControl(const casacore::Float autoThreshold = 0.0,const casacore::Int autoMaxiter = 0, const casacore::Float autoGain = 0.0, const casacore::Bool autoHogbom = false, const casacore::Float autoTrigger = 1.0, const casacore::Float autoPower = 1.0, const casacore::Int autoXMask = 0, const casacore::Int autoYMask = 0) { itsAutoThreshold = autoThreshold; itsAutoMaxiter = autoMaxiter; itsAutoGain = autoGain; itsAutoHogbom = autoHogbom; itsAutoTrigger = autoTrigger; itsAutoPower = autoPower; itsAutoXMask = autoXMask; itsAutoYMask = autoYMask;}

protected:
  //friend class MatrixNACleaner;
  // Make sure that the peak of the Psf is within the image
  casacore::Bool validatePsf(const casacore::Matrix<casacore::Float> & psf);

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
  casacore::Bool itsScalesValid;
  casacore::Int itsNscales;
  casacore::Float itsMaskThreshold;

  //# The following functions are used in various places in the code and are
  //# documented in the .cc file. Static functions are used when the functions
  //# do not modify the object state. They ensure that implicit assumptions
  //# about the current state and implicit side-effects are not possible
  //# because all information must be supplied in the input arguments


  //casacore::CountedPtr<casacore::Matrix<casacore::Float> > itsDirty;
  casacore::Matrix<casacore::Float> itsDirty;

  //casacore::CountedPtr<casacore::Matrix<casacore::Float> > itsScales;
  casacore::Matrix<casacore::Float> itsScales;
  casacore::Matrix<casacore::Float> itsPsf;

  //casacore::Matrix<casacore::Float> cmap;
  //casacore::Matrix<casacore::Float> mod;
  //casacore::Matrix<casacore::Float> delta;

  //casacore::Matrix<casacore::Float> shifted;
  //casacore::Matrix<casacore::Float> clean_map;

  //casacore::Matrix<casacore::Float> BB;
  //casacore::Matrix<casacore::Float> tildeII;
  //casacore::Matrix<casacore::Float> BI;
  //casacore::Matrix<casacore::Float> basis_function;
  //casacore::Matrix<casacore::Float> tildeMB;
  //casacore::Matrix<casacore::Float> mspsf;
  //casacore::Matrix<casacore::Float> tildeMI;
  //casacore::Matrix<casacore::Float> tildeMBB;
  //casacore::Matrix<casacore::Float> MtildeMBB;
  //casacore::Matrix<casacore::Float> tildeMBI;
  //casacore::Matrix<casacore::Float> MtildeMB;

  casacore::Int itsIteration;	// what iteration did we get to?
  casacore::Int itsStartingIter;	// what iteration did we get to?
  casacore::Quantum<casacore::Double> itsFracThreshold;

  casacore::Float itsMaximumResidual;
  casacore::Float itsStrengthOptimum;

  casacore::Vector<casacore::Float> itsTotalFluxScale;
  casacore::Float itsTotalFlux;

  casacore::Bool itsIgnoreCenterBox;

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

  casacore::Float itsAutoThreshold;
  casacore::Int itsAutoMaxiter;
  casacore::Float itsAutoGain;
  casacore::Bool itsAutoHogbom;
  casacore::Float itsAutoTrigger;
  casacore::Float itsAutoPower;
  casacore::Bool itsdimensionsareeven;
  casacore::Int itsAutoXMask;
  casacore::Int itsAutoYMask;

  //# Stop now?
  //#//  casacore::Bool stopnow();   Removed on 8-Apr-2004 by GvD

  // threshold for masks. If negative, mask values are used as weights and no pixels are
  // discarded (although effectively they would be discarded if the mask value is 0.)
};

} //# NAMESPACE CASA - END

#endif
