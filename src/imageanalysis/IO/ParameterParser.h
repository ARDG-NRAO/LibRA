//# ParameterParser.h
//# Copyright (C) 2007
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

#ifndef IMAGEANALYSIS_PARAMETERPARSER_H
#define IMAGEANALYSIS_PARAMETERPARSER_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/ArrayFwd.h>
#include <casacore/casa/BasicSL/String.h>
#include <vector>

namespace casa {



// <summary>
// Parse various common input parameters to image analysis tasks and tool methods
// </summary>
// <author>Dave Mehringer</author>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>

// </prerequisite>

// <etymology>
// Parse parameters
// </etymology>

// <synopsis>
// This class is for parsing input parameters associated with image analysis tasks and tool methods.

class ParameterParser {
public:

    // <src>ranges</src> are pairs describing the pixel range over which to select.
    // If you want to select just one pixel in the "range", you must specify that pixel
    // in both parts of the pair. So if you want to select pixels 0 through 5 and pixel 13,
    // you'd specify ranges[0] = 0; ranges[1] = 5; ranges[2] = 13; ranges[3] = 13
    static std::vector<casacore::uInt> consolidateAndOrderRanges(
        casacore::uInt& nSelected, const std::vector<casacore::uInt>& ranges
    );

	// get spectral ranges from chans parameter,
	// nSelectedChannels will have the total number of channels selected for the
	// specification, for an image that has nChannels.
	static std::vector<casacore::uInt> spectralRangesFromChans(
		casacore::uInt& nSelectedChannels, const casacore::String& specification, const casacore::uInt nChannels
	);

	// get a vector of correlations, represented by casacore::String, from a stokes string specifcation,
	// eg, "LLRR", "IV", etc that is commonly used in the stokes parameter in
	// casa image analysis tasks
	static std::vector<casacore::String> stokesFromString(casacore::String& specification);
};

};

#endif
