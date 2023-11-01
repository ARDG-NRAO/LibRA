//# Image2DConvolver.h: 2D convolution of an image
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

#ifndef IMAGES_IMAGECONVOLVERTASK_H
#define IMAGES_IMAGECONVOLVERTASK_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <casacore/casa/aips.h>

namespace casa {

// <summary>
// This class is the high level interface to image convolving
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="casacore::ImageInterface">casacore::ImageInterface</linkto>
//   <li> <linkto class="casacore::Convolver">casacore::Convolver</linkto>
// </prerequisite>

// <etymology>
// This class is the high level interface to image convolving
// </etymology>

// <synopsis>
// This class is the high level interface to image convolving
// </synopsis>

// <example>
// <srcBlock>
// </srcBlock>
// </example>

// <motivation>
// Convolution is a standard image processing requirement.
// </motivation>

// <todo asof="2001/08/28">
//   <li> 
// </todo>

template <class T> class ImageConvolverTask : public ImageTask<T> {
public:

	const static casacore::String CLASS_NAME;

	ImageConvolverTask() = delete;

	ImageConvolverTask(
		const SPCIIT image, const casacore::Record *const &regionPtr,
	    const casacore::String& mask, const casacore::String& outname, const casacore::Bool overwrite
	);
	
    ImageConvolverTask(const ImageConvolverTask<T> &other) = delete;

	~ImageConvolverTask() {}

	ImageConvolverTask &operator=(const ImageConvolverTask<T> &other) = delete;

	SPIIT convolve();

	void setKernel(const casacore::Array<T>& kernel);

	void setKernel(SPIIT image) { setKernel(image->get()); }

	void setScale(casacore::Double d) { _scale = d; }

	casacore::String getClass() const { return CLASS_NAME; }

protected:

   	CasacRegionManager::StokesControl _getStokesControl() const {
   		return CasacRegionManager::USE_ALL_STOKES;
   	}

    std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
    	return std::vector<casacore::Coordinate::Type>();
    }

    inline casacore::Bool _supportsMultipleRegions() const {return true;}

private:
    casacore::Array<T> _kernel;
    casacore::Double _scale;

};

}

#ifndef CASACORE_NO_AUTO_TEMPLATES
#include <imageanalysis/ImageAnalysis/ImageConvolverTask.tcc>
#endif //# CASACORE_NO_AUTO_TEMPLATES
#endif



