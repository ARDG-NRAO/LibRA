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

#ifndef SYNTHESIS_SPECTRALASPCLEANER_H
#define SYNTHESIS_SPECTRALASPCLEANER_H

//# Includes
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <synthesis/MeasurementEquations/AspMatrixCleaner.h>
#include <deque>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/casa/Utilities/CountedPtr.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class SpectralAspCleaner : public AspMatrixCleaner
{
public:
  // Create a cleaner : default constructor
  SpectralAspCleaner();

  // The destructor does nothing special.
  ~SpectralAspCleaner();

  void MFaspclean(casacore::Matrix<casacore::Float> & model);

  void setWaveletControl(const casacore::Vector<casacore::Float> waveletScales, const casacore::Vector<casacore::Float> waveletAmps, const casacore::Bool waveletTrigger = false, const casacore::Bool mfasp = false) { itsWaveletScales = waveletScales; itsWaveletAmps=waveletAmps; itsWaveletTrigger=waveletTrigger; itsmfasp=mfasp;}

protected:
//private:

  using AspMatrixCleaner::findMaxAbs;
  using AspMatrixCleaner::findMaxAbsMask;
  using AspMatrixCleaner::itsGain;
  using AspMatrixCleaner::itsMaxNiter;
  using AspMatrixCleaner::itsThreshold;
  using AspMatrixCleaner::itsMask;
  using AspMatrixCleaner::itsScalesValid;
  using AspMatrixCleaner::itsNscales;
  using AspMatrixCleaner::itsMaskThreshold;
  using AspMatrixCleaner::itsDirty;
  using AspMatrixCleaner::itsXfr;
  using AspMatrixCleaner::itsScaleSizes;
  
  casacore::Block<casacore::Matrix<casacore::Float> > itsInitScales;
  casacore::Block<casacore::Matrix<casacore::Complex> > itsInitScaleXfrs;
  casacore::Block<casacore::Matrix<casacore::Float> > itsDirtyConvInitScales;
  casacore::Block<casacore::Matrix<casacore::Float> > itsInitScaleMasks;
  casacore::Block<casacore::Matrix<casacore::Float> > itsPsfConvInitScales;

  using AspMatrixCleaner::itsIteration;
  using AspMatrixCleaner::itsStartingIter;
  using AspMatrixCleaner::itsFracThreshold;
  using AspMatrixCleaner::itsMaximumResidual;
  using AspMatrixCleaner::itsStrengthOptimum;
  using AspMatrixCleaner::itsTotalFlux;
  using AspMatrixCleaner::index;

  using AspMatrixCleaner::destroyScales;
  using AspMatrixCleaner::destroyAspScales;
  using AspMatrixCleaner::destroyInitScales;
  using AspMatrixCleaner::destroyMasks;
  using AspMatrixCleaner::destroyInitMasks;
  using AspMatrixCleaner::computeThreshold;

  using AspMatrixCleaner::itsIgnoreCenterBox;
  using AspMatrixCleaner::itsStopAtLargeScaleNegative;
  using AspMatrixCleaner::itsStopPointMode;
  using AspMatrixCleaner::itsDidStopPointMode;
  using AspMatrixCleaner::psfShape_p;
  using AspMatrixCleaner::noClean_p;

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
  casacore::Float itsHogbomGain;
  unsigned int itsNumNoChange; // number of times peakres rarely changes
  casacore::Int itsBinSizeForSumFlux ;   // number of bins for histogram of the sum of Flux
  float itsLargestInitScale; // estimated largest initial scale
  float itsUserLargestScale; // user-specified largest initial scale
  casacore::IPosition blcDirty, trcDirty;
  
  casacore::Vector<casacore::Float> itsWaveletScales;
  casacore::Vector<casacore::Float> itsWaveletAmps;
  casacore::Bool itsWaveletTrigger;
  casacore::Bool itsmfasp;
  casacore::Bool itsdimensionsareeven;
  casacore::Bool itsstopMS;
  casacore::Float itsLbfgsEpsF;
  casacore::Float itsLbfgsEpsX;
  casacore::Float itsLbfgsEpsG;
  casacore::Int itsLbfgsMaxit;
};

} //# NAMESPACE CASA - END

#endif
