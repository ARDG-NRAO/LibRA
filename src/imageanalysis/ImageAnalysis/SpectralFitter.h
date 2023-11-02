//# SpectralCollapser.h: Header file for class SpectralCollapser
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

#ifndef IMAGEANALYSIS_SPECTRALFITTER_H
#define IMAGEANALYSIS_SPECTRALFITTER_H

#include <components/SpectralComponents/SpectralList.h>
#include <components/SpectralComponents/ProfileFit1D.h>

#include <casacore/casa/namespace.h>

namespace casacore{

//class SpectralCoordinate;
class LogIO;
}

namespace casa {

template <class T> class ProfileFit1D;

class SpectralList;

class SpectralFitter {
	// <summary>
	// Does a simple fit to data in vectors. It is possible to specify weights (or errors)
	// for each element. A single Gaussian and a polynimial of order N are the only
	// component that can be fitted.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	//   <li> ProfileFit1D
	// </prerequisite>

	// <etymology>
	// Created to fit components to spectra from the spectral profiler, hence
	// SpectralFitter.
	// </etymology>

	// <synopsis>
	// </synopsis>

public:
	enum FitStatus {// report the status of the last fit
		UNKNOWN,     // mostly means not fit has yet been executed
		FAILED,
		SUCCESS,
	};

	// default constructor
	SpectralFitter();

	// destructor
	virtual ~SpectralFitter();

	// Parameters:
	// <src>spcVals</src>  - independent values
	// <src>yVals</src>    - dependent values
	// <src>eVals</src>    - error values
	// <src>startVal</src> - lower boundary for independent values to be included in the fit
	// <src>endVal</src>   - upper boundary for independent values to be included in the fit
	// <src>fitGauss</src> - fit Gaussian component
	// <src>fitPoly</src>  - fit polynomial
	// <src>nPoly</src>    - order of polynomial to be fitted
	// <src>msg</src>      - message back to the calling routine
	virtual casacore::Bool fit(const casacore::Vector<casacore::Float> &spcVals, const casacore::Vector<casacore::Float> &yVals, const casacore::Vector<casacore::Float> &eVals,
			const casacore::Float startVal, const casacore::Float endVal, const casacore::Bool fitGauss, const casacore::Bool fitPoly, const casacore::uInt nPoly, casacore::String &msg);

	// get the status of the last fit
	const SpectralFitter::FitStatus &getStatus(){return _fitStatus;};

	// get Chi Squared of the last fit
	casacore::Double getChiSquared () const {return _fit.getChiSquared();}

	// get number of iterations for the last fit
	casacore::Double getNumberIterations() const {return _fit.getNumberIterations();}

	const SpectralList &getList() const {return _fit.getList();};

	// get all values for the last fit
	casacore::Vector<casacore::Double> getFit() const {return _fit.getFit();};

	// get the values in the specified data range for the last fit
	void getFit(const casacore::Vector<casacore::Float> &spcVals, casacore::Vector<casacore::Float> &spcFit, casacore::Vector<casacore::Float> &yFit) const;

	// get all residuals for the last fit
	casacore::Vector<casacore::Double> getResidual() const {return _fit.getResidual();};

	// report on the last fit to a stream
	casacore::String report(casacore::LogIO &os, const casacore::String &xUnit="", const casacore::String &yUnit="", const casacore::String &yPrefixUnit="") const;

private:
   casacore::LogIO *_log;

   ProfileFit1D<casacore::Double> _fit;

	SpectralFitter::FitStatus _fitStatus;

	casacore::Double _startVal;
	casacore::Double _endVal;
	casacore::uInt   _startIndex;
	casacore::uInt   _endIndex;

	casacore::String _resultMsg;

	// do all necessary setup
   void _setUp();

   // prepare the data which means give all data (independent, dependent, weights)
   // to the fitting class
   casacore::Bool _prepareData(const casacore::Vector<casacore::Float> &xVals, const casacore::Vector<casacore::Float> &eVals,
   		const casacore::Int &startIndex, const casacore::Int &endIndex, casacore::Vector<casacore::Bool> &maskVals, casacore::Vector<casacore::Double> &weightVals) const;

   // prepare the components that shall be fitted; this includes the setting
   // of reasonable initial parameters
   casacore::Bool _prepareElems(const casacore::Bool fitGauss, const casacore::Bool fitPoly, const casacore::uInt nPoly, casacore::Vector<casacore::Double> &xVals,
   		casacore::Vector<casacore::Double> &yVals, SpectralList& list);

   // report on a list of spectral elements to a stream
   casacore::String _report(casacore::LogIO &os, const SpectralList &list, const casacore::String &xUnit="", const casacore::String &yUnit="", const casacore::String &yPrefixUnit="") const;
};
}

#endif
