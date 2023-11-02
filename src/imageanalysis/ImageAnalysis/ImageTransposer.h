//#ImageReorderer.h
//#
//# Copyright (C) 1998,1999,2000,2001,2003 Associated Universities, Inc. Washington DC, USA.
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
//#  Created on: May 7, 2010
//#     Author: dmehring

#ifndef IMAGETRANSPOSER_H_
#define IMAGETRANSPOSER_H_

#include <imageanalysis/ImageAnalysis/ImageTask.h>
#include <memory>

namespace casa {
class ImageTransposer : public ImageTask<casacore::Float> {
    // <summary>
      // Top level interface for transposing image axes
      // </summary>

      // <reviewed reviewer="" date="" tests="" demos="">
      // </reviewed>

      // <prerequisite>
      // </prerequisite>

      // <etymology>
      // Reorders images axes.
      // </etymology>

      // <synopsis>
      // ImageReorderer is the top level interface for transposing image axes.
      // </synopsis>

      // <example>
      // <srcblock>
      // ImageTransposer transposer(...)
      // reorderer.reorder();
      // </srcblock>
      // </example>
public:
	// This constructor only allows transposing of axes, it does
	// not allow inverting.
	ImageTransposer(
			const SPCIIF image,
		casacore::uInt order, const casacore::String& outputImage
	);

	// This constructor allows both transposing and inverting of axes
	ImageTransposer(
			const SPCIIF image,
		const casacore::String& order, const casacore::String& outputImage
	);

	// This constructor allows both transposing and inverting of axes
	ImageTransposer(
			const SPCIIF image,
		const casacore::Vector<casacore::String> order, const casacore::String& outputImage
	);
	// destructor
	~ImageTransposer();

	// transpose the axes and write the output image. Returns the associated casacore::PagedImage object.
	SPIIF transpose() const;

	inline casacore::String getClass() const {
		return _class;
	}

protected:
   	inline  CasacRegionManager::StokesControl _getStokesControl() const {
   		return CasacRegionManager::USE_ALL_STOKES;
   	}

	inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>(0);
	}

private:
	casacore::Vector<casacore::Int> _order;
	casacore::IPosition _reverse;
	static const casacore::String _class;

	casacore::Vector<casacore::Int> _getOrder(casacore::uInt order) const;

	casacore::Vector<casacore::Int> _getOrder(const casacore::String& order);
};
}


#endif /* IMAGETRANSPOSER_H_ */
