//# ImageHistogramCalculatorr.h: 2D convolution of an image
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# $Id: Image2DConvolver.h 20229 2008-01-29 15:19:06Z gervandiepen $

#ifndef IMAGEANALYSIS_IMAGEHISTOGRAMSCALCULATOR_H
#define IMAGEANALYSIS_IMAGEHISTOGRAMSCALCULATOR_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <casa/aips.h>

namespace casa {

// <summary>
// This class computes histograms of image pixel values.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
// </prerequisite>

// <etymology>
//  This class computes histograms of image pixel values.
// </etymology>

// <synopsis>
// This class computes histograms of image pixel values.
// </synopsis>

// <example>
// <srcBlock>
// </srcBlock>
// </example>

// <motivation>
// users like histograms.
// </motivation>
//   <li> 
// </todo>

template <class T> class ImageHistogramsCalculator : public ImageTask<T> {
public:

	const static casacore::String CLASS_NAME;

	ImageHistogramsCalculator() = delete;

	ImageHistogramsCalculator(
		const SPCIIT image, const casacore::Record *const &regionPtr,
	    const casacore::String& mask
	);
	
	ImageHistogramsCalculator(const ImageHistogramsCalculator &other) = delete;

	~ImageHistogramsCalculator();

	ImageHistogramsCalculator &operator=(
	    const ImageHistogramsCalculator &other
	) = delete;

	casacore::Record compute() const;

	casacore::String getClass() const { return CLASS_NAME; }

	// set cursor axes
	void setAxes(const std::vector<casacore::uInt>& axes) { _axes = axes; };

	// should the histogram be cumulative
	void setCumulative(casacore::Bool b) { _cumulative = b; }

	// If true, force the storage lattice to be written to disk.
	void setDisk(casacore::Bool b) { _disk = b; }

	// If true, the counts should be returned as log10 of the actual counts
	void setDoLog10(casacore::Bool b) { _doLog10 = b; }

	// set include range
	void setIncludeRange(const std::vector<double>& r) { _includeRange = r; }

	// If true list stats to logger
	void setListStats(casacore::Bool b) { _listStats = b; }

	// set number of bins
	void setNBins(casacore::uInt n) { _nbins = n; }

protected:

   	CasacRegionManager::StokesControl _getStokesControl() const {
   		return CasacRegionManager::USE_ALL_STOKES;
   	}

    std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
    	return std::vector<casacore::Coordinate::Type>();
    }

    inline casacore::Bool _supportsMultipleRegions() const {return true;}

private:
    casacore::Bool _disk = false;
    casacore::Bool _cumulative = false;
    casacore::Bool _listStats = false;
    casacore::Bool _doLog10 = false;
    std::vector<casacore::uInt> _axes;
    casacore::uInt _nbins = 25;
    std::vector<double> _includeRange;

};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageHistogramsCalculator.tcc>
#endif

#endif
