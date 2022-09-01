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

#ifndef IMAGES_IMAGEPADDER_H
#define IMAGES_IMAGEPADDER_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <casacore/casa/namespace.h>


namespace casacore{

class MDirection;
}

namespace casa {

class ImagePadder : public ImageTask<casacore::Float> {
	// <summary>
	// Top level interface for padding an image in direction space.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// Collapses image.
	// </etymology>

	// <synopsis>
	// High level interface for padding an image.
	// </synopsis>

	// <example>
	// <srcblock>
	// ImagePadder padder();
	// padder.pad();
	// </srcblock>
	// </example>

public:
	// <group>

	ImagePadder(
			const SPCIIF  image,
		const casacore::Record *const regionRec=0,
		const casacore::String& box="", const casacore::String& chanInp="",
		const casacore::String& stokes="", const casacore::String& maskInp="",
		const casacore::String& outname="", const casacore::Bool overwrite=""
	);

	// </group>

	// destructor
	~ImagePadder();

	// perform the padding. If <src>wantReturn</src> is true, return a pointer to the
	// padded image. The returned pointer is created via new(); it is the caller's
	// responsibility to delete the returned pointer. If <src>wantReturn</src> is false,
	// a NULL pointer is returned and pointer deletion is performed internally.
	SPIIF pad(const casacore::Bool wantReturn) const;

	// set the number of pixels to use for padding and their values and if they should be
	// masked on each edge of the direction plane. <src>good</src>=true means the padding
	// pixels will not be masked (set to good, mask values = true).
	void setPaddingPixels(const casacore::uInt nPixels, const casacore::Float value=0, const casacore::Bool good=false);

	casacore::String getClass() const;

protected:
	inline  CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_ALL_STOKES;
	}

	inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		std::vector<casacore::Coordinate::Type> v;
		v.push_back(casacore::Coordinate::DIRECTION);
		return v;
 	}

private:
	casacore::uInt _nPixels;
	casacore::Float _value;
	casacore::Bool _good;
	static const casacore::String _class;

	// disallow default constructor
	ImagePadder();
};
}

#endif
