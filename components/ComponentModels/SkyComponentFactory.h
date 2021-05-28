//# Copyright (C) 1995,1996,1999-2001
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

#ifndef COMPONENTS_SKYCOMPONENTFACTORY_H
#define COMPONENTS_SKYCOMPONENTFACTORY_H

#include <casa/aips.h>
#include <casacore/casa/Arrays/ArrayFwd.h>
#include <components/ComponentModels/SkyComponent.h>

namespace casacore{

class GaussianBeam;
class ImageInfo;
template <class T> class ImageInterface;

}

namespace casa {

class SkyComponentFactory {
public:

    typedef casacore::GaussianBeam Angular2DGaussian;
	// These functions convert between a vector of doubles holding SkyComponent values
	// (the output from SkyComponent::toPixel) and a SkyComponent.   The coordinate
	// values are in the 'x' and 'y' frames.  It is possible that the x and y axes of
	// the pixel array are lat/long (xIsLong=false)  rather than  long/lat.
	// facToJy converts the brightness units from whatevers per whatever
	// to Jy per whatever (e.g. mJy/beam to Jy/beam).  It is unity if it
	// can't be done and you get a warning.  In the SkyComponent the flux
	// is integral.  In the parameters vector it is peak.
	//
	//   pars(0) = FLux     image units (e.g. Jy/beam).
	//   pars(1) = x cen    abs pix
	//   pars(2) = y cen    abs pix
	//   pars(3) = major    pix
	//   pars(4) = minor    pix
	//   pars(5) = pa radians (pos +x -> +y)
	//
	//  5 values for ComponentType::Gaussian, CT::Disk.  3 values for CT::Point.
	//
	// <group>
	static SkyComponent encodeSkyComponent(
		casacore::LogIO& os, casacore::Double& facToJy,
		const casacore::CoordinateSystem& cSys,
		const casacore::Unit& brightnessUnit,
		ComponentType::Shape type,
		const casacore::Vector<casacore::Double>& parameters,
		casacore::Stokes::StokesTypes stokes,
		casacore::Bool xIsLong, const casacore::GaussianBeam& beam
	);

	// for some reason, this method was in ImageAnalysis but also belongs here.
	// Obviously hugely confusing to have to methods with the same name and
	// which presumably are for the same thing in two different classes. I'm
	// moving ImageAnalysis' method here and also moving that implamentation to
	// here as well and also being consistent regarding callers (ie those that
	// called the ImageAnalysis method will now call this method). I couldn't
	// tell you which of the two implementations is the better one to use
	// for new code, but this version does call the version that already existed
	// in casacore::ImageUtilities, so this version seems to do a bit more.
	// I also hate having a class with anything like Utilities in the name,
	// but I needed to move this somewhere and can only tackle one issue
	// at a time.
	template <class T> static SkyComponent encodeSkyComponent(
		casacore::LogIO& os, casacore::Double& fluxRatio,
		const casacore::ImageInterface<T>& im,
		casa::ComponentType::Shape modelType,
		const casacore::Vector<casacore::Double>& parameters,
		casacore::Stokes::StokesTypes stokes,
		casacore::Bool xIsLong, casacore::Bool deconvolveIt,
		const casacore::GaussianBeam& beam
	);

	// Deconvolve SkyComponent from beam
	// moved from ImageAnalysis. this needs to be moved to a more appropriate class at some point
	static SkyComponent deconvolveSkyComponent(
		casacore::LogIO& os, const SkyComponent& skyIn,
		const casacore::GaussianBeam& beam
	);

	// moved from ImageAnalysis. this needs to be moved to a more appropriate class at some point
	static casacore::Vector<casacore::Double> decodeSkyComponent (
		const SkyComponent& sky, const casacore::ImageInfo& ii,
		const casacore::CoordinateSystem& cSys, const casacore::Unit& brightnessUnit,
		casacore::Stokes::StokesTypes stokes, casacore::Bool xIsLong
	);
	// </group>

	// Convert 2d shape from world (world parameters=x, y, major axis,
	// minor axis, position angle) to pixel (major, minor, pa).
	// Can handle quantum units 'pix'.  If one width is
	// in pixel units both must be in pixel units.  pixelAxes describes which
	// 2 pixel axes of the coordinate system our 2D shape is in.
	// If axes are not from the same coordinate type units must be pixels.
	// If doRef is true, then x and y are taken from the reference
	// value rather than the parameters vector.

	// On input, pa is N->E (at ref pix) for celestial planes.
	// Otherwise pa is in pixel coordinate system +x -> +y
	// On output, pa (radians) is positive +x -> +y in pixel frame
	static void worldWidthsToPixel(
		casacore::Vector<casacore::Double>& dParameters,
		const casacore::Vector<casacore::Quantum<casacore::Double> >& parameters,
		const casacore::CoordinateSystem& cSys,
		const casacore::IPosition& pixelAxes, casacore::Bool doRef=false
	);

	// Convert 2d shape  from pixels (parameters=x,y, major axis,
	// minor axis, position angle) to world (major, minor, pa)
	// at specified location. pixelAxes describes which
	// 2 pixel axes of the coordinate system our 2D shape is in.
	// If doRef is true, then x and y are taken from the reference
	// pixel rather than the paraneters vector.
	//
	// On input pa is positive for +x -> +y in pixel frame
	// On output pa is positive N->E
	// Returns true if major/minor exchanged themselves on conversion to world.
	static casacore::Bool pixelWidthsToWorld(
		casacore::GaussianBeam& wParameters,
		const casacore::Vector<casacore::Double>& pParameters,
		const casacore::CoordinateSystem& cSys,
		const casacore::IPosition& pixelAxes,
		casacore::Bool doRef=false
	);

private:
	// Convert 2d sky shape (parameters=major axis, minor axis, position angle)
	// from pixels to world at reference pixel. pixelAxes describes which
	// 2 pixel axes of the coordinate system our 2D shape is in.
	// On input pa is positive for +x -> +y in pixel frame
	// On output pa is positive N->E
	// Returns true if major/minor exchanged themselves on conversion to world.
	static casacore::Bool _skyPixelWidthsToWorld(
		Angular2DGaussian& gauss2d,
		const casacore::CoordinateSystem& cSys,
		const casacore::Vector<casacore::Double>& pParameters,
		const casacore::IPosition& pixelAxes, casacore::Bool doRef
	);

	// Convert a length and position angle in world units (for a non-coupled
	// coordinate) to pixels. The length is in some 2D plane in the
	// casacore::CoordinateSystem specified  by pixelAxes.
    static casacore::Double _worldWidthToPixel (
    	casacore::Double positionAngle,
    	const casacore::Quantum<casacore::Double>& length,
    	const casacore::CoordinateSystem& cSys,
    	const casacore::IPosition& pixelAxes
    );

    static casacore::Quantum<casacore::Double> _pixelWidthToWorld (
    	casacore::Double positionAngle, casacore::Double length,
    	const casacore::CoordinateSystem& cSys,
    	const casacore::IPosition& pixelAxes
    );

};

}


#ifndef AIPS_NO_TEMPLATE_SRC
#include <components/ComponentModels/SkyComponentFactory2.tcc>
#endif


#endif
