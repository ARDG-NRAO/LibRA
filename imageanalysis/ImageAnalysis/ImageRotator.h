//# ImageRotator.h
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

#ifndef IMAGES_IMAGEROTATOR_H
#define IMAGES_IMAGEROTATOR_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <casa/aips.h>

namespace casa {

// <summary>
// This class rotates an image through a specified angle.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
// </prerequisite>

// <etymology>
// This class rotates an image through a specified angle.
// </etymology>

// <synopsis>
// This class rotates an image through a specified angle.

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

template <class T> class ImageRotator : public ImageTask<T> {
public:

	const static casacore::String CLASS_NAME;

	ImageRotator() = delete;

	ImageRotator(
	    const SPCIIT image, const casacore::Record *const &regionPtr,
	    const casacore::String& mask,const casacore::String& outname,
	    casacore::Bool overwrite
	);
	
	ImageRotator(const ImageRotator &other) = delete;

	~ImageRotator();

	ImageRotator &operator=(const ImageRotator &other) = delete;

	casacore::String getClass() const { return CLASS_NAME; }

	SPIIT rotate();

	// set angle through which to rotate the direction or linear coordinate
	void setAngle(const casacore::Quantity& angle) { _angle = angle; }

	void setDecimate(casacore::Int d) { _decimate = d; }

	// set the interpolation method
	void setInterpolationMethod(const casacore::String& method) {
	    _method = method;
	    _method.upcase();
	}

	void setReplicate(casacore::Bool b) { _replicate = b; }

	// set shape of output image
	void setShape(const casacore::IPosition& shape) { _shape = shape; }

protected:

   	CasacRegionManager::StokesControl _getStokesControl() const {
   		return CasacRegionManager::USE_ALL_STOKES;
   	}

    std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
    	return std::vector<casacore::Coordinate::Type>();
    }

    casacore::Bool _supportsMultipleRegions() const {return false;}

private:
    casacore::IPosition _shape;
    casacore::String _method = "cubic";
    casacore::Quantity _angle = casacore::Quantity(0, "deg");
    casacore::Int _decimate = 0;
    casacore::Bool _replicate = false;

};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageRotator.tcc>
#endif

#endif
