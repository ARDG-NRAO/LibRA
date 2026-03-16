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

#include<synthesis/MeasurementEquations/SpectralAspCleaner.h>

// for alglib
#include <synthesis/MeasurementEquations/objfunc_alglib.h>
using namespace alglib;

using namespace casacore;
using namespace std::chrono;
namespace casa { //# NAMESPACE CASA - BEGIN
SpectralAspCleaner::SpectralAspCleaner():
  AspMatrixCleaner()
{
}

SpectralAspCleaner::~SpectralAspCleaner()
{
  destroyAspScales();
  destroyInitMasks();
  destroyInitScales();
  if(!itsMask.null())
    itsMask=0;
}

// ALGLIB
void SpectralAspCleaner::MFaspclean(Matrix<Float>& model)
{
    LogIO os(LogOrigin("SpectralAspCleaner", "MFaspclean", WHERE));

    // the new aspen is always added to the active-set
    casacore::Vector<casacore::Float> Optimums(itsMaxNiter);
    casacore::Vector<casacore::Float> ScaleSizes(itsMaxNiter);
    casacore::Vector<casacore::IPosition> Positions(itsMaxNiter);
    casacore::Vector<casacore::Int> xPositions(itsMaxNiter);
    casacore::Vector<casacore::Int> yPositions(itsMaxNiter);

    vector<Float> tempx;
    vector<IPosition> activeSetCenter;

    //read in amplitudes and scales
    read_array(Optimums, std::string("./strengthoptimum"));
    read_array(ScaleSizes, std::string("./scalesizes"));
    //read_array(Positions, std::string("./positions"));
    read_array(xPositions, std::string("./xpositions"));
    read_array(yPositions, std::string("./ypositions"));

    for (int ii=0; ii < itsMaxNiter; ii++)
    {
	IPosition pos(2,0);
	pos(0) = xPositions(ii);
	pos(1) = yPositions(ii);
	Positions(ii) = pos;
    }

    itsMaxNiter = Optimums.size();

    //package information in the correct way
    for (int ii=0; ii < itsMaxNiter; ii++){
	tempx.push_back(Optimums(ii));
	if (ScaleSizes(ii) > 0){
		tempx.push_back(ScaleSizes(ii));}
	else{
		tempx.push_back(1.0);}
	activeSetCenter.push_back(Positions(ii));
    }

    // initialize alglib option
    unsigned int length = tempx.size();
    real_1d_array x;
    x.setlength(length);

    real_1d_array s;
    s.setlength(length);

    // initialize starting point
    for (unsigned int i = 0; i < length; i+=2)
	  {
	      x[i] = tempx[i];
	      x[i+1] = tempx[i+1];

	      s[i] = tempx[i];
	      s[i+1] = tempx[i+1];
	  }

	//real_1d_array s = "[1,1]";
	double epsg = itsLbfgsEpsG;//1e-3;
	double epsf = itsLbfgsEpsF;//1e-3;
	double epsx = itsLbfgsEpsX;//1e-3;
	ae_int_t maxits = itsLbfgsMaxit;//5;
	minlbfgsstate state;
	minlbfgscreate(1, x, state);
	minlbfgssetcond(state, epsg, epsf, epsx, maxits);
	minlbfgssetscale(state, s);
	minlbfgssetprecscale(state);
	minlbfgsreport rep;

	ParamAlglibObj optParam(*itsDirty, *itsXfr, activeSetCenter, fft);
		ParamAlglibObj *ptrParam;
		ptrParam = &optParam;

	alglib::minlbfgsoptimize(state, objfunc_alglib, NULL, (void *) ptrParam);
    
    minlbfgsresults(state, x, rep);

    // end alglib bfgs optimization
    for (int ii=0; ii < itsMaxNiter; ii++){
        double amp = x[2*ii];
        double scale = x[2*ii+1];
	itsGoodAspAmplitude.push_back(amp);
	itsGoodAspActiveSet.push_back(scale);
	itsGoodAspCenter.push_back(activeSetCenter[ii]);
    }

    // now perform the CLEANing
    // Initialization
    Float totalFlux=0.0;
    // Define a subregion so that the peak is centered
    IPosition support(model.shape());
    support(0) = max(Int(itsInitScaleSizes[itsNInitScales-1] + 0.5), support(0));
    support(1) = max(Int(itsInitScaleSizes[itsNInitScales-1] + 0.5), support(1));
    IPosition inc(model.shape().nelements(), 1);
    Matrix<Float> itsScale = Matrix<Float>(psfShape_p);
    Matrix<Complex>itsScaleXfr = Matrix<Complex> ();
    Float maxVal=0;
    IPosition posmin((*itsDirty).shape().nelements(), 0);
    Float minVal=0;
    IPosition posmax((*itsDirty).shape().nelements(), 0);

    //Loop over Asps
    for(int ii=0; ii < itsMaxNiter; ii++){
	//Load the Asp parameters
	itsOptimumScaleSize = itsGoodAspActiveSet[ii];
	itsStrengthOptimum = itsGoodAspAmplitude[ii];
	itsPositionOptimum = itsGoodAspCenter[ii];

	//Gain is one, since we substitute the full Asp
	itsGain = 1.0;

	//Make the scale
    	makeScaleImage(itsScale, itsOptimumScaleSize, itsStrengthOptimum, itsPositionOptimum);
    	itsScaleXfr.resize();
    	fft.fft0(itsScaleXfr, itsScale);

	// Now add to the total flux
	totalFlux += (itsStrengthOptimum*itsGain);
	itsTotalFlux = totalFlux;

	//The slicing box
	IPosition blc(itsPositionOptimum - support/2);
	IPosition trc(itsPositionOptimum + support/2 - 1);

	LCBox::verify(blc, trc, inc, model.shape());
	IPosition blcPsf(blc);
	IPosition trcPsf(trc);

	// Update the model image
	Matrix<Float> modelSub = model(blc, trc);
	Float scaleFactor;
	scaleFactor = itsGain * itsStrengthOptimum;
	Matrix<Float> scaleSub = (itsScale)(blcPsf,trcPsf);
	modelSub += scaleFactor * scaleSub;

	// Now update the residual image
	// PSF * model
	Matrix<Complex> cWork;
	cWork = ((*itsXfr)*(itsScaleXfr)); //Asp's
	Matrix<Float> itsPsfConvScale = Matrix<Float>(psfShape_p);
	fft.fft0(itsPsfConvScale, cWork, false);
	fft.flip(itsPsfConvScale, false, false); //need this if conv with 1 scale; don't need this if conv with 2 scales

	IPosition nullnull(2,0);
	Matrix<Float> shift(psfShape_p);
	shift.assign_conforming(itsPsfConvScale);
	Matrix<Float> sub = itsPsfConvScale(nullnull+2,support-1);
	sub.assign_conforming(shift(nullnull,support-3));

	Matrix<Float> psfSub = (itsPsfConvScale)(blcPsf, trcPsf);
	Matrix<Float> dirtySub=(*itsDirty)(blc,trc);

	os  << "itsStrengthOptimum " << itsStrengthOptimum << LogIO::POST;

	// subtract the peak that we found from the dirty image
	dirtySub -= scaleFactor * psfSub;

	// update peakres
	itsPrevPeakResidual = itsPeakResidual;
	maxVal=0;
	posmin = 0;
	minVal=0;
	posmax = 0;
	minMaxMasked(minVal, maxVal, posmin, posmax, (*itsDirty), itsInitScaleMasks[0]);
	itsPeakResidual = (fabs(maxVal) > fabs(minVal)) ? fabs(maxVal) : fabs(minVal);
	os  << "current peakres " << itsPeakResidual << LogIO::POST; 
	}  
    os << "Aspclean finished" << LogIO::POST;
}

} //# NAMESPACE CASA - END
