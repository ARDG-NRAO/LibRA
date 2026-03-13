//# Copyright (C) 1996-2010
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
//#        Internet email: casa-feedback@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id:  $AspMatrixCleaner.cc

// Same include list as in MatrixCleaner.cc
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MatrixMath.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Logging/LogIO.h>

#include <synthesis/MeasurementEquations/AspMatrixCleaner.h>
#include <synthesis/TransformMachines2/Utils.h>

#include<synthesis/MeasurementEquations/WaveletAspCleaner.h>

// for alglib
#include <synthesis/MeasurementEquations/objfunc_alglib.h>
#include <synthesis/MeasurementEquations/objfunc_alglib_wavelets.h>
using namespace alglib;

using namespace casacore;
using namespace std::chrono;
namespace casa { //# NAMESPACE CASA - BEGIN
WaveletAspCleaner::WaveletAspCleaner():
  AspMatrixCleaner(),
  itsInitScaleSizes(0),
  //itsAspScaleSizes(0),
  //itsAspAmplitude(0),
  itsNInitScales(5),
  itsPsfWidth(0.0),
  itsUseZhang(false),
  itsSwitchedToHogbom(false),
  itsNumHogbomIter(0),
  itsNthHogbom(0),
  itsOptimumScale(0),
  itsOptimumScaleSize(0.0),
  itsPeakResidual(1000.0), // temp. should this be changed to MAX?
  itsPrevPeakResidual(0.0),
  itsOrigDirty( ),
  itsFusedThreshold(0.0),
  itsdimensionsareeven(true),
  itsstopMS(false),
  itsLbfgsEpsF(0.001),
  itsLbfgsEpsX(0.001),
  itsLbfgsEpsG(0.001),
  itsLbfgsMaxit(5),
  itsNumNoChange(0),
  itsBinSizeForSumFlux(4),
  itsUserLargestScale(-1.0)
{
  itsInitScales.resize(0);
  itsInitScaleXfrs.resize(0);
  itsDirtyConvInitScales.resize(0);
  itsInitScaleMasks.resize(0);
  itsPsfConvInitScales.resize(0);
  itsNumIterNoGoodAspen.resize(0);
  //itsAspCenter.resize(0);
  itsGoodAspActiveSet.resize(0);
  itsGoodAspAmplitude.resize(0);
  itsGoodAspCenter.resize(0);
  //itsPrevAspActiveSet.resize(0);
  //itsPrevAspAmplitude.resize(0);
  itsUsedMemoryMB = double(HostInfo::memoryUsed()/2014);
  itsNormMethod = casa::refim::SynthesisUtils::getenv("ASP_NORM", itsDefaultNorm);
  itsWaveletScales.resize(0);
  itsWaveletAmps.resize(0);
}

WaveletAspCleaner::~WaveletAspCleaner()
{
  destroyAspScales();
  destroyInitMasks();
  destroyInitScales();
  if(!itsMask.null())
    itsMask=0;
}

// Make a single initial scale size image by Gaussian
void WaveletAspCleaner::makeInitScaleImage(Matrix<Float>& iscale, const Float& scaleSize)
{
  LogIO os( LogOrigin("AspMatrixCleaner","makeInitScaleImage",WHERE) );

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

    /* const Int mini = max(0, (Int)(refi - scaleSize));
    const Int maxi = min(nx-1, (Int)(refi + scaleSize));
    const Int minj = max(0, (Int)(refj - scaleSize));
    const Int maxj = min(ny-1, (Int)(refj + scaleSize));*/

    os << "Initial scale size " << scaleSize << " pixels." << LogIO::POST;

    //Gaussian2D<Float> gbeam(1.0/(sqrt(2*M_PI)*scaleSize), 0, 0, scaleSize, 1, 0);

    // 04/06/2022 Has to make the whole scale image. If only using min/max i/j, 
    // .image looks spotty and not as smooth as before.
    for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {
        //const int px = i - refi;
        //const int py = j - refj;
        //iscale(i,j) = gbeam(px, py); // gbeam with the above def is equivalent to the following
		iscale(i,j) = (1.0/(2*M_PI*pow(scaleSize,2)))*exp(-(pow(i-refi,2) + pow(j-refj,2))*0.5/pow(scaleSize,2)); // this is for 2D, gives unit area but bad init scale (always picks 0)
		  
      }
    }

  }
}


