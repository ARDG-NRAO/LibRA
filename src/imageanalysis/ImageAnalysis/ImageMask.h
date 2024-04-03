//# ImageMask.h
//# Copyright (C) 1998,1999,2000,2001
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

#ifndef IMAGEANALYSIS_IMAGEMASK_H
#define IMAGEANALYSIS_IMAGEMASK_H

#include <casacore/images/Images/ImageInterface.h>

#include <imageanalysis/ImageTypedefs.h>

namespace casa {

// <summary>
// Static functions for interrogating image masks
// </summary>


class ImageMask {

public:
	ImageMask() = delete;

	virtual ~ImageMask();

	// check the mask piecemeal because loading it all into memory will crash
	// for very large images
	template <class T> static casacore::Bool isAllMaskFalse(const casacore::ImageInterface<T>& image);

    // is the image's mask all true valued?
	template <class T> static casacore::Bool isAllMaskTrue(const casacore::MaskedLattice<T>& image);

    static casacore::Bool isAllMaskTrue(const casacore::Lattice<casacore::Bool>& mask);

};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageMask2.tcc>
#endif

#endif
