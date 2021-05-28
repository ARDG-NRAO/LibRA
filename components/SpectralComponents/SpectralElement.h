//# SpectralElement.h: Describes (a set of related) spectral lines
//# Copyright (C) 2001,2003,2004
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
//#
//# $Id: SpectralElement.h 20652 2009-07-06 05:04:32Z Malte.Marquarding $

#ifndef COMPONENTS_SPECTRALELEMENT_H
#define COMPONENTS_SPECTRALELEMENT_H

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/RecordInterface.h>

namespace casacore{

template <class T, class U> class Function;
}

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>
// Describes (a set of related) spectral lines
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tSpectralFit" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto module=Functionals>Functionals</linkto> module
// </prerequisite>
//
// <etymology>
// From spectral line and element
// </etymology>
//
// <synopsis>
// The SpectralElement class is the abstract base class for classes
// describing spectral components (Gaussian, Polynonomial, etc).
//
// The element can be used in the
// <linkto class=SpectralFit>SpectralFit</linkto> class and in the
// <linkto class=SpectralEstimate>SpectralEstimate</linkto> class.
//
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// To have a container for fitting of spectral profiles to an observed spectrum
// </motivation>
//
// <todo asof="2001/02/04">
//   <li> add more profile types
// </todo>

class SpectralElement {
public:

	//# Enumerations
	// Supported spectral components
	enum Types {
		// A gaussian profile
		GAUSSIAN,
		// A polynomial baseline
		POLYNOMIAL,
		// Any compiled string functional
		COMPILED,
		// Gaussian multiplet
		GMULTIPLET,
		// Lorentzian
		LORENTZIAN,
		// power log polynomial
		POWERLOGPOLY,
		// log transformed polynomial
		LOGTRANSPOLY,
		N_Types
	};

	virtual ~SpectralElement();

	virtual SpectralElement* clone() const = 0;

	// Evaluate the value of the element at x
	virtual casacore::Double operator()(const casacore::Double x) const;

	casacore::Bool operator==(const SpectralElement& other) const;

	// Get parameter n
	// <thrown>
	//  <li> casacore::AipsError if illegal n
	// </thrown>
	virtual casacore::Double operator[](const casacore::uInt n) const;

	// Get all the types available as casacore::String and codes, and number available
	static const casacore::String* allTypes(casacore::Int &nall,
			const SpectralElement::Types *&typ);
	// Get a string from the type
	static const casacore::String &fromType(SpectralElement::Types tp);
	// Get a type from a (non-case sensitive; minimum match) String
	static casacore::Bool toType(SpectralElement::Types &tp,
			const casacore::String &typName);

	// Get type of this element
	SpectralElement::Types getType() const { return _type; }

	// Get all parameters
	void get(casacore::Vector<casacore::Double>& params) const;

	casacore::Vector<casacore::Double> get() const;

	// Get error estimates of parameters
	void getError(casacore::Vector<casacore::Double> &err) const;
	casacore::Vector<casacore::Double> getError() const;

	// Get the order (i.e. the number of parameters)
	casacore::uInt getOrder() const { return _params.size(); };

	// Set the error fields
	virtual void setError(const casacore::Vector<casacore::Double> &err);

	// Set fixed parameters (true) or unset them (false)
	// <thrown>
	//   <li> casacore::AipsError if incorrect number of parameters (e.g. not 3 for GAUSSIAN)
	// </thrown>

	// Fix/unfix all in one go
	virtual void fix(const casacore::Vector<casacore::Bool>& fix);

	// Get the fix state[s]
	const casacore::Vector<casacore::Bool> &fixed() const;

	// Save to a record.
	virtual casacore::Bool toRecord(casacore::RecordInterface& out) const;

	// set parameters
	virtual void set(const casacore::Vector<casacore::Double>& params);

protected:

	SpectralElement() {}

	SpectralElement(Types type, const casacore::Vector<casacore::Double>& parms=casacore::Vector<casacore::Double>(0));

	SpectralElement(const SpectralElement& other);

	SpectralElement &operator=(const SpectralElement& other);

	void _set(const casacore::Vector<casacore::Double>& params);

	void _setType(const Types type);

	void _setFunction(const std::shared_ptr<casacore::Function<casacore::Double, casacore::Double> >& f);

	virtual std::shared_ptr<casacore::Function<casacore::Double, casacore::Double> > _getFunction() const {
		return _function;
	}

private:
	//#Data
	// type of element
	Types _type;

	// The parameters of the function. I.e. the polynomial coefficients;
	// amplitude, center and sigma of a Gaussian.
	casacore::Vector<casacore::Double> _params;
	// The errors of the parameters
	casacore::Vector<casacore::Double> _errors;
	// The indication if the parameter has to be fixed (true) or solved (false).
	// Solved is the default.
	casacore::Vector<casacore::Bool> _fixed;

	std::shared_ptr<casacore::Function<casacore::Double, casacore::Double> > _function;

};

std::ostream &operator<<(std::ostream& os, const SpectralElement& elem);

bool near(const SpectralElement& s1, const SpectralElement& s2, const casacore::Double tol);

bool nearAbs(const SpectralElement& s1, const SpectralElement& s2, const casacore::Double tol);


} //# NAMESPACE CASA - END

#endif