// Make a single scale size image by Gaussian
void WaveletAspCleaner::makeScaleImage(Matrix<Float>& iscale, const Float& scaleSize, const Float& amp, const IPosition& center)
{

  const Int nx = iscale.shape()(0);
  const Int ny = iscale.shape()(1);
  iscale = 0.0;

  if(scaleSize == 0.0)
    iscale(Int(center[0]), Int(center[1])) = 1.0;
  else
  {
    AlwaysAssert(scaleSize>0.0, AipsError);

    /* const Double refi = nx/2;
    const Double refj = ny/2;
    const Int mini = max(0, (Int)(refi - scaleSize));
    const Int maxi = min(nx-1, (Int)(refi + scaleSize));
    const Int minj = max(0, (Int)(refj - scaleSize));
    const Int maxj = min(ny-1, (Int)(refj + scaleSize));*/
    //cout << "makeScaleImage: scalesize " << scaleSize << " center " << center << " amp " << amp << endl;

    ////Gaussian2D<Float> gbeam(1.0 / (sqrt(2*M_PI)*scaleSize), center[0], center[1], scaleSize, 1, 0);

    // all of the following was trying to replicate Sanjay's code but doesn't work
    //const Float d = sqrt(pow(1.0/itsPsfWidth, 2) + pow(1.0/scaleSize, 2));
    //Gaussian2D<Float> gbeam(amp / (sqrt(2*M_PI)*scaleSize), center[0], center[1], scaleSize, 1, 0);
    //Gaussian2D<Float> gbeam(amp * sqrt(2*M_PI)/d, center[0], center[1], scaleSize * d, 1, 0);
    //Gaussian2D<Float> gbeam(amp / (2*M_PI), center[0], center[1], scaleSize, 1, 0);
    //Gaussian2D<Float> gbeam(amp / pow(2,scaleSize-1), center[0], center[1], itsInitScaleSizes[scaleSize], 1, 0);

    for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {
        //const int px = i;
        //const int py = j;
        // iscale(i,j) = gbeam(px, py); // this is equivalent to the following with the above gbeam definition
        // This is for 1D, but represents Sanjay's and gives good init scale
        // Note that "amp" is not used in the expression

        // This is for 2D, gives unit area but bad init scale (always picks 0)
		iscale(i,j) = (1.0/(2*M_PI*pow(scaleSize,2)))*exp(-(pow(i-center[0],2) + pow(j-center[1],2))*0.5/pow(scaleSize,2));
		
      }
    }

  }
}


