//# ImageFFT.cc: FFT an image
//# Copyright (C) 1995,1997,1998,1999,2000,2001,2002,2003
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
//# $Id: ImageFFT.cc 19940 2007-02-27 05:35:22Z Malte.Marquarding $

#ifndef IMAGEANALYSIS_IMAGEFFT_TCC
#define IMAGEANALYSIS_IMAGEFFT_TCC

#include <imageanalysis/ImageAnalysis/ImageFFT.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogIO.h>
#include <casa/Quanta/Unit.h>
#include <casa/Utilities/Assert.h>
#include <casa/iostream.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/CoordinateUtil.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/LatticeMath/LatticeFFT.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/MaskedLatticeIterator.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/TempImage.h>

using namespace casacore;
namespace casa {

template <class T> ImageFFT<T>::ImageFFT() {}

template <class T> ImageFFT<T>::~ImageFFT() {}

template <class T> ImageFFT<T>::ImageFFT(const ImageFFT& other) {
	if (this != &other) {
		if (other._tempImagePtr) {
			_tempImagePtr.reset(other._tempImagePtr->cloneII());
		}
		if (other._image) {
			_image.reset(other._image->cloneII());
		}
		_done = other._done;
	}
}

template <class T> ImageFFT<T>& ImageFFT<T>::operator=(
    const ImageFFT<T>& other
) {
	if (this != &other) {
		_tempImagePtr = other._tempImagePtr
			? SPIIC(other._tempImagePtr->cloneII()) : SPIIC();
		_image = other._image ? SPIIT(other._image->cloneII()) : SPIIT();
		_done = other._done;
   }
   return *this;
}

template <class T> void ImageFFT<T>::fftsky(const ImageInterface<T>& in) {
	// Try and find the sky first.   Exception if not there
	Int dC;
	Vector<Int> pixelAxes, worldAxes;
	_findSky(dC, pixelAxes, worldAxes, in.coordinates(), true);
	_image.reset(in.cloneII());
	// Create TempImage
	_tempImagePtr.reset(
		new casacore::TempImage<ComplexType>(in.shape(), in.coordinates())
	);
	// Set new coordinate system in TempImage
	uInt dC2 = dC;
	_setSkyCoordinates (
		*_tempImagePtr, _image->coordinates(), _image->shape(), dC2
	);
	// Do complex FFT
	_fftsky(*_tempImagePtr, *_image, pixelAxes);
	_done = true;
}

template <class T> void ImageFFT<T>::fft(
    const casacore::ImageInterface<T>& in,
    const casacore::Vector<casacore::Bool>& axes
) {
    // Check axes are ok
    checkAxes (in.coordinates(), in.ndim(), axes);
    _image.reset(in.cloneII());
    _tempImagePtr.reset(
        new TempImage<ComplexType>(in.shape(), in.coordinates())
    );
    // Set new coordinate system in TempImage
    _setCoordinates(*_tempImagePtr, _image->coordinates(), axes, in.shape());
    // Do complex FFT
    _fft(*_tempImagePtr, *_image, axes);
    _done = true;
}

template <class T>
void ImageFFT<T>::getComplex(casacore::ImageInterface<ComplexType>& out) const {
    ThrowIf(
        ! casacore::isComplex(out.dataType()),
        "Data type of input must be a complex type"
    );
    _copyMost(out);
    out.copyData(*_tempImagePtr);
    _fixBUnit(out);
}

template <class T>
void ImageFFT<T>::getReal(ImageInterface<RealType>& out) const {
    ThrowIf(
        ! casacore::isReal(out.dataType()),
        "Data type of input must be a real type"
    );
    _copyMost(out);
	out.copyData(LatticeExpr<RealType>(real(*_tempImagePtr)));
    _fixBUnit(out);
}

template <class T>
void ImageFFT<T>::getImaginary(ImageInterface<RealType>& out) const {
    ThrowIf(
        ! casacore::isReal(out.dataType()),
        "Data type of input must be a real type"
    );
    _copyMost(out);
    out.copyData(LatticeExpr<RealType>(imag(*_tempImagePtr)));
    _fixBUnit(out);
}

template <class T>
void ImageFFT<T>::getAmplitude(ImageInterface<RealType>& out) const {
    ThrowIf(
        ! casacore::isReal(out.dataType()),
        "Data type of input must be a real type"
    );
    _copyMost(out);
    out.copyData(LatticeExpr<RealType>(abs(*_tempImagePtr)));
    _fixBUnit(out);
}

template <class T>
void ImageFFT<T>::getPhase(ImageInterface<RealType>& out) const {
    ThrowIf(
        ! casacore::isReal(out.dataType()),
        "Data type of input must be a real type"
    );
	_copyMost(out);
  	out.copyData(LatticeExpr<RealType>(arg(*_tempImagePtr)));
  	out.setUnits(Unit("rad"));
}

template <class T> template <class U>
void ImageFFT<T>::_copyMost(casacore::ImageInterface<U>& out) const {
    ThrowIf(! _done, "You must call function fft first");
    ThrowIf(
        ! out.shape().isEqual(_tempImagePtr->shape()),
        "Input and output images have inconsistent shapes"
    );
    _copyMask(out, *_image);
    ThrowIf(
        ! out.setCoordinateInfo(_tempImagePtr->coordinates()),
        "Could not replace CoordinateSystem in output phase image"
    );
    _copyMiscellaneous(out);
}

template <class T> template <class U>
void ImageFFT<T>::_fixBUnit(casacore::ImageInterface<U>& out) const {
    String bu = out.units().getName();
    if (bu == "Jy/beam" || bu == "Jy/pixel" ) {
        out.setUnits("Jy");
    }
    if (bu == "Jy") {
        if (out.imageInfo().hasBeam()) {
            // uv-plane -> image-plane with beam
            out.setUnits("Jy/beam");
        }
        else {
            out.setUnits("Jy/pixel");
        }
    }
}

template <class T> void ImageFFT<T>::checkAxes(
    const CoordinateSystem& cSys, uInt ndim, const Vector<Bool>& axes
) {
    ThrowIf (
        axes.nelements() != ndim,
        "The length of the axes vector must be the number of image dimensions"
    );
    // See if we have the sky.  If the user wishes to FFT the sky, they
    // must have both sky axes in their list
    Int dC;
    Vector<Int> pixelAxes, worldAxes;
    Bool haveSky = _findSky(dC, pixelAxes, worldAxes, cSys, false);
    if (haveSky) {
        if (axes(pixelAxes(0)) || axes(pixelAxes(1))) {
            ThrowIf(
                ! (axes[pixelAxes[0]] && axes[pixelAxes[1]]),
                "You must specify both the DirectionCoordinate "
                "(sky) axes to FFT"
            );
        }
    }
}

template <class T> template <class U> void ImageFFT<T>::_copyMask(
    ImageInterface<U>& out, const ImageInterface<T>& in
) {
    if (in.isMasked()) {
        if (out.isMasked() && out.hasPixelMask()) {
            if (! out.pixelMask().isWritable()) {
                LogIO os(LogOrigin("ImageFFT", "copyMask(...)", WHERE));
                os << LogIO::WARN << "The input image is masked but the output "
                    << "image does "<< endl;
                os << "not have a writable mask.  Therefore no mask will be "
                    << "transferred" << LogIO::POST;
                return;
            }
        }
        else {
            return;
        }
    }
    else {
        return;
    }
    IPosition cursorShape = out.niceCursorShape();
    LatticeStepper stepper (out.shape(), cursorShape, LatticeStepper::RESIZE);
    RO_MaskedLatticeIterator<T> iter(in, stepper);
    Lattice<Bool>& outMask = out.pixelMask();
    for (iter.reset(); !iter.atEnd(); iter++) {
        outMask.putSlice(iter.getMask(false), iter.position());
    }
}

template <class T> template <class U> void ImageFFT<T>::_copyMiscellaneous(
    ImageInterface<U>& out
) const {
    out.setMiscInfo(_image->miscInfo());
    out.setImageInfo(_image->imageInfo());
    out.setUnits(_image->units());
    out.appendLog(_image->logger());
}

template <class T> template <class U> void ImageFFT<T>::_fftsky(
	ImageInterface<U>& out, const ImageInterface<T>& in,
	const Vector<Int>& pixelAxes
) {
	Vector<Bool> whichAxes(in.ndim(), false);
	whichAxes[pixelAxes[0]] = true;
	whichAxes[pixelAxes[1]] = true;
	_fft(out, in, whichAxes);
	// LatticeFFT::cfft(out, whichAxes, true);
}

template <class T> template <class U> void ImageFFT<T>::_fft(
    ImageInterface<U>& out, const ImageInterface<T>& in,
    const Vector<Bool>& axes
) {
    static const U dummy(0.0);
    static const auto myType = casacore::whatType(&dummy);
    ThrowIf(
        ! (myType == casacore::TpDComplex || myType == casacore::TpComplex),
        "Logic error. ImageFFT<T>::_fft called with "
        "output image of unsupported type"
    );
    // Do the FFT.  Use in place complex because it does
    // all the unscrambling for me.  Replace masked values
    // by zero and then convert to Complex.  LEL is a marvel.
    static const T zero(0.0);
    LatticeExpr<U> expr;
    if (in.isMasked()) {
        auto zeroed = replace(in, zero);
        expr = casacore::isReal(in.dataType())
            ? LatticeExpr<U>(toComplex(zeroed))
            : LatticeExpr<U>(zeroed);
    }
    else {
        expr = casacore::isReal(in.dataType())
            ? LatticeExpr<U>(toComplex(in))
            : LatticeExpr<U>(in);
    }
    out.copyData(expr);
    LatticeFFT::cfft(out, axes, true);
}

template <class T>
void ImageFFT<T>::_setSkyCoordinates (
	casacore::ImageInterface<ComplexType>& out,
	const casacore::CoordinateSystem& csys, const casacore::IPosition& shape,
	casacore::uInt dC
) {
	// dC is the DC coordinate number
	// Find the input CoordinateSystem
	auto pixelAxes = csys.pixelAxes(dC);
	AlwaysAssert(pixelAxes.nelements()==2,AipsError);
	// Set the DirectionCoordinate axes to true
	casacore::Vector<casacore::Bool> axes(csys.nPixelAxes(), false);
	axes[pixelAxes[0]] = true;
	axes[pixelAxes[1]] = true;
	// FT the CS
	std::shared_ptr<casacore::Coordinate> pC(
		csys.makeFourierCoordinate(axes, shape.asVector())
	);
	// Replace TempImage CS with the new one
	auto* pC2 = (CoordinateSystem*)(pC.get());
	ThrowIf(
		! out.setCoordinateInfo(*pC2),
		"Could not replace Coordinate System in internal complex image"
	);
}

template <class T>
void ImageFFT<T>::_setCoordinates (
	casacore::ImageInterface<ComplexType>& out,
	const casacore::CoordinateSystem& cSys,
	const casacore::Vector<casacore::Bool>& axes,
	const casacore::IPosition& shape
) {
	std::shared_ptr<casacore::Coordinate> pC(
		cSys.makeFourierCoordinate(axes, shape.asVector())
	);
	auto *pCS = (CoordinateSystem*)(pC.get());
	ThrowIf(
		! out.setCoordinateInfo(*pCS),
		"Could not replace Coordinate System in internal complex image"
	);
}

template <class T> Bool ImageFFT<T>::_findSky(
	Int& dC, Vector<Int>& pixelAxes,
	Vector<Int>& worldAxes, const CoordinateSystem& csys,
	Bool throwIt
) {
	if (! csys.hasDirectionCoordinate()) {
		ThrowIf(
			throwIt,
			"Coordinate system does not have a direction coordinate"
		);
		return false;
	}
	dC = csys.directionCoordinateNumber();
	pixelAxes = csys.directionAxesNumbers();
	worldAxes = csys.worldAxes(dC);
   return true;
}

}

#endif

