//# SpectralElement.cc: Describes (a set of related) spectral lines
//# Copyright (C) 2001,2004
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
//# $Id: SpectralElement.cc 21024 2011-03-01 11:46:18Z gervandiepen $

#include <components/SpectralComponents/CompiledSpectralElement.h>
#include <components/SpectralComponents/GaussianSpectralElement.h>
#include <components/SpectralComponents/GaussianMultipletSpectralElement.h>
#include <components/SpectralComponents/LorentzianSpectralElement.h>
#include <components/SpectralComponents/PolynomialSpectralElement.h>
#include <components/SpectralComponents/PowerLogPolynomialSpectralElement.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicMath/Math.h>

#include <iostream>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

ostream &operator<<(ostream &os, const SpectralElement &elem) {
	switch (elem.getType()) {
	case SpectralElement::GAUSSIAN:
		os << *dynamic_cast<const GaussianSpectralElement*>(&elem);
		break;
	case SpectralElement::POLYNOMIAL:
		os << *dynamic_cast<const PolynomialSpectralElement*>(&elem);
		break;
	case SpectralElement::COMPILED:
	case SpectralElement::POWERLOGPOLY:
	case SpectralElement::LOGTRANSPOLY:
		os << *dynamic_cast<const CompiledSpectralElement*>(&elem);
        break;
	case SpectralElement::GMULTIPLET:
		os << *dynamic_cast<const GaussianMultipletSpectralElement*>(&elem);
		break;
	case SpectralElement::LORENTZIAN:
		os << *dynamic_cast<const LorentzianSpectralElement*>(&elem);
		break;
	default:
		throw AipsError("SpectralElement2::<<((): Logic Error. Unhandled spectral element type");
	}
	return os;
}

bool near(const SpectralElement& s1, const SpectralElement& s2, const double tol) {
	if (s1.getType() != s2.getType()) {
		return false;
	}
	for (uInt j=0; j<s1.get().size(); j++) {
		if (! casacore::near(s1.get()[j], s2.get()[j], tol)) {
			return false;
		}
		if (! casacore::near(s1.getError()[j], s2.getError()[j], tol)) {
			return false;
		}
		if (s1.fixed()[j] != s2.fixed()[j]) {
			return false;
		}
	}
	return true;
}

bool nearAbs(const SpectralElement& s1, const SpectralElement& s2, const Double tol) {
	if (s1.getType() != s2.getType()) {
		return false;
	}
	for (uInt j=0; j<s1.get().size(); j++) {
		if (! casacore::nearAbs(s1.get()[j], s2.get()[j], tol)) {
			return false;
		}
		if (! casacore::nearAbs(s1.getError()[j], s2.getError()[j], tol)) {
			return false;
		}
		if (s1.fixed()[j] != s2.fixed()[j]) {
			return false;
		}
	}
	return true;
}



} //# NAMESPACE CASA - END


