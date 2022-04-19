//# MultiTermMatrixCleaner.h: Minor Cycle for MSMFS deconvolution
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
//# $Id: MultiTermMatrixCleaner Urvashi R.V.  2010-12-04 <rurvashi@aoc.nrao.edu$

#ifndef SYNTHESIS_MULTITERMLATTICECLEANER_H
#define SYNTHESIS_MULTITERMLATTICECLEANER_H

#include <scimath/Mathematics/FFTServer.h>
#include <synthesis/MeasurementEquations/MatrixCleaner.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class MultiTermMatrixCleaner : public MatrixCleaner
{
public:
  // Create a cleaner 
  MultiTermMatrixCleaner();

  // The copy constructor uses reference semantics
  //  MultiTermMatrixCleaner(const MultiTermMatrixCleaner & other);

  // The assignment operator also uses reference semantics
  //  MultiTermMatrixCleaner & operator=(const MultiTermMatrixCleaner & other); 

  // The destructor resizes arrays to empty before destruction.
  ~MultiTermMatrixCleaner();

  // casacore::Input : number of Taylor terms
  //         Reshapes PtrBlocks to hold the correct number of PSFs and Residual images
  casacore::Bool setntaylorterms(const int & nterms);
  
  // casacore::Input : scales
  casacore::Bool setscales(const casacore::Vector<casacore::Float> & scales);

  // Initialize all the memory being used.
  casacore::Bool initialise(casacore::Int nx,casacore::Int ny);

  // Calculate Hessian elements and check for invertibility
  // Does not have to be called externally, but can be. Either way, it executes only once.
  casacore::Int computeHessianPeak();

  // casacore::Input : psfs and dirty images
  casacore::Bool setpsf(int order, casacore::Matrix<casacore::Float> & psf);
  
  // casacore::Input : psfs and dirty images
  casacore::Bool setresidual(int order, casacore::Matrix<casacore::Float> & dirty);
 
  // casacore::Input : model images
  casacore::Bool setmodel(int order, casacore::Matrix<casacore::Float> & model);

  // casacore::Input : mask
  casacore::Bool setmask(casacore::Matrix<casacore::Float> & mask);
 
  // Run the minor cycle
  casacore::Int mtclean(casacore::Int maxniter, casacore::Float stopfraction, casacore::Float inputgain, casacore::Float userthreshold);

  // Output : Model images
  casacore::Bool getmodel(int order, casacore::Matrix<casacore::Float> & model);
  
  // Output : psfs and dirty images
  casacore::Bool getresidual(int order, casacore::Matrix<casacore::Float> & residual);
  
  // Compute principal solution - in-place on the residual images in vecDirty. 
  casacore::Bool computeprincipalsolution();
 
  // Output : Hessian matrix
  casacore::Bool getinvhessian(casacore::Matrix<casacore::Double> & invhessian);

  // Output : Peak residual computed from matR_p (residual convolved with PSF).
  casacore::Float getpeakresidual(){return rmaxval_p;}


private:
  casacore::LogIO os;

  using MatrixCleaner::itsCleanType;
  using MatrixCleaner::itsMaxNiter;
  using MatrixCleaner::itsGain;
  using MatrixCleaner::itsThreshold;
  using MatrixCleaner::itsMask;
  using MatrixCleaner::itsPositionPeakPsf;
  //  using MatrixCleaner::itsScaleMasks;
  //  using MatrixCleaner::itsScaleXfrs;
  //  using MatrixCleaner::itsNscales;
  //  using MatrixCleaner::itsScalesValid;
  using MatrixCleaner::itsMaskThreshold;

  using MatrixCleaner::findMaxAbs;
  using MatrixCleaner::findMaxAbsMask;
  using MatrixCleaner::makeScale;
  //  using MatrixCleaner::addTo;
  using MatrixCleaner::makeBoxesSameSize;
  using MatrixCleaner::validatePsf;
  //using MatrixCleaner::makeScaleMasks;

  casacore::Int ntaylor_p; // Number of terms in the Taylor expansion to use.
  casacore::Int psfntaylor_p; // Number of terms in the Taylor expansion for PSF.
  casacore::Int nscales_p; // Number of scales to use for the multiscale part.
  casacore::Int nx_p;
  casacore::Int ny_p;
  casacore::Int totalIters_p; // Total number of minor-cycle iterations
  casacore::Float globalmaxval_p;
  casacore::Int maxscaleindex_p;
  casacore::IPosition globalmaxpos_p;
  casacore::Int itercount_p; // Number of minor cycle iterations
  casacore::Int maxniter_p;
  casacore::Float stopfraction_p;
  casacore::Float inputgain_p;
  casacore::Float userthreshold_p;
  casacore::Float prev_max_p;
  casacore::Float min_max_p;
  casacore::Float rmaxval_p;

  casacore::IPosition psfsupport_p;
  casacore::IPosition psfpeak_p;
  casacore::IPosition blc_p, trc_p, blcPsf_p, trcPsf_p;

  casacore::Vector<casacore::Float> scaleSizes_p; // casacore::Vector of scale sizes in pixels.
  casacore::Vector<casacore::Float> scaleBias_p; // casacore::Vector of scale biases !!
  casacore::Vector<casacore::Float> totalScaleFlux_p; // casacore::Vector of total scale fluxes.
  casacore::Vector<casacore::Float> totalTaylorFlux_p; // casacore::Vector of total flux in each taylor term.
  casacore::Vector<casacore::Float> maxScaleVal_p; // casacore::Vector for peaks at each scale size
  casacore::Vector<casacore::IPosition> maxScalePos_p; // casacore::Vector of peak positions at each scale size.

  casacore::IPosition gip;
  //casacore::Int nx,ny;
  casacore::Bool /*donePSF_p,*/ donePSP_p, doneCONV_p;
 
  casacore::Matrix<casacore::Complex> dirtyFT_p;
  casacore::Block<casacore::Matrix<casacore::Float> > vecScaleMasks_p;
  
  casacore::Matrix<casacore::Complex> cWork_p;
  casacore::Block<casacore::Matrix<casacore::Float> > vecWork_p;
  
  // h(s) [nx,ny,nscales]
  casacore::Block<casacore::Matrix<casacore::Float> > vecScales_p; 
  casacore::Block<casacore::Matrix<casacore::Complex> > vecScalesFT_p; 
  
  // B_k  [nx,ny,ntaylor]
  // casacore::Block<casacore::Matrix<casacore::Float> > vecPsf_p; 
  casacore::Block<casacore::Matrix<casacore::Complex> > vecPsfFT_p; 
  
  // I_D : Residual/Dirty Images [nx,ny,ntaylor]
  casacore::Block<casacore::Matrix<casacore::Float> > vecDirty_p; 
 
  // I_M : Model Images [nx,ny,ntaylor]
  casacore::Block<casacore::Matrix<casacore::Float> > vecModel_p; 
  //  casacore::Block <casacore::Matrix<casacore::Float> > vecScaleModel_p;
 
  // A_{smn} = B_{sm} * B{sn} [nx,ny,ntaylor,ntaylor,nscales,nscales]
  // A_{s1s2mn} = B_{s1m} * B{s2n} [nx,ny,ntaylor,ntaylor,nscales,nscales]
  casacore::Block<casacore::Matrix<casacore::Float> > cubeA_p; 

  // R_{sk} = I_D * B_{sk} [nx,ny,ntaylor,nscales]
  casacore::Block<casacore::Matrix<casacore::Float> > matR_p; 
  
  // a_{sk} = Solution vectors. [nx,ny,ntaylor,nscales]
  casacore::Block<casacore::Matrix<casacore::Float> > matCoeffs_p; 

  // casacore::Memory to be allocated per Matrix
  casacore::Double memoryMB_p;
  
  // Solve [A][Coeffs] = [I_D * B]
  // Shape of A : [ntaylor,ntaylor]
  casacore::Block<casacore::Matrix<casacore::Double> > matA_p;    // 2D matrix to be inverted.
  casacore::Block<casacore::Matrix<casacore::Double> > invMatA_p; // Inverse of matA_p;

  // FFTserver
  casacore::FFTServer<casacore::Float,casacore::Complex> fftcomplex;

  // Initial setup functions  
  casacore::Int verifyScaleSizes();
  casacore::Int allocateMemory();
  casacore::Int setupScaleFunctions();

  // Setup per major cycle
  casacore::Int setupUserMask();
  casacore::Int computeFluxLimit(casacore::Float &fluxlimit, casacore::Float threshold);
  casacore::Int computeRHS();

  // Solver functions : minor-cycle iterations. Need to be efficient.
  casacore::Int solveMatrixEqn(casacore::Int ntaylor,casacore::Int scale, casacore::IPosition blc, casacore::IPosition trc);
  casacore::Int chooseComponent(casacore::Int ntaylor,casacore::Int scale, casacore::Int criterion, casacore::IPosition blc, casacore::IPosition trc);
  casacore::Int updateModelAndRHS(casacore::Float loopgain);
  casacore::Int updateRHS(casacore::Int ntaylor, casacore::Int scale, casacore::Float loopgain,casacore::Vector<casacore::Float> coeffs, casacore::IPosition blc, casacore::IPosition trc, casacore::IPosition blcPsf, casacore::IPosition trcPsf);
  casacore::Int checkConvergence(casacore::Int updatetype, casacore::Float &fluxlimit, casacore::Float &loopgain); 
  casacore::Bool buildImagePatches();

  // Helper functions
  casacore::Int writeMatrixToDisk(casacore::String imagename, casacore::Matrix<casacore::Float> &themat);
  casacore::Int IND2(casacore::Int taylor,casacore::Int scale);
  casacore::Int IND4(casacore::Int taylor1, casacore::Int taylor2, casacore::Int scale1, casacore::Int scale2);
  
  casacore::Bool adbg;
};

} //# NAMESPACE CASA - END

#endif

