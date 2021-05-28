/* -*- mode: c++ -*- */
//# ParallelImagerParams.h: Parameters for parallel imaging
//# Copyright (C) 2016
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
#ifndef PARALLEL_IMAGER_PARAMS_H_
#define PARALLEL_IMAGER_PARAMS_H_

#include <casa/Containers/Record.h>

namespace casa {

/**
 * Collection of parameters for parallel imaging, categorized roughly by imaging
 * component.
 */
struct ParallelImagerParams {
	ParallelImagerParams(casacore::Record sl, casacore::Record im, casacore::Record gr, casacore::Record wt, casacore::Record nm,
	                     casacore::Record dc, casacore::Record it)
		: selection(sl)
		, image(im)
		, grid(gr)
		, weight(wt)
		, normalization(nm)
		, deconvolution(dc)
		, iteration(it) {};

	ParallelImagerParams()
		: selection(casacore::Record())
		, image(casacore::Record())
		, grid(casacore::Record())
		, weight(casacore::Record())
		, normalization(casacore::Record())
		, deconvolution(casacore::Record())
		, iteration(casacore::Record()) {};

	casacore::Record selection;
	casacore::Record image;
	casacore::Record grid;
	casacore::Record weight;
	casacore::Record normalization;
	casacore::Record deconvolution;
	casacore::Record iteration;
};

} // namespace casa

#endif // PARALLEL_IMAGER_PARAMS_H_
