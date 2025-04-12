//# MTAspMatrixCleaner.h: Minor Cycle for Asp deconvolution
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
//# $Id: MTAspMatrixCleaner.h Genie H. 2020-04-06 <mhsieh@nrao.edu $

#ifndef SYNTHESIS_MTASPMATRIXCLEANER_H
#define SYNTHESIS_MTASPMATRIXCLEANER_H

//# Includes
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <synthesis/MeasurementEquations/MatrixCleaner.h>
#include <synthesis/MeasurementEquations/AspMatrixCleaner.h>
#include <deque>

namespace casa { //# NAMESPACE CASA - BEGIN

class MTAspMatrixCleaner : public AspMatrixCleaner
{
public:
  // Create a cleaner : default constructor
  MTAspMatrixCleaner();

  // The destructor does nothing special.
  ~MTAspMatrixCleaner();

  using AspMatrixCleaner::setaspcontrol;
  using AspMatrixCleaner::getPsfGaussianWidth;
  ////float getMTPsfGaussianWidth(casacore::ImageInterface<casacore::Float>& psf);
  using AspMatrixCleaner::defineAspScales;
  using AspMatrixCleaner::setInitScaleXfrs;
  using AspMatrixCleaner::setInitScaleMasks;
  using AspMatrixCleaner::setFusedThreshold;
  using AspMatrixCleaner::getterPeakResidual;
  using AspMatrixCleaner::getLargestScaleSize;
  using AspMatrixCleaner::setUserLargestScale;

  // mtasp
  // Clean an image.
  //return value gives you a hint of what's happening
  //  1 = converged
  casacore::Int mtaspclean();
  casacore::Int allocateMemory(casacore::Int nx,casacore::Int ny);
  casacore::Bool setNTaylorTerms(const int & nterms);
  bool setMTPsf(int order, casacore::Matrix<casacore::Float> & psf);
  casacore::Bool getResidual(int order, casacore::Matrix<casacore::Float> & residual);
  casacore::Bool setResidual(int order, casacore::Matrix<casacore::Float> & dirty);
  casacore::Bool setModel(int order, casacore::Matrix<casacore::Float> & model);
  casacore::Bool getModel(int order, casacore::Matrix<casacore::Float> & model);
  // Calculate Hessian elements and check for invertibility
  casacore::Int computeHessianPeak(casacore::Bool compPrinSol, casacore::Bool usePatch, casacore::IPosition blcPsf, casacore::IPosition trcPsf);
  casacore::Bool computePrincipalSolution();
  void makeMTScaleImage(casacore::Matrix<casacore::Float>& iscale, const casacore::Float& scaleSize);
  // returns the active-set aspen for cleaning
  std::vector<casacore::Float> getActiveSetAspen();

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
  using AspMatrixCleaner::itsInitScales;
  using AspMatrixCleaner::itsInitScaleXfrs;
  using AspMatrixCleaner::itsDirtyConvInitScales;
  using AspMatrixCleaner::itsInitScaleMasks;

  using MatrixCleaner::itsIteration;
  using MatrixCleaner::itsStartingIter;
  using MatrixCleaner::itsFracThreshold;
  using MatrixCleaner::itsMaximumResidual;
  using MatrixCleaner::itsStrengthOptimum;
  using MatrixCleaner::itsTotalFlux;
  using MatrixCleaner::index;

  using MatrixCleaner::destroyScales;
  using AspMatrixCleaner::destroyAspScales;
  using AspMatrixCleaner::destroyInitScales;
  using MatrixCleaner::destroyMasks;
  using AspMatrixCleaner::destroyInitMasks;

  using MatrixCleaner::itsIgnoreCenterBox;
  using MatrixCleaner::itsStopAtLargeScaleNegative;
  using MatrixCleaner::itsStopPointMode;
  using MatrixCleaner::itsDidStopPointMode;
  using MatrixCleaner::psfShape_p;
  using MatrixCleaner::noClean_p;

  // FFTServer
  using AspMatrixCleaner::fft;

  // set to 0, 1*, 2*, 4*, and 8* width for initial scales in Asp
  using AspMatrixCleaner::itsInitScaleSizes;
  using AspMatrixCleaner::itsNInitScales;
  using AspMatrixCleaner::itsNumIterNoGoodAspen;
  using AspMatrixCleaner::itsPsfWidth;
  using AspMatrixCleaner::itsUseZhang;
  using AspMatrixCleaner::itsSwitchedToHogbom;
  using AspMatrixCleaner::itsNumHogbomIter;
  using AspMatrixCleaner::itsNthHogbom;
  using AspMatrixCleaner::itsGoodAspActiveSet; // avtice-set of aspens (updated)
  using AspMatrixCleaner::itsGoodAspAmplitude; // amplitude of avtice-set of aspens (updated)
  using AspMatrixCleaner::itsGoodAspCenter; // center of aspens in active-set
  using AspMatrixCleaner::itsOptimumScale;
  using AspMatrixCleaner::itsPositionOptimum;
  using AspMatrixCleaner::itsOptimumScaleSize;
  using AspMatrixCleaner::itsUsedMemoryMB;
  using AspMatrixCleaner::itsPeakResidual;
  using AspMatrixCleaner::itsPrevPeakResidual;
  using AspMatrixCleaner::itsDefaultNorm;
  using AspMatrixCleaner::itsNormMethod;
  using AspMatrixCleaner::itsFusedThreshold;
  using AspMatrixCleaner::itsNumNoChange;

