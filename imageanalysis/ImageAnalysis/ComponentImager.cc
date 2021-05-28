//# ComponentImager.cc:  this defines ComponentImager which modifies images by ComponentLists
//# Copyright (C) 1999,2000,2001,2002,2003
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
//# $Id: ComponentImager.cc 18855 2005-07-21 08:03:40Z nkilleen $

#include <imageanalysis/ImageAnalysis/ComponentImager.h>

#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/Block.h>
#include <casa/Exceptions/Error.h>
#include <casa/Arrays/IPosition.h>
#include <casa/BasicMath/Math.h>
#include <casa/BasicSL/Constants.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MeasRef.h>
#include <measures/Measures/Stokes.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Quanta/Unit.h>
#include <casa/Quanta/UnitMap.h>
#include <casa/Quanta/UnitVal.h>
#include <casa/Quanta/MVAngle.h>
#include <casa/Quanta/MVDirection.h>
#include <casa/Quanta/MVFrequency.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/QMath.h>
#include <casa/Utilities/Assert.h>
#include <casa/BasicSL/String.h>
#include <casa/Arrays/ArrayLogical.h>

#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/SpectralModel.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/GaussianShape.h>
#include <components/ComponentModels/PointShape.h>
#include <components/ComponentModels/C11Timer.h>
#include <components/ComponentModels/SkyCompRep.h>

#include <coordinates/Coordinates/Coordinate.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/ImageInfo.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeStepper.h>

#include <casa/iostream.h>

#include <iomanip>

