//# Copyright (C) 1993,1994,1995,1996,1999,2001
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

#ifndef COMPONENTS_SKYCOMPONENTFACTORY2_TCC
#define COMPONENTS_SKYCOMPONENTFACTORY2_TCC

#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <components/ComponentModels/SkyComponentFactory.h>
#include <casa/Quanta/MVAngle.h>
#include <components/ComponentModels/GaussianDeconvolver.h>
#include <components/ComponentModels/GaussianShape.h>
#include <components/ComponentModels/ComponentType.h>
#include <images/Images/ImageUtilities.h>

namespace casa { 

// moved from ImageAnalysis. See comments in ImageUtilities.h
// TODO the only thing that uses this is ImageFitter. So move it there
template <class T> SkyComponent SkyComponentFactory::encodeSkyComponent(
    casacore::LogIO& os, casacore::Double& facToJy,
    const casacore::ImageInterface<T>& subIm, ComponentType::Shape model,
    const casacore::Vector<casacore::Double>& parameters,
    casacore::Stokes::StokesTypes stokes, casacore::Bool xIsLong,
    casacore::Bool deconvolveIt, const casacore::GaussianBeam& beam
) {
    //
    // This function takes a vector of doubles and converts them to
    // a SkyComponent.   These doubles are in the 'x' and 'y' frames
    // (e.g. result from Fit2D). It is possible that the
    // x and y axes of the pixel array are lat/long rather than
    // long/lat if the CoordinateSystem has been reordered.  So we have
    // to take this into account before making the SkyComponent as it
    // needs to know long/lat values.  The subImage holds only the sky

    // Input
    //   pars(0) = Flux     image units
    //   pars(1) = x cen    abs pix
    //   pars(2) = y cen    abs pix
    //   pars(3) = major    pix
    //   pars(4) = minor    pix
    //   pars(5) = pa radians (pos +x -> +y)
    // Output
    //   facToJy = converts brightness units to Jy
    //

	const auto& cSys = subIm.coordinates();
	const auto& bU = subIm.units();
	auto sky = SkyComponentFactory::encodeSkyComponent(
		os, facToJy, cSys, bU, model,
		parameters, stokes, xIsLong, beam
	);
	if (!deconvolveIt) {
		return sky;
    }
	if (beam.isNull()) {
		os << casacore::LogIO::WARN << "This image does not have a restoring "
		    << "beam so no deconvolution possible" << casacore::LogIO::POST;
		return sky;
	}
	if (! cSys.hasDirectionCoordinate()) {
		os << casacore::LogIO::WARN << "This image does not have a "
		    << "DirectionCoordinate so no deconvolution possible"
			<< casacore::LogIO::POST;
		return sky;
	}
	return SkyComponentFactory::deconvolveSkyComponent(os, sky, beam);
}

}

#endif