void WaveletAspCleaner::maxDirtyConvInitScales(float& strengthOptimum, int& optimumScale, IPosition& positionOptimum)
{
  LogIO os(LogOrigin("AspMatrixCleaner", "maxDirtyConvInitScales()", WHERE));

  /* We still need the following to define a region. Using minMaxMasked itself is NOT sufficient and results in components outside of mask.
  // this can be done only once at setup since maxDirtyConvInitScales is called every iter
  const int nx = itsDirty->shape()[0];
  const int ny = itsDirty->shape()[1];

  IPosition blcDirty(itsDirty->shape().nelements(), 0);
  IPosition trcDirty(itsDirty->shape() - 1);

  if(!itsMask.null())
  {
    os << LogIO::NORMAL3 << "Finding initial scales for Asp using given mask" << LogIO::POST;
    if (itsMaskThreshold < 0)
    {
        os << LogIO::NORMAL3
           << "Mask thresholding is not used, values are interpreted as weights"
           <<LogIO::POST;
    }
    else
    {
      // a mask that does not allow for clean was sent
      if(noClean_p)
        return;

      os << LogIO::NORMAL3
         << "Finding initial scales with mask values above " << itsMaskThreshold
         << LogIO::POST;
    }

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
  else
    os << LogIO::NORMAL3 << "Finding initial scales using the entire image" << LogIO::POST;  */


  Vector<Float> maxima(itsNInitScales);
  Block<IPosition> posMaximum(itsNInitScales);

  /*int nth = itsNInitScales;
  #ifdef _OPENMP
    nth = min(nth, omp_get_max_threads());
  #endif*/

  //#pragma omp parallel default(shared) private(scale) num_threads(nth)
  //{
  //  #pragma omp for // genie pragma seems to sometimes return wrong value to maxima on tesla

    /* debug info
    Float maxVal=0;
    IPosition posmin((*itsDirty).shape().nelements(), 0);
    Float minVal=0;
    IPosition posmax((*itsDirty).shape().nelements(), 0);
    minMaxMasked(minVal, maxVal, posmin, posmax, (*itsDirty), itsInitScaleMasks[0]);
    cout << "orig itsDirty : min " << minVal << " max " << maxVal << endl;
    cout << "posmin " << posmin << " posmax " << posmax << endl; */

    IPosition gip;
    const int nx = itsDirty->shape()[0];
    const int ny = itsDirty->shape()[1];
    gip = IPosition(2, nx, ny);
    Block<casacore::Matrix<Float>> vecWork_p;
    vecWork_p.resize(itsNInitScales);

    for (int scale = 0; scale < itsNInitScales; ++scale)
    {
      // Find absolute maximum of each smoothed residual
      vecWork_p[scale].resize(gip);
      Matrix<Float> work = (vecWork_p[scale])(blcDirty,trcDirty);
      work = 0.0;
      work = work + (itsDirtyConvInitScales[scale])(blcDirty,trcDirty);

      maxima(scale) = 0;
      posMaximum[scale] = IPosition(itsDirty->shape().nelements(), 0);

      /* debug info
      Float maxVal=0;
      Float minVal=0;
      IPosition posmin(itsDirty->shape().nelements(), 0);
      IPosition posmax(itsDirty->shape().nelements(), 0);
      minMaxMasked(minVal, maxVal, posmin, posmax, itsDirtyConvInitScales[scale], itsInitScaleMasks[scale]);
      cout << "DirtyConvInitScale " << scale << ": min " << minVal << " max " << maxVal << endl;
      cout << "posmin " << posmin << " posmax " << posmax << endl; */

      // Note, must find peak from the (blcDirty, trcDirty) subregion to ensure components are within mask
      // this is using patch already
      if (!itsMask.null())
      {
        findMaxAbsMask(vecWork_p[scale], itsInitScaleMasks[scale],
          maxima(scale), posMaximum[scale]);
      }
      else
        findMaxAbs(vecWork_p[scale], maxima(scale), posMaximum[scale]);

      if (itsNormMethod == 2)
      {
        if (scale > 0)
        {
  	      float normalization;
  	      normalization = 2 * M_PI / pow(itsInitScaleSizes[scale], 2); //sanjay's
	      
  	      maxima(scale) /= normalization;
        } //sanjay's code doesn't normalize peak here.
         // Norm Method 2 may work fine with GSL with derivatives, but Norm Method 1 is still the preferred approach.
      }
    }
  //}//End parallel section

  // Find the peak residual among the 4 initial scales, which will be the next Aspen
  for (int scale = 0; scale < itsNInitScales; scale++)
  {
    if(abs(maxima(scale)) > abs(strengthOptimum))
    {
      optimumScale = scale;
      strengthOptimum = maxima(scale);
      positionOptimum = posMaximum[scale];
    }
  }

  if (optimumScale > 0)
  {
	  
    float normalization = 2 * M_PI / (pow(1.0/itsPsfWidth, 2) + pow(1.0/itsInitScaleSizes[optimumScale], 2)); // sanjay

    // norm method 2 recovers the optimal strength and then normalize it to get the init guess
    if (itsNormMethod == 2)
      strengthOptimum *= sqrt(2 * M_PI *itsInitScaleSizes[optimumScale]); // this is needed if we also first normalize and then compare.

    strengthOptimum /= normalization;
    // cout << "normalization " << normalization << " strengthOptimum " << strengthOptimum << endl;
  }

  AlwaysAssert(optimumScale < itsNInitScales, AipsError);
}


// ALGLIB - gold - not "log"

