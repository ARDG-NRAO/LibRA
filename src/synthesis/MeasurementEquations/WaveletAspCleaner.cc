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
#include <synthesis/MeasurementEquations/objfunc_alglib_wavelets.h>
using namespace alglib;

using namespace casacore;
using namespace std::chrono;
namespace casa { //# NAMESPACE CASA - BEGIN
WaveletAspCleaner::WaveletAspCleaner():
  AspMatrixCleaner()
{
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

float WaveletAspCleaner::aspScaleModel(const Int& i, const Int& j, const Float& scaleSize, const Int& refi, const Int& refj)
{
	return (1.0/(2*M_PI*pow(scaleSize,2)))*exp(-(pow(i-refi,2) + pow(j-refj,2))*0.5/pow(scaleSize,2));
}

float WaveletAspCleaner::aspPeakNormModel(const Float& width)
{
	return 2 * M_PI / pow(width,2) ;
}

float WaveletAspCleaner::aspNormalizationModel(const Float& width1, const Float& width2)
{
	return 2 * M_PI / (pow(1.0/width1, 2) + pow(1.0/width2, 2));
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
