//# tSubImage.cc: Test program for class SubImage
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
//# $Id: tSubImage.cc 20567 2009-04-09 23:12:39Z gervandiepen $

#ifndef IMAGEANALYSIS_PROFILEFITRESULTS_H
#define IMAGEANALYSIS_PROFILEFITRESULTS_H

#include <imageanalysis/ImageAnalysis/ImageFit1D.h>

#include <casa/namespace.h>

namespace casa {

class ProfileFitResults {
	// <summary>
	// Class to store results from a ImageFit1D object that ImageProfileFitter needs
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// Results of a profile fit.
	// </etymology>

	// <synopsis>
	// Class to store results from a ImageFit1D object that ImageProfileFitter needs
	// </synopsis>

	// <example>
	// </example>

public:

	ProfileFitResults(const ImageFit1D<casacore::Float>& fitter);

	~ProfileFitResults() {};

	const SpectralList getList() const {return _spectralList; }

	casacore::Bool converged() const {return _converged; }

	casacore::uInt getNumberIterations() const { return _iterations; }

	casacore::Bool isValid() const { return _isValid; }

    casacore::Bool succeeded() const { return _succeeded; }
private:
	SpectralList _spectralList;
	casacore::Bool _converged, _isValid, _succeeded;
	casacore::uInt _iterations;

};

}

#endif
