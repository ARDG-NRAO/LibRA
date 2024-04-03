//# AspMatrixCleaner.h: Minor Cycle for Asp deconvolution
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
//# $Id: AspMatrixCleaner.h Genie H. 2020-04-06 <mhsieh@nrao.edu $

#ifndef SYNTHESIS_ASPMATRIXCLEANER_H
#define SYNTHESIS_ASPMATRIXCLEANER_H

//# Includes
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <synthesis/MeasurementEquations/MatrixCleaner.h>
#include <deque>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class AspMatrixCleaner : public MatrixCleaner
{
public:
  // Create a cleaner : default constructor
  AspMatrixCleaner();

  // The destructor does nothing special.
  ~AspMatrixCleaner();

  casacore::Bool setaspcontrol(const casacore::Int niter,
      const casacore::Float gain, const casacore::Quantity& aThreshold,
      const casacore::Quantity& fThreshold);


  // Clean an image.
  //return value gives you a hint of what's happening
  //  1 = converged
  //  0 = not converged but behaving normally
  // -1 = not converged and stopped on cleaning consecutive smallest scale
  // -2 = not converged and either large scale hit negative or diverging
  // -3 = clean is diverging rather than converging
  casacore::Int aspclean(casacore::Matrix<casacore::Float> & model, casacore::Bool doPlotProgress=false);

  // helper functions for ASP
  float getPsfGaussianWidth(casacore::ImageInterface<casacore::Float>& psf);
  void getLargestScaleSize(casacore::ImageInterface<casacore::Float>& psf);

  // Make an image of the specified scale by Gaussian
  void makeInitScaleImage(casacore::Matrix<casacore::Float>& iscale, const casacore::Float& scaleSize);
  void makeScaleImage(casacore::Matrix<casacore::Float>& iscale, const casacore::Float& scaleSize, const casacore::Float& amp, const casacore::IPosition& center);

  void setInitScales();
  void setInitScaleXfrs(const casacore::Float width);

  // calculate the convolutions of the psf with the initial scales
  void setInitScalePsfs();

  casacore::Bool setInitScaleMasks(const casacore::Array<casacore::Float> arrmask, const casacore::Float& maskThreshold = 0.99);

  void maxDirtyConvInitScales(float& strengthOptimum, int& optimumScale, casacore::IPosition& positionOptimum);

  // returns the active-set aspen for cleaning
  std::vector<casacore::Float> getActiveSetAspen();

  // Juat define the active-set aspen scales
  void defineAspScales(std::vector<casacore::Float>& scaleSizes);

  void switchedToHogbom(bool runlong= false);
  void setOrigDirty(const casacore::Matrix<casacore::Float>& dirty);
  void setFusedThreshold(const casacore::Float fusedThreshold = 0.0) { itsFusedThreshold = fusedThreshold; }
  void setUserLargestScale(const casacore::Int largestScale = -1) { itsUserLargestScale = float(largestScale); }

  // setter/getter
  float getterPsfWidth() { return itsPsfWidth; }
  bool getterSwitchedHogbom() { return itsSwitchedToHogbom; }
  casacore::Matrix<casacore::Float>  getterResidual() { return (*itsDirty); }
  float getterPeakResidual() { return itsPeakResidual; }

  void setBinSizeForSumFlux(const casacore::Int binSize = 4) { itsBinSizeForSumFlux = binSize ; } ;
  void getFluxByBins(const std::vector<casacore::Float>& scaleSizes,const std::vector<casacore::Float>& optimum, casacore::Int binSize, std::vector<casacore::Float>&  sumFluxByBins, std::vector<casacore::Float>&  rangeFluxByBins);


//protected:
private:

  using MatrixCleaner::findMaxAbs;
  using MatrixCleaner::findMaxAbsMask;
  using MatrixCleaner::itsGain;
  using MatrixCleaner::itsMaxNiter;
  using MatrixCleaner::itsThreshold;
  using MatrixCleaner::itsMask;
  using MatrixCleaner::itsScalesValid;
  using MatrixCleaner::itsNscales;
  using MatrixCleaner::itsMaskThreshold;
  using MatrixCleaner::itsDirty;
  using MatrixCleaner::itsXfr;
  using MatrixCleaner::itsScaleSizes;
  casacore::Block<casacore::Matrix<casacore::Float> > itsInitScales;
  casacore::Block<casacore::Matrix<casacore::Complex> > itsInitScaleXfrs;
  casacore::Block<casacore::Matrix<casacore::Float> > itsDirtyConvInitScales;
  casacore::Block<casacore::Matrix<casacore::Float> > itsInitScaleMasks;
  casacore::Block<casacore::Matrix<casacore::Float> > itsPsfConvInitScales;

  using MatrixCleaner::itsIteration;
  using MatrixCleaner::itsStartingIter;
  using MatrixCleaner::itsFracThreshold;
  using MatrixCleaner::itsMaximumResidual;
  using MatrixCleaner::itsStrengthOptimum;
  using MatrixCleaner::itsTotalFlux;
  using MatrixCleaner::index;

  using MatrixCleaner::destroyScales;
  casacore::Bool destroyAspScales();
  casacore::Bool destroyInitScales();
  using MatrixCleaner::destroyMasks;
  casacore::Bool destroyInitMasks();
  casacore::Float computeThreshold() const;

  using MatrixCleaner::itsIgnoreCenterBox;
  using MatrixCleaner::itsStopAtLargeScaleNegative;
  using MatrixCleaner::itsStopPointMode;
  using MatrixCleaner::itsDidStopPointMode;
  using MatrixCleaner::psfShape_p;
  using MatrixCleaner::noClean_p;

  // FFTServer
  casacore::FFTServer<casacore::Float,casacore::Complex> fft;

  // set to 0, 1*, 2*, 4*, and 8* width for initial scales in Asp
  std::vector<casacore::Float> itsInitScaleSizes;
  //std::vector<casacore::Float> itsAspScaleSizes; // No longer needed. permanent list for making model image
  //std::vector<casacore::Float> itsAspAmplitude;
  //std::vector<casacore::IPosition> itsAspCenter;
  casacore::Int itsNInitScales;
  std::deque<int> itsNumIterNoGoodAspen;
  float itsPsfWidth;
  bool itsUseZhang;
  bool itsSwitchedToHogbom;
  unsigned int itsNumHogbomIter;
  unsigned int itsNthHogbom;
  std::vector<casacore::Float> itsGoodAspActiveSet; // avtice-set of aspens (updated)
  std::vector<casacore::Float> itsGoodAspAmplitude; // amplitude of avtice-set of aspens (updated)
  std::vector<casacore::IPosition> itsGoodAspCenter; // center of aspens in active-set
  //std::vector<casacore::Float> itsPrevAspActiveSet; // No longer needed. avtice-set of aspens (before bfgs)
  //std::vector<casacore::Float> itsPrevAspAmplitude; // No longer needed. amplitude of avtice-set of aspens (before bfgs)
  casacore::Int itsOptimumScale;
  casacore::IPosition itsPositionOptimum;
  casacore::Float itsOptimumScaleSize;
  double itsUsedMemoryMB;
  float itsPeakResidual;
  float itsPrevPeakResidual;
  casacore::CountedPtr<casacore::Matrix<casacore::Float> > itsOrigDirty;

  const casacore::Int itsDefaultNorm = 1;
  casacore::Int itsNormMethod;
  casacore::Float itsFusedThreshold;
  casacore::Int itsBinSizeForSumFlux ;   // number of bins for histogram of the sum of Flux
  unsigned int itsNumNoChange; // number of times peakres rarely changes
  float itsLargestInitScale; // estimated largest initial scale
  float itsUserLargestScale; // user-specified largest initial scale
};

} //# NAMESPACE CASA - END

#endif
