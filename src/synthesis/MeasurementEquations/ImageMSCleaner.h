//# ImageMSCleaner.h: this defines Cleaner a class for doing convolution
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
//# $Id: $

#ifndef SYNTHESIS_IMAGEMSCLEANER_H
#define SYNTHESIS_IMAGEMSCLEANER_H

//# Includes
#include <synthesis/MeasurementEquations/MatrixCleaner.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

template <class T> class ImageInterface;

}

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations



// <summary>A class interfacing images to MatrixCleaner </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tLatticeCleaner">
// </reviewed>

// <prerequisite>
//  <li> ImageInterface
//  <li> MatrixCleaner
// </prerequisite>
//
// <etymology>

// The ImageCleaner class will use MSClean on Images.

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

class ImageMSCleaner

{

 public:
  //Default
  ImageMSCleaner();
  // Create a cleaner with a psf and dirty image
  ImageMSCleaner(casacore::ImageInterface<casacore::Float>& psf, casacore::ImageInterface<casacore::Float>& dirty);
  //assignmemnt constructor
  ImageMSCleaner(const ImageMSCleaner& other);
  //assignment operator
  ImageMSCleaner & operator=(const ImageMSCleaner & other);

  //The destructor
  ~ImageMSCleaner();
  
  // Update the dirty image only
  void update(casacore::ImageInterface<casacore::Float> & dirty);
  // Change the psf image 
  void setPsf(casacore::ImageInterface<casacore::Float> & psf);

  //// Set a number of scale sizes. The units of the scale are pixels.
  void setscales(const casacore::Int nscales, const casacore::Float scaleInc=1.0);
  // Set a specific set of scales
  void  setscales(const casacore::Vector<casacore::Float> & scales);

  // Set up control parameters
  // cleanType - type of the cleaning algorithm to use (HOGBOM, MULTISCALE)
  // niter - number of iterations
  // gain - loop gain used in cleaning (a fraction of the maximum 
  //        subtracted at every iteration)
  // aThreshold - absolute threshold to stop iterations
  // fThreshold - fractional threshold (i.e. given w.r.t. maximum residual)
  //              to stop iterations. This parameter is specified as
  //              casacore::Quantity so it can be given in per cents. 
  casacore::Bool setcontrol(casacore::CleanEnums::CleanType cleanType, const casacore::Int niter,
		  const casacore::Float gain, const casacore::Quantity& aThreshold,
		  const casacore::Quantity& fThreshold);
  
  // This version of the method disables stopping on fractional threshold
  casacore::Bool setcontrol(casacore::CleanEnums::CleanType cleanType, const casacore::Int niter,
		  const casacore::Float gain, const casacore::Quantity& threshold);
  
  // return how many iterations we did do
  casacore::Int iteration() const ;
  casacore::Int numberIterations() const ;
  
  // what iteration number to start on
  void startingIteration(const casacore::Int starting = 0);

  // Clean an image. 
  //return value gives you a hint of what's happening
  //  1 = converged
  //  0 = not converged but behaving normally
  // -1 = not converged and stopped on cleaning consecutive smallest scale
  // -2 = not converged and either large scale hit negative or diverging 
  // -3 = clean is diverging rather than converging 
  casacore::Int clean(casacore::ImageInterface<casacore::Float> & model, const casacore::String& algorithm, 
	    const casacore::Int niter, const casacore::Float gain, const casacore::Quantity& threshold, 
	    const casacore::Quantity& fthresh=casacore::Quantity(0.0, "%"),
	    casacore::Bool doPlotProgress=false);
  // Set the mask
  // mask - input mask lattice
  // maskThreshold - if positive, the value is treated as a threshold value to determine
  // whether a pixel is good (mask value is greater than the threshold) or has to be 
  // masked (mask value is below the threshold). Negative threshold switches mask clipping
  // off. The mask value is used to weight the flux during cleaning. This mode is used
  // to implement cleaning based on the signal-to-noise as opposed to the standard cleaning
  // based on the flux. The default threshold value is 0.9, which ensures the behavior of the
  // code is exactly the same as before this parameter has been introduced.
  void setMask(casacore::ImageInterface<casacore::Float> & mask, const casacore::Float& maskThreshold = 0.9);
  // Tell the algorithm to NOT clean just the inner quarter
  // (This is useful when multiscale clean is being used
  // inside a major cycle for MF or WF algorithms)
  // if true, the full image deconvolution will be attempted
  void ignoreCenterBox(casacore::Bool ign) ;
  // Consider the case of a point source: 
  // the flux on all scales is the same, and the first scale will be chosen.
  // Now, consider the case of a point source with a *little* bit of extended structure:
  // thats right, the largest scale will be chosen.  In this case, we should provide some
  // bias towards the small scales, or against the large scales.  We do this in
  // an ad hoc manner, multiplying the maxima found at each scale by
  // 1.0 - itsSmallScaleBias * itsScaleSizes(scale)/itsScaleSizes(nScalesToClean-1);
  // Typical bias values range from 0.2 to 1.0.
  void setSmallScaleBias(const casacore::Float x=0.5);
   // During early iterations of a cycled casacore::MS Clean in mosaicing, it common
  // to come across an ocsilatory pattern going between positive and
  // negative in the large scale.  If this is set, we stop at the first
  // negative in the largest scale.
  void stopAtLargeScaleNegative() ;
  // Some algorithms require that the cycles be terminated when the image
  // is dominated by point sources; if we get nStopPointMode of the
  // smallest scale components in a row, we terminate the cycles
  void stopPointMode(casacore::Int nStopPointMode) ;
   // After completion of cycle, querry this to find out if we stopped because
  // of stopPointMode
  casacore::Bool queryStopPointMode() const ;
  // speedup() will speed the clean iteration by raising the
  // threshold.  This may be required if the threshold is
  // accidentally set too low (ie, lower than can be achieved
  // given errors in the approximate PSF).
  //
  // threshold(iteration) = threshold(0) 
  //                        * ( exp( (iteration - startingiteration)/Ndouble )/ 2.718 )
  // If speedup() is NOT invoked, no effect on threshold
  void speedup(const casacore::Float Ndouble);

  //Max residual after last clean
  casacore::Float maxResidual() {return maxResidual_p;};

 private:
  //Helper function to setup some param
  casacore::Bool setupMatCleaner(const casacore::String& alg, const casacore::Int niter, const casacore::Float gain, 
		       const casacore::Quantity& threshold, const casacore::Quantity& fthresh=casacore::Quantity(0.0, "%"));
  MatrixCleaner matClean_p;
  casacore::ImageInterface<casacore::Float>* psf_p;
  casacore::ImageInterface<casacore::Float>* dirty_p;
  casacore::ImageInterface<casacore::Float>* mask_p;
  casacore::Int nPsfChan_p;
  casacore::Int nImChan_p;
  casacore::Int nPsfPol_p;
  casacore::Int nImPol_p;
  casacore::Int chanAxis_p;
  casacore::Int polAxis_p;
  casacore::Int nMaskChan_p;
  casacore::Int nMaskPol_p;
  casacore::Vector<casacore::Float> scales_p;
  casacore::Float maskThresh_p;
  casacore::Float maxResidual_p;

};

} //# NAMESPACE CASA - END

#endif
