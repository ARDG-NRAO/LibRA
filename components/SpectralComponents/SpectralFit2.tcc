//# SpectralFit2.cc: Least Squares fitting of spectral elements: templated part
//# Copyright (C) 2001,2002,2004
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
//# $Id: SpectralFit2.tcc 19935 2007-02-27 05:07:40Z Malte.Marquarding $

//# Includes
#include <components/SpectralComponents/SpectralFit.h>

#include <casa/Utilities/PtrHolder.h>
#include <components/SpectralComponents/CompiledSpectralElement.h>
#include <components/SpectralComponents/GaussianSpectralElement.h>
#include <components/SpectralComponents/LogTransformedPolynomialSpectralElement.h>
#include <components/SpectralComponents/LorentzianSpectralElement.h>
#include <components/SpectralComponents/PolynomialSpectralElement.h>
#include <components/SpectralComponents/PowerLogPolynomialSpectralElement.h>
#include <scimath/Fitting/NonLinearFitLM.h>
#include <scimath/Functionals/CompiledFunction.h>
#include <scimath/Functionals/CompoundFunction.h>
#include <scimath/Functionals/CompoundParam.h>
#include <scimath/Functionals/Gaussian1D.h>
#include <scimath/Functionals/Lorentzian1D.h>
#include <scimath/Functionals/Polynomial.h>
#include <scimath/Functionals/PowerLogarithmicPolynomial.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# Templated member functions

template <class MT>
bool SpectralFit::fit(const casacore::Vector<MT> &y,
		      const casacore::Vector<MT> &x,
		      const casacore::Vector<casacore::Bool> *mask) {
  casacore::Vector<MT> sigma(x.nelements());
  sigma = 1.0;
  return fit(sigma, y, x, mask);
}

template <class MT>
bool SpectralFit::fit(
	const casacore::Vector<MT> &sigma, const casacore::Vector<MT> &y,
	const casacore::Vector<MT> &x, const casacore::Vector<casacore::Bool> *mask
) {
	casacore::NonLinearFitLM<MT> fitter;
	iter_p = 0;
	// The functions to fit
	casacore::CompoundFunction<casacore::AutoDiff<MT> > func;
	casacore::uInt ncomps = slist_p.nelements();
	casacore::PtrHolder<casacore::Function<casacore::AutoDiff<MT> > > autodiff;
	for (casacore::uInt i=0; i<ncomps; i++) {
		SpectralElement *elem = slist_p[i];
		casacore::uInt nparms = elem->getOrder();
		SpectralElement::Types type = slist_p[i]->getType();
		switch(type) {
		case SpectralElement::GAUSSIAN: {
			autodiff.set(new casacore::Gaussian1D<casacore::AutoDiff<MT> >());
		}
		break;
		case SpectralElement::POLYNOMIAL: {
			PolynomialSpectralElement *x = dynamic_cast<PolynomialSpectralElement *>(elem);
			autodiff.set(new casacore::Polynomial<casacore::AutoDiff<MT> >(x->getDegree()));
		}
		break;
		case SpectralElement::COMPILED:
			// Allow fall through; these use the same code
		case SpectralElement::GMULTIPLET: {
			CompiledSpectralElement *x = dynamic_cast<CompiledSpectralElement *>(elem);
			autodiff.set(new casacore::CompiledFunction<casacore::AutoDiff<MT> >());
			dynamic_cast<casacore::CompiledFunction<casacore::AutoDiff<MT> > *>(
				autodiff.ptr()
			)->setFunction(x->getFunction());
		}
		break;
		case SpectralElement::LORENTZIAN: {
			autodiff.set(new casacore::Lorentzian1D<casacore::AutoDiff<MT> >());
		}
		break;
		case SpectralElement::POWERLOGPOLY: {
			casacore::Vector<casacore::Double> parms = elem->get();
			autodiff.set(new casacore::PowerLogarithmicPolynomial<casacore::AutoDiff<MT> > (nparms));
		}
		break;
		case SpectralElement::LOGTRANSPOLY: {
			LogTransformedPolynomialSpectralElement *x = dynamic_cast<
				LogTransformedPolynomialSpectralElement*
			>(elem);
			// treated as a polynomial for fitting purposes. The caller is responsible for passing the ln's of
			// the ordinate and obscissa values to the fitter.
			autodiff.set(new casacore::Polynomial<casacore::AutoDiff<MT> > (x->getDegree()));
		}
		break;
		default:
			throw casacore::AipsError("SpectralFit::fit(): Logic Error: Unhandled SpectralElement type");
		}
		casacore::Vector<casacore::Double> parms = elem->get();
		casacore::Vector<casacore::Bool> fixed = elem->fixed();
		for (casacore::uInt j=0; j<nparms; j++) {
			(*autodiff)[j] = casacore::AutoDiff<MT>(parms[j], nparms, j);
			if (j == PCFSpectralElement::WIDTH && type == SpectralElement::GAUSSIAN) {
				(*autodiff)[j] *= GaussianSpectralElement::SigmaToFWHM;
			}
			autodiff->mask(j) = ! fixed[j];
		}
		func.addFunction(*autodiff);
	}
	fitter.setFunction(func);
	// Max. number of iterations
	fitter.setMaxIter(50+ ncomps*10);
	// Convergence criterium
	fitter.setCriteria(0.001);
	// Fit
	casacore::Vector<MT> sol;
	casacore::Vector<MT> err;
	sol = fitter.fit(x, y, sigma, mask);
	err = fitter.errors();
	// Number of iterations
	iter_p = fitter.currentIteration();
	chiSq_p = fitter.getChi2();
	casacore::uInt j = 0;
	casacore::Vector<casacore::Double> tmp, terr;
	for (casacore::uInt i=0; i<ncomps; i++) {
		SpectralElement *element = slist_p[i];
		casacore::uInt nparms = element->getOrder();
		tmp.resize(nparms);
		terr.resize(nparms);
		SpectralElement::Types type = element->getType();
		for (casacore::uInt k=0; k<nparms; k++) {
			casacore::Bool convertGaussWidth = k==PCFSpectralElement::WIDTH && type == SpectralElement::GAUSSIAN;
			tmp(k) = convertGaussWidth
				? sol(j) / GaussianSpectralElement::SigmaToFWHM
				: sol(j);
			terr(k) = convertGaussWidth
				? err(j) / GaussianSpectralElement::SigmaToFWHM
				: err(j);
			j++;
		};
		element->set(tmp);
		element->setError(terr);
	}
	return fitter.converged();
}


} //# NAMESPACE CASA - END

