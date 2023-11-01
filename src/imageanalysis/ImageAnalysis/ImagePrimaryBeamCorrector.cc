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
//# $Id: $

#include <imageanalysis/ImageAnalysis/ImagePrimaryBeamCorrector.h>

#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/OS/RegularFile.h>
#include <casacore/casa/OS/SymLink.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/images/Images/ExtendImage.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/images/Images/PagedImage.h>
#include <imageanalysis/ImageAnalysis/SubImageFactory.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/images/Regions/WCBox.h>
#include <casacore/lattices/LatticeMath/LatticeStatistics.h>
#include <casacore/lattices/LRegions/LCLELMask.h>

#include <imageanalysis/ImageAnalysis/ImageInputProcessor.h>

#include <unistd.h>


using namespace casacore;
namespace casa {

const String ImagePrimaryBeamCorrector::_class = "ImagePrimaryBeamCorrector";
uInt _tempTableNumber = 0;

ImagePrimaryBeamCorrector::ImagePrimaryBeamCorrector(
	const SPCIIF image, const SPCIIF pbImage, const Record *const &regionPtr,
	const String& region, const String& box, const String& chanInp,
	const String& stokes, const String& maskInp, const String& outname,
	const Bool overwrite, const Float cutoff, const Bool useCutoff,
	const ImagePrimaryBeamCorrector::Mode mode
) : ImageTask<Float>(
		image, region, regionPtr, box, chanInp, stokes, maskInp, outname,
		overwrite
	), _pbImage(pbImage->cloneII()), _cutoff(cutoff),
	_mode(mode), _useCutoff(useCutoff), _concatHistories(True) {
	_checkPBSanity();
	_construct();
}

ImagePrimaryBeamCorrector::ImagePrimaryBeamCorrector(
	const SPCIIF image, const Array<Float>& pbArray,
	const Record *const &regionPtr, const String& region, const String& box,
	const String& chanInp, const String& stokes, const String& maskInp,
	const String& outname, const Bool overwrite, const Float cutoff,
	const Bool useCutoff, const ImagePrimaryBeamCorrector::Mode mode
) : ImageTask<Float>(
		image, region, regionPtr, box, chanInp, stokes, maskInp, outname, overwrite
	), _cutoff(cutoff), _mode(mode), _useCutoff(useCutoff),
	_concatHistories(False) {
	*_getLog() << LogOrigin(_class, __func__, WHERE);
	auto imShape = _getImage()->shape();
	if (pbArray.shape().isEqual(imShape)) {
		_pbImage.reset(
		    new TempImage<Float>(imShape, _getImage()->coordinates())
		);
	}
	else if (pbArray.ndim() == 2) {
		if (_getImage()->coordinates().hasDirectionCoordinate()) {
			auto dirAxes = _getImage()->coordinates().directionAxesNumbers();
			ThrowIf(
				pbArray.shape()[0] != imShape[dirAxes[0]]
				|| pbArray.shape()[1] != imShape[dirAxes[1]],
				"Array shape does not equal image direction plane shape"
			);
			IPosition boxShape(imShape.size(), 1);
			boxShape[dirAxes[0]] = imShape[dirAxes[0]];
			boxShape[dirAxes[1]] = imShape[dirAxes[1]];
			LCBox x(IPosition(imShape.size(), 0), boxShape - 1, imShape);
			auto sub = SubImageFactory<Float>::createSubImageRO(
				*_getImage(), x.toRecord(""), "", _getLog().get(),
				AxesSpecifier(false)
			);
			_pbImage.reset(new TempImage<Float>(
			    sub->shape(), sub->coordinates())
			);
		}
		else {
			ThrowCc(
			    "Image " + _getImage()->name()
			    + " does not have direction coordinate"
			);
		}
	}
	else {
		ThrowCc(
		    "Primary beam array is of wrong shape ("
		    + pbArray.shape().toString() + ")"
		);
	}
	_pbImage->put(pbArray);
	_construct();
}

ImagePrimaryBeamCorrector::~ImagePrimaryBeamCorrector() {}

String ImagePrimaryBeamCorrector::getClass() const {
	return _class;
}

void ImagePrimaryBeamCorrector::_checkPBSanity() {
	*_getLog() << LogOrigin(_class, __FUNCTION__, WHERE);
	if (_getImage()->shape().isEqual(_pbImage->shape())) {
		const CoordinateSystem csys = _getImage()->coordinates();
        // tolerance allows round trip conversion casa -> fits -> casa
        // in which precision may be lost
        if (
			! csys.directionCoordinate().near(
				_pbImage->coordinates().directionCoordinate(),
                1e-11
			)
		) {
			ThrowCc(
				"Coordinate systems of image and template are different: "
				+ csys.directionCoordinate().errorMessage()
			);
		}
		else {
			// direction coordinates and image shapes are the same
			// which is sufficient to proceed
			// but set the coordinate system just in case the non-directional coordinates
			// differ (CAS-5096)
			_pbImage->setCoordinateInfo(csys);
			return;
		}
	}
	auto dcn = _pbImage->coordinates().pixelAxes(
		_pbImage->coordinates().directionCoordinateNumber()
	);
	auto pbCopy = _pbImage;
	if (_pbImage->ndim() != 2) {
	    pbCopy.reset(
	        new SubImage<Float>(
	            *_pbImage, AxesSpecifier(IPosition(dcn))
	        )
        );
	}
	if (pbCopy->ndim() == 2) {
		ThrowIf(
			! _getImage()->coordinates().directionCoordinate().near(
				pbCopy->coordinates().directionCoordinate()
			),
			"Direction coordinates of input image and primary beam "
			"image are different. Cannot do primary beam correction."
		);
	}
	else {
		ThrowCc(
			"Input image and primary beam image have different shapes. "
			"Cannot do primary beam correction."
		);
	}
	_pbImage = pbCopy;
	LatticeStatistics<Float> ls(*_pbImage);
	Float myMin, myMax;
	ls.getFullMinMax(myMin, myMax);
	if (_mode == DIVIDE && myMax > 1.0 && ! near(myMax, 1.0)) {
		*_getLog() << LogIO::WARN
			<< "Mode DIVIDE chosen but primary beam has one or more pixels "
			<< "greater than 1.0. Proceeding but you may want to check your inputs"
			<< LogIO::POST;
	}
	if (myMin < 0 && near(myMin, 0.0)) {
		*_getLog() << LogIO::WARN
			<< "Primary beam has one or more pixels less than 0."
			<< "Proceeding but you may want to check your inputs"
			<< LogIO::POST;
	}
}

vector<Coordinate::Type> ImagePrimaryBeamCorrector::_getNecessaryCoordinates() const {
	return vector<Coordinate::Type>(1, Coordinate::DIRECTION);
}

CasacRegionManager::StokesControl ImagePrimaryBeamCorrector::_getStokesControl() const {
	return CasacRegionManager::USE_ALL_STOKES;
}

SPIIF ImagePrimaryBeamCorrector::correct(Bool wantReturn) {
    auto origin = LogOrigin(_class, __FUNCTION__, WHERE);
	*_getLog() << origin;
    std::unique_ptr<ImageInterface<Float> > tmpStore;
    ImageInterface<Float> *pbTemplate = _pbImage.get();
	if (! _getImage()->shape().isEqual(_pbImage->shape())) {
		pbTemplate = new ExtendImage<Float>(
			*_pbImage, _getImage()->shape(),
			 _getImage()->coordinates()
		);
		tmpStore.reset(pbTemplate);
	}
    std::shared_ptr<const SubImage<Float> > subImage;
	if (_useCutoff) {
		LatticeExpr<Bool> mask = (_mode == DIVIDE)
			? *pbTemplate >= _cutoff
			: *pbTemplate <= _cutoff;
		if (pbTemplate->hasPixelMask()) {
			mask = mask && pbTemplate->pixelMask();
		}
		subImage.reset(new SubImage<Float>(*_getImage(), LattRegionHolder(LCLELMask(mask))));
		subImage = SubImageFactory<Float>::createSubImageRO(
		    *subImage, *_getRegion(), _getMask(), _getLog().get(),
		    AxesSpecifier(), _getStretch()
		);
	}
	else {
		std::unique_ptr<ImageInterface<Float> > tmp(
			pbTemplate->hasPixelMask()
			? new SubImage<Float>(
				*_getImage(),
				LattRegionHolder(
					LCLELMask(LatticeExpr<Bool>(pbTemplate->pixelMask()))
				)
			)
			: _getImage()->cloneII());
		subImage = SubImageFactory<Float>::createSubImageRO(
		    *tmp, *_getRegion(), _getMask(), _getLog().get(),
		    AxesSpecifier(), _getStretch()
        );
	}
	auto pbSubImage = SubImageFactory<Float>::createSubImageRO(
    	*pbTemplate, *_getRegion(), _getMask(), _getLog().get(),
        AxesSpecifier(), _getStretch()
    );
	tmpStore.reset(0);
	LatticeExpr<Float> expr = (_mode == DIVIDE)
		? *subImage/(*pbSubImage)
		: *subImage*(*pbSubImage);
	if (_concatHistories) {
	    // don't write history initially, since we need to concat the two
	    // image histories
	    suppressHistoryWriting(True);
	}
	SPIIF outImage = _prepareOutputImage(*subImage, expr);
	if (_concatHistories) {
	    // now write the history as per CAS-10591
	    ImageHistory<Float> history(outImage);
	    // because SubImageFactory::createImage() copied the history when
	    // called in ImageTask, and that currently can't be supressed. That
	    // probably needs to be addressed.
	    history.clear();
	    history.addHistory(origin, "History of image " + _getImage()->name());
	    history.append(_getImage());
        history.addHistory(
            origin, "End history of image " + _getImage()->name()
        );
        history.addHistory(origin, "History of image " + _pbImage->name());
        history.append(_pbImage);
        history.addHistory(origin, "End history of image " + _pbImage->name());
	}
    if (! wantReturn) {
    	outImage.reset();
    }
    return outImage;
}
}

