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

#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <components/ComponentModels/SkyComponentFactory.h>
#include <casa/Quanta/MVAngle.h>
#include <components/ComponentModels/GaussianDeconvolver.h>
#include <components/ComponentModels/GaussianShape.h>
#include <components/ComponentModels/ComponentType.h>
#include <images/Images/ImageUtilities.h>

using namespace casacore;
namespace casa { 

SkyComponent SkyComponentFactory::encodeSkyComponent(
    LogIO& logIO, Double& facToJy,
    const CoordinateSystem& cSys, const Unit& brightnessUnit,
    ComponentType::Shape type, const Vector<Double>& parameters,
    Stokes::StokesTypes stokes, Bool xIsLong, const GaussianBeam& beam
) {
    // Input:
    //   pars(0) = FLux     image units  (e.g. peak flux in Jy/beam)
    //   pars(1) = x cen    abs pix
    //   pars(2) = y cen    abs pix
    //   pars(3) = major    pix
    //   pars(4) = minor    pix
    //   pars(5) = pa radians (pos +x -> +y)

    SkyComponent sky;

    // Account for the fact that 'x' could be longitude or latitude.  Urk.

    Vector<Double> pars = parameters.copy();
    if (!xIsLong) {
        Double tmp = pars(0);

        pars(0) = pars(1);
        pars(1) = tmp;

        Double pa0 = pars(5);
        MVAngle pa(pa0 + C::pi_2);
        pa();                         // +/- pi
        pars(5) = pa.radian();
    }
    GaussianBeam cbeam = beam;
    if (brightnessUnit.getName().contains("beam") &&  beam.isNull()) {
    	cbeam = ImageUtilities::makeFakeBeam(logIO, cSys);
    }

    sky.fromPixel(facToJy, pars, brightnessUnit, cbeam, cSys, type, stokes);
        return sky;
}

/*
// moved from ImageAnalysis. See comments in ImageUtilities.h
// TODO the only thing that uses this is ImageFitter. So move it there
SkyComponent SkyComponentFactory::encodeSkyComponent(
    LogIO& os, Double& facToJy,
    const ImageInterface<Float>& subIm, ComponentType::Shape model,
    const Vector<Double>& parameters, Stokes::StokesTypes stokes,
    Bool xIsLong, Bool deconvolveIt, const GaussianBeam& beam
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

	const CoordinateSystem& cSys = subIm.coordinates();
	const Unit& bU = subIm.units();
	SkyComponent sky = SkyComponentFactory::encodeSkyComponent(
		os, facToJy, cSys, bU, model,
		parameters, stokes, xIsLong, beam
	);
	if (!deconvolveIt) {
		return sky;
    }

	if (beam.isNull()) {
		os << LogIO::WARN
				<< "This image does not have a restoring beam so no deconvolution possible"
				<< LogIO::POST;
		return sky;
	}
	Int dirCoordinate = cSys.findCoordinate(Coordinate::DIRECTION);
	if (dirCoordinate == -1) {
		os << LogIO::WARN
			<< "This image does not have a DirectionCoordinate so no deconvolution possible"
			<< LogIO::POST;
		return sky;
	}
	return SkyComponentFactory::deconvolveSkyComponent(os, sky, beam);
}
*/

// moved from ImageAnalysis. See comments in ImageUtilities.h
SkyComponent SkyComponentFactory::deconvolveSkyComponent(
	LogIO& os,
	const SkyComponent& skyIn, const GaussianBeam& beam
) {
	const ComponentShape& shapeIn = skyIn.shape();
	ComponentType::Shape type = shapeIn.type();
	if (type == ComponentType::POINT) {
		return skyIn;
	}
    SkyComponent skyOut = skyIn.copy();
    if (type == ComponentType::GAUSSIAN) {
        // Recover shape
        const TwoSidedShape& ts = dynamic_cast<const TwoSidedShape&> (shapeIn);
        Quantity major = ts.majorAxis();
        Quantity minor = ts.minorAxis();
        Quantity pa = ts.positionAngle();
        Angular2DGaussian source(major, minor, pa);
        Angular2DGaussian deconvolvedSize;
        GaussianDeconvolver::deconvolve(deconvolvedSize, source, beam);
        const MDirection dirRefIn = shapeIn.refDirection();
        GaussianShape shapeOut(
        	dirRefIn, deconvolvedSize.getMajor(),
        	deconvolvedSize.getMinor(),
        	deconvolvedSize.getPA(true)
        );
        skyOut.setShape(shapeOut);
    }
    else {
        os << "Cannot deconvolve components of type " << shapeIn.ident()
    	    << LogIO::EXCEPTION;
    }
	return skyOut;
}

Vector<Double> SkyComponentFactory::decodeSkyComponent (
	const SkyComponent& sky,
	const ImageInfo& ii,
	const CoordinateSystem& cSys,
	const Unit& brightnessUnit,
	Stokes::StokesTypes stokes,
	Bool xIsLong
) {
//
// The decomposition of the SkyComponent gives things as longitide
// and latitude.  But it is possible that the x and y axes of the
// pixel array are lat/long rather than long/lat if the CoordinateSystem
// has been reordered.  So we have to take this into account.
//
// Output:
//   pars(0) = FLux     image units  (e.g. peak flux in Jy/beam)
//   pars(1) = x cen    abs pix
//   pars(2) = y cen    abs pix
//   pars(3) = major    pix
//   pars(4) = minor    pix
//   pars(5) = pa radians (pos +x -> +y)
//
   GaussianBeam beam = ii.restoringBeam();

// pars(1,2) = longitude, latitude centre

   Vector<Double> pars = sky.toPixel (brightnessUnit, beam, cSys, stokes).copy();

// Now account for the fact that 'x' (horizontally displayed axis) could be
// longitude or latitude.  Urk.

   Double pa0 = pars(5);
   if (!xIsLong) {
      Double tmp = pars(0);
      pars(0) = pars(1);
      pars(1) = tmp;
//
      MVAngle pa(pa0 - C::pi_2);
      pa();                         // +/- pi
      pa0 = pa.radian();
   }
   pars(5) = pa0;
//
   return pars;
}

void SkyComponentFactory::worldWidthsToPixel(
	Vector<Double>& dParameters,
	const Vector<Quantum<Double> >& wParameters,
	const CoordinateSystem& cSys,
	const IPosition& pixelAxes,
	Bool doRef
)
//
// world parameters: x, y, major, minor, pa
// pixel parameters: major, minor, pa (rad)
//
{
	ThrowIf(
		pixelAxes.nelements()!=2,
		"You must give two pixel axes"
	);
	ThrowIf(
		wParameters.nelements() != 5,
		"The world parameters vector must be of length 5."
    );

	dParameters.resize(3);
	Int c0, c1, axisInCoordinate0, axisInCoordinate1;
	cSys.findPixelAxis(c0, axisInCoordinate0, pixelAxes(0));
	cSys.findPixelAxis(c1, axisInCoordinate1, pixelAxes(1));

	// Find units

	String majorUnit = wParameters(2).getFullUnit().getName();
	String minorUnit = wParameters(3).getFullUnit().getName();

	// This saves me trying to handle mixed pixel/world units which is a pain for coupled coordinates

	ThrowIf(
		(majorUnit==String("pix") && minorUnit!=String("pix"))
		|| (majorUnit!=String("pix") && minorUnit==String("pix")),
        "If pixel units are used, both major and minor axes must have pixel units"
	);

	// Some checks

	Coordinate::Type type0 = cSys.type(c0);
	Coordinate::Type type1 = cSys.type(c1);
	ThrowIf(
		type0 != type1
		&& (majorUnit!=String("pix") || minorUnit!=String("pix")),
        "The coordinate types for the convolution axes are different. "
        "Therefore the units of the major and minor axes of "
        "the convolution kernel widths must both be pixels."
	);
	ThrowIf(
		type0 == Coordinate::DIRECTION && type1 == Coordinate::DIRECTION && c0 != c1,
		"The given axes do not come from the same Direction coordinate. "
		"This situation requires further code development."
	);
	ThrowIf(
		type0 == Coordinate::STOKES || type1 == Coordinate::STOKES,
        "Cannot convolve Stokes axes."
	);

	// Deal with pixel units separately.    Both are in pixels if either is in pixels.

	if (majorUnit==String("pix")) {
		dParameters(0) = max(wParameters(2).getValue(), wParameters(3).getValue());
		dParameters(1) = min(wParameters(2).getValue(), wParameters(3).getValue());

		if (type0==Coordinate::DIRECTION && type1==Coordinate::DIRECTION) {
			const DirectionCoordinate& dCoord = cSys.directionCoordinate (c0);

			// Use GaussianShape to get the position angle right. Use the specified
			// direction or the reference direction

			MDirection world;
			if (doRef) {
				dCoord.toWorld(world, dCoord.referencePixel());
			}
			else {
				world = MDirection(wParameters(0), wParameters(1), dCoord.directionType());
			}

			Quantity tmpMaj(1.0, Unit("arcsec"));
			GaussianShape gaussShape(world, tmpMaj, dParameters(1)/dParameters(0),
                                  wParameters(4));                              // pa is N->E
			Vector<Double> pars = gaussShape.toPixel (dCoord);
			dParameters(2) = pars(4);                                              // pa: +x -> +y
		}
		else {

			// Some 'mixed' plane; the pa is already +x -> +y

			dParameters(2) = wParameters(4).getValue(Unit("rad"));                  // pa
		}
		return;
	}

	// Continue on if non-pixel units

	if (type0==Coordinate::DIRECTION && type1==Coordinate::DIRECTION) {

		// Check units are angular

		Unit rad(String("rad"));
		ThrowIf(
			! wParameters(2).check(rad.getValue()),
			"The units of the major axis must be angular"
		);
		ThrowIf(
			! wParameters(3).check(rad.getValue()),
			"The units of the minor axis must be angular"
		);

		// Make a Gaussian shape to convert to pixels at specified location

		const DirectionCoordinate& dCoord = cSys.directionCoordinate (c0);

		MDirection world;
		if (doRef) {
			dCoord.toWorld(world, dCoord.referencePixel());
		}
		else {
			world = MDirection(wParameters(0), wParameters(1), dCoord.directionType());
		}
		GaussianShape gaussShape(world, wParameters(2), wParameters(3), wParameters(4));
		Vector<Double> pars = gaussShape.toPixel (dCoord);
		dParameters(0) = pars(2);
		dParameters(1) = pars(3);
		dParameters(2) = pars(4);      // radians; +x -> +y
	}
	else {

		// The only other coordinates currently available are non-coupled
		// ones and linear except for Tabular, which can be non-regular.
		// Urk.

		// Find major and minor axes in pixels

		dParameters(0) = _worldWidthToPixel (dParameters(2), wParameters(2),
                                          cSys, pixelAxes);
		dParameters(1) = _worldWidthToPixel (dParameters(2), wParameters(3),
                                          cSys, pixelAxes);
		dParameters(2) = wParameters(4).getValue(Unit("rad"));                // radians; +x -> +y
	}

	// Make sure major > minor

	Double tmp = dParameters(0);
	dParameters(0) = max(tmp, dParameters(1));
	dParameters(1) = min(tmp, dParameters(1));
}

Bool SkyComponentFactory::pixelWidthsToWorld(
	GaussianBeam& wParameters,
	const Vector<Double>& pParameters, const CoordinateSystem& cSys,
	const IPosition& pixelAxes, Bool doRef
) {
	// pixel parameters: x, y, major, minor, pa (rad)
	// world parameters: major, minor, pa
	ThrowIf(
		pixelAxes.nelements() != 2,
		"You must give two pixel axes"
	);
	ThrowIf(
		pParameters.nelements() != 5,
		"The parameters vector must be of length 5"
	);
	Int c0, axis0, c1, axis1;
	cSys.findPixelAxis(c0, axis0, pixelAxes(0));
	cSys.findPixelAxis(c1, axis1, pixelAxes(1));
	Bool flipped = false;
	if (
		cSys.type(c1) == Coordinate::DIRECTION
		&& cSys.type(c0) == Coordinate::DIRECTION
	) {
		ThrowIf(
			c0 != c1,
			"Cannot handle axes from different DirectionCoordinates"
		);
		flipped = _skyPixelWidthsToWorld(wParameters, cSys, pParameters, pixelAxes, doRef);
	}
	else {
		wParameters = GaussianBeam();
		// Major/minor
		Quantity q0 = _pixelWidthToWorld(
			pParameters(4), pParameters(2),
			cSys, pixelAxes
		);
		Quantity q1 = _pixelWidthToWorld(
			pParameters(4), pParameters(3),
			cSys, pixelAxes
		);
		// Position angle; radians; +x -> +y
		if (q0.getValue() < q1.getValue(q0.getFullUnit())) {
			flipped = true;
			wParameters = GaussianBeam(q1, q0, Quantity(pParameters(4), "rad"));

		}
		else {
			wParameters = GaussianBeam(q0, q1, Quantity(pParameters(4), "rad"));
		}
	}
	return flipped;
}


Bool SkyComponentFactory::_skyPixelWidthsToWorld (
	Angular2DGaussian& gauss2d,
	const CoordinateSystem& cSys,
	const Vector<Double>& pParameters,
	const IPosition& pixelAxes, Bool doRef
)
//
// pixel parameters: x, y, major, minor, pa (rad)
// world parameters: major, minor, pa
//
{
	// What coordinates are these axes ?

	Int c0, c1, axisInCoordinate0, axisInCoordinate1;
	cSys.findPixelAxis(c0, axisInCoordinate0, pixelAxes(0));
	cSys.findPixelAxis(c1, axisInCoordinate1, pixelAxes(1));
	// See what sort of coordinates we have. Make sure it is called
	// only for the Sky.  More development needed otherwise.

	Coordinate::Type type0 = cSys.type(c0);
	Coordinate::Type type1 = cSys.type(c1);
	ThrowIf(
		type0!=Coordinate::DIRECTION || type1!=Coordinate::DIRECTION,
		"Can only be called for axes holding the sky"
	);
	ThrowIf(
		c0 != c1,
		"The given axes do not come from the same Direction coordinate. "
		"This situation requires further code development"
	);
	// Is the 'x' (first axis) the Longitude or Latitude ?

	Vector<Int> dirPixelAxes = cSys.pixelAxes(c0);
	Bool xIsLong = dirPixelAxes(0)==pixelAxes(0) && dirPixelAxes(1)==pixelAxes(1);
	uInt whereIsX = 0;
	uInt whereIsY = 1;
	if (!xIsLong) {
		whereIsX = 1;
		whereIsY = 0;
	}
	// Encode a pretend GaussianShape from these values as a means
	// of converting to world.

	const DirectionCoordinate& dCoord = cSys.directionCoordinate(c0);
	GaussianShape gaussShape;
	Vector<Double> cParameters(pParameters.copy());
	if (doRef) {
		cParameters(0) = dCoord.referencePixel()(whereIsX);     // x centre
		cParameters(1) = dCoord.referencePixel()(whereIsY);     // y centre
	}
	else {
		if (xIsLong) {
			cParameters(0) = pParameters(0);
			cParameters(1) = pParameters(1);
		} else {
			cParameters(0) = pParameters(1);
			cParameters(1) = pParameters(0);
		}
	}
	Bool flipped = gaussShape.fromPixel (cParameters, dCoord);
	gauss2d = Angular2DGaussian(
			gaussShape.majorAxis(), gaussShape.minorAxis(),
			gaussShape.positionAngle()
	);
	return flipped;
}

Double SkyComponentFactory::_worldWidthToPixel (
	Double positionAngle,
	const Quantum<Double>& length,
	const CoordinateSystem& cSys,
	const IPosition& pixelAxes
) {
	Int worldAxis0 = cSys.pixelAxisToWorldAxis(pixelAxes(0));
	Int worldAxis1 = cSys.pixelAxisToWorldAxis(pixelAxes(1));

	// Units of the axes must be consistent for now.
	// I will be able to relax this criterion when I get the time

	Vector<String> units = cSys.worldAxisUnits();
	Unit unit0(units(worldAxis0));
	Unit unit1(units(worldAxis1));
	ThrowIf(
		unit0 != unit1,
		"Units of the two axes must be conformant"
	);
	Unit unit(unit0);

	// Check units are ok

	if (!length.check(unit.getValue())) {
		ostringstream oss;
		oss << "The units of the world length (" << length.getFullUnit().getName()
        	<< ") are not consistent with those of Coordinate System ("
        	<< unit.getName() << ")";
		ThrowCc(oss.str());
	}

	Double w0 = cos(positionAngle) * length.getValue(unit);
	Double w1 = sin(positionAngle) * length.getValue(unit);

	// Find pixel coordinate of tip of axis  relative to reference pixel

	Vector<Double> world = cSys.referenceValue().copy();
	world(worldAxis0) += w0;
	world(worldAxis1) += w1;

	Vector<Double> pixel;
	ThrowIf(
		! cSys.toPixel (pixel, world),
		cSys.errorMessage()
	);

	return hypot(pixel(pixelAxes(0)), pixel(pixelAxes(1)));
}

Quantum<Double> SkyComponentFactory::_pixelWidthToWorld (
	Double positionAngle, Double length,
	const CoordinateSystem& cSys2,
	const IPosition& pixelAxes
) {
	CoordinateSystem cSys(cSys2);
	Int worldAxis0 = cSys.pixelAxisToWorldAxis(pixelAxes(0));
	Int worldAxis1 = cSys.pixelAxisToWorldAxis(pixelAxes(1));

	// Units of the axes must be consistent for now.
	// I will be able to relax this criterion when I get the time

	Vector<String> units = cSys.worldAxisUnits().copy();
	Unit unit0(units(worldAxis0));
	Unit unit1(units(worldAxis1));
	ThrowIf(
		unit0 != unit1,
		"Units of the axes must be conformant"
	);

	// Set units to be the same for both axes

	units(worldAxis1) = units(worldAxis0);
	ThrowIf(
		!cSys.setWorldAxisUnits(units),
		cSys.errorMessage()
    );

	Double p0 = cos(positionAngle) * length;
	Double p1 = sin(positionAngle) * length;

	// Find world coordinate of tip of length relative to reference pixel

	Vector<Double> pixel= cSys.referencePixel().copy();
	pixel(pixelAxes(0)) += p0;
	pixel(pixelAxes(1)) += p1;

	Vector<Double> world;
	ThrowIf(
		! cSys.toWorld(world, pixel),
		cSys.errorMessage()
	);
	Double lengthInWorld = hypot(world(worldAxis0), world(worldAxis1));
	return Quantum<Double>(lengthInWorld, Unit(units(worldAxis0)));
}



using namespace casacore;
} // end namespace casa
