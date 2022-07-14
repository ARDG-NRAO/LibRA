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

#ifndef IMAGEANALYSIS_IMAGECROPPER_H
#define IMAGEANALYSIS_IMAGECROPPER_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>
#include <casacore/casa/namespace.h>

namespace casa {

template <class T>  class ImageCropper : public ImageTask<T> {
	// <summary>
	// Top level interface for cropping masked edges of an image
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// Crop an image
	// </etymology>

	// <synopsis>
	// High level interface for cropping images.
	// </synopsis>

	// <example>
	// <srcblock>
	// ImageCropper cropper();
	// cropper.crop();
	// </srcblock>
	// </example>

public:

	// At most one of <src>regionRec</src>
	// or <src>box/chanInp/stokes</src> should be supplied. If specifying <src>regionRec</src> that should
	// be a non-null pointer and box, chanInp and stokes should all be empty strings. If specifying either or
	// both of chanInp and/or stokes, the one(s) being specified should be non-empty strings corresponding
	// to correct syntax for that particular parameter, and regionRec should be null.
	// If you specify <src>regionRec</src>=0
	// and <src>box</src>="", <src>stokes</src>="", and <src>chanInp</src>="", that implies you want to use all
	// of the input image.
	ImageCropper(
		const std::shared_ptr<const casacore::ImageInterface<T> > image,
		const casacore::Record *const &regionRec, const casacore::String& box, const casacore::String& chanInp,
		const casacore::String& stokes, const casacore::String& maskInp,
		const casacore::String& outname, const casacore::Bool overwrite
	);

	// destructor
	~ImageCropper();

	// Perform the cropping. If <src>wantReturn</src> is true, a shared_ptr which
	// wraps the cropped image is returned. If false, an empty shared_ptr is returned.
	std::shared_ptr<casacore::ImageInterface<T> > crop(const casacore::Bool wantReturn) const;

	// Set the axes along which to do the cropping. An empty set means
	// crop all axes.
	void setAxes(const std::set<casacore::uInt>& axes);

	casacore::String getClass() const;

protected:
	inline  CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_ALL_STOKES;
	}

	inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>(0);
 	}

private:
	std::set<casacore::uInt> _axes;
	static const casacore::String _class;


	// disallow default constructor
	ImageCropper();

};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageCropper.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif
