//# ImageMetaData.h: Meta information for Images
//# Copyright (C) 2009
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
//# $Id$

#ifndef IMAGES_IMAGEDECONVOLVER_H
#define IMAGES_IMAGEDECONVOLVER_H

#include <casa/aipstype.h>

namespace casacore{

class GaussianBeam;
}

namespace casa {


// <summary>
// Deconvolve a gaussian source from a gaussian beam
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
// </prerequisite>

// <etymology>
// </etymology>

// <synopsis> 
// </synopsis>

// <example>
// </example>

// <motivation> 

// </motivation>


class GaussianDeconvolver {

public:
	typedef casacore::GaussianBeam Angular2DGaussian;


	// Deconvolve the parameters of a source Gaussian from a a GaussianBeam
	// to give the deconvolved Gaussian source.  The return is true if the model appears
	// to be a point source and the output model will be set to
	// the parameters of the beam.
	static casacore::Bool deconvolve(
		Angular2DGaussian& deconvolvedSize,
		const Angular2DGaussian& convolvedSize,
		const casacore::GaussianBeam& beam
	);
};



} //# NAMESPACE CASA - END

#endif
