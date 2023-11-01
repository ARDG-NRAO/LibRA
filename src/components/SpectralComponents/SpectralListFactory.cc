//# Copyright (C) 1993,1994,1995,1996,1999,2001
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

#include <components/SpectralComponents/SpectralListFactory.h>

#include <components/SpectralComponents/GaussianSpectralElement.h>
#include <components/SpectralComponents/GaussianMultipletSpectralElement.h>
#include <components/SpectralComponents/LorentzianSpectralElement.h>
#include <components/SpectralComponents/PowerLogPolynomialSpectralElement.h>
#include <components/SpectralComponents/LogTransformedPolynomialSpectralElement.h>

#include <stdcasa/StdCasa/CasacSupport.h>

using namespace casacore;
using namespace casac;

using namespace casacore;
namespace casa {

SpectralList SpectralListFactory::create(
	LogIO& log, const variant& pampest,
	const variant& pcenterest, const variant& pfwhmest,
	const variant& pfix, const variant& gmncomps,
	const variant& gmampcon, const variant& gmcentercon,
	const variant& gmfwhmcon, const vector<double>& gmampest,
	const vector<double>& gmcenterest, const vector<double>& gmfwhmest,
	const variant& gmfix, const variant& pfunc,
	const variant& plpest, const variant& plpfix,
	const variant& ltpest, const variant& ltpfix
) {
	vector<double> myampest = toVectorDouble(pampest, "pampest");
	vector<double> mycenterest = toVectorDouble(pcenterest, "pcenterest");
	vector<double> myfwhmest = toVectorDouble(pfwhmest, "pfwhmest");
	vector<string> myfix = toVectorString(pfix, "pfix");
	vector<string> myfunc = toVectorString(pfunc, "pfunc");
	vector<double> myplpest = toVectorDouble(plpest, "plpest");
	vector<bool> myplpfix = toVectorBool(plpfix, "plpfix");
	vector<double> myltpest = toVectorDouble(ltpest, "ltpest");
	vector<bool> myltpfix = toVectorBool(ltpfix, "ltpfix");

	vector<int> mygmncomps = gmncomps.type() == variant::INT
		&& gmncomps.toInt() == 0
		? vector<int>(0)
		: toVectorInt(gmncomps, "gmncomps");
	vector<double> mygmampcon = toVectorDouble(gmampcon, "gmampcon");
	vector<double> mygmcentercon = toVectorDouble(gmcentercon, "gmcentercon");
	vector<double> mygmfwhmcon = toVectorDouble(gmfwhmcon, "gmfwhmcon");
	vector<string> mygmfix = toVectorString(gmfix, "gmfix");
	Bool makeSpectralList = (
		mygmncomps.size() + myplpest.size() + myltpest.size() > 0
		|| ! (
			myampest.size() == 0
			&& mycenterest.size() == 0
			&& myfwhmest.size() == 0
		)
	);
	SpectralList spectralList;
	Bool gfixSpecified = (myfix.size() > 0);
	if (! makeSpectralList) {
		if (gfixSpecified) {
			log << "The fix array is specified but no corresponding estimates are "
				<< "set via ampest, centerest, and fwhmest. The fix array will be ignored."
				<< LogIO::WARN;
		}
		return spectralList;
	}
	uInt mynpcf = myampest.size();
	if (myfunc.size() == 0) {
		myfunc.resize(myampest.size(), "G");
	}
	ThrowIf(
		mycenterest.size() != mynpcf
		|| myfwhmest.size() != mynpcf
		|| myfunc.size() != mynpcf,
		"pampest, pcenterest, pfwhmest, and pfunc arrays "
		"must all be the same length"
	);
	ThrowIf(
		gfixSpecified && myfix.size() != mynpcf,
		"If the gfix array is specified the number of "
		"elements it has must be the same as the number of elements "
		"in the ampest array even if some elements are empty strings"
	);
	if (mygmncomps.size() > 0) {
		_addGaussianMultiplets(
			spectralList, log, mygmncomps, mygmampcon,
			mygmcentercon, mygmfwhmcon,
			gmampest, gmcenterest, gmfwhmest,
			mygmfix
		);
	}
	for (uInt i=0; i<mynpcf; i++) {
		String func(myfunc[i]);
		func.upcase();
		Bool doGauss = func.startsWith("G");
		Bool doLorentz = func.startsWith("L");
		if (! doGauss && ! doLorentz) {
			log << myfunc[i] << " does not minimally match 'gaussian' or 'lorentzian'"
				<< LogIO::EXCEPTION;
		}
		std::unique_ptr<PCFSpectralElement> pcf(
			doGauss
				? dynamic_cast<PCFSpectralElement*>(
					new GaussianSpectralElement(
						myampest[i], mycenterest[i],
						GaussianSpectralElement::sigmaFromFWHM(myfwhmest[i])
					)
				)
				: doLorentz
				    ? dynamic_cast<PCFSpectralElement*>(
				    	new LorentzianSpectralElement(
				    		myampest[i], mycenterest[i], myfwhmest[i]
						)
					  )
				    : 0
			);
		if (gfixSpecified) {
			pcf->fixByString(myfix[i]);
		}
		if (! spectralList.add(*pcf)) {
			log << "Unable to add element to spectral list"
				<< LogIO::EXCEPTION;
		}
	}
	if (myplpest.size() > 0) {
		_addPowerLogPolynomial(
			spectralList, log, myplpest, myplpfix
		);
	}
	if (myltpest.size() > 0) {
		_addLogTransformedPolynomial(
			spectralList, myltpest, myltpfix
		);
	}
	return spectralList;
}

void SpectralListFactory::_addGaussianMultiplets(
	SpectralList& spectralList,
	LogIO& log,
	const vector<int>& mygmncomps,
	vector<double>& mygmampcon,
	vector<double>& mygmcentercon,
	vector<double>& mygmfwhmcon,
	const vector<double>& gmampest,
	const vector<double>& gmcenterest, const vector<double>& gmfwhmest,
	const vector<string>& mygmfix
) {
	uInt sum = 0;
	for (uInt i=0; i<mygmncomps.size(); i++) {
		if (mygmncomps[i] <= 0) {
			log << "All elements of gmncomps must be greater than 0" << LogIO::EXCEPTION;
		}
		sum += mygmncomps[i];
	}
	if (gmampest.size() != sum) {
		log << "gmampest must have exactly "
			<< sum << " elements" << LogIO::EXCEPTION;
	}
	if (gmcenterest.size() != sum) {
		log << "gmcenterest must have exactly "
				<< sum << " elements" << LogIO::EXCEPTION;
	}
	if (gmfwhmest.size() != sum) {
		log << "gmfwhmest must have exactly "
			<< sum << " elements" << LogIO::EXCEPTION;
	}
	if (mygmfix.size() > 0 && mygmfix.size() != sum) {
		log << "gmfwhmest must have either zero or " << sum
			<< " elements, even if some are empty strings."
			<< LogIO::EXCEPTION;
	}
	uInt nConstraints = sum - mygmncomps.size();
	if (mygmampcon.size() == 0) {
		mygmampcon = vector<double>(nConstraints, 0);
	}
	else if (
		mygmampcon.size() > 0
		&& mygmampcon.size() != nConstraints
	) {
		log << "If specified, gmampcon must have exactly "
			<< nConstraints << " elements, even if some are zero"
			<< LogIO::EXCEPTION;
	}
	if (mygmcentercon.size() == 0) {
		mygmcentercon = vector<double>(nConstraints, 0);
	}
	else if (
		mygmcentercon.size() > 0
		&& mygmcentercon.size() != nConstraints
	) {
		log << "If specified, gmcentercon must have exactly "
			<< nConstraints << " elements, even if some are zero"
			<< LogIO::EXCEPTION;
	}
	if (mygmfwhmcon.size() == 0) {
		mygmfwhmcon = vector<double>(nConstraints, 0);
	}
	else if (
		mygmfwhmcon.size() > 0
		&& mygmfwhmcon.size() != nConstraints
	) {
		log << "If specified, gmfwhmcon must have exactly "
			<< nConstraints << " elements, even if some are zero"
			<< LogIO::EXCEPTION;
	}
	uInt compNumber = 0;
	for (uInt i=0; i<mygmncomps.size(); i++) {
		if (mygmncomps[i] < 0) {
			log << "All elements of gmncomps must be positive"
				<< LogIO::EXCEPTION;
		}
		vector<GaussianSpectralElement> g(mygmncomps[i]);
		Matrix<Double> constraints(mygmncomps[i] - 1, 3);
		for (uInt j=0; j<(uInt)mygmncomps[i]; j++) {
			g[j] = GaussianSpectralElement(
				gmampest[compNumber], gmcenterest[compNumber],
				GaussianSpectralElement::sigmaFromFWHM(gmfwhmest[compNumber])
			);
			if (mygmfix.size() > 0) {
				g[j].fixByString(mygmfix[compNumber]);
			}
			if (j > 0) {
				constraints(j-1, 0) = mygmampcon[compNumber - (i+1)];
				constraints(j-1, 1) = mygmcentercon[compNumber - (i+1)];
				constraints(j-1, 2) = mygmfwhmcon[compNumber - (i+1)];
			}
			compNumber++;
		}
		if (
			! spectralList.add(
				GaussianMultipletSpectralElement(g, constraints)
			)
		) {
			log << "Unable to add gaussian multiplet to spectral list"
				<< LogIO::EXCEPTION;
		}
	}
}

void SpectralListFactory::_addPowerLogPolynomial(
	SpectralList& spectralList,
	LogIO& log,	vector<double>& myplpest,
	vector<bool>& myplpfix
) {
	uInt nest = myplpest.size();
	if (nest < 2) {
		log << "Number of elements in the power log polynomial estimates list must be at least 2"
			<< LogIO::EXCEPTION;
	}
	uInt nfix = myplpfix.size();
	if (nfix == 0) {
		myplpfix = vector<bool>(nest, false);
	}
	else if (nfix != myplpest.size()) {
		log << "Number of elements in the power logarithmic polynomial fixed parameter list must "
			<< "either be 0 or equal to the number of elements in the estimates list"
			<< LogIO::EXCEPTION;
	}
    Vector<double> const myplpestV(myplpest);
    Vector<bool> const myplpfixV(myplpfix);
	PowerLogPolynomialSpectralElement plp(myplpestV);
	plp.fix(myplpfixV);
	spectralList.add(plp);
}

void SpectralListFactory::_addLogTransformedPolynomial(
	SpectralList& spectralList,
	vector<double>& myltpest,
	vector<bool>& myltpfix
) {
	uInt nest = myltpest.size();
	ThrowIf(
		nest < 2,
		"Number of elements in the log transformed polynomial "
		"estimates list must be at least 2"
	);
	uInt nfix = myltpfix.size();
	if (nfix == 0) {
		myltpfix = vector<bool>(nest, false);
	}
	else {
		ThrowIf(
			nfix != myltpest.size(),
			"Number of elements in the logarithmic transformed polynomial "
			"fixed parameter list must either be 0 or equal to the number "
			"of elements in the estimates list"
		);
	}
    Vector<double> const myltpestV(myltpest);
    Vector<bool> const myltpfixV(myltpfix);
	LogTransformedPolynomialSpectralElement ltp(myltpestV);
	ltp.fix(myltpfixV);
	spectralList.add(ltp);
}

using namespace casacore;
} // end namespace casa