  // helper functions for ASP
  using AspMatrixCleaner::makeInitScaleImage;
  using AspMatrixCleaner::makeScaleImage;
  using AspMatrixCleaner::maxDirtyConvInitScales;
  using AspMatrixCleaner::switchedToHogbom;
  using AspMatrixCleaner::getterPsfWidth;
  using AspMatrixCleaner::getterSwitchedHogbom;

  // for multi-term
  casacore::IPosition gip;
  casacore::Int nscales_p; // Number of scales. It's always 2 for asp-clean (i.e. 0 scale and the opt scale)
  casacore::Int ntaylor_p; // Number of terms in the Taylor expansion to use.
  casacore::Int psfntaylor_p; // Number of terms in the Taylor expansion for PSF.
  casacore::Int nx_p;
  casacore::Int ny_p;
  casacore::IPosition psfsupport_p;
  casacore::IPosition psfpeak_p;
  casacore::Vector<casacore::Float> totalTaylorFlux_p; // casacore::Vector of total flux in each taylor term.
  casacore::Matrix<casacore::Complex> cWork_p; // Temporary work-holder
  casacore::Block<casacore::Matrix<casacore::Float> > vecWork_p; // Temporary work-holder
  casacore::Matrix<casacore::Complex> dirtyFT_p;
  casacore::IPosition blcDirty;
  casacore::IPosition trcDirty;

  // h(s) [nx,ny,nscales]
  casacore::Block<casacore::Matrix<casacore::Float> > vecScales_p;
  casacore::Block<casacore::Matrix<casacore::Complex> > vecScalesFT_p;

  casacore::Block<casacore::Matrix<casacore::Complex> > vecPsfFT_p;
  // I_D : Residual/Dirty Images [nx,ny,ntaylor]
  casacore::Block<casacore::Matrix<casacore::Float> > vecDirty_p;
  // I_M : Model Images [nx,ny,ntaylor]
  casacore::Block<casacore::Matrix<casacore::Float> > vecModel_p;
  // A_{smn} = B_{sm} * B{sn} [nx,ny,ntaylor,ntaylor]
  casacore::Block<casacore::Matrix<casacore::Float> > cubeA_p;
  // R_{sk} = I_D * B_{sk} [nx,ny,ntaylor]
  casacore::Block<casacore::Matrix<casacore::Float> > matR_p;
  // a_{sk} = Solution vectors. [nx,ny,ntaylor]
  casacore::Block<casacore::Matrix<casacore::Float> > matCoeffs_p;
  // Solve [A][Coeffs] = [I_D * B]
  // Shape of A : [ntaylor,ntaylor]
  casacore::Block<casacore::Matrix<casacore::Double>> matA_p;    // 2D matrix to be inverted.
  casacore::Block<casacore::Matrix<casacore::Double>> invMatA_p; // Inverse of matA_p;

  casacore::Int setSupport(const casacore::Float scaleSize, const casacore::Int& nx, const casacore::Int& ny,
        const casacore::Float psfBeam=4.0, const casacore::Float nBeams=20.0);

  casacore::Int solveMatrixEqn(casacore::Int ntaylor, casacore::Int scale, casacore::IPosition blc, casacore::IPosition trc);
  casacore::Int computeRHS(); // residuals * opt scale
  void checkConvergence(casacore::Int &converged);
  void checkMTConvergence(casacore::Int &converged, casacore::Float tmpMaximumResidual, casacore::Float minMaximumResidual);
  casacore::Int updateModelAndRHS(casacore::Float loopgain, casacore::IPosition blc, casacore::IPosition trc, casacore::IPosition blcPsf, casacore::IPosition trcPsf);
  casacore::Int updateRHS(casacore::Int ntaylor, casacore::Int scale, casacore::Float loopgain, casacore::Vector<casacore::Float> coeffs, casacore::IPosition blc, casacore::IPosition trc, casacore::IPosition blcPsf, casacore::IPosition trcPsf);
  casacore::Bool buildImagePatches();
  casacore::Float getOptStrength();
  void updatePeakResidual();

  casacore::Int IND2(casacore::Int taylor,casacore::Int scale);
  casacore::Int IND4(casacore::Int taylor1, casacore::Int taylor2, casacore::Int scale1, casacore::Int scale2);
};

} //# NAMESPACE CASA - END

#endif