using namespace casacore;
namespace casa {

ComponentImager::ComponentImager(
	const SPIIF image, const Record *const &region, const String& mask
) : ImageTask<Float>(image, region, mask, "", false),
	_image(image) {
	_construct();
}

ComponentImager::~ComponentImager() {}

void ComponentImager::modify(Bool verbose) {
	*this->_getLog() << LogOrigin(getClass(), __func__);
	int nelem = _list.nelements();
	Vector<SkyComponent> mod(nelem);
	for (int i = 0; i < nelem; ++i) {
		mod[i] = _list.component(i);
	}
	const auto n = mod.size();
	ThrowIf(n == 0, "There are no components in the model componentlist");
	auto subImage = SubImageFactory<Float>::createSubImageRW(
		*_image, *this->_getRegion(), this->_getMask(),
		(verbose ? this->_getLog().get() : nullptr),
        AxesSpecifier(), this->_getStretch()
	);
	// Allow for subtraction/addition
	ComponentList cl;
	for (uInt i = 0; i < n; ++i) {
		SkyComponent sky = mod(i);
		if (_subtract) {
			sky.flux().scaleValue(-1.0);
		}
		cl.add(sky);
	}
    ComponentListImage cli(
        cl, subImage->coordinates(), subImage->shape(), False
    );
    Lattice<Float>* x = &cli;
    LatticeExpr<Float> expr;
	const auto& imageUnitName = subImage->units().getName();
	static const Unit solidAngleUnit("arcsec.arcsec");
	if (imageUnitName.contains("pixel")) {
	    // nothing needs to be done
	}
	else if (imageUnitName.contains("beam")) {
	    const auto& imageInfo = subImage->imageInfo();
	    const auto& csys = subImage->coordinates();
	    const auto& dc = csys.directionCoordinate();
	    if (imageInfo.hasSingleBeam()) {
	        auto f = imageInfo.getBeamAreaInPixels(0, 0, dc);
	        expr = cli * f;
	        x = &expr;
	    }
	    else if (imageInfo.hasMultipleBeams()) {
	        auto nchan = imageInfo.nChannels();
	        auto nstokes = imageInfo.nStokes();
	        auto freqAxis = csys.spectralAxisNumber(False);
	        auto stokesAxis = csys.polarizationAxisNumber(False);
	        auto n = subImage->ndim();
	        IPosition latShape(n, 1);
	        auto hasFreq = freqAxis >= 0;
	        auto hasStokes = stokesAxis >= 0;
	        if (hasFreq) {
	            latShape[freqAxis] = nchan;
	        }
	        if (hasStokes) {
	            latShape[stokesAxis] = nstokes;
	        }
	        ArrayLattice<Float> beamAreas(latShape);
	        IPosition pos(n, 0);
	        for (uInt c=0; c<nchan; ++c) {
	            if (hasFreq) {
	                pos[freqAxis] = c;
	            }
	            for (uInt s=0; s<nstokes; ++s) {
	                if (hasStokes) {
	                    pos[stokesAxis] = s;
	                }
	                beamAreas.putAt(imageInfo.getBeamAreaInPixels(c, s, dc), pos);
	            }
	        }
	        expr = cli * beamAreas;
	        x = &expr;
	    }
	    else {
	        *_getLog() << LogIO::WARN
	            << "No beam defined even though the image units contain a beam. "
	            << "Will assume the beam area is one pixel" << LogIO::POST;
	    }
	}
	else {
	    *_getLog() << LogIO::WARN
	        << "Image units [" << imageUnitName
	        << "] are not dimensionally equivalent to Jy/pixel or Jy/beam. "
	        << "Assuming they are equivalent Jy/pixel" << LogIO::POST;
	}
    *subImage += *x;
}

void ComponentImager::project(ImageInterface<Float>& image, const ComponentList& list) {
	const auto& coords = image.coordinates();
	const auto imageShape = image.shape();
	LogIO os(LogOrigin("ComponentImager", __func__));

	// I currently REQUIRE that:
	// * The list has at least one element.
	// * The image has at least one pixel.
	// * The image has one direction coordinate (only).
	// * The direction coordinate has two pixel and two world axes.
	// * Polarization and frequency coordinates are optional, however at most one
	//   each of these coordinates can exist.
	// * If there is a Stokes axis it can only contain Stokes::I,Q,U,V pols.
	// * No other coordinate types, like LinearCoordinate, are used.
	ThrowIf(
		! coords.hasDirectionCoordinate(), "Image does not have a direction coordinate"
	);
	uInt longAxis, latAxis;
	{
		const Vector<Int> dirAxes = coords.directionAxesNumbers();
		longAxis = dirAxes(0);
		latAxis = dirAxes(1);
	}
	DirectionCoordinate dirCoord = coords.directionCoordinate();
	dirCoord.setWorldAxisUnits(Vector<String>(2, "rad"));

	// Make sure get conversion frame, not just the native one

	MDirection::Types dirFrame;
	dirCoord.getReferenceConversion(dirFrame);
	const MeasRef<MDirection> dirRef(dirFrame);

	MVAngle pixelLatSize, pixelLongSize;
	{
		const Vector<Double> inc = dirCoord.increment();
		pixelLongSize = MVAngle(abs(inc[0]));
		pixelLatSize = MVAngle(abs(inc[1]));
		;
	}

	// Check if there is a Stokes Axes and if so which polarizations. Otherwise
	// only grid the I polarisation.
	// because the code that puts pixel values needs at least one stokes for this to work.
	uInt nStokes = 1;
	if (coords.hasPolarizationCoordinate()) {
		StokesCoordinate stCoord = coords.stokesCoordinate();
		Vector<Int> types = stCoord.stokes();
		nStokes = types.nelements();
		for (uInt p = 0; p < nStokes; p++) {
			Stokes::StokesTypes type = Stokes::type(types[p]);
			ThrowIf(
				type != Stokes::I && type != Stokes::Q
				&& type != Stokes::U && type != Stokes::V,
				"Unsupported stokes type " + Stokes::name(type)
			);
		}
	}

	// Check if there is a frequency axis and if so get the all the frequencies
	// as a Vector<MVFrequency>. Otherwise assume the reference frequency is the
	// same as the reference frequency of the first component in the list.

	MeasRef<MFrequency> freqRef;
	uInt nFreqs = 1;
	Int freqAxis = -1;
	Vector<MVFrequency> freqValues(nFreqs);
	if (coords.hasSpectralAxis()) {
		freqAxis = coords.spectralAxisNumber(false);
		nFreqs = (uInt)imageShape[freqAxis];
		freqValues.resize(nFreqs);
		SpectralCoordinate specCoord = coords.spectralCoordinate();
		specCoord.setWorldAxisUnits(Vector<String>(1, "Hz"));

		// Create Frequency MeasFrame; this will enable conversions between
		// spectral frames (e.g. the CS frame might be TOPO and the CL
		// frame LSRK)

		MFrequency::Types specConv;
		MEpoch epochConv;
		MPosition posConv;
		MDirection dirConv;
		specCoord.getReferenceConversion(specConv,  epochConv, posConv, dirConv);
		MeasFrame measFrame(epochConv, posConv, dirConv);
		freqRef = MeasRef<MFrequency>(specConv, measFrame);
		Double thisFreq;
		for (uInt f = 0; f < nFreqs; f++) {
			// Includes any frame conversion
			ThrowIf (
				!specCoord.toWorld(thisFreq, f),
				"cannot convert a frequency value"
			);
			freqValues(f) = MVFrequency(thisFreq);
		}
	}
	else {
		const MFrequency& defaultFreq =
				list.component(0).spectrum().refFrequency();
		freqRef = defaultFreq.getRef();
		freqValues(0) = defaultFreq.getValue();
	}

	// Find out what the units are. Currently allowed units are anything
	// dimensionally equivalent to Jy/pixel or Jy/beam. If the former then the
	// pixel size at the center of the image is assumed to hold throughout the
	// image. If the latter then the beam is fished out of the header and a
	// 'beam' unit defined. If the units are not defined or are not one of the
	// above they are assumed to be Jy/pixel and a warning message is sent to the
	// logger.

	Unit fluxUnits;
	{
		Unit imageUnit = image.units();
		const String& imageUnitName = imageUnit.getName();
		UnitMap::putUser(
			"pixel", UnitVal(pixelLatSize.radian() *
			pixelLongSize.radian(), "rad.rad")
		);
		// redefine is required to reset Unit Cache
		const Unit pixel("pixel");
		if (imageUnitName.contains("pixel")) {
			// Get the new definition of the imageUnit which uses the new
			// definition of the pixels unit.
			imageUnit = image.units();
			fluxUnits.setValue(imageUnit.getValue() * pixel.getValue());
			fluxUnits.setName(imageUnitName + String(".") + pixel.getName());
		}
		else if (imageUnitName.contains("beam")) {
			const ImageInfo imageInfo = image.imageInfo();
			// FIXME this needs to support multiple beams
			const GaussianBeam beam = imageInfo.restoringBeam();
			if (beam.isNull()) {
				os << LogIO::WARN
					<< "No beam defined even though the image units contain a beam"
					<< endl << "Assuming the beam is one pixel" << LogIO::POST;
				UnitMap::putUser("beam", pixel.getValue());
			}
			else {
				const Quantity beamArea = beam.getArea("sr");
				UnitMap::putUser(
					"beam", UnitVal(beamArea.getValue(),
					beamArea.getFullUnit().getName()))
				;
			}
			const Unit beamUnit("beam");
			const UnitVal fudgeFactor(
				pixel.getValue().getFac()/
				beamUnit.getValue().getFac()
			);

			// Get the new definition of the imageUnit which uses the new
			// definition of the beam unit.  The re-use of the Unit constructor
			// from the String forces the new Unit definitions to take effect
			imageUnit = Unit(image.units().getName());
			fluxUnits.setValue(imageUnit.getValue() *
					beamUnit.getValue() * fudgeFactor);
			fluxUnits.setName(imageUnitName + String(".") + beamUnit.getName());
		}
		// 20101013 the code above for Jy/pixel doesn't work, since Unit doesn't
		// understand that Jy/pixel.pixel == Jy.
		const Unit jy("Jy");
		os << "Adding components to image with units [" << fluxUnits.getName() << "]" << LogIO::POST;
		if (fluxUnits.getName()=="Jy/pixel.pixel") {
			fluxUnits=jy;
		}
		if (fluxUnits != jy) {
			os << LogIO::WARN
					<< "Image units [" << fluxUnits.getName() << "] are not dimensionally equivalent to "
					<< "Jy/pixel or Jy/beam " << endl
					<< "Ignoring the specified units and proceeding on the assumption"
					<< " they are Jy/pixel" << LogIO::POST;
			fluxUnits = jy;
		}
	}

	// Does the image have a writable mask ?  Output pixel values are
	// only modified if the mask==T  and the coordinate conversions
	// succeeded.  The mask==F on output if the coordinate conversion
	// fails (usually means a pixel is outside of the valid CoordinateSystem)
	//
	Bool doMask = false;
	if (image.isMasked() && image.hasPixelMask()) {
		if (image.pixelMask().isWritable()) {
			doMask = true;
		}
		else {
			os << LogIO::WARN
				<< "The image is masked, but it cannot be written to" << LogIO::POST;
		}
	}

	auto myList = &list;
	const auto naxis = imageShape.nelements();
	IPosition pixelPosition(naxis, 0);
	Int polAxis = coords.polarizationAxisNumber(false);
	auto modifiedList = _doPoints(
		image, list, longAxis, latAxis, fluxUnits, dirRef,
		pixelLatSize, pixelLongSize, freqValues, freqRef,
		freqAxis, polAxis, nStokes
	);


	if (modifiedList) {
		myList = modifiedList.get();
	}


	// Setup an iterator to step through the image in chunks that can fit into
	// memory. Go to a bit of effort to make the chunk size as large as
	// possible but still minimize the number of tiles in the cache.
	auto chunkShape = imageShape;
	{
		const IPosition tileShape = image.niceCursorShape(2048*2048);
		chunkShape(longAxis) = tileShape(longAxis);
		chunkShape(latAxis) = tileShape(latAxis);
	}
	auto pixelShape = imageShape;
	pixelShape(longAxis) = pixelShape(latAxis) = 1;
	LatticeStepper pixelStepper(imageShape, pixelShape, LatticeStepper::RESIZE);
	LatticeIterator<Float> chunkIter(image, chunkShape);
	const uInt nDirs = chunkShape(longAxis) * chunkShape(latAxis);
	Cube<Double> pixelVals(4, nDirs, nFreqs);
	Vector<MVDirection> dirVals(nDirs);
	Vector<Bool> coordIsGood(nDirs);
	Vector<Double> pixelDir(2);
	uInt d;

	auto doSample = myList->nelements() > 0;
	Lattice<Bool>* pixelMaskPtr = 0;
	if (doMask) {
		pixelMaskPtr = &image.pixelMask();
	}
	PtrHolder<Array<Bool> > maskPtr;
	for (chunkIter.reset(); !chunkIter.atEnd(); chunkIter++) {
		// Iterate through sky plane of cursor and do coordinate conversions

		const IPosition& blc = chunkIter.position();
		const IPosition& trc = chunkIter.endPosition();
		d = 0;
		pixelDir[1] = blc[latAxis];
		coordIsGood = true;
		auto endLat = trc[latAxis];
		while (pixelDir[1] <= endLat) {
			pixelDir[0] = blc[longAxis];
			auto endLong = trc[longAxis];
			while (pixelDir[0] <= endLong) {
				if (!dirCoord.toWorld(dirVals[d], pixelDir)) {
					// These pixels will be masked
					coordIsGood[d] = false;
				}
				++d;
				++pixelDir[0];
			}
			++pixelDir[1];
		}
		if (doSample) {
			// Sample model, converting the values in the components
			// to the specified direction and spectral frames
			myList->sample(
				pixelVals, fluxUnits, dirVals, dirRef, pixelLatSize,
				pixelLongSize, freqValues, freqRef
			);
		}
		else {
			pixelVals = 0;
		}
		// Modify data by model for this chunk of data
		auto& imageChunk = chunkIter.rwCursor();

		// Get input mask values if available
		if (doMask) {
			maskPtr.set(
				new Array<Bool>(
					image.getMaskSlice(chunkIter.position(),
					chunkIter.cursorShape(), false)
				)
			);
		}
		d = 0;
		pixelPosition[latAxis] = 0;
		coordIsGood = true;

		while (pixelPosition[latAxis] < chunkShape[latAxis]) {
			pixelPosition(longAxis) = 0;
			while (pixelPosition[longAxis] < chunkShape[longAxis]) {
				if (coordIsGood[d]) {
					for (uInt f = 0; f < nFreqs; f++) {
						if (freqAxis >= 0) {
							pixelPosition(freqAxis) = f;
						}
						for (uInt s = 0; s < nStokes; s++) {
							if (polAxis >= 0) {
								pixelPosition(polAxis) = s;
							}
							if (! doMask || (doMask && (*maskPtr)(pixelPosition))) {
								imageChunk(pixelPosition) += pixelVals(s, d, f);
							}
						}
					}
				}
				else if (doMask) {
					(*maskPtr)(pixelPosition) = false;
				}
				d++;
				pixelPosition(longAxis)++;
			}
			pixelPosition(latAxis)++;
		}
		// Update output mask in appropriate fashion

		if (doMask) {
			pixelMaskPtr->putSlice(*maskPtr, chunkIter.position());
		}
	}
}

std::unique_ptr<ComponentList> ComponentImager::_doPoints(
	ImageInterface<Float>& image, const ComponentList& list,
	int longAxis, int latAxis, const Unit& fluxUnits,
	const MeasRef<MDirection>& dirRef, const MVAngle& pixelLatSize,
	const MVAngle& pixelLongSize, const Vector<MVFrequency>& freqValues,
	const MeasRef<MFrequency>& freqRef, Int freqAxis, Int polAxis, uInt nStokes
) {
	// deal with point sources separately
	vector<Int> pointSourceIdx;
	auto n = list.nelements();
	Vector<Double> pixel;
	MVDirection imageWorld;
	auto nFreqs = freqValues.size();
	Cube<Double> values(4, 1, nFreqs);
	IPosition pixelPosition(image.ndim(), 0);
	const auto& dirCoord = image.coordinates().directionCoordinate();
	const auto imageShape = image.shape();
	std::unique_ptr<ComponentList> modifiedList;
	for (uInt i=0; i<n; ++i) {
		if (list.getShape(i)->type() == ComponentType::POINT) {
			auto dir = list.getRefDirection(i);
			dirCoord.toPixel(pixel, dir);
			pixelPosition[longAxis] = floor(pixel[0] + 0.5);
			pixelPosition[latAxis] = floor(pixel[1] + 0.5);
			// in case the source and the coordinate system have different
			// ref frames
			dirCoord.toWorld(imageWorld, pixel);
			const auto& point = list.component(i);
			values = 0;
			Bool foundPixel = false;
			if (
				pixelPosition[longAxis] >= 0 && pixelPosition[latAxis] >= 0
				&& pixelPosition[longAxis] < imageShape[longAxis]
				&& pixelPosition[latAxis] < imageShape[latAxis]
			) {
				point.sample(
					values, fluxUnits, Vector<MVDirection>(1, imageWorld),
					dirRef, pixelLatSize, pixelLongSize, freqValues, freqRef
				);
				foundPixel = anyNE(values, 0.0);
			}
			if (! foundPixel) {
				// look for the pixel in a 3x3 square around the target pixel
				auto targetPixel = pixelPosition;
				for (
					pixelPosition[longAxis]=targetPixel[longAxis]-1;
					pixelPosition[longAxis]<=targetPixel[longAxis]+1; ++pixelPosition[longAxis]
				) {
					for (
						pixelPosition[latAxis]=targetPixel[latAxis]-1;
						pixelPosition[latAxis]<=targetPixel[latAxis]+1; ++pixelPosition[latAxis]
					) {
						if (
							(pixelPosition[longAxis] != targetPixel[longAxis]
							|| pixelPosition[latAxis] != targetPixel[latAxis])
							&& pixelPosition[longAxis] >= 0 && pixelPosition[latAxis] >= 0
							&& pixelPosition[longAxis] < imageShape[longAxis]
							&& pixelPosition[latAxis] < imageShape[latAxis]
						) {
							dirCoord.toWorld(imageWorld, pixel);
							point.sample(
								values, fluxUnits, Vector<MVDirection>(1, imageWorld),
								dirRef, pixelLatSize, pixelLongSize, freqValues, freqRef
							);
							foundPixel = anyNE(values, 0.0);
							if (foundPixel) {
								break;
							}
						}
						if (foundPixel) {
							break;
						}
					}
				}
			}
			if (foundPixel) {
				pointSourceIdx.push_back(i);
				for (uInt f = 0; f < nFreqs; f++) {
					if (freqAxis >= 0) {
						pixelPosition[freqAxis] = f;
					}
					for (uInt s = 0; s < nStokes; s++) {
						if (polAxis >= 0) {
							pixelPosition[polAxis] = s;
						}
						image.putAt(image.getAt(pixelPosition) + values(s, 0, f), pixelPosition);
					}
				}
			}
		}
	}
	if (! pointSourceIdx.empty()) {
		modifiedList.reset(new ComponentList(list));
		modifiedList->remove(Vector<Int>(pointSourceIdx));
	}
	return modifiedList;
}

}
