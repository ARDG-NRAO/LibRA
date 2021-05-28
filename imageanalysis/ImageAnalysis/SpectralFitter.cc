//# SpectralCollapser.cc: Implementation of class SpectralCollapser
//# Copyright (C) 1998,1999,2000,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: $

#include <imageanalysis/ImageAnalysis/SpectralFitter.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/OS/Directory.h>
#include <casa/OS/RegularFile.h>
#include <casa/OS/SymLink.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
//#include <imageanalysis/ImageAnalysis/ImageFit1D.h>
#include <images/Images/ImageUtilities.h>
#include <imageanalysis/ImageAnalysis/ImageMoments.h>
#include <images/Images/FITSImage.h>
#include <images/Images/FITSQualityImage.h>
#include <images/Images/MIRIADImage.h>
#include <images/Images/PagedImage.h>
#include <images/Images/SubImage.h>
#include <images/Images/TempImage.h>
#include <components/SpectralComponents/SpectralList.h>
#include <components/SpectralComponents/SpectralElement.h>
#include <components/SpectralComponents/ProfileFit1D.h>
#include <lattices/Lattices/LatticeUtilities.h>

using namespace casacore;
namespace casa {
SpectralFitter::SpectralFitter():
	_log(new LogIO()), _resultMsg(""){
	_setUp();
}

SpectralFitter::~SpectralFitter() {
	delete _log;
}

Bool SpectralFitter::fit(const Vector<Float> &spcVals,
		const Vector<Float> &yVals, const Vector<Float> &eVals,
		const Float startVal, const Float endVal,
		const Bool fitGauss, const Bool fitPoly,
		const uInt nPoly, String &msg) {

	*_log << LogOrigin("SpectralFitter", "fit", WHERE);
	_fitStatus=SpectralFitter::UNKNOWN;

	if (spcVals.size() < 1) {
		msg = String("No spectral values provided!");
		*_log << LogIO::WARN << msg << LogIO::POST;
		return false;
	}

	Bool ascending = true;
	if (spcVals(spcVals.size() - 1) < spcVals(0))
		ascending = false;

	uInt startIndex, endIndex;
	if (ascending) {
		if (endVal < spcVals(0)) {
			msg = String("Start value: ") + String::toString(endVal) + String(
					" is smaller than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return false;
		}
		if (startVal > spcVals(spcVals.size() - 1)) {
			msg = String("End value: ") + String::toString(startVal) + String(
					" is larger than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return false;
		}
		startIndex = 0;
		while (spcVals(startIndex) < startVal)
			startIndex++;

		endIndex = spcVals.size() - 1;
		while (spcVals(endIndex) > endVal)
			endIndex--;
	} else {
		if (endVal < spcVals(spcVals.size() - 1)) {
			msg = String("Start value: ") + String::toString(endVal) + String(
					" is smaller than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return false;
		}
		if (startVal > spcVals(0)) {
			msg = String("End value: ") + String::toString(startVal) + String(
					" is larger than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return false;
		}
		startIndex = 0;
		while (spcVals(startIndex) > endVal)
			startIndex++;

		endIndex = spcVals.size() - 1;
		while (spcVals(endIndex) < startVal)
			endIndex--;
	}

	// prepare the fit images
	Vector<Bool> maskVals;
	Vector<Double> weightVals;
	if (!_prepareData(spcVals, eVals, startIndex, endIndex, maskVals, weightVals)){
		msg = String("The error array contains values <0.0!");
		*_log << LogIO::WARN << msg << LogIO::POST;
		return false;
	}

	// make sure that something can be done
	if ((endIndex-startIndex) + 1 < 2){
		msg = String("Only one data value selected. Can not fit anything.");
		*_log << LogIO::WARN << msg << LogIO::POST;
		return false;
	}
	else if (fitGauss && ((endIndex-startIndex) + 1 < 3)){
		msg = String("Only two data value selected. Can not fit a Gaussian.");
		*_log << LogIO::WARN << msg << LogIO::POST;
		return false;
	}

	// convert the input values to Double
	Vector<Double> dspcVals(spcVals.size()), dyVals(yVals.size());
	convertArray(dspcVals,	spcVals);
	convertArray(dyVals,	yVals);


	// store start and end values
	_startVal   = startVal;
	_endVal     = endVal;
	_startIndex = startIndex;
	_endIndex   = endIndex;

	// set data, weights and status
	_fit.clearList();
	if (weightVals.size()>0){
		_fit.setData (dspcVals, dyVals, maskVals, weightVals);
	}
	else {
		_fit.setData (dspcVals, dyVals, maskVals);
	}

	// set the estimated elements
	SpectralList elemList;
	_prepareElems(fitGauss, fitPoly, nPoly, dspcVals, dyVals, elemList);
	_fit.setElements(elemList);
	//_report(_fit.getList(false), *_log);

	// do the fit
	Bool ok(false);
	try {
		ok = _fit.fit();
	} catch (AipsError x) {
		msg = x.getMesg();
		*_log << LogIO::WARN << msg << LogIO::POST;
		return false;
	}
	if (ok){
		_fitStatus=SpectralFitter::SUCCESS;
	}
	else{
		_fitStatus=SpectralFitter::FAILED;
	   msg = "Fitter did not converge in " + String::toString(_fit.getNumberIterations()) + " iterations";
	   *_log << LogIO::NORMAL  << msg << LogIO::POST;
	   return false;
	}

	return true;
}

void SpectralFitter::getFit(const Vector<Float> &spcVals, Vector<Float> &spcFit, Vector<Float> &yFit) const{
	Vector<Double> tmp;

	// re-size all vectors
	spcFit.resize(_endIndex-_startIndex+1);
	yFit.resize(_endIndex-_startIndex+1);
	tmp.resize(_endIndex-_startIndex+1);

	// extract the range of the independent coordinate
	spcFit = spcVals(IPosition(1, _startIndex), IPosition(1, _endIndex));

	// extract the range of the dependent coordinate
	tmp    = (getFit())(IPosition(1, _startIndex), IPosition(1, _endIndex));

	// convert to Float
	convertArray(yFit, tmp);
}

String SpectralFitter::report(LogIO &os, const String &xUnit, const String &yUnit, const String &yPrefixUnit) const{
	String resultMsg("");
	SpectralList list = _fit.getList(true);

	switch (_fitStatus){
	case SpectralFitter::SUCCESS:
		os << LogIO::NORMAL << " " << LogIO::POST;
		os << LogIO::NORMAL << "Successful fit!" << LogIO::POST;
		os << LogIO::NORMAL << "No. of iterations: " << String::toString(_fit.getNumberIterations()) << LogIO::POST;
		os << LogIO::NORMAL << "Chi-square:       " << String::toString(_fit.getChiSquared())       << LogIO::POST;
		// report the spectral elements
		resultMsg  = _report(os, list, xUnit, yUnit, yPrefixUnit);

		break;
	case SpectralFitter::FAILED:
		resultMsg = "Fit did not converge in " + String::toString(_fit.getNumberIterations()) + " iterations!";
		os << LogIO::NORMAL << " " << LogIO::POST;
		os << LogIO::NORMAL << resultMsg << LogIO::POST;
		break;
	default:
		resultMsg = "The fit is in an undefined state!";
		os << LogIO::NORMAL << " " << LogIO::POST;
		os << LogIO::NORMAL << resultMsg << LogIO::POST;
	}

	return resultMsg;
}

void SpectralFitter::_setUp() {
	*_log << LogOrigin("SpectralFitter", "setUp");

	// setup the fitter and the status
	_fit = ProfileFit1D<Double>();
	_fitStatus=SpectralFitter::UNKNOWN;
}

Bool SpectralFitter::_prepareData(const Vector<Float> &xVals, const Vector<Float> &eVals,
		const Int &startIndex, const Int &endIndex, Vector<Bool> &maskVals, Vector<Double> &weightVals) const {

	// create the mask
	maskVals.resize(xVals.size());
	maskVals = false;
	maskVals(IPosition(1, startIndex), IPosition(1, endIndex)) = true;

	// if possible, compute the weights
	if (eVals.size()>0){
		weightVals.resize(xVals.size());
		weightVals=0.0;
		Vector<Double> one(eVals.size(), 1.0);
		Vector<Double> deVals(eVals.size(), 0.0);
		convertArray(deVals, eVals);

		// find the minimum of the error values
		Double minVal=min(eVals(IPosition(1, startIndex), IPosition(1, endIndex)));

		// a value smaller zero make no sense
		if (minVal<0.0){
			return false;
		}
		// if the error is zero, discard all errors
		else if (minVal==0.0){
			String msg = String("The error array contains values=0.0 ==> ALL error values are discarded!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			weightVals.resize(0);
		}
		// compute the weights
		else {
			weightVals(IPosition(1, startIndex), IPosition(1, endIndex)) = one(IPosition(1, startIndex), IPosition(1, endIndex)) / deVals(IPosition(1, startIndex), IPosition(1, endIndex));
		}
	}
	return true;
}

Bool SpectralFitter::_prepareElems(const Bool fitGauss, const Bool fitPoly, const uInt nPoly, Vector<Double> &xVals,
		Vector<Double> &yVals, SpectralList& list){
	Int nQuart=max(1,Int((_endIndex-_startIndex)/4));

	Double leftYVal(0.0), rightYVal(0.0);
	Double leftXVal(0.0), rightXVal(0.0);
	for (uInt index=_startIndex; index < (_startIndex+nQuart); index++){
		leftXVal += xVals(index);
		leftYVal += yVals(index);
	}
	leftXVal /= Double(nQuart);
	leftYVal /= Double(nQuart);

	for (uInt index=_endIndex; index > (_endIndex-nQuart); index--){
		rightXVal += xVals(index);
		rightYVal += yVals(index);
	}
	rightXVal /= Double(nQuart);
	rightYVal /= Double(nQuart);

	// make sure that the wavelength
	// is 'ascending'
	if (xVals(_startIndex)>xVals(_endIndex)){
		Double tmp;
		tmp       = leftXVal;
		leftXVal  = rightXVal;
		rightXVal = tmp;

		tmp       = leftYVal;
		leftYVal  = rightYVal;
		rightYVal = tmp;
	}

	// estimate the parameters
	// of the polynomial and add it
	if (fitPoly) {
		if (nPoly==0){
			Vector<Double> pPar(1, 0.5*(rightYVal+leftYVal));
			list.add(PolynomialSpectralElement(pPar));
		}
		else if (nPoly==1){
			Vector<Double> pPar(2, 0.0);
			pPar(1) = (rightYVal-leftYVal) / (rightXVal-leftXVal);
			pPar(0) = rightYVal - pPar(1)*rightXVal;
			list.add(PolynomialSpectralElement(pPar));
		}
	}

	// estimate the parameters
	// of the Gaussian and add it
	if (fitGauss){
		Double gAmp(0.0), gCentre(0.0), gSigma(0.0);

		// integrate over the data
		Double curveIntegral(0.0), polyIntegral(0.0), averDisp(0.0);
		for (uInt index=_startIndex; index < (_endIndex+1); index++)
			curveIntegral += yVals(index);

		// integrate over the estimated polynomial
		polyIntegral   = 0.5*(rightYVal+leftYVal)*Double(_endIndex-_startIndex+1);

		// compute the average dispersion
		averDisp = fabs(xVals(_endIndex) - xVals(_startIndex)) /  Double(_endIndex-_startIndex);

		// make an estimate for the sigma (FWHM ~1/4 of x-range);
		// get the amplitude estimate from the integral and the sigma;
		// the centre estimate is set to the middle of the x-range;
		gSigma = (xVals(_startIndex+nQuart)-xVals(_endIndex-nQuart))/(2.0*GaussianSpectralElement::SigmaToFWHM);
		if (gSigma<0.0)
			gSigma *= -1.0;
		gAmp = averDisp*(curveIntegral-polyIntegral)/(gSigma*sqrt(2.0*C::pi));
		gCentre = xVals(_startIndex) + (xVals(_endIndex) - xVals(_startIndex)) / 2.0;

		// add the Gaussian element
		list.add(GaussianSpectralElement(gAmp, gCentre, gSigma));
	}

	return true;
}

String SpectralFitter::_report(LogIO &os, const SpectralList &list, const String &xUnit, const String &yUnit, const String &yPrefixUnit) const{
	ostringstream sstream;

	String spTypeStr;
	String intUnit(""), slopeUnit(""), xStreamUnit(""), yStreamUnit("");
	//Vector<Double> params, errors;
	Double gaussAmpV(0.0), gaussCentV(0.0), gaussSigmaV(0.0), gaussFWHMV(0.0);
	Double gaussAmpE(0.0), gaussCentE(0.0), gaussSigmaE(0.0), gaussFWHME(0.0);
	Double gaussAreaV(0.0), gaussAreaE(0.0);
	Double polyOffsetV(0.0), polySlopeV(0.0);
	Double polyOffsetE(0.0), polySlopeE(0.0);
	Int gaussIndex(-1), polyIndex(-1);

	// compose the unit for the Gauss integral
	if (xUnit.size()>0 && yUnit.size()>0) {
		intUnit = String(" [")+yPrefixUnit+yUnit+String("*")+xUnit+String("]");
		if (xUnit.contains("/"))
			slopeUnit = String(" [")+yPrefixUnit+yUnit+String("/(")+xUnit+String(")]");
		else
			slopeUnit = String(" [")+yPrefixUnit+yUnit+String("/")+xUnit+String("]");
	}

	// compose the units for the fit
	// values on the x-axis
	if (xUnit.size()>0)
		xStreamUnit = String(" [")+xUnit+String("]");

	// compose the units for the fit
	// values on the y-axis
	if (yUnit.size()>0)
		yStreamUnit = String(" [")+ yPrefixUnit + yUnit+String("]");
	else if (yPrefixUnit.size()>0)
		yStreamUnit = String(" (")+yPrefixUnit+String(")");

	// go over all elements
	for (uInt index=0; index < list.nelements(); index++){

		// report element type and get the parameters/errors
		SpectralElement::Types spType = list[index]->getType();
		spTypeStr = list[index]->fromType(spType);
		//returnMsg += spTypeStr;
		//os << LogIO::NORMAL << "Element No. " << String::toString(index) << ": " << spTypeStr << LogIO::POST;
		Vector<Double> params = list[index]->get();
		//list[index]->getError(errors);
		Vector<Double> errors = list[index]->getError();

		switch (spType){

		// extract and report the Gaussian parameters
		case SpectralElement::GAUSSIAN:
			gaussIndex  = index;
			gaussAmpV   = params(0);
			gaussCentV  = params(1);
			gaussSigmaV = params(2);
			gaussFWHMV  = gaussSigmaV * GaussianSpectralElement::SigmaToFWHM;
			gaussAreaV  = gaussAmpV * gaussSigmaV * sqrt(2.0*C::pi);
			gaussAmpE   = errors(0);
			gaussCentE  = errors(1);
			gaussSigmaE = errors(2);
			gaussFWHME  = gaussSigmaE * GaussianSpectralElement::SigmaToFWHM;
			gaussAreaE  = sqrt(C::pi) * sqrt(gaussAmpV*gaussAmpV*gaussSigmaE*gaussSigmaE + gaussSigmaV*gaussSigmaV*gaussAmpE*gaussAmpE);

			//os << LogIO::NORMAL << "  Amplitude: " << String::toString(gaussAmpV) << "+-" << gaussAmpE << yStreamUnit << " centre: " << String::toString(gaussCentV) << "+-" << gaussCentE << xStreamUnit << " FWHM: " << String::toString(gaussFWHMV) << "+-" << gaussFWHME << xStreamUnit << LogIO::POST;
			os << LogIO::NORMAL << "  Gauss amplitude: " << String::toString(gaussAmpV) << "+-" << gaussAmpE << yStreamUnit << LogIO::POST;
			os << LogIO::NORMAL << "  Gauss centre:    " << String::toString(gaussCentV) << "+-" << gaussCentE << xStreamUnit << LogIO::POST;
			os << LogIO::NORMAL << "  Gauss FWHM:      " << String::toString(gaussFWHMV) << "+-" << gaussFWHME << xStreamUnit << LogIO::POST;
			os << LogIO::NORMAL << "  Gaussian area:   " << String::toString(gaussAreaV) <<"+-"<< gaussAreaE << intUnit << LogIO::POST;
			//returnMsg += " Cent.: " + String::toString(gaussCentV) + " FWHM: " + String::toString(gaussFWHMV) + "  Ampl.: " + String::toString(gaussAmpV);
			sstream << " Cent.: " << setiosflags(ios::scientific) << setprecision(6) << gaussCentV << " FWHM: " << setprecision(4) << gaussFWHMV << "  Ampl.: " << setprecision(3) << gaussAmpV;
			break;

		// extract and report the polynomial parameters
		case SpectralElement::POLYNOMIAL:
			polyIndex  = index;
			polyOffsetV = params(0);
			polyOffsetE = errors(0);
			if (params.size()>1){
				polySlopeV = params(1);
				polySlopeE = errors(2);
			}
			os << LogIO::NORMAL << "  Offset: " << String::toString(polyOffsetV) << "+-"<< String::toString(polyOffsetE) << yStreamUnit <<LogIO::POST;
			//returnMsg += "  Offs.: " + String::toString(polyOffsetV);
			sstream << "  Offs.: " << setiosflags(ios::scientific) << setprecision(3) << polyOffsetV;
			if (params.size()>1){
				os << LogIO::NORMAL << "  Slope:  " << String::toString(polySlopeV) << "+-"<< String::toString(polySlopeE) << slopeUnit << LogIO::POST;
				sstream << "  Slope:  " << setiosflags(ios::scientific) << setprecision(3) << polySlopeV;
				//returnMsg += "  Slope:  " + String::toString(polySlopeV);
			}
			break;

		// report the parameters
		default:
			os << LogIO::NORMAL << "  parameters: " << String::toString(params) << LogIO::POST;
			os << LogIO::NORMAL << "  errors:     " << String::toString(errors) << LogIO::POST;
			//returnMsg += "  Params:  " + String::toString(params);
			sstream << "  Params:  " << params;
			break;
		}
	}

	// if possible, compute and report the equivalent width
	if (gaussIndex > -1 && polyIndex >- 1){
		Double centVal = (*list[polyIndex])(gaussCentV);
		if (centVal==0.0){
			sstream << LogIO::NORMAL << "  Continuum is 0.0 - can not compute equivalent width!" << LogIO::POST;
		}
		else{
			os << LogIO::NORMAL << "Can compute equivalent width" << LogIO::POST;
			os << LogIO::NORMAL << "  Continuum value: " << String::toString(centVal) << yStreamUnit << LogIO::POST;
			os << LogIO::NORMAL << "  --> Equivalent width: " << String::toString(-1.0*gaussAreaV/centVal) << xStreamUnit << LogIO::POST;
			sstream << " Equ.Width: " << setiosflags(ios::scientific) << setprecision(4) << -1.0*gaussAreaV/centVal;
			//returnMsg += " Equ.Width: "+ String::toString(-1.0*gaussAreaV/centVal);
		}
	}
	return String(sstream);
}
}

