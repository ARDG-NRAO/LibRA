//# Copyright (C) 1996,1997,1998,1999,2001,2002
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
//# $Id:  $

#ifndef IMAGEANALYSIS_FITTERESTIMATESFILEPARSER2_TCC
#define IMAGEANALYSIS_FITTERESTIMATESFILEPARSER2_TCC

#include <imageanalysis/IO/FitterEstimatesFileParser.h>

#include <casacore/casa/aips.h>
#include <casacore/casa/IO/RegularFileIO.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/Containers/Record.h>
#include <components/ComponentModels/ConstantSpectrum.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/GaussianShape.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/images/Images/ImageStatistics.h>


namespace casa {

template <class T> FitterEstimatesFileParser::FitterEstimatesFileParser (
		const casacore::String& filename,
		const casacore::ImageInterface<T>& image
	) : _componentList(), _fixedValues(0), _log(new casacore::LogIO()),
		_peakValues(0), _xposValues(0), _yposValues(0),
		_majValues(0), _minValues(0), _paValues(0), _contents("") {
	casacore::RegularFile myFile(filename);
	_log->origin(casacore::LogOrigin("FitterEstimatesFileParser","constructor"));
	ThrowIf(
	    ! myFile.exists(), "Estimates file " + filename + " does not exist"
	);
	ThrowIf(
	    ! myFile.isReadable(),
	    "Estimates file " + filename + " is not readable"
	);
	_parseFile(myFile);
	_createComponentList(image);
}

template <class T> void FitterEstimatesFileParser::_createComponentList(
	const casacore::ImageInterface<T>& image
) {
	ConstantSpectrum spectrum;
    auto& csys = image.coordinates();
    casacore::Vector<casacore::Double> pos(2,0);
    const auto dirAxesNums = csys.directionAxesNumbers();
    casacore::Vector<casacore::Double> world;
    const auto dirCoordNumber = csys.directionCoordinateNumber();
    const auto& dirCoord = csys.directionCoordinate(
     	dirCoordNumber
    );
    const auto mtype = dirCoord.directionType();
    // SkyComponents require the flux density but users and the fitting
	// code really want to specify peak intensities. So we must convert
	// here. To do that, we need to know the brightness units of the image.

	casacore::Quantity resArea;
	casacore::Quantity intensityToFluxConversion(1.0, "beam");

	// does the image have a restoring beam?
    if (image.imageInfo().hasBeam()) {
        if (image.imageInfo().hasMultipleBeams()) {
            *_log << casacore::LogIO::WARN << "This image has multiple beams. "
                << "The first will be used to determine flux density estimates."
                << casacore::LogIO::POST;
        }
        resArea = casacore::Quantity(
            image.imageInfo().getBeamSet().getBeams().begin()->getArea("sr"),
            "sr"
        );
    }
    else {
		// if no restoring beam, let's hope the the brightness units are
		// in [prefix]Jy/pixel and let's find the pixel size.
    	resArea = dirCoord.getPixelArea();
	}
    const auto units = csys.directionCoordinate().worldAxisUnits();
    casacore::String raUnit = units[0];
    casacore::String decUnit = units[1];
	for(casacore::uInt i=0; i<_peakValues.size(); ++i) {
		pos[dirAxesNums[0]] = _xposValues[i];
		pos[dirAxesNums[1]] = _yposValues[i];
        csys.directionCoordinate().toWorld(world, pos);
        casacore::Quantity ra(world[0], raUnit);
        casacore::Quantity dec(world[1], decUnit);
        casacore::MDirection mdir(ra, dec, mtype);
        GaussianShape gaussShape(
        	mdir, _majValues[i], _minValues[i], _paValues[i]
        );
		const auto brightnessUnit = image.units();
		// Estimate the flux density
		auto fluxQuantity = casacore::Quantity(
		    _peakValues[i], brightnessUnit
		) * intensityToFluxConversion;
		fluxQuantity.convert("Jy");
		fluxQuantity = fluxQuantity*gaussShape.getArea()/resArea;
		// convert to Jy again to get rid of the superfluous sr/(sr)
		fluxQuantity.convert("Jy");
		// Just fill the Stokes which aren't being fit with the same value as
		// the Stokes that is. Doesn't matter that the other three are bogus
		// for the purposes of this, since we only fit one stokes at a time
		casacore::Vector<casacore::Double> fluxStokes(4);
		for(casacore::uInt j=0; j<4; ++j) {
			fluxStokes[j] = fluxQuantity.getValue();
		}
		casacore::Quantum<casacore::Vector<casacore::Double>> fluxVector(
		    fluxStokes, fluxQuantity.getUnit()
		);
		Flux<casacore::Double> flux(fluxVector);
        SkyComponent skyComp(flux, gaussShape, spectrum);
        _componentList.add(skyComp);
	}
}

}

#endif

