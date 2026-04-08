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

float WaveletAspCleaner::computePeakNormalization(float width)
{
	return 2 * M_PI / pow(width,2) ;
}

float WaveletAspCleaner::computeScaleNormalization(float width1, float width2)
{
	return 2 * M_PI / (pow(1.0/width1, 2) + pow(1.0/width2, 2));
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
    /*for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {
        //const int px = i - refi;
        //const int py = j - refj;
        //iscale(i,j) = gbeam(px, py); // gbeam with the above def is equivalent to the following
		iscale(i,j) = (1.0/(sqrt(2*M_PI)*scaleSize))*exp(-(pow(i-refi,2) + pow(j-refj,2))*0.5/pow(scaleSize,2)); //this is for 1D, but represents Sanjay's and gives good init scale
        //iscale(i,j) = (1.0/(2*M_PI*pow(scaleSize,2)))*exp(-(pow(i-refi,2) + pow(j-refj,2))*0.5/pow(scaleSize,2)); // this is for 2D, gives unit area but bad init scale (always picks 0)
      }
    }*/
    // use template helper function instead
    fillScaleImage(iscale, gaussianScaleValue2D, refi, refj, scaleSize);
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

    /*for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {
        //const int px = i;
        //const int py = j;
        // iscale(i,j) = gbeam(px, py); // this is equivalent to the following with the above gbeam definition
        // This is for 1D, but represents Sanjay's and gives good init scale
        // Note that "amp" is not used in the expression
        iscale(i,j) = (1.0/(sqrt(2*M_PI)*scaleSize))*exp(-(pow(i-center[0],2) + pow(j-center[1],2))*0.5/pow(scaleSize,2));

        // This is for 2D, gives unit area but bad init scale (always picks 0)
        // iscale(i,j) = (1.0/(2*M_PI*pow(scalefSize,2)))*exp(-(pow(i-center[0],2) + pow(j-center[1],2))*0.5/pow(scaleSize,2));
      }
    }*/
    // use template helper function
    fillScaleImage(iscale, gaussianScaleValue2D, center[0], center[1], scaleSize);

  }
}

void WaveletAspCleaner::runLBFGS(
    minlbfgsstate &state,
    real_1d_array &x,
    minlbfgsreport &rep,
    const vector<IPosition> &activeSetCenter,
    FFTServer<Float,Complex> &fft) const
{
    ParamAlglibObjWavelet optParam(*itsDirty, activeSetCenter, itsWaveletScales, itsWaveletAmps);
	ParamAlglibObjWavelet *ptrParam;
    ptrParam = &optParam;

    alglib::minlbfgsoptimize(state, objfunc_alglib_wavelet, NULL, (void *) ptrParam);

    minlbfgsresults(state, x, rep);
}

} //# NAMESPACE CASA - END
