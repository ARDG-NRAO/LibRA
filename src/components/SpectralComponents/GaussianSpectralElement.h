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

#ifndef COMPONENTS_GAUSSIANSPECTRALELEMENT_H
#define COMPONENTS_GAUSSIANSPECTRALELEMENT_H

#include <components/SpectralComponents/PCFSpectralElement.h>

namespace casa {

// <summary>
// Describes a Gaussian spectral line
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tSpectralFit" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto module=SpectralElement>SpectralElement</linkto> module
// </prerequisite>
//
// <etymology>
// From Gaussian and spectral line and element
// </etymology>
//
// <synopsis>
// The GaussianSpectralElement class describes a Gaussian spectral line.

// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// To have a container for data descrbing a Gaussian spectral profile for fitting to an observed spectrum
// </motivation>

class GaussianSpectralElement : public PCFSpectralElement {

public:

	// Default constructor creates a default Gaussian element with an amplitude
	// of 1; an integral <src>(sigma=2sqrt(ln2)/pi)</src> of 1;
	// a central frequency of zero. It's necessary for this to be public because
	// Arrays of this class require access to the default constructor. It should never
	// be used in code developers write though.
	GaussianSpectralElement();


	//# Constants
	// Sigma to FWHM conversion factor
	static const casacore::Double SigmaToFWHM;

	//# Constructors

	// Construct with given type and values
	// <thrown>
	//   <li> casacore::AipsError if sigma == 0.0
	//   <li> casacore::AipsError if type not GAUSSIAN
	// </thrown>
	GaussianSpectralElement(
		const casacore::Double ampl, const casacore::Double center,
		const casacore::Double sigma
	);

	// Construct the given tp with the given param
	// <thrown>
	//   <li> casacore::AipsError if incorrect number of parameters (e.g. not 3 for GAUSSIAN)
	//   <li> casacore::AipsError if sigma == 0.0
	// </thrown>
	GaussianSpectralElement(const casacore::Vector<casacore::Double> &param);
	// Copy constructor (deep copy)
	// <thrown>
	//   <li> casacore::AipsError if sigma == 0.0
	// </thrown>
	GaussianSpectralElement(const GaussianSpectralElement &other);

	//#Destructor
	// Destructor
	~GaussianSpectralElement();

	SpectralElement* clone() const;

	// Assignment (copy semantics)
	// <thrown>
	//   <li> casacore::AipsError if sigma == 0.0
	// </thrown>
//	GaussianSpectralElement& operator=(const GaussianSpectralElement &other);
	// Evaluate the value of the element at x
	//casacore::Double operator()(const casacore::Double x) const;

	casacore::Double getSigma() const;
	casacore::Double getFWHM() const;

	casacore::Double getSigmaErr() const;
	casacore::Double getFWHMErr() const;


	void setSigma(casacore::Double sigma);
	void setFWHM(casacore::Double fwhm);

	void fixSigma(const casacore::Bool fix=true);

	casacore::Bool fixedSigma() const;

	casacore::Double getIntegral() const;

	// Save to a record.   For Gaussian elements,
	// the width is defined as a FWHM in the record interface.
	casacore::Bool toRecord(casacore::RecordInterface &out) const;

	// Sigma to FWHM
	// Convert from sigma to FWHM and vice versa
	// <group>
	static casacore::Double sigmaFromFWHM (const casacore::Double fwhm);

	static casacore::Double sigmaToFWHM (const casacore::Double sigma);
	// </group>

	void set(const casacore::Vector<casacore::Double>& v);

private:
	// need to overrride SpectralElement::_set() because _param[2] is sigma
	// but the second param of the corresponding casacore::Gaussian1D function is the
	// FWHM :(
	void _set(const casacore::Vector<casacore::Double>& v);

};

std::ostream &operator<<(std::ostream& os, const GaussianSpectralElement& elem);


} //# NAMESPACE CASA - END

#endif