vector<Float> WaveletAspCleaner::getActiveSetAspen()
{
  LogIO os(LogOrigin("AspMatrixCleaner", "getActiveSetAspen()", WHERE));

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
  fft.fft0(dirtyFT, *itsDirty);
  itsDirtyConvInitScales.resize(0);
  itsDirtyConvInitScales.resize(itsNInitScales); // 0, 1width, 2width, 4width and 8width

  //cout << "itsInitScaleSizes.size() " << itsInitScaleSizes.size() << " itsInitScales.size() " << itsInitScales.size() << " NInitScales # " << itsNInitScales << endl;
  for (int scale=0; scale < itsNInitScales; scale++)
  {
    Matrix<Complex> cWork;

    itsDirtyConvInitScales[scale] = Matrix<Float>(itsDirty->shape());
    cWork=((dirtyFT)*(itsInitScaleXfrs[scale]));
    fft.fft0((itsDirtyConvInitScales[scale]), cWork, false);
    fft.flip((itsDirtyConvInitScales[scale]), false, false);

    //cout << "remake itsDirtyConvInitScales " << scale << " max itsInitScales[" << scale << "] = " << max(fabs(itsInitScales[scale])) << endl;
    //cout << " max itsInitScaleXfrs[" << scale << "] = " << max(fabs(itsInitScaleXfrs[scale])) << endl;
  }

  float strengthOptimum = 0.0;
  int optimumScale = 0;
  IPosition positionOptimum(itsDirty->shape().nelements(), 0);
  itsGoodAspActiveSet.resize(0);
  itsGoodAspAmplitude.resize(0);
  itsGoodAspCenter.resize(0);

  maxDirtyConvInitScales(strengthOptimum, optimumScale, positionOptimum);

  os << "Peak among the smoothed residual image is " << strengthOptimum  << " and initial scale: " << optimumScale << LogIO::POST;
  //cout << "Peak among the smoothed residual image is " << strengthOptimum  << " and initial scale: " << optimumScale << endl;
  // cout << " its itsDirty is " << (*itsDirty)(positionOptimum);
  // cout << " at location " << positionOptimum[0] << " " << positionOptimum[1] << " " << positionOptimum[2];


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
	 

  	//real_1d_array s = "[1,1]";
	double epsg = itsLbfgsEpsG;//1e-3;
	double epsf = itsLbfgsEpsF;//1e-3;
	double epsx = itsLbfgsEpsX;//1e-3;
	ae_int_t maxits = itsLbfgsMaxit;//5;
	//double epsg = 1e-3;
	//double epsf = 1e-3;
	//double epsx = 1e-3;
	//ae_int_t maxits = 5;
  	minlbfgsstate state;
  	minlbfgscreate(1, x, state);
  	minlbfgssetcond(state, epsg, epsf, epsx, maxits);
  	minlbfgssetscale(state, s);

	//minlbfgssetprecscale(state);
  	minlbfgsreport rep;

	ParamAlglibObjWavelet optParam(*itsDirty, activeSetCenter, itsWaveletScales, itsWaveletAmps);
	ParamAlglibObjWavelet *ptrParam;
	ptrParam = &optParam;

	alglib::minlbfgsoptimize(state, objfunc_alglib_wavelet, NULL, (void *) ptrParam);

	minlbfgsresults(state, x, rep);
	//double *x1 = x.getcontent();
	//cout << "x1[0] " << x1[0] << " x1[1] " << x1[1] << endl;
	
	// end alglib bfgs optimization






    double amp = x[0]; // i
    double scale = x[1]; // i+1

    if (fabs(scale) < 0.4)
    {
      scale = 0;
      amp = (*itsDirty)(itsPositionOptimum); // This is to avoid divergence due to amp being too large.
                                             // amp=strengthOptimum gives similar results
    }
    else
      scale = fabs(scale);

    itsGoodAspAmplitude.push_back(amp); // active-set amplitude
    itsGoodAspActiveSet.push_back(scale); // active-set

    itsStrengthOptimum = amp;
    itsOptimumScaleSize = scale;
    itsGoodAspCenter = activeSetCenter;

    // debug
    //os << "optimized strengthOptimum " << itsStrengthOptimum << " scale size " << itsOptimumScaleSize << LogIO::POST;
    //cout << "optimized strengthOptimum " << itsStrengthOptimum << " scale size " << itsOptimumScaleSize << " at " << itsPositionOptimum << endl;

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

} //# NAMESPACE CASA - END
