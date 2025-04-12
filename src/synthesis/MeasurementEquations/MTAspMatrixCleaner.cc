//# Copyright (C) 1995-2010
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
//# $Id:  $MTAspMatrixCleaner.cc

// Same include list as in MatrixCleaner.cc
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MatrixMath.h>
//#include <casacore/casa/Arrays/ArrayIO.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Containers/Record.h>

#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/casa/Arrays/Slicer.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <casacore/casa/OS/HostInfo.h>
#include <casacore/casa/Arrays/ArrayError.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/casa/Arrays/VectorIter.h>

#include <casacore/casa/Utilities/GenSort.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/Fallible.h>

#include <casacore/casa/BasicSL/Constants.h>

#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>

#include <synthesis/MeasurementEquations/MatrixCleaner.h>
//#include <synthesis/MeasurementEquations/AspMatrixCleaner.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <casacore/coordinates/Coordinates/TabularCoordinate.h>

#ifdef _OPENMP
#include <omp.h>
#endif

// Additional include files
#include <algorithm>
#include <chrono>

#include<synthesis/MeasurementEquations/MTAspMatrixCleaner.h>

//for gsl bfgs
//#include <synthesis/MeasurementEquations/gslobjfunc.h>
// for gsl
//using Eigen::VectorXd;

// for alglib
#include <synthesis/MeasurementEquations/objfunc_alglib.h>
using namespace alglib;

