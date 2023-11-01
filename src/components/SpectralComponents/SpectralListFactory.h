//# Copyright (C) 1995,1996,1999-2001
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


#ifndef SPECTRALLISTFACTORY_H_
#define SPECTRALLISTFACTORY_H_

#include <casacore/casa/aips.h>
#include <stdcasa/variant.h>

#include <components/SpectralComponents/SpectralList.h>

namespace casa {

// create a SpectralList

class SpectralListFactory {
public:
	// Create a SpectralList object
	// <src>log</src> logger
	// <src>pampest</src> initial amplitude estimates for pfc functions
	// <src>pcenterest</src> initial center estimates for pfc functions
	// <src>pfwhmest</src> initial FWHM estimates for pfc functions
	// <src>pfix</src> fixed parameters for pfc functions. Any combination of "p", "f", and "c"
	// <src>plpest</src> initial estimates for power log polynomial coefficients
	// <src>plpfix</src> fixed parameters for power log polynomial coefficients (true means fix the
	// coefficient during fitting).

	static SpectralList create(
		casacore::LogIO& log, const casac::variant& pampest,
		const casac::variant& pcenterest, const casac::variant& pfwhmest,
		const casac::variant& pfix=casac::initialize_variant(""),
		const casac::variant& gmncomps=casac::initialize_variant(""),
		const casac::variant& gmampcon=casac::initialize_variant(""),
		const casac::variant& gmcentercon=casac::initialize_variant(""),
		const casac::variant& gmfwhmcon=casac::initialize_variant(""),
		const std::vector<double>& gmampest=std::vector<double>(0),
		const std::vector<double>& gmcenterest=std::vector<double>(0),
		const std::vector<double>& gmfwhmest=std::vector<double>(0),
		const casac::variant& gmfix=casac::initialize_variant(""),
		const casac::variant& pfunc=casac::initialize_variant(""),
		const casac::variant& plpest=casac::initialize_variant(""),
		const casac::variant& plpfix=casac::initialize_variant(""),
		const casac::variant& ltpest=casac::initialize_variant(""),
		const casac::variant& ltpfix=casac::initialize_variant("")

	);

private:
	static void _addGaussianMultiplets(
		SpectralList& spectralList,
		casacore::LogIO& log,
		const std::vector<int>& mygmncomps,
		std::vector<double>& mygmampcon,
		std::vector<double>& mygmcentercon,
		std::vector<double>& mygmfwhmcon,
		const std::vector<double>& gmampest,
		const std::vector<double>& gmcenterest, const std::vector<double>& gmfwhmest,
		const std::vector<string>& mygmfix
	);

	static void _addPowerLogPolynomial(
		SpectralList& spectralList,
		casacore::LogIO& log, std::vector<double>& myplpest,
		std::vector<bool>& myplpfix
	);

	static void _addLogTransformedPolynomial(
		SpectralList& spectralList,
		std::vector<double>& myltpest,
		std::vector<bool>& myltpfix
	);

};

} // end namespace casa
#endif /* SPECTRALLISTFACTORY_H_ */