using namespace casacore;
using namespace std::chrono;
namespace casa { //# NAMESPACE CASA - BEGIN

#define MIN(a,b) ((a)<=(b) ? (a) : (b))
#define MAX(a,b) ((a)>=(b) ? (a) : (b))

MTAspMatrixCleaner::MTAspMatrixCleaner():
  AspMatrixCleaner(),
  nscales_p(2), // always 2 (0 scale and the opt scale)
  ntaylor_p(0),
  psfntaylor_p(0),
  nx_p(0),
  ny_p(0)
{

}

MTAspMatrixCleaner::~MTAspMatrixCleaner()
{
  destroyAspScales();
  destroyInitMasks();
  destroyInitScales();
  if(!itsMask.null())
    itsMask=0;
}


// MT asp-clean
Int MTAspMatrixCleaner::mtaspclean()
{
  LogIO os(LogOrigin("MTAspMatrixCleaner", "mtaspclean()", WHERE));
  os << LogIO::NORMAL1 << "MT Asp clean algorithm" << LogIO::POST;

  Int scale;

  //no need to use all cores if possible
  Int nth = itsNscales;
#ifdef _OPENMP

    nth = min(nth, omp_get_max_threads());

#endif

  // Start the iteration
  //Float totalFlux=0.0;
  Int converged=0;
  Float tmpMaximumResidual = 0.0;
  Float minMaximumResidual = 1000.0;
  Float initRMSResidual = 1000.0;
  float initModelFlux = -1.0;

  os << "Starting iteration"<< LogIO::POST;
  vector<Float> tempScaleSizes;
  itsIteration = itsStartingIter; // 0

  Matrix<Float> itsScale0 = Matrix<Float>(gip);
  Matrix<Complex> itsScaleXfr0 = Matrix<Complex> ();

  Matrix<Float> itsScale = Matrix<Float>(gip);
  Matrix<Complex> itsScaleXfr = Matrix<Complex> ();

  // Define a subregion so that the peak is centered
  IPosition support(gip);
  //support(0) = max(Int(itsInitScaleSizes[itsNInitScales-1] + 0.5), support(0)); //ms-clean's way
  //support(1) = max(Int(itsInitScaleSizes[itsNInitScales-1] + 0.5), support(1));

  // try beampatch - same as mtmfs' buildImagePatches()
  Int psupport = findBeamPatch(itsInitScaleSizes[itsNInitScales-1], nx_p, ny_p);
  support(0) = psupport;
  support(1) = psupport;
  // Generic support-size.
  psfsupport_p = IPosition(2, psupport, psupport);
  psfpeak_p = IPosition(2, psupport/2, psupport/2);

  IPosition inc(2, 1);
  for(Int i=0; i<nscales_p; i++)
  {
    vecScales_p[i].resize(psfsupport_p, false);
    vecScalesFT_p[i].resize();
  }

  
  // calculate rms residual
  float rms = 0.0;
  // should be masked
  int num = int((trcDirty(0) -blcDirty(0))* (trcDirty(1) - blcDirty(1))); 
  for (int j = blcDirty(1); j <= trcDirty(1); ++j)
  {
    for (int i = blcDirty(0); i <= trcDirty(0); ++i)
    {
      rms += pow((matR_p[IND2(0,0)])(i, j), 2);
    }
  }
  rms = rms / num;
  initRMSResidual = rms;
  //os << LogIO::NORMAL3 << "initial rms residual " << initRMSResidual << LogIO::POST;

  initModelFlux = sum(vecModel_p[0]); 
  //os << LogIO::NORMAL3 << "initial model flux " << initModelFlux << LogIO::POST; 

  for (Int ii = itsStartingIter; ii < itsMaxNiter; ii++)
  {
    //cout << "cur iter " << itsIteration << " max iter is "<< itsMaxNiter << endl;
    itsIteration++;

    // calculate rms residual
    float rms = 0.0;
    // should be masked
    int num = int((trcDirty(0) -blcDirty(0))* (trcDirty(1) - blcDirty(1))); 
    for (int j = blcDirty(1); j <= trcDirty(1); ++j)
    { 
      for (int i = blcDirty(0); i <= trcDirty(0); ++i)
      {
        rms += pow((matR_p[IND2(0,0)])(i, j), 2);
      }
    }
    rms = rms / num;

    // make single optimized scale image
    os << "Making optimized scale " << itsOptimumScaleSize << LogIO::POST;
    //cout << "Making optimized scale " << itsOptimumScaleSize << endl;

    /*if (itsSwitchedToHogbom)
    {
      makeScaleImage(itsScale0, 0.0, itsStrengthOptimum, itsPositionOptimum);
      fft.fft0(itsScaleXfr0, itsScale0);
      itsScale = 0.0;
      itsScale = itsScale0;
      itsScaleXfr.resize();
      itsScaleXfr = itsScaleXfr0;
    }
    else
    {
      makeScaleImage(itsScale, itsOptimumScaleSize, itsStrengthOptimum, itsPositionOptimum);
      itsScaleXfr.resize();
      fft.fft0(itsScaleXfr, itsScale);
    }*/

    // save scales & its FFT
    IPosition immid(2,nx_p/2, ny_p/2);
    makeMTScaleImage(itsScale0, 0.0);
    itsScaleXfr0.resize();
    fft.fft0(itsScaleXfr0, itsScale0);
    Matrix<Float> psfpatch0 = (itsScale0)(immid-psfsupport_p/2, immid+psfsupport_p/2-IPosition(2,1,1)); 
    //vecScales_p[0] = itsScale0;
    vecScales_p[0] = psfpatch0;
    vecScalesFT_p[0] = itsScaleXfr0;


    // Opt scale which can be 0 scale too. There may be more efficient way to handle 0 scale as opt scale.
    makeMTScaleImage(itsScale, itsOptimumScaleSize);
    itsScaleXfr.resize();
    fft.fft0(itsScaleXfr, itsScale);
    Matrix<Float> psfpatch = (itsScale)(immid-psfsupport_p/2, immid+psfsupport_p/2-IPosition(2,1,1));
    //vecScales_p[1] = itsScale;
    vecScales_p[1] = psfpatch;
    vecScalesFT_p[1] = itsScaleXfr;


    // try beampatch - same as mtmfs' buildImagePatches()
    /*Int psupport = setSupport(itsOptimumScaleSize, nx_p, ny_p);
    support(0) = psupport;
    support(1) = psupport;*/

    IPosition blc(itsPositionOptimum - support/2); // mtmfs
    IPosition trc(itsPositionOptimum + support/2 - 1);
    // try 2.5 sigma
    /*Int sigma5 = (Int)(5 * itsOptimumScaleSize / 2);
    IPosition blc(itsPositionOptimum - sigma5);
    IPosition trc(itsPositionOptimum + sigma5 -1);*/
    LCBox::verify(blc, trc, inc, gip);

    IPosition blcPsf(blc + psfpeak_p - itsPositionOptimum);
    IPosition trcPsf(trc + psfpeak_p - itsPositionOptimum);
    LCBox::verify(blcPsf, trcPsf, inc, psfsupport_p);

   // Reconcile box sizes/locations with the image size
   makeBoxesSameSize(blc, trc, blcPsf, trcPsf); 

    // Compute all convolutions and matA, invMatA
    // If matrix is not invertible, return!
    // This is done repeatedly because opt scale is different each time
    bool compPrinSol = false;
    bool usePatch = true;
    if (computeHessianPeak(compPrinSol, usePatch, blcPsf, trcPsf) == -1)
      return -1;

    // Only need to do once for init. The rest of the RHS update is handled by updateRHS
    // matR = residuals * the opt scale
    if (ii == itsStartingIter)
    {
      os << "Calculating convolutions of residual images with the optimal scale" << LogIO::POST;
      //cout << "Calculating convolutions of residual images with the optimal scale" << endl;
      computeRHS();
    }

    // solve matCoeffs (amplitudes) from invMatA and matR
    for(scale=0; scale<nscales_p; scale++)
      solveMatrixEqn(ntaylor_p, scale, blc, trc);
    //cout << "matCoeffs_p[IND2(0,0)] " << matCoeffs_p[IND2(0,0)](itsPositionOptimum) << endl;
    //cout << "matCoeffs_p[IND2(0,1)] " << matCoeffs_p[IND2(0,1)](itsPositionOptimum) << endl;

    // updates itsPeakResidual
    updatePeakResidual();
    
    float thres = 1e-4; //1e-4 works better for WAsp M100 and CygA cube imaging
    if (ntaylor_p > 1) //1e-8 seems to work better for wide-band
      thres = 1e-8;

    if (!itsSwitchedToHogbom &&
        //(fabs(itsPeakResidual - itsPrevPeakResidual) < thres /*1e-8/*1e-4*/)) //peakres rarely changes. 
        (fabs(itsPeakResidual - itsPrevPeakResidual) < thres )) //peakres rarely changes. 
      itsNumNoChange += 1;

    itsPrevPeakResidual = itsPeakResidual;

    if(ii == itsStartingIter)
    {
      tmpMaximumResidual = itsPeakResidual;
      os << "Initial maximum residual is " << itsPeakResidual;
      if( !itsMask.null() )
        os << " within the mask ";

      os << LogIO::POST;
    }

    //save the min peak residual
    if (abs(minMaximumResidual) > abs(itsPeakResidual))
      minMaximumResidual = abs(itsPeakResidual);

    // Various ways of stopping
    // matCoeffs should be available now.
    checkMTConvergence(converged, tmpMaximumResidual, minMaximumResidual);

    // Break out of minor-cycle loop
    if(converged != 0)
        break;

    // determine if switch to hogbom or not
    // genie redo this
    // trigger hogbom when optimal strength or itsPeakResidual is small enough
    if (itsNormMethod == 1) // only Norm Method 1 needs hogbom for speedup
    {
      if(!itsSwitchedToHogbom && (abs(itsPeakResidual) < itsFusedThreshold
         || ((abs(getOptStrength()) < (5e-4 * itsFusedThreshold)) && (itsNumNoChange >= 2))))
        // 5e-4 is a experimental number here assuming under that threshold opt strength is too small to take affect.
      {
        os << "Switch to hogbom b/c peak residual or optimum strength is small enough: " << itsFusedThreshold << LogIO::POST;
        bool runlong = false;
        
        //option 1: use rms residual to detect convergence
        if (initRMSResidual > rms && initRMSResidual/rms < 1.5)
        {
          runlong = true;
          os << LogIO::NORMAL3 << "Run hogbom for longer iterations b/c it's approaching convergence. initial rms " << initRMSResidual << " rms " << rms << LogIO::POST;
        }
        //option 2: use model flux to detect convergence
        /*float modelFlux = 0.0;
        modelFlux = sum(model);
        if (initModelFlux != 0 && (abs(initModelFlux - modelFlux)/initModelFlux < 0.01))
        {
          runlong = true;
          os << LogIO::NORMAL3 << "Run hogbom for longer iterations b/c it's approaching convergence. init model flux " << initModelFlux << " model flux " << modelFlux << LogIO::POST;
        }*/

        switchedToHogbom(runlong);

        if (itsNumNoChange >= 2)
          itsNumNoChange = 0;
      }

      if (!itsSwitchedToHogbom && itsNumNoChange >= 2)
      {
        os << "Switched to hogbom at iteration "<< ii << " b/c peakres rarely changes" << LogIO::POST;
        itsNumNoChange = 0;

        //os << LogIO::NORMAL3 << "total flux " << totalFlux << " model flux " << sum(vecModel_p[0]) << LogIO::POST; 
        bool runlong = false;

        //option 1: use rms residual to detect convergence
        if (initRMSResidual > rms && initRMSResidual/rms < 1.5)
        {
          runlong = true;
          os << LogIO::NORMAL3 << "Run hogbom for longer iterations b/c it's approaching convergence. initial rms " << initRMSResidual << " rms " << rms << LogIO::POST;
        }
        //option 2: use model flux to detect convergence
        /*float modelFlux = 0.0;
        modelFlux = sum(model);
        if (initModelFlux != 0 && (abs(initModelFlux - modelFlux)/initModelFlux < 0.01))
        {
          runlong = true;
          os << LogIO::NORMAL3 << "Run hogbom for longer iterations b/c it's approaching convergence. init model flux " << initModelFlux << " model flux " << modelFlux << LogIO::POST;
        }*/

        switchedToHogbom(runlong);
      }
    }

    if (!itsSwitchedToHogbom)
    {
      if (itsNumIterNoGoodAspen.size() >= 10)
        itsNumIterNoGoodAspen.pop_front(); // only track the past 10 iters
      if (itsOptimumScaleSize == 0)
        itsNumIterNoGoodAspen.push_back(1); // Zhang 2018 fused-Asp approach
      else
        itsNumIterNoGoodAspen.push_back(0);
    }

    // Update the model and matR from cubeA and matCoeffs
    // also updates flux counters by nterm
    //cout << "update model and rhs" << endl;
    updateModelAndRHS(itsGain, blc, trc, blcPsf, trcPsf);

    // Fill the updated residual image for scale 0 back into vecDirty_p
    for(Int taylor=0; taylor<ntaylor_p; taylor++)
    {
      vecDirty_p[taylor] = matR_p[IND2(taylor,0)]; // This is the one that gets updated during iters.
      //cout << "update residual: vecDirty_p[" << taylor <<"](positionOptimum) " << vecDirty_p[taylor](itsPositionOptimum) << " at " << itsPositionOptimum << endl;
    }

    // If we switch to hogbom (i.e. only have 0 scale size),
    // we still need to do the following Aspen update to get the new optimumStrength
    if (itsSwitchedToHogbom)
    {
      if (itsNumHogbomIter == 0)
      {
        itsSwitchedToHogbom = false;
        os << "switched back to Asp." << LogIO::POST;

        //option 1: use rms residual to detect convergence
        if (!(initRMSResidual > rms && initRMSResidual/rms < 1.5))
        {
          os << "Reached convergence at iteration "<< ii << " b/c hogbom finished" << LogIO::POST;
          converged = 1;
          os << LogIO::NORMAL3 << "initial rms " << initRMSResidual << " final rms residual " << rms << LogIO::POST; 

          break;
        }
        //option 2: use model flux to detect convergence
        /*float modelFlux = 0.0;
        modelFlux = sum(model);
        if (!(initModelFlux != 0 && (abs(initModelFlux - modelFlux)/initModelFlux < 0.01)))
        {
          os << "Reached convergence at iteration "<< ii << " b/c hogbom finished" << LogIO::POST;
          converged = 1;
          os << LogIO::NORMAL3 << "initial model flux " << initModelFlux << " final model flux " << modelFlux << LogIO::POST; 

          break;
        }*/
      }
      else
        itsNumHogbomIter -= 1;
    }

    tempScaleSizes.clear();
    tempScaleSizes = getActiveSetAspen();
    tempScaleSizes.push_back(0.0); // put 0 scale
    defineAspScales(tempScaleSizes);
  }
  // End of iteration

  if(!converged)
    os << "Failed to reach stopping threshold" << LogIO::POST;

  // Print out flux counts so far
  {
    os << "Total flux by Taylor coefficient :";
    for(Int taylor=0; taylor<ntaylor_p; taylor++)
      os << "  [" << taylor << "]: " << totalTaylorFlux_p[taylor] ;
    os << LogIO::POST;
  }
  //cout << "end of minor cycle: vecDirty_p[0](256,231) " << (vecDirty_p[0])(256,231) << endl;
  return converged;
}



// ALGLIB
vector<Float> MTAspMatrixCleaner::getActiveSetAspen()
{
  LogIO os(LogOrigin("MTAspMatrixCleaner", "getActiveSetAspen()", WHERE));

  if(int(itsInitScaleXfrs.nelements()) == 0)
    throw(AipsError("Initial scales for Asp are not defined"));

  if (!itsSwitchedToHogbom &&
  	  accumulate(itsNumIterNoGoodAspen.begin(), itsNumIterNoGoodAspen.end(), 0) >= 5)
  {
  	os << "Switched to hogbom because of frequent small components." << LogIO::POST;
    switchedToHogbom();
  }

  if (itsSwitchedToHogbom)
  	itsNInitScales = 1;
  else
  	itsNInitScales = itsInitScaleSizes.size();

  // Dirty * initial scales
  Matrix<Complex> dirtyFT;
  //if (ntaylor_p > 1) //wide-band
  fft.fft0(dirtyFT, vecDirty_p[0]); //use 0 nterm residual to getActiveSetAspen

  itsDirtyConvInitScales.resize(0);
  itsDirtyConvInitScales.resize(itsNInitScales); // 0, 1width, 2width, 4width and 8width
  //cout << "itsInitScaleSizes.size() " << itsInitScaleSizes.size() << " itsInitScales.size() " << itsInitScales.size() << " NInitScales # " << itsNInitScales << endl;
  for (int scale=0; scale < itsNInitScales; scale++)
  {
    Matrix<Complex> cWork;

    itsDirtyConvInitScales[scale] = Matrix<Float>(psfShape_p);
    cWork=((dirtyFT)*(itsInitScaleXfrs[scale]));
    fft.fft0((itsDirtyConvInitScales[scale]), cWork, false);
    fft.flip((itsDirtyConvInitScales[scale]), false, false);

    //cout << "remake itsDirtyConvInitScales " << scale << " max itsInitScales[" << scale << "] = " << max(fabs(itsInitScales[scale])) << endl;
    //cout << " max itsInitScaleXfrs[" << scale << "] = " << max(fabs(itsInitScaleXfrs[scale])) << endl;
  }

  float strengthOptimum = 0.0;
  int optimumScale = 0;
  IPosition positionOptimum(2/*itsDirty->shape().nelements()*/, 0);
  itsGoodAspActiveSet.resize(0);
  itsGoodAspAmplitude.resize(0);
  itsGoodAspCenter.resize(0);

  maxDirtyConvInitScales(strengthOptimum, optimumScale, positionOptimum);
  os << "Peak among the smoothed residual image is " << strengthOptimum  << " and initial scale: " << optimumScale << LogIO::POST;
  //cout << " its itsDirty is " << (*itsDirty)(positionOptimum);
  //cout << "Peak among the smoothed residual image (strengthOptimum) is " << strengthOptimum  << " and initial scale: " << optimumScale << endl;
  //cout << " its dirty vecDirty_p[0] is " << (vecDirty_p[0])(positionOptimum);
  //cout << " at location " << positionOptimum[0] << " " << positionOptimum[1] << endl;


  itsStrengthOptimum = strengthOptimum;
  itsPositionOptimum = positionOptimum;
  itsOptimumScale = optimumScale;
  itsOptimumScaleSize = itsInitScaleSizes[optimumScale];

  // initial scale size = 0 gives the peak res, so we don't
  // need to do the LBFGS optimization for it
  if (itsOptimumScale == 0)
    return {};
  else
  {
    // the new aspen is always added to the active-set
    vector<Float> tempx;
    vector<IPosition> activeSetCenter;

    tempx.push_back(strengthOptimum);
    tempx.push_back(itsInitScaleSizes[optimumScale]);
    activeSetCenter.push_back(positionOptimum);

    // initialize alglib option
    unsigned int length = tempx.size();
    real_1d_array x;
    x.setlength(length);

    // for G55 ,etc
    real_1d_array s;
    s.setlength(length);

    // initialize starting point
    for (unsigned int i = 0; i < length; i+=2)
    {
	      x[i] = tempx[i]; //amp
	      x[i+1] = tempx[i+1]; //scale

        s[i] = tempx[i]; //amp
        s[i+1] = tempx[i+1]; //scale
	  }

    ParamAlglibObj *ptrParam;
    /*if (ntaylor_p > 1) //wide-band
    {
      ParamAlglibObj optParam(vecDirty_p[0], vecPsfFT_p[0], activeSetCenter, fft);
      ptrParam = &optParam;
    }
    else //narrow-band
    {*/
    /*itsDirty=new Matrix<Float>(psfShape_p);
    itsDirty->assign(vecDirty_p[0]);
    itsXfr=new Matrix<Complex>();
    itsXfr->assign(vecPsfFT_p[0]);*/
    if (ntaylor_p > 1)
      itsDirty->assign(vecDirty_p[0]); // update residual
	  ParamAlglibObj optParam(*itsDirty, *itsXfr, activeSetCenter, fft);
    ptrParam = &optParam;
    //}

	  //real_1d_array s = "[1,1]";
	  double epsg = 1e-3;
	  double epsf = 1e-3;
	  double epsx = 1e-3;
	  ae_int_t maxits = 5;
	  minlbfgsstate state;
	  minlbfgscreate(1, x, state);
	  minlbfgssetcond(state, epsg, epsf, epsx, maxits);
	  minlbfgssetscale(state, s);
	  minlbfgsreport rep;
	  alglib::minlbfgsoptimize(state, objfunc_alglib, NULL, (void *) ptrParam);
	  minlbfgsresults(state, x, rep);
	  // end alglib bfgsoptimization

    double amp = x[0]; // i
    double scale = x[1]; // i+1
    //scale = (scale = fabs(scale)) < 0.4 ? 0 : scale;
    // genie check on this since it's a new change adopted from Asp
    if (fabs(scale) < 0.4)
    {
      scale = 0;
      amp = (*itsDirty)(itsPositionOptimum); // This is to avoid divergence due to amp being too large.                                             // amp=strengthOptimum gives similar results
    }
    else
      scale = fabs(scale);


    itsGoodAspAmplitude.push_back(amp); // active-set amplitude
    itsGoodAspActiveSet.push_back(scale); // active-set

    itsStrengthOptimum = amp;
    itsOptimumScaleSize = scale;
    itsGoodAspCenter = activeSetCenter;

    // debug
    os << "optimized strengthOptimum " << itsStrengthOptimum << " scale size " << itsOptimumScaleSize << LogIO::POST;
    //cout << "optimized strengthOptimum " << itsStrengthOptimum << " scale size " << itsOptimumScaleSize << endl;

  } // finish bfgs optimization

  AlwaysAssert(itsGoodAspCenter.size() == itsGoodAspActiveSet.size(), AipsError);
  AlwaysAssert(itsGoodAspAmplitude.size() == itsGoodAspActiveSet.size(), AipsError);

  // debug info
  /*for (unsigned int i = 0; i < itsAspAmplitude.size(); i++)
  {
    //cout << "After opt AspApm[" << i << "] = " << itsAspAmplitude[i] << endl;
    //cout << "After opt AspScale[" << i << "] = " << itsAspScaleSizes[i] << endl;
    //cout << "After opt AspCenter[" << i << "] = " << itsAspCenter[i] << endl;
    cout << "AspScale[ " << i << " ] = " << itsAspScaleSizes[i] << " center " << itsAspCenter[i] << endl;
  }*/

  return itsGoodAspActiveSet; // return optimized scale
}



Bool MTAspMatrixCleaner::setNTaylorTerms(const int & nterms)
{
   ntaylor_p = nterms;
   psfntaylor_p = 2*nterms-1;
   totalTaylorFlux_p.resize(ntaylor_p);
   totalTaylorFlux_p.set(0.0);
   return true;
}

Int MTAspMatrixCleaner::allocateMemory(casacore::Int nx,casacore::Int ny)
{
  LogIO os(LogOrigin("MTAspMatrixCleaner", "allocateMemory()", WHERE));

  nx_p = nx;
  ny_p = ny;

  Int ntotal4d = (nscales_p*(nscales_p+1)/2) * (ntaylor_p*(ntaylor_p+1)/2);

  // shape of all matrices
  gip = IPosition(2, nx_p, ny_p);
  IPosition tgip(2, ntaylor_p, ntaylor_p);
  psfShape_p = gip;

  // Small HessianPeak matrix to be inverted for each point..
  matA_p.resize(nscales_p);
  invMatA_p.resize(nscales_p);
  for (Int i=0; i<nscales_p; i++)
  {
    matA_p[i].resize(tgip);
    invMatA_p[i].resize(tgip);
  }

  // I_D
  //dirtyFT_p.resize();

  cWork_p.resize();

  // Scales
  vecScales_p.resize(nscales_p);
  vecScalesFT_p.resize(nscales_p);
  vecWork_p.resize(nscales_p);
  for(Int i=0; i<nscales_p; i++)
  {
    //vecScales_p[i].resize(gip);
    vecScales_p[i].resize(psfsupport_p);
    vecScalesFT_p[i].resize();
    vecWork_p[i].resize(gip);
  }

  // Psfs and Models
  vecPsfFT_p.resize(psfntaylor_p);
  for(Int i=0; i < psfntaylor_p; i++)
    vecPsfFT_p[i].resize();

  // Dirty/Residual Images
  vecDirty_p.resize(ntaylor_p);
  vecModel_p.resize(ntaylor_p);
  for(Int i=0; i<ntaylor_p; i++)
  {
    vecDirty_p[i].resize();
    vecModel_p[i].resize(gip);
  }

  // (Psf * Scales) * (Psf * Scales)
  cubeA_p.resize(ntotal4d);
  for(Int i=0; i<ntotal4d; i++)
    cubeA_p[i].resize();

  // I_D * (Psf * Scales)
  matR_p.resize(ntaylor_p * nscales_p);
  // Coefficients to be solved for.
  matCoeffs_p.resize(ntaylor_p * nscales_p);

  for(Int i=0; i < ntaylor_p * nscales_p; i++)
  {
    matR_p[i].resize(gip);
    matCoeffs_p[i].resize(gip);
    matCoeffs_p[i] = 0.0;
  }

  // set trc, blc for efficiency
  blcDirty=IPosition(2/*itsDirty->shape().nelements()*/, 0);
  trcDirty=IPosition(2, nx -1, ny -1/*itsDirty->shape() - 1*/);

  if(!itsMask.null())
  {
    AlwaysAssert(itsMask->shape()(0) == nx, AipsError);
    AlwaysAssert(itsMask->shape()(1) == ny, AipsError);
    Int xbeg=nx-1;
    Int ybeg=ny-1;
    Int xend=0;
    Int yend=0;
    for (Int iy=0;iy<ny;iy++)
    {
      for (Int ix=0;ix<nx;ix++)
      {
        if((*itsMask)(ix,iy)>0.000001)
        {
          xbeg=min(xbeg,ix);
          ybeg=min(ybeg,iy);
          xend=max(xend,ix);
          yend=max(yend,iy);
        }
      }
    }
    blcDirty(0)=xbeg;
    blcDirty(1)=ybeg;
    trcDirty(0)=xend;
    trcDirty(1)=yend;
  }

  return 0;
}

Int MTAspMatrixCleaner::setSupport(const Float scaleSize, const Int& nx, const Int& ny,
         const Float psfBeam, const Float nBeams)
{
  Int psupport = (Int)(sqrt(psfBeam*psfBeam + scaleSize*scaleSize) * nBeams);

  // At least this big...
  if(psupport < psfBeam*nBeams)
    psupport = static_cast<Int>(psfBeam*nBeams);

  // Not too big...
  if(psupport > nx || psupport > ny)
    psupport = std::min(nx,ny);

  // Make it even.
  if (psupport%2 != 0)
    psupport -= 1;

  return psupport;
}

bool MTAspMatrixCleaner::setMTPsf(int order, Matrix<Float> & psf)
{
  AlwaysAssert((order >= (int)0 && order < (int)vecPsfFT_p.nelements()), AipsError);
  if (order == 0)
  {
    AlwaysAssert(validatePsf(psf), AipsError); // this is critical to set itsPositionPeakPsf
    //psfShape_p.resize(0, false);
    //psfShape_p=psf.shape(); // init this so wide-band can re-use narrow-band code
  }

  // Directly store the FFT of the PSFs.
  // The FT'd matrix is reshaped automatically.
  fft.fft0(vecPsfFT_p[order], psf, false);

  if (order == 0)
  {
    itsXfr = new Matrix<Complex>();
    itsXfr->assign(vecPsfFT_p[0]);
  }

  return true;
}

Bool MTAspMatrixCleaner::setResidual(int order, Matrix<Float> & dirty)
{
  AlwaysAssert((order >= (int)0 && order < (int)vecDirty_p.nelements()), AipsError);
  vecDirty_p[order].reference(dirty);

  if (order == 0)
  {
    itsDirty=new Matrix<Float>(psfShape_p);
    itsDirty->assign(vecDirty_p[0]); 
    //cout << "setResidual: vecDirty_p[0](256,231) " << (vecDirty_p[0])(256,231) << endl;
  }

  return true;
}

Bool MTAspMatrixCleaner::getResidual(int order, Matrix<Float> & residual)
{
  AlwaysAssert((order >= (int)0 && order < (int)vecDirty_p.nelements()), AipsError);
  residual.assign(vecDirty_p[order]);

  //if (order == 0)
  //  cout << "getResidual: vecDirty_p[0](256,231) " << (vecDirty_p[0])(256,231) << endl;

  return true;
}

Bool MTAspMatrixCleaner::setModel(int order, Matrix<Float> & model)
{
  AlwaysAssert((order >= (int)0 && order < (int)vecModel_p.nelements()), AipsError);
  vecModel_p[order].assign(model);
  totalTaylorFlux_p[order] = (sum(vecModel_p[order]));

  return true;
}

Bool MTAspMatrixCleaner::getModel(int order, Matrix<Float> & model)
{
  AlwaysAssert((order>=(int)0 && order<(int)vecModel_p.nelements()), AipsError);
  model.assign(vecModel_p[order]);
  return true;
}

/* Indexing Rules... */
Int MTAspMatrixCleaner::IND2(Int taylor, Int scale)
{
  return  taylor * nscales_p + scale;
}

Int MTAspMatrixCleaner::IND4(Int taylor1, Int taylor2, Int scale1, Int scale2)
{
  Int tt1=taylor1;
  Int tt2=taylor2;
  Int ts1=scale1;
  Int ts2=scale2;
  scale1 = MAX(ts1,ts2);
  scale2 = MIN(ts1,ts2);
  taylor1 = MAX(tt1,tt2);
  taylor2 = MIN(tt1,tt2);
  Int totscale = nscales_p*(nscales_p+1)/2;
  return ((taylor1*(taylor1+1)/2)+taylor2)*totscale + ((scale1*(scale1+1)/2)+scale2);
}

// Compute the convolutions of the smoothed psfs with each other.
Int MTAspMatrixCleaner::computeHessianPeak(Bool compPrinSol, Bool usePatch, IPosition blcPsf, IPosition trcPsf)
{
  LogIO os(LogOrigin("MTAspMatrixCleaner", "computeHessianPeak", WHERE));
  gip = IPosition(2, nx_p, ny_p);
  IPosition tgip(2, ntaylor_p, ntaylor_p);

  // computePrincipalSolution only needs Hessian, invMatA, for 0 scale
  if (compPrinSol)
  {
    vecScalesFT_p[0] = itsInitScaleXfrs[0];
    vecScalesFT_p[1] = itsInitScaleXfrs[0];
  }

  // (PSF * opt scale) * (PSF * opt scale) -> cubeA_p [nx_p,ny_p,ntaylor,ntaylor]
  os << "Calculating PSF and optimal scale convolutions " << LogIO::POST;
  for (Int taylor1=0; taylor1<ntaylor_p; taylor1++)
  {
    for (Int taylor2=0; taylor2<=taylor1; taylor2++)
    {
      for (Int scale1=0; scale1<nscales_p; scale1++)
      {
        for (Int scale2=0; scale2<=scale1; scale2++)
        {
          Int ttay1 = taylor1 + taylor2;
          // CALC Hess : Calculate  PSF_(t1+t2)  * opt scale * opt scale
          cWork_p.assign((vecPsfFT_p[ttay1])*(vecScalesFT_p[scale1])*(vecScalesFT_p[scale2]));

          if(usePatch)
          {
            cubeA_p[IND4(taylor1,taylor2,scale1,scale2)].resize(psfsupport_p, false); //mtmfs's
            //cubeA_p[IND4(taylor1,taylor2,scale1,scale2)].resize(gip, false); // this fixes gip but has error at "I'm fine 2"

            fft.fft0(vecWork_p[0], cWork_p, false);
            Matrix<Float> psfpatch = (vecWork_p[0])(itsPositionPeakPsf-psfsupport_p/2,itsPositionPeakPsf+psfsupport_p/2-IPosition(2,1,1));
            cubeA_p[IND4(taylor1,taylor2,scale1,scale2)] = psfpatch; //cout << "I'm fine 2" << endl;
          }
          else
          {
            cubeA_p[IND4(taylor1,taylor2,scale1,scale2)].resize(gip, false);
            fft.fft0(cubeA_p[IND4(taylor1,taylor2,scale1,scale2)], cWork_p, false);
          }
        }
      }
    }
  }

  // debug info
  float maxvalue;
  IPosition peakpos;
  /*findMaxAbs(vecScales_p[0], maxvalue, peakpos);
  cout << "vecScales_p[0] pos " << peakpos << " maxval " << maxvalue << " itsPositionOptimum " << itsPositionOptimum << endl;
  findMaxAbs(vecScales_p[1], maxvalue, peakpos);
  cout << "vecScales_p[1] pos " << peakpos << " maxval " << maxvalue << endl;*/

  findMaxAbs(cubeA_p[IND4(0,0,0,0)], maxvalue, peakpos);
  //cout << "cubeA_p[0] pos " << peakpos << " maxval " << maxvalue << endl;
  psfpeak_p = peakpos; //genie trial or norm to make cubeA_p[0](psfpeak_p) 1
  //cout << "cubeA_p[0](psfpeak_p) " << cubeA_p[0](psfpeak_p) << " (psfpeak_p) " << (psfpeak_p) << endl;

  // Construct A, invA for each scale.
  if (itsPositionPeakPsf != IPosition(2,(nx_p/2),(ny_p/2)))
    os << LogIO::WARN << "The PSF peak is not at the center of the image..." << LogIO::POST;

  bool stopnow = false;
  for (Int scale=0; scale<nscales_p; scale++)
  {
    // Fill up A
    for (Int taylor1=0; taylor1<ntaylor_p; taylor1++)
    {
      for (Int taylor2=0; taylor2<ntaylor_p; taylor2++)
      {
        matA_p[scale].resize(tgip, false);
        invMatA_p[scale].resize(tgip, false);
        //cout << "taylor1 " << taylor1 << " taylor2 " << taylor2 << " scale " << scale << " psfpeak_p " << psfpeak_p << endl;
        (matA_p[scale])(taylor1,taylor2) = (cubeA_p[IND4(taylor1,taylor2,scale,scale)])(psfpeak_p);
        // Check for exact zeros ON MAIN DIAGONAL. Usually indicative of error
        if (taylor1==taylor2 && fabs((matA_p[scale])(taylor1,taylor2)) == 0.0)
          stopnow = true;
      }
    }

    if (stopnow)
    {
      os << "Multi-Term Hessian has exact zeros on its main diagonal. Not proceeding further." << LogIO::WARN << endl;
      os << "The Matrix [A] is : " << (matA_p[scale]) << LogIO::POST;
      return -1;
    }

    // If it's narrow-band, invH = 1/H (we are sure it's non-zero)
    if (ntaylor_p == 1)
    {
      (invMatA_p[scale]) = 1.0/(matA_p[scale]);
      os << "The Matrix [H] for scale[" << scale << "] is : " << (matA_p[scale]) << LogIO::POST;
      os << "Inverse of [H] is : " << (invMatA_p[scale]) << LogIO::POST;
      
      continue;
    }

    // If all elements are non-zero, check by brute-force if the rows/cols
    // are nearly linearly dependant, making the matrix nearly singular.
    Vector<Float> ratios(ntaylor_p);
    for (Int taylor1=0; taylor1<ntaylor_p-1; taylor1++)
    {
      for (Int taylor2=0; taylor2<ntaylor_p; taylor2++)
        ratios[taylor2] = (matA_p[scale])(taylor1,taylor2) / (matA_p[scale])(taylor1+1,taylor2);

      Float tsum=0.0;
      for (Int taylor2=0; taylor2<ntaylor_p-1; taylor2++)
        tsum += fabs(ratios[taylor2] - ratios[taylor2+1]);
      tsum /= ntaylor_p-1;

      if (tsum < 1e-04)
        stopnow = true;
    }

    if(stopnow)
    {
      os << "Multi-Term Hessian has linearly-dependent rows/cols. Not proceeding further." << LogIO::WARN << endl;
      os << "The Matrix [A] is : " << (matA_p[scale]) << LogIO::POST;
      return -1;
    }

    // Try to invert the matrix. If it fails, return.
    try
    {
      Double deter = 0.0;
      os << "The Matrix [H] for scale[" << scale << "] is : " << (matA_p[scale]) << LogIO::POST;
      //invert((invMatA_p[scale]), deter, (matA_p[scale])); //genie this works for full image
      //os << "Matrix Inverse : inv(A) : " << (invMatA_p[scale]) << LogIO::POST;

      invertSymPosDef((invMatA_p[scale]), deter, (matA_p[scale]));
      os << "Lapack Cholesky Decomposition Inverse of [A] is : " << (invMatA_p[scale]) << LogIO::POST;
    }

    catch(AipsError &x)
    {
      os << "The Matrix [A] is : " << (matA_p[scale]) << LogIO::POST;
      os << "Cannot Invert matrix : " << x.getMesg() << LogIO::WARN;
      return -1;
    }
  } // end of scales

   return 0;
}

/***************************************
 *  Compute residual images (all terms) * with the opt scale.
 *  --> the Right-Hand-Side of the matrix equation.
 ****************************************/
Int MTAspMatrixCleaner::computeRHS()
{ 
  // debug
  /*Float maxres = 0.0;
  IPosition maxrespos;
  findMaxAbsMask((matR_p[IND2(0,0)]), itsInitScaleMasks[0], maxres, maxrespos);
  cout << "computeRHS: before matR_p[0] maxres " << fabs(maxres) << ", maxrespos " << maxrespos << endl;*/

  // (I_D * scale) -> matR_p [nx_p,ny_p,ntaylor]
  for (Int taylor=0; taylor<ntaylor_p; taylor++)
  {
     /* Compute FT of dirty image */
    fft.fft0(dirtyFT_p, vecDirty_p[taylor], false);

    for (Int scale=0; scale<nscales_p; scale++)
    {
      cWork_p.assign((dirtyFT_p) * (vecScalesFT_p[scale]));
      fft.fft0(matR_p[IND2(taylor,scale)], cWork_p, false);
      fft.flip(matR_p[IND2(taylor,scale)], false, false);
    }
  }
  //cout << "computeRHS: after compute matR_p[0](256,231) " << (matR_p[IND2(0,0)])(256,231) << endl;
  // debug
  /*findMaxAbsMask((matR_p[IND2(0,0)]), itsInitScaleMasks[0], maxres, maxrespos);
  cout << "computeRHS: after matR_p[0] maxres " << fabs(maxres) << ", maxrespos " << maxrespos << endl;*/

  return 0;
}

/***************************************
 *  Solve for the coefficients, matCoeff_p, for the opt scale
 ****************************************/
Int MTAspMatrixCleaner::solveMatrixEqn(Int ntaylor, Int scale, IPosition blc, IPosition trc)
{
  //cout << "solveMatrixEqn: matR_p[0," << scale << "](256,56) " << (matR_p[IND2(0,scale)])(256,56) << " " << blc << " " << trc << endl;
  //cout << "solveMatrixEqn: matR_p[1," << scale << "](256,56) " << (matR_p[IND2(1,scale)])(256,56) << endl;
  //cout << "solveMatrixEqn: matR_p[2," << scale << "](256,56) " << (matR_p[IND2(2,scale)])(256,56) << endl;
  for(Int taylor1=0; taylor1<ntaylor; taylor1++)
  {
    Matrix<Float> coeffs = (matCoeffs_p[IND2(taylor1,scale)])(blc,trc);
    coeffs = 0.0;

    for(Int taylor2=0; taylor2<ntaylor; taylor2++)
    {
      Matrix<Float> rhs = (matR_p[IND2(taylor2,scale)])(blc,trc);
      coeffs = coeffs + ((Float)(invMatA_p[scale])(taylor1,taylor2)) * rhs;
    }
  }

  return 0;
}


void MTAspMatrixCleaner::updatePeakResidual()
{
  LogIO os(LogOrigin("MTAspMatrixCleaner", "updatePeakResidual()", WHERE));
  Float rmaxval = 0.0;
  Float maxres = 0.0;
  IPosition maxrespos;

  findMaxAbsMask((matR_p[IND2(0,0)]), itsInitScaleMasks[0], maxres, maxrespos);
  //Float norma = (1.0/(matA_p[0])(0,0));
  //rmaxval = fabs(maxres * norma);
  rmaxval = fabs(maxres); //genie, I probably don't need norma. In mtmfs, norma is always 1.
  itsPeakResidual = fabs(rmaxval);
  //cout << "maxres " << maxres << ", maxrespos " << maxrespos << " norma " << (1.0/(matA_p[0])(0,0)) << endl;
  //cout << "current peakres " << itsPeakResidual << endl;
  os << "current peakres " << itsPeakResidual << LogIO::POST;
}

//mtasp
void MTAspMatrixCleaner::checkMTConvergence(Int &converged, Float tmpMaximumResidual, Float minMaximumResidual)
{
  LogIO os(LogOrigin("MTAspMatrixCleaner", "checkMTConvergence()", WHERE));

  // Use the maximum residual from term0 to compare against the convergence threshold
  if(/*!itsSwitchedToHogbom &&*/ fabs(itsPeakResidual) < threshold())
  {
    os << "Reached stopping threshold " << threshold()<< ". Peak residual " << itsPeakResidual ;
    if( ! itsMask.null() ) {os << " (within mask) " ;}
    os << LogIO::POST;
    converged = 1;

    itsSwitchedToHogbom = false;
    //os << LogIO::NORMAL3 << "final rms residual " << rms << ", model flux " << sum(vecModel_p[0]) << LogIO::POST;

    return;
  }

  // Stop if below fusedthreshold. 1e-6 is an experimental number
  if (/*!itsSwitchedToHogbom &&*/ fabs(getOptStrength()) < (1e-6 * itsFusedThreshold))
  {
    //cout << "Reached stopping fusedthreshold " << 1e-6 * itsFusedThreshold << endl;
    os << "Reached stopping fusedthreshold " << 1e-6 * itsFusedThreshold << LogIO::POST;
    os << "Optimum flux is " << fabs(getOptStrength()) << LogIO::POST;
    converged = 1;

    itsSwitchedToHogbom = false;
    //os << LogIO::NORMAL3 << "final rms residual " << rms << ", model flux " << sum(vecModel_p[0]) << LogIO::POST;

    return;
  }

 //    2. negatives on largest scale?
  if ((itsNscales > 1) && itsStopAtLargeScaleNegative &&
      itsOptimumScale == (itsNInitScales - 1) &&
      getOptStrength() < 0.0)
  {
    os << "Reached negative on largest scale" << LogIO::POST;
    converged = -2;
    return;
  }

  //4. Diverging large scale
  //If actual value is 50% above the maximum residual. ..good chance it will not recover at this stage
  if ((abs(itsPeakResidual) - abs(tmpMaximumResidual)) > (abs(tmpMaximumResidual)/2.0) ||
      (abs(itsPeakResidual) - abs(minMaximumResidual)) > (abs(minMaximumResidual)/2.0))
  {
    //cout << "Diverging due to large scale?" << endl;
    os << "Diverging due to large scale?" << LogIO::POST;
    os << "itsPeakResidual " << itsPeakResidual << " tmp " << tmpMaximumResidual << " minMaximumResidual " << minMaximumResidual << LogIO::POST;   
    converged = -2;

    itsSwitchedToHogbom = false;
    //os << LogIO::NORMAL3 << "final rms residual " << rms << ", model flux " << sum(vecModel_p[0]) << LogIO::POST;
    
    return;
  }
}

/***************************************
 *  Update the model images and the convolved residuals
 ****************************************/
Int MTAspMatrixCleaner::updateModelAndRHS(Float loopgain, IPosition blc, IPosition trc, IPosition blcPsf, IPosition trcPsf)
{
  // Update the model images
  Matrix<Float> scaleSub = (vecScales_p[1])(blcPsf, trcPsf);
  for(Int taylor=0; taylor<ntaylor_p; taylor++)
  {
    Matrix<Float> modelSub = (vecModel_p[taylor])(blc,trc);
    modelSub += scaleSub * loopgain * (matCoeffs_p[IND2(taylor,1)])(itsPositionOptimum);

    // debug
    /*float maxvalue;
    IPosition peakpos;
    findMaxAbs(vecModel_p[taylor], maxvalue, peakpos);
    cout << "model[t" << taylor << "]: maxval " << maxvalue << endl;*/
  }
  //cout << "(vecModel_p[0])(itsPositionOptimum) " << (vecModel_p[0])(itsPositionOptimum) << " itsPositionOptimum " << itsPositionOptimum << endl;

  // Update the convolved residuals
  Vector<Float> coeffs(ntaylor_p);
  for(Int taylor=0; taylor<ntaylor_p; taylor++)
    coeffs[taylor] = (matCoeffs_p[IND2(taylor,1)])(itsPositionOptimum);
  /*cout << "coeffs[t0] " << coeffs[0] << endl;
  cout << "coeffs[t1] " << coeffs[1] << endl;
  cout << "coeffs[t2] " << coeffs[2] << endl;*/

  for (Int scale=0; scale<nscales_p; scale++)
    updateRHS(ntaylor_p, scale, loopgain, coeffs, blc, trc, blcPsf, trcPsf);

  // Update flux counters
  for(Int taylor=0; taylor<ntaylor_p; taylor++)
    totalTaylorFlux_p[taylor] += loopgain*(matCoeffs_p[IND2(taylor,1)])(itsPositionOptimum);

  return 0;
}

// Update the RHS vector, matR
Int MTAspMatrixCleaner::updateRHS(Int ntaylor, Int scale, Float loopgain, Vector<Float> coeffs, IPosition blc, IPosition trc, IPosition blcPsf, IPosition trcPsf)
{ 
  //cout << "before update matR_p[0](256,231) " << (matR_p[IND2(0,0)])(256,231) << " blc trc " << blc << " " << trc << " " << blcPsf << " " << trcPsf << endl;
  for (Int taylor1=0; taylor1<ntaylor; taylor1++)
  {
    Matrix<Float> residSub = (matR_p[IND2(taylor1,scale)])(blc,trc);
    for (Int taylor2=0; taylor2<ntaylor; taylor2++)
    {
      Matrix<Float> smoothSub = (cubeA_p[IND4(taylor1,taylor2,scale,1)])(blcPsf,trcPsf);
      residSub -= smoothSub * loopgain * coeffs[taylor2];

      // debug info
      /*if (taylor1 == 0 && taylor2 == 0 && scale == 0)
        cout << " smoothSub " << smoothSub(256,231) << " loopgain " << loopgain << " coeffs " << coeffs[0] << endl;*/

    }
  }
  //cout << "after update matR_p[0](256,231) " << (matR_p[IND2(0,0)])(256,231) << endl;

  // debug
  /*Float rmaxval = 0.0;
  Float maxres = 0.0;
  IPosition maxrespos;
  findMaxAbsMask((matR_p[IND2(0,0)]), itsInitScaleMasks[0], maxres, maxrespos);
  cout << "updateRHS: matR_p[0] maxres " << fabs(maxres) << ", maxrespos " << maxrespos << endl;*/

  return 0;
}

Float MTAspMatrixCleaner::getOptStrength()
{
  Float maxOptStrength = 0.0;
  Float maxval = 0.0;
  IPosition maxpos;

  for(Int taylor=0; taylor<ntaylor_p; taylor++)
  {
    findMaxAbsMask((matCoeffs_p[IND2(taylor,1)]), itsInitScaleMasks[0], maxval, maxpos);
    if (fabs(maxval) > fabs(maxOptStrength))
      maxOptStrength = maxval;
  }

  return maxOptStrength;
}

// Compute principal solution in-place on the list of residual images (vecDirty)
Bool MTAspMatrixCleaner::computePrincipalSolution()
{
    LogIO os(LogOrigin("MTAspMatrixCleaner", "computePrincipalSolution()", WHERE));

    os << "Asp-Clean:: Computing principal solution on residuals" << LogIO::POST;

    bool compPrinSol = true;
    bool usePatch = false;
    if (computeHessianPeak(compPrinSol, usePatch, IPosition(2,0,0), IPosition(2,nx_p-1,ny_p-1)) == -1)
      return false;

    AlwaysAssert((vecDirty_p.nelements()>0), AipsError);

    /* Solve for the coefficients at the delta-function scale*/
    for (Int taylor1=0; taylor1<ntaylor_p; taylor1++)
    {
      (matCoeffs_p[IND2(taylor1,0)]) = 0.0;
      for (Int taylor2=0; taylor2<ntaylor_p; taylor2++)
        matCoeffs_p[IND2(taylor1,0)] = matCoeffs_p[IND2(taylor1,0)] + ((Float)(invMatA_p[0])(taylor1,taylor2))*(vecDirty_p[taylor2]);
    }

    /* Copy this into the residual vector */
    for (Int taylor=0; taylor<ntaylor_p; taylor++)
      vecDirty_p[taylor].assign(matCoeffs_p[IND2(taylor,0)]);

    return true;
}

// Make a single scale size image by Gaussian with peak at the center
void MTAspMatrixCleaner::makeMTScaleImage(Matrix<Float>& iscale, const Float& scaleSize)
{
  LogIO os( LogOrigin("MTAspMatrixCleaner","makeMTScaleImage",WHERE) );

  const Int nx = iscale.shape()(0);
  const Int ny = iscale.shape()(1);
  iscale = 0.0;

  const Double refi = nx/2;
  const Double refj = ny/2;

  if(scaleSize == 0.0)
    iscale(Int(refi), Int(refj)) = 1.0;
  else
  {
    AlwaysAssert(scaleSize>0.0, AipsError);
    os << "Making MTAsp scale size " << scaleSize << " pixels." << LogIO::POST;

    // has to make the whole scale image
    for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {
        iscale(i,j) = (1.0/(sqrt(2*M_PI)*scaleSize))*exp(-(pow(i-refi,2) + pow(j-refj,2))*0.5/pow(scaleSize,2)); //this is for 1D, but represents Sanjay's and gives good init scale
      }
    }

  }
}


} //# NAMESPACE CASA - END
