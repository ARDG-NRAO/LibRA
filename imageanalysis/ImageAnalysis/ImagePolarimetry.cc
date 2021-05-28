//# ImagePolarimetry.cc: polarimetric analysis
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# $Id: ImagePolarimetry.cc 20652 2009-07-06 05:04:32Z Malte.Marquarding $

#include <casa/OS/Timer.h>

#include <imageanalysis/ImageAnalysis/ImagePolarimetry.h>

#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Arrays/MaskArrMath.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/LinearCoordinate.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/scimath/Functionals/Polynomial.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Images/ImageExpr.h>
#include <imageanalysis/ImageAnalysis/ImageFFT.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Images/ImageSummary.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/lattices/Lattices/Lattice.h>
#include <casacore/lattices/LRegions/LCSlicer.h>
#include <casacore/lattices/LEL/LatticeExprNode.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/Lattices/TiledLineStepper.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/Lattices/LatticeUtilities.h>
#include <casacore/lattices/Lattices/MaskedLatticeIterator.h>
#include <casacore/lattices/LatticeMath/LatticeStatistics.h>
#include <casacore/lattices/LRegions/LCPagedMask.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/scimath/Mathematics/NumericTraits.h>
#include <casacore/casa/System/ProgressMeter.h>
#include <casacore/casa/Quanta/QC.h>
#include <casacore/casa/Quanta/MVAngle.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/BasicSL/String.h>

#include <casa/sstream.h>

using namespace casacore;
namespace casa {

const std::map<ImagePolarimetry::StokesTypes, String> ImagePolarimetry::polMap = {
    {I, "I"}, {Q, "Q"}, {U, "U"}, {V, "V"}
};

ImagePolarimetry::ImagePolarimetry (const ImageInterface<Float>& image)
: _image(image.cloneII())
{
   _stokes.resize(4);
   _stokesStats.resize(4);
   _stokes.set(0);
   _stokesStats.set(0);
   _findStokes();
   _createBeamsEqMat();
}

ImagePolarimetry::ImagePolarimetry(const ImagePolarimetry &other) {
   operator=(other);
}

ImagePolarimetry &ImagePolarimetry::operator=(const ImagePolarimetry &other) {
   if (this != &other) {
      _image.reset(other._image->cloneII());
      const auto n = _stokes.size();
      for (size_t i=0; i<n; ++i) {
         if (_stokes[i]) {
            delete _stokes[i];
            _stokes[i] = nullptr;
         }
         if (other._stokes[i]) {
            _stokes[i] = other._stokes[i]->cloneII();
         }
      }
      // Just delete fitter. It will make a new one when needed.
      if (_fitter) {
         delete _fitter;
         _fitter = nullptr;
      }
      // Remake Statistics objects as needed
      _oldClip = 0.0;
      for (size_t i=0; i<n; ++i) {
         if (_stokesStats[i]) {
            delete _stokesStats[i];
            _stokesStats[i] = nullptr;
         }
      }
      _beamsEqMat.assign(other._beamsEqMat);
   }
   return *this;
}

ImagePolarimetry::~ImagePolarimetry() {
   _cleanup();
}

ImageExpr<Complex> ImagePolarimetry::complexLinearPolarization() {
    _hasQU();
    _checkQUBeams(false);
    LatticeExprNode node(
        casacore::formComplex(
            *_stokes[ImagePolarimetry::Q], *_stokes[ImagePolarimetry::U]
        )
    );
    LatticeExpr<Complex> le(node);
    ImageExpr<Complex> ie(le, String("ComplexLinearPolarization"));
    // Need a Complex Linear Polarization type
    _fiddleStokesCoordinate(ie, Stokes::Plinear);
    ie.setUnits(_image->units());
    _setInfo(ie, Q);
    return ie;
}

void ImagePolarimetry::_setInfo(
    ImageInterface<Complex>& im, StokesTypes stokes
) const {
    auto info = _image->imageInfo();
	if (info.hasMultipleBeams()) {
		info.setBeams(_stokes[stokes]->imageInfo().getBeamSet());
	}
	im.setImageInfo(info);
}

void ImagePolarimetry::_setInfo(
    ImageInterface<Float>& im, StokesTypes stokes
) const {
    auto info = _image->imageInfo();
	if (info.hasMultipleBeams()) {
		info.setBeams(_stokes[stokes]->imageInfo().getBeamSet());
	}
	im.setImageInfo(info);
}

ImageExpr<Complex> ImagePolarimetry::complexFractionalLinearPolarization() {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    _hasQU();
    ThrowIf(
       ! _stokes[ImagePolarimetry::I],
       "This image does not have Stokes I so cannot "
        "provide fractional linear polarization"
    );
    _checkIQUBeams(false);
    LatticeExprNode nodeQU(
        casacore::formComplex(
            *_stokes[ImagePolarimetry::Q], *_stokes[ImagePolarimetry::U]
        )
    );
    LatticeExprNode nodeI(*_stokes[ImagePolarimetry::I]);
    LatticeExpr<Complex> le(nodeQU/nodeI);
    ImageExpr<Complex> ie(le, String("ComplexFractionalLinearPolarization"));
    // Need a Complex Linear Polarization type
    _fiddleStokesCoordinate(ie, Stokes::PFlinear);
    ie.setUnits(Unit(""));
    _setInfo(ie, I);
    return ie;
}

ImageExpr<Float> ImagePolarimetry::fracLinPol(
    Bool debias, Float clip, Float sigma
) {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    _hasQU();
    ThrowIf(
        ! _stokes[ImagePolarimetry::I],
        "This image does not have Stokes I so cannot "
        "provide fractional linear polarization"
    );
    Vector<StokesTypes> types(3);
    types[0] = I; types[1] = Q; types[2] = U;
    _checkIQUBeams(false);
    auto nodePol = _makePolIntNode(os, debias, clip, sigma, true, false);
    LatticeExprNode nodeI(*_stokes[ImagePolarimetry::I]);
    LatticeExpr<Float> le(nodePol/nodeI);
    ImageExpr<Float> ie(le, String("FractionalLinearPolarization"));
    ie.setUnits(Unit(""));
    auto ii = _image->imageInfo();
    ii.removeRestoringBeam();
    ie.setImageInfo(ii);
   _fiddleStokesCoordinate(ie, Stokes::PFlinear);
   return ie;
}

ImageExpr<Float> ImagePolarimetry::sigmaFracLinPol(Float clip, Float sigma) {
    // sigma_m = m * sqrt( (sigmaP/p)**2 + (sigmaI/I)**2) )
    // sigmaP = sigmaQU
    // sigmaI = sigmaI
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    _hasQU();
    ThrowIf(
        ! _stokes[ImagePolarimetry::I],
        "This image does not have Stokes I so cannot provide "
        "fractional linear polarization"
    );
    _checkIQUBeams(false);
    // Make nodes.  Don't bother debiasing.
   Bool debias = false;
   auto nodePol = _makePolIntNode(os, debias, clip, sigma, true, false);
   LatticeExprNode nodeI(*_stokes[ImagePolarimetry::I]);

   // Make expression.  We assume sigmaI = sigmaQU which is true with
   // no dynamic range limititation.  Perhaps we should work out
   // sigmaI as well.

   const auto sigma2 = sigmaLinPolInt(clip, sigma);
   LatticeExprNode n0(nodePol/nodeI);
   LatticeExprNode n1(pow(sigma2/nodePol,2));
   LatticeExprNode n2(pow(sigma2/nodeI,2));
   LatticeExpr<Float> le(n0 * sqrt(n1 + n2));
   ImageExpr<Float> ie(le, String("FractionalLinearPolarizationError"));
   ie.setUnits(Unit(""));
   auto ii = _image->imageInfo();
   ii.removeRestoringBeam();
   ie.setImageInfo(ii);
   _fiddleStokesCoordinate(ie, Stokes::PFlinear);
   return ie;
}

ImageExpr<Float> ImagePolarimetry::fracTotPol(
    Bool debias, Float clip, Float sigma
) {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    Bool doLin, doCirc;
    _setDoLinDoCirc(doLin, doCirc);
    auto nodePol = _makePolIntNode(os, debias, clip, sigma, doLin, doCirc);
    LatticeExprNode nodeI(*_stokes[ImagePolarimetry::I]);
    LatticeExpr<Float> le(nodePol/nodeI);
    ImageExpr<Float> ie(le, String("FractionalTotalPolarization"));
    ie.setUnits(Unit(""));
    auto ii = _image->imageInfo();
    ii.removeRestoringBeam();
    ie.setImageInfo(ii);
    _fiddleStokesCoordinate(ie, Stokes::PFtotal);
    return ie;
}

void ImagePolarimetry::_setDoLinDoCirc(Bool& doLin, Bool& doCirc) const {
	LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
	doLin = _stokes[ImagePolarimetry::Q] && _stokes[ImagePolarimetry::U];
	doCirc = _stokes[ImagePolarimetry::V];
	// Should never happen
	AlwaysAssert((doLin || doCirc), AipsError);
	ThrowIf(
	    ! _stokes[ImagePolarimetry::I],
		"This image does not have Stokes I so this calculation "
	    "cannot be carried out"
	);
	if (doLin && ! _checkIQUBeams(false, false)) {
	    os << LogIO::WARN << "I, Q, and U beams are not the same, cannot do "
	        << "linear portion" << LogIO::POST;
	    doLin = false;
	}
	if (doCirc && ! _checkIVBeams(false, false)) {
	    os << LogIO::WARN << "I and V beams are not the same, cannot do "
	        << "circular portion" << LogIO::POST;
	    doCirc = false;
	}
	ThrowIf(
	    ! (doLin || doCirc), "Can do neither linear nor circular portions"
	);
}

ImageExpr<Float> ImagePolarimetry::sigmaFracTotPol(Float clip, Float sigma) {
    // sigma_m = m * sqrt( (sigmaP/P)**2 + (sigmaI/I)**2) )
    // sigmaP = sigmaQU
    // sigmaI = sigmaI
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    Bool doLin, doCirc;
    _setDoLinDoCirc(doLin, doCirc);
    // Make nodes.  Don't bother debiasing.
    Bool debias = false;
    auto nodePol = _makePolIntNode(os, debias, clip, sigma, doLin, doCirc);
    LatticeExprNode nodeI(*_stokes[ImagePolarimetry::I]);
    // Make expression.  We assume sigmaI = sigmaQU which is true with
    // no dynamic range limitation.  Perhaps we should work out
    // sigmaI as well.
    const auto sigma2 = sigmaTotPolInt(clip, sigma);
    LatticeExprNode n0(nodePol/nodeI);
    LatticeExprNode n1(pow(sigma2/nodePol,2));
    LatticeExprNode n2(pow(sigma2/nodeI,2));
    LatticeExpr<Float> le(n0 * sqrt(n1 + n2));
    ImageExpr<Float> ie(le, String("FractionalLinearPolarizationError"));
    ie.setUnits(Unit(""));
    auto ii = _image->imageInfo();
    ii.removeRestoringBeam();
    ie.setImageInfo(ii);
    _fiddleStokesCoordinate(ie, Stokes::PFlinear);
    return ie;
}

void ImagePolarimetry::fourierRotationMeasure(
    ImageInterface<Complex>& cpol, Bool zeroZeroLag
) {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    _hasQU();
    _checkQUBeams(true, false);
    CoordinateSystem dCS;
    Stokes::StokesTypes dType = Stokes::Plinear;
    const auto shape = singleStokesShape(dCS, dType);
    if(! cpol.shape().isEqual(shape)) {
        os << "The provided  image has the wrong shape " << cpol.shape()
            << endl;
        os << "It should be of shape " << shape  << LogIO::EXCEPTION;
    }
    const auto& cSys = _image->coordinates();
    Int spectralCoord, fAxis;
    _findFrequencyAxis (spectralCoord, fAxis, cSys, -1);
    // Make Complex (Q,U) image
    LatticeExprNode node;
    if (zeroZeroLag) {
       TempImage<Float> tQ(
           _stokes[ImagePolarimetry::Q]->shape(),
           _stokes[ImagePolarimetry::Q]->coordinates()
       );
       if (_stokes[ImagePolarimetry::Q]->isMasked()) {
           tQ.makeMask(String("mask0"), true, true, false, false);
       }
       LatticeUtilities::copyDataAndMask(
           os, tQ, *_stokes[ImagePolarimetry::Q], false
       );
       _subtractProfileMean (tQ, fAxis);
       TempImage<Float> tU(
           _stokes[ImagePolarimetry::U]->shape(),
           _stokes[ImagePolarimetry::U]->coordinates()
       );
       if (_stokes[ImagePolarimetry::U]->isMasked()) {
           tU.makeMask(String("mask0"), true, true, false, false);
       }
       LatticeUtilities::copyDataAndMask(
           os, tU, *_stokes[ImagePolarimetry::U], false
       );
       _subtractProfileMean (tU, fAxis);
       // The TempImages will be cloned be LatticeExprNode so it's ok
       // that they go out of scope
       node = LatticeExprNode(formComplex(tQ, tU));
    }
    else {
        node = LatticeExprNode(
            formComplex(
                *_stokes[ImagePolarimetry::Q], *_stokes[ImagePolarimetry::U]
            )
        );
    }
    LatticeExpr<Complex> le(node);
    ImageExpr<Complex> ie(le, String("ComplexLinearPolarization"));
    // Do FFT of spectral coordinate
    Vector<Bool> axes(ie.ndim(),false);
    axes(fAxis) = true;
    ImageFFT<Complex> fftserver;
    fftserver.fft(ie, axes);
    // Recover Complex result. Coordinates are updated to include Fourier
    // coordinate, miscellaneous things (MiscInfo, ImageInfo, units, history)
    // and mask (if output has one) are copied to cpol
    fftserver.getComplex(cpol);
    // Fiddle time coordinate to be a RotationMeasure coordinate
    auto f = _findCentralFrequency(
        cSys.coordinate(spectralCoord), ie.shape()(fAxis)
    );
    _fiddleTimeCoordinate(cpol, f, spectralCoord);
    // Set Stokes coordinate to be correct type
    _fiddleStokesCoordinate(cpol, Stokes::Plinear);
    // Set units and ImageInfo
    cpol.setUnits(_image->units());
    _setInfo(cpol, Q);
}

Float ImagePolarimetry::sigmaLinPolInt(Float clip, Float sigma) {
    // sigma_P = sigma_QU
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    ThrowIf(
        ! _stokes[ImagePolarimetry::Q] && ! _stokes[ImagePolarimetry::U],
       "This image does not have Stokes Q and U so cannot "
        "provide linear polarization"
    );
    _checkQUBeams(false);
    Float sigma2 = 0.0;
    if (sigma > 0) {
        sigma2 = sigma;
    }
    else {
        os << LogIO::NORMAL << "Determined noise from Q&U images to be ";
        auto sq = _sigma(ImagePolarimetry::Q, clip);
        auto su = _sigma(ImagePolarimetry::U, clip);
        sigma2 = (sq+su)/2.0;
    }
    return sigma2;
}

ImageExpr<Float> ImagePolarimetry::linPolPosAng(Bool radians) const {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    ThrowIf(
        ! _stokes[ImagePolarimetry::Q] && ! _stokes[ImagePolarimetry::U],
        "This image does not have Stokes Q and U so cannot "
        "provide linear polarization"
    );
    _checkQUBeams(false);
    // Make expression. LEL function "pa" returns degrees
    Float fac = radians ? C::pi / 180.0 : 1.0;
    LatticeExprNode node(
        fac*pa(*_stokes[ImagePolarimetry::U], *_stokes[ImagePolarimetry::Q])
    );
    LatticeExpr<Float> le(node);
    ImageExpr<Float> ie(le, String("LinearlyPolarizedPositionAngle"));
    ie.setUnits(Unit(radians ? "rad" : "deg"));
    auto ii = _image->imageInfo();
    ii.removeRestoringBeam();
    ie.setImageInfo(ii);
    _fiddleStokesCoordinate(ie, Stokes::Pangle);
    return ie;
}

ImageExpr<Float> ImagePolarimetry::sigmaLinPolPosAng(
    Bool radians, Float clip, Float sigma
) {
    // sigma_PA = sigmaQU / 2P
    ThrowIf(
        ! (_stokes[ImagePolarimetry::Q] || _stokes[ImagePolarimetry::U]),
        "This image does not have Stokes Q and U so "
        "cannot provide linear polarization"
    );
    _checkQUBeams(false);
    Float sigma2 = sigma > 0 ? sigma : this->sigma(clip);
    Float fac = 0.5 * sigma2;
    if (! radians) {
        fac *= 180 / C::pi;
    }
    LatticeExprNode node(
        fac / amp(*_stokes[ImagePolarimetry::U], *_stokes[ImagePolarimetry::Q])
    );
    LatticeExpr<Float> le(node);
    ImageExpr<Float> ie(le, String("LinearlyPolarizedPositionAngleError"));
    ie.setUnits(Unit(radians ? "rad" : "deg"));
    auto ii = _image->imageInfo();
    ii.removeRestoringBeam();
    ie.setImageInfo(ii);
    _fiddleStokesCoordinate(ie, Stokes::Pangle);
    return ie;
}

Float ImagePolarimetry::sigma(Float clip) {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    Float sigma2 = 0.0;
    if (_stokes[ImagePolarimetry::V]) {
        os << LogIO::NORMAL << "Determined noise from V image to be ";
        sigma2 = _sigma(ImagePolarimetry::V, clip);
    }
    else if (
        _stokes[ImagePolarimetry::Q] && _stokes[ImagePolarimetry::U]
        && _checkQUBeams(false, false)
    ) {
        sigma2 = sigmaLinPolInt(clip);
    }
    else if (_stokes[ImagePolarimetry::Q]) {
        os << LogIO::NORMAL << "Determined noise from Q image to be "
            << LogIO::POST;
        sigma2 = _sigma(ImagePolarimetry::Q, clip);
    }
    else if (_stokes[ImagePolarimetry::U]) {
        os << LogIO::NORMAL << "Determined noise from U image to be "
            << LogIO::POST;
        sigma2 = _sigma(ImagePolarimetry::U, clip);
    }
    else if (_stokes[ImagePolarimetry::I]!=0) {
        os << LogIO::NORMAL << "Determined noise from I image to be "
            << LogIO::POST;
        sigma2 = _sigma(ImagePolarimetry::I, clip);
    }
    os << sigma2 << LogIO::POST;
    return sigma2;
}

void ImagePolarimetry::rotationMeasure(
    ImageInterface<Float>*& rmOutPtr, ImageInterface<Float>*& rmOutErrorPtr,
    ImageInterface<Float>*& pa0OutPtr, ImageInterface<Float>*& pa0OutErrorPtr,
    ImageInterface<Float>*& nTurnsOutPtr, ImageInterface<Float>*& chiSqOutPtr,
    Int axis,  Float rmMax, Float maxPaErr, Float sigma, Float rmFg,
    Bool showProgress
) {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    _hasQU();
    _checkQUBeams(false);
    // Do we have anything to do ?
    ThrowIf(
        ! (rmOutPtr || rmOutErrorPtr || pa0OutPtr || pa0OutErrorPtr),
        "No output images specified"
    );
    // Find expected shape of output RM images (Stokes and spectral axes gone)
    CoordinateSystem cSysRM;
    Int fAxis, sAxis;
    const auto shapeRM = rotationMeasureShape(cSysRM, fAxis, sAxis, os, axis);
    const auto shapeNTurns = shapeRM;
    const auto shapeChiSq = shapeRM;
    // Check RM image shapes
    if (rmOutPtr && ! rmOutPtr->shape().isEqual(shapeRM)) {
        os << "The provided Rotation Measure image has the wrong shape "
             << rmOutPtr->shape() << endl;
        os << "It should be of shape " << shapeRM << LogIO::EXCEPTION;
    }
    if (rmOutErrorPtr && !rmOutErrorPtr->shape().isEqual(shapeRM)) {
        os << "The provided Rotation Measure error image has the wrong shape "
            << rmOutErrorPtr->shape() << endl;
        os << "It should be of shape " << shapeRM << LogIO::EXCEPTION;
    }
    // Check position angle image shapes
    CoordinateSystem cSysPA;
    const auto shapePA = positionAngleShape(cSysPA, fAxis, sAxis, os, axis);
    if (pa0OutPtr && ! pa0OutPtr->shape().isEqual(shapePA)) {
        os << "The provided position angle at zero wavelength image has the "
            << "wrong shape " << pa0OutPtr->shape() << endl;
        os << "It should be of shape " << shapePA << LogIO::EXCEPTION;
    }
    if (pa0OutErrorPtr && ! pa0OutErrorPtr->shape().isEqual(shapePA)) {
        os << "The provided position angle at zero wavelength image has the "
            << "wrong shape " << pa0OutErrorPtr->shape() << endl;
        os << "It should be of shape " << shapePA << LogIO::EXCEPTION;
    }
    if (nTurnsOutPtr && ! nTurnsOutPtr->shape().isEqual(shapeNTurns)) {
        os << "The provided nTurns image has the wrong shape "
            << nTurnsOutPtr->shape() << endl;
        os << "It should be of shape " << shapeNTurns << LogIO::EXCEPTION;
    }
    if (chiSqOutPtr && ! chiSqOutPtr->shape().isEqual(shapeChiSq)) {
        os << "The provided chi squared image has the wrong shape "
            << chiSqOutPtr->shape() << endl;
        os << "It should be of shape " << shapeChiSq << LogIO::EXCEPTION;
    }
    // Generate linear polarization position angle image expressions
    // and error in radians
    Bool radians = true;
    Float clip = 10.0;
    const auto pa = linPolPosAng(radians);
    const auto paerr = sigmaLinPolPosAng(radians, clip, sigma);
    CoordinateSystem cSys0 = pa.coordinates();
    // Set frequency axis units to Hz
    auto fAxisWorld = cSys0.pixelAxisToWorldAxis(fAxis);
    ThrowIf(
        fAxisWorld < 0,
        "World axis has been removed for the frequency pixel axis"
    );
    auto axisUnits = cSys0.worldAxisUnits();
    axisUnits(fAxisWorld) = String("Hz");
    ThrowIf(
        ! cSys0.setWorldAxisUnits(axisUnits),
        "Failed to set frequency axis units to Hz because "
        + cSys0.errorMessage()
    );
    // Do we have enough frequency pixels ?
    const uInt nFreq = pa.shape()(fAxis);
    ThrowIf(
        nFreq < 3,
        "This image only has " + String::toString(nFreq)
        + " frequencies, this is not enough"
    );
    // Copy units only over.  The output images don't have a beam
    // so unset beam.   MiscInfo and history require writable II.
    // We leave this to the caller  who knows what sort of II these are.
    auto ii = _image->imageInfo();
    ii.removeRestoringBeam();
    if (rmOutPtr) {
        rmOutPtr->setImageInfo(ii);
        rmOutPtr->setUnits(Unit("rad/m/m"));
    }
    if (rmOutErrorPtr) {
        rmOutErrorPtr->setImageInfo(ii);
        rmOutErrorPtr->setUnits(Unit("rad/m/m"));
    }
    if (pa0OutPtr) {
        pa0OutPtr->setImageInfo(ii);
        pa0OutPtr->setUnits(Unit("deg"));
    }
    if (pa0OutErrorPtr) {
        pa0OutErrorPtr->setImageInfo(ii);
        pa0OutErrorPtr->setUnits(Unit("deg"));
    }
    if (nTurnsOutPtr) {
        nTurnsOutPtr->setImageInfo(ii);
        nTurnsOutPtr->setUnits(Unit(""));
    }
    if (chiSqOutPtr) {
        chiSqOutPtr->setImageInfo(ii);
        chiSqOutPtr->setUnits(Unit(""));
    }
    // Get lambda squared in m**2
    Vector<Double> freqs(nFreq);
    Vector<Float> wsq(nFreq);
    Vector<Double> world;
    Vector<Double> pixel(cSys0.referencePixel().copy());
    Double c = QC::c( ).getValue(Unit("m/s"));
    Double csq = c*c;
    for (uInt i=0; i<nFreq; ++i) {
        pixel(fAxis) = i;
        ThrowIf(
            !cSys0.toWorld(world, pixel),
            "Failed to convert pixel to world because "
            + cSys0.errorMessage()
        );
        freqs(i) = world(fAxisWorld);
        // m**2
        wsq(i) = csq / freqs(i) / freqs(i);
    }
    // Sort into increasing wavelength
    Vector<uInt> sortidx;
    GenSortIndirect<Float>::sort(
        sortidx, wsq, Sort::Ascending, Sort::QuickSort|Sort::NoDuplicates
    );
    Vector<Float> wsqsort(sortidx.size());
    for (uInt i=0; i<wsqsort.size(); ++i) {
        wsqsort[i] = wsq[sortidx[i]];
    }
    // Make fitter
    if (! _fitter) {
        _fitter = new LinearFitSVD<Float>;
        // Create and set the polynomial functional
        // p = c(0) + c(1)*x where x = lambda**2
        // PA = PA0 + RM*Lambda**2
        Polynomial<AutoDiff<Float> > poly1(1);
        // Makes a copy of poly1
        _fitter->setFunction(poly1);
    }
    // Deal with masks.  The outputs are all given a mask if possible as we
    // don't know at this point whether output points will be masked or not
    IPosition whereRM;
    auto isMaskedRM = false;
    Lattice<Bool>* outRMMaskPtr = nullptr;
    if (rmOutPtr) {
        isMaskedRM = _dealWithMask (outRMMaskPtr, rmOutPtr, os, "RM");
        whereRM.resize(rmOutPtr->ndim());
        whereRM = 0;
    }
    auto isMaskedRMErr = false;
    Lattice<Bool>* outRMErrMaskPtr = nullptr;
    if (rmOutErrorPtr) {
        isMaskedRMErr = _dealWithMask(
            outRMErrMaskPtr, rmOutErrorPtr, os, String("RM error")
        );
        whereRM.resize(rmOutErrorPtr->ndim());
        whereRM = 0;
    }
    IPosition wherePA;
    auto isMaskedPa0 = false;
    Lattice<Bool>* outPa0MaskPtr = nullptr;
    if (pa0OutPtr) {
        isMaskedPa0 = _dealWithMask(
            outPa0MaskPtr, pa0OutPtr, os, String("Position Angle")
        );
        wherePA.resize(pa0OutPtr->ndim());
        wherePA = 0;
    }
    auto isMaskedPa0Err = false;
    Lattice<Bool>* outPa0ErrMaskPtr = nullptr;
    if (pa0OutErrorPtr) {
        isMaskedPa0Err = _dealWithMask(
            outPa0ErrMaskPtr, pa0OutErrorPtr, os, "Position Angle error"
        );
        wherePA.resize(pa0OutErrorPtr->ndim());
        wherePA = 0;
    }
    IPosition whereNTurns;
    auto isMaskedNTurns = false;
    Lattice<Bool>* outNTurnsMaskPtr = 0;
    if (nTurnsOutPtr) {
        isMaskedNTurns = _dealWithMask(
            outNTurnsMaskPtr, nTurnsOutPtr, os, "nTurns"
        );
        whereNTurns.resize(nTurnsOutPtr->ndim());
        whereNTurns = 0;
    }
    IPosition whereChiSq;
    auto isMaskedChiSq = false;
    Lattice<Bool>* outChiSqMaskPtr = nullptr;
    if (chiSqOutPtr) {
        isMaskedChiSq = _dealWithMask(
            outChiSqMaskPtr, chiSqOutPtr, os, String("chi sqared")
        );
        whereChiSq.resize(chiSqOutPtr->ndim());
        whereChiSq = 0;
    }
    Array<Bool> tmpMaskRM(IPosition(shapeRM.size(), 1), true);
    Array<Float> tmpValueRM(IPosition(shapeRM.size(), 1), 0.0f);
    Array<Bool> tmpMaskPA(IPosition(shapePA.size(), 1), true);
    Array<Float> tmpValuePA(IPosition(shapePA.size(), 1), 0.0f);
    Array<Float> tmpValueNTurns(IPosition(shapeNTurns.size(), 1), 0.0f);
    Array<Bool> tmpMaskNTurns(IPosition(shapeNTurns.size(), 1), true);
    Array<Float> tmpValueChiSq(IPosition(shapeChiSq.size(), 1), 0.0f);
    Array<Bool> tmpMaskChiSq(IPosition(shapeChiSq.size(), 1), true);
    // Iterate
    const IPosition tileShape = pa.niceCursorShape();
    TiledLineStepper ts(pa.shape(), tileShape, fAxis);
    RO_MaskedLatticeIterator<Float> it(pa, ts);
    Float rm, rmErr, pa0, pa0Err, rChiSq, nTurns;
    uInt j, k, l, m;
    static const Double degPerRad = 180/C::pi;
    maxPaErr /= degPerRad;
    maxPaErr = abs(maxPaErr);
    Bool doRM = whereRM.size() > 0;
    Bool doPA = wherePA.size() > 0;
    Bool doNTurns = whereNTurns.size() > 0;
    Bool doChiSq = whereChiSq.size() > 0;
    unique_ptr<ProgressMeter> pProgressMeter;
    if (showProgress) {
        Double nMin = 0.0;
        Double nMax = 1.0;
        for (Int i=0; i<Int(pa.ndim()); ++i) {
            if (i!=fAxis) {
                nMax *= pa.shape()[i];
            }
        }
        pProgressMeter.reset(
            new ProgressMeter(
                nMin, nMax, "Profiles fitted", "Fitting",
                "", "", true, max(1,Int(nMax/100))
            )
        );
    }
    // As a (temporary?) workaround the cache of the main image is set up in
    // such a way that it can hold the full frequency and stokes axes.
    // The stokes axis is important, otherwise the cache is set up
    // (by the TiledStMan) such that it can hold only 1 stokes
    // with the result that iterating is tremendously slow.
    // We also need to cast the const away from _image.
    const IPosition mainShape = _image->shape();
    uInt nrtiles = (1 + (mainShape(fAxis)-1) / tileShape(fAxis)) *
            (1 + (mainShape(sAxis)-1) / tileShape(sAxis));
    auto* mainImagePtr = const_cast<ImageInterface<Float>*>(_image.get());
    mainImagePtr->setCacheSizeInTiles (nrtiles);
    String posString;
    auto ok = false;
    IPosition shp;
    for (it.reset(); ! it.atEnd(); it++) {
        // Find rotation measure for this line
        ok = _findRotationMeasure(
            rm, rmErr, pa0, pa0Err, rChiSq, nTurns, sortidx, wsqsort,
            it.vectorCursor(), it.getMask(false),
            paerr.getSlice(it.position(),it.cursorShape()),
            rmFg, rmMax, maxPaErr, posString
        );
        // Plonk values into output  image.  This is slow and clunky, but
        // should be relatively fast c.f. the fitting.  Could be reimplemented
        // with LatticeApply if need be.  Buffering is hard because the
        // navigator doesn't take a regular path.  If I used a LatticeStepper
        // instead, the path would be regular and then I could buffer, but then
        // the iteration would be less efficient !!!
        j = k = l = m = 0;
        for (Int i=0; i<Int(it.position().size()); ++i) {
            if (doRM && i != fAxis && i != sAxis) {
                whereRM(j) = it.position()[i];
                ++j;
            }
            if (doPA && i != fAxis) {
                wherePA(k) = it.position()[i];
                ++k;
            }
            if (doNTurns && i != fAxis && i != sAxis) {
                whereNTurns[l] = it.position()[i];
                ++l;
            }
            if (doChiSq && i != fAxis && i != sAxis) {
                whereChiSq[m] = it.position()[i];
                ++m;
            }
        }
        if (isMaskedRM) {
            tmpMaskRM.set(ok);
            outRMMaskPtr->putSlice(tmpMaskRM, whereRM);
        }
        if (isMaskedRMErr) {
            tmpMaskRM.set(ok);
            outRMErrMaskPtr->putSlice(tmpMaskRM, whereRM);
        }
        if (isMaskedPa0) {
            tmpMaskPA.set(ok);
            outPa0MaskPtr->putSlice(tmpMaskPA, wherePA);
        }
        if (isMaskedPa0Err) {
            tmpMaskPA.set(ok);
            outPa0ErrMaskPtr->putSlice(tmpMaskPA, wherePA);
        }
        if (isMaskedNTurns) {
            tmpMaskNTurns.set(ok);
            outNTurnsMaskPtr->putSlice(tmpMaskNTurns, whereNTurns);
        }
        if (isMaskedChiSq) {
            tmpMaskChiSq.set(ok);
            outChiSqMaskPtr->putSlice(tmpMaskChiSq, whereChiSq);
        }
        // If the output value is masked, the value itself is 0
        if (rmOutPtr) {
            tmpValueRM.set(rm);
            rmOutPtr->putSlice(tmpValueRM, whereRM);
        }
        if (rmOutErrorPtr) {
            tmpValueRM.set(rmErr);
            rmOutErrorPtr->putSlice(tmpValueRM, whereRM);
        }
        // Position angles in degrees
        if (pa0OutPtr) {
            tmpValuePA.set(pa0*degPerRad);
            pa0OutPtr->putSlice(tmpValuePA, wherePA);
        }
        if (pa0OutErrorPtr) {
            tmpValuePA.set(pa0Err*degPerRad);
            pa0OutErrorPtr->putSlice(tmpValuePA, wherePA);
        }
        // Number of turns and chi sq
        if (nTurnsOutPtr) {
            tmpValueNTurns.set(nTurns);
            nTurnsOutPtr->putSlice(tmpValueNTurns, whereNTurns);
        }
        if (chiSqOutPtr) {
            tmpValueChiSq.set(rChiSq);
            chiSqOutPtr->putSlice(tmpValueChiSq, whereChiSq);
        }
        if (showProgress) {
            pProgressMeter->update(Double(it.nsteps()));
        }
    }
    // Clear the cache of the main image again.
    mainImagePtr->clearCache();
}

IPosition ImagePolarimetry::rotationMeasureShape(
    CoordinateSystem& cSys, Int& fAxis, Int& sAxis, LogIO&, Int spectralAxis
) const {
    const auto cSys0 = coordinates();
    Int spectralCoord;
    _findFrequencyAxis(spectralCoord, fAxis, cSys0, spectralAxis);
    Int afterCoord = -1;
    auto stokesCoord = cSys0.findCoordinate(Coordinate::STOKES, afterCoord);
    auto pixelAxes = cSys0.pixelAxes(stokesCoord);
    sAxis = pixelAxes(0);
    // What shape should the image be ?  Frequency and stokes axes should be gone.
    const auto shape0 = shape();
    IPosition shape(shape0.size()-2);
    Int j = 0;
    for (Int i=0; i<Int(shape0.size()); ++i) {
        if (i != fAxis && i != sAxis) {
            shape[j] = shape0[i];
            ++j;
        }
    }
    CoordinateSystem tmp;
    cSys = tmp;
    for (Int i=0; i<Int(cSys0.nCoordinates()); ++i) {
        if (i != spectralCoord && i != stokesCoord) {
            cSys.addCoordinate(cSys0.coordinate(i));
        }
    }
    return shape;
}

IPosition ImagePolarimetry::positionAngleShape(
    CoordinateSystem& cSys,  Int& fAxis, Int& sAxis, LogIO&, Int spectralAxis
) const {
    CoordinateSystem cSys0 = coordinates();
    Int spectralCoord = -1;
    _findFrequencyAxis (spectralCoord, fAxis, cSys0, spectralAxis);
    Int afterCoord = -1;
    Int stokesCoord = cSys0.findCoordinate(Coordinate::STOKES, afterCoord);
    Vector<Int> pixelAxes = cSys0.pixelAxes(stokesCoord);
    sAxis = pixelAxes(0);
    _fiddleStokesCoordinate(cSys0, Stokes::Pangle);
    CoordinateSystem tmp;
    cSys = tmp;
    for (Int i=0; i<Int(cSys0.nCoordinates()); ++i) {
       if (i != spectralCoord) {
           cSys.addCoordinate(cSys0.coordinate(i));
       }
    }
    // What shape should the image be ?  Frequency axis should be gone.
    // and Stokes length 1
    const auto shape0 = ImagePolarimetry::shape();
    IPosition shape(shape0.size()-1);
    Int j = 0;
    for (Int i=0; i<Int(shape0.size()); ++i) {
        if (i == sAxis) {
            shape[j] = 1;
            ++j;
        }
        else {
            if (i != fAxis) {
                shape[j] = shape0[i];
                ++j;
            }
        }
    }
    return shape;
}

ImageExpr<Float> ImagePolarimetry::stokesI() const {
   return _makeStokesExpr(
       _stokes[ImagePolarimetry::I], Stokes::I, "StokesI"
   );
}

Float ImagePolarimetry::sigmaStokesI(Float clip) {
   return _sigma(ImagePolarimetry::I, clip);
}

ImageExpr<Float> ImagePolarimetry::stokesQ() const {
   return _makeStokesExpr(_stokes[ImagePolarimetry::Q], Stokes::Q, "StokesQ");
}

Float ImagePolarimetry::sigmaStokesQ(Float clip) {
   return _sigma(ImagePolarimetry::Q, clip);
}

ImageExpr<Float> ImagePolarimetry::stokesU() const {
   return _makeStokesExpr(_stokes[ImagePolarimetry::U], Stokes::U, "StokesU");
}

Float ImagePolarimetry::sigmaStokesU(Float clip) {
   return _sigma(ImagePolarimetry::U, clip);
}

ImageExpr<Float> ImagePolarimetry::stokesV() const {
   return _makeStokesExpr(_stokes[ImagePolarimetry::V], Stokes::V, "StokesV");
}

Float ImagePolarimetry::sigmaStokesV(Float clip) {
   return _sigma(ImagePolarimetry::V, clip);
}

ImageExpr<Float> ImagePolarimetry::stokes(
    ImagePolarimetry::StokesTypes stokes
) const {
   const auto type = _stokesType(stokes);
   return _makeStokesExpr(_stokes[stokes], type, _stokesName(stokes));
}

Float ImagePolarimetry::sigmaStokes(
    ImagePolarimetry::StokesTypes stokes, Float clip
) {
   return _sigma(stokes, clip);
}

void ImagePolarimetry::summary(LogIO& os) const {
   ImageSummary<Float> s(*_image);
   s.list(os);
}

ImageExpr<Float> ImagePolarimetry::totPolInt(
    Bool debias, Float clip, Float sigma
) {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    Bool doLin, doCirc;
    _setDoLinDoCirc(doLin, doCirc);
    auto node = _makePolIntNode(os, debias, clip, sigma, doLin, doCirc);
    LatticeExpr<Float> le(node);
    ImageExpr<Float> ie(le, String("totalPolarizedIntensity"));
    // Dodgy. The beam is now rectified
    ie.setUnits(_image->units());
    StokesTypes stokes = _stokes[Q] ? Q : _stokes[U] ? U : V;
    _setInfo(ie, stokes);
    _fiddleStokesCoordinate(ie, Stokes::Ptotal);
    return ie;
}

Float ImagePolarimetry::sigmaTotPolInt(Float clip, Float sigma) {
    // sigma_P = sigma_QUV
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    Bool doLin, doCirc;
    _setDoLinDoCirc(doLin, doCirc);
    Float sigma2 = sigma > 0 ? sigma : this->sigma(clip);
    return sigma2;
}


IPosition ImagePolarimetry::singleStokesShape(
    CoordinateSystem& cSys, Stokes::StokesTypes type
) const {
    // We know the image has a Stokes coordinate or it
    // would have failed at construction
    auto cSys0 = _image->coordinates();
    _fiddleStokesCoordinate(cSys0, type);
    cSys = cSys0;
    Int afterCoord = -1;
    const auto iStokes = cSys0.findCoordinate(Coordinate::STOKES, afterCoord);
    const auto pixelAxes = cSys0.pixelAxes(iStokes);
    auto shape = _image->shape();
    shape[pixelAxes[0]] = 1;
    return shape;
}

ImageExpr<Float> ImagePolarimetry::depolarizationRatio(
    const ImageInterface<Float>& im1, const ImageInterface<Float>& im2,
    Bool debias, Float clip, Float sigma
) {
    ImagePolarimetry p1(im1);
    ImagePolarimetry p2(im2);
    ImageExpr<Float> m1(p1.fracLinPol(debias, clip, sigma));
    ImageExpr<Float> m2(p2.fracLinPol(debias, clip, sigma));
    LatticeExprNode n1(m1/m2);
    LatticeExpr<Float> le(n1);
    ImageExpr<Float> depol(le, "DepolarizationRatio");
    return depol;
}

ImageExpr<Float> ImagePolarimetry::sigmaDepolarizationRatio(
    const ImageInterface<Float>& im1, const ImageInterface<Float>& im2,
    Bool debias, Float clip, Float sigma
) {
    Vector<StokesTypes> types(3);
    types[0] = I;
    types[1] = Q;
    types[2] = U;
    _checkBeams(im1, im2, types);
    ImagePolarimetry p1(im1);
    ImagePolarimetry p2(im2);
    ImageExpr<Float> m1 = p1.fracLinPol(debias, clip, sigma);
    ImageExpr<Float> sm1 = p1.sigmaFracLinPol(clip, sigma);
    ImageExpr<Float> m2 = p2.fracLinPol(debias, clip, sigma);
    ImageExpr<Float> sm2 = p2.sigmaFracLinPol(clip, sigma);
    LatticeExprNode n0(m1/m2);
    LatticeExprNode n1(sm1*sm1/m1/m1);
    LatticeExprNode n2(sm2*sm2/m2/m2);
    LatticeExprNode n3(n0 * sqrt(n1+n2));
    LatticeExpr<Float> le(n3);
    ImageExpr<Float> sigmaDepol(le, "DepolarizationRatioError");
     return sigmaDepol;
}

void ImagePolarimetry::_cleanup() {
    _image.reset();
    for (uInt i=0; i<4; ++i) {
        delete _stokes[i];
        _stokes[i] = nullptr;
        delete _stokesStats[i];
        _stokesStats[i] = nullptr;
    }
    delete _fitter;
    _fitter = nullptr;
}

void ImagePolarimetry::_findFrequencyAxis(
    Int& spectralCoord, Int& fAxis, const CoordinateSystem& cSys,
    Int spectralAxis
) const {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    spectralCoord = -1;
    fAxis = -1;
    if (spectralAxis >= 0) {
        ThrowIf(
            spectralAxis >= Int(cSys.nPixelAxes()),
            "Illegal spectral axis " + String::toString(spectralAxis) +" given"
        );
        fAxis = spectralAxis;
        Int axisInCoordinate;
        cSys.findPixelAxis(spectralCoord, axisInCoordinate, fAxis);
        // Check coordinate type is one of expected types
        ThrowIf(
            ! (
                cSys.type(spectralCoord)==Coordinate::TABULAR
                || cSys.type(spectralCoord)==Coordinate::LINEAR
                || cSys.type(spectralCoord)==Coordinate::SPECTRAL
            ),
            "The specified axis of type " + cSys.showType(spectralCoord)
            + " cannot be a frequency axis"
        );
    }
    else {
        spectralCoord = _findSpectralCoordinate(cSys, os, false);
        if (spectralCoord < 0) {
            for (uInt i=0; i<cSys.nCoordinates(); ++i) {
                if (
                    cSys.type(i)==Coordinate::TABULAR
                    || cSys.type(i)==Coordinate::LINEAR
                ) {
                    const auto axisNames = cSys.coordinate(i).worldAxisNames();
                    String tmp = axisNames[0];
                    tmp.upcase();
                    if (tmp.contains(String("FREQ"))) {
                        spectralCoord = i;
                        break;
                    }
                }
            }
        }
        ThrowIf(
            spectralCoord < 0,
            "Cannot find SpectralCoordinate in this image"
        );
        fAxis = cSys.pixelAxes(spectralCoord)[0];
    }
}

void ImagePolarimetry::_findStokes() {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    const CoordinateSystem& cSys = _image->coordinates();
    Int polAxisNum = cSys.polarizationAxisNumber();
    ThrowIf(
        polAxisNum < 0, "There is no Stokes Coordinate in this image"
    );
    const uInt ndim = _image->ndim();
    auto shape = _image->shape();
    IPosition blc(ndim,0);
    auto trc = shape - 1;
    for (const auto& kv : polMap) {
        const auto pix = cSys.stokesPixelNumber(kv.second);
        if (pix >= 0) {
            _stokes[kv.first] = _makeSubImage(blc, trc, polAxisNum, pix);
        }
    }
    if((_stokes[Q] && ! _stokes[U]) || (! _stokes[Q] && _stokes[U])) {
        _cleanup();
        ThrowCc(
            "This Stokes coordinate has only one of Q and U. This is not useful"
        );
    }
    if (! (_stokes[Q] || _stokes[U] || _stokes[V])) {
       _cleanup();
       ThrowCc("This image has no Stokes Q, U, nor V.  This is not useful");
    }
}

void ImagePolarimetry::_fiddleStokesCoordinate(
    ImageInterface<Float>& im, Stokes::StokesTypes type
) const {
    CoordinateSystem cSys = im.coordinates();
    _fiddleStokesCoordinate(cSys, type);
    im.setCoordinateInfo(cSys);
}

void ImagePolarimetry::_fiddleStokesCoordinate(
    CoordinateSystem& cSys, Stokes::StokesTypes type
) const {
    Int afterCoord = -1;
    Int iStokes = cSys.findCoordinate(Coordinate::STOKES, afterCoord);
    const Vector<Int> which(1, Int(type));
    StokesCoordinate stokes(which);
    cSys.replaceCoordinate(stokes, iStokes);
}

void ImagePolarimetry::_fiddleStokesCoordinate(
    ImageInterface<Complex>& ie, Stokes::StokesTypes type
) const {
    CoordinateSystem cSys = ie.coordinates();
    _fiddleStokesCoordinate(cSys, type);
    ie.setCoordinateInfo(cSys);
}

void ImagePolarimetry::_fiddleTimeCoordinate(
    ImageInterface<Complex>& ie, const Quantum<Double>& f, Int coord
) const {
    LogIO os(LogOrigin("ImagePolarimetry", __func__, WHERE));
    CoordinateSystem cSys = ie.coordinates();
    unique_ptr<Coordinate> pC(cSys.coordinate(coord).clone());
    AlwaysAssert(pC->nPixelAxes()==1,AipsError);
    AlwaysAssert(pC->type()==Coordinate::LINEAR,AipsError);
    auto axisUnits = pC->worldAxisUnits();
    axisUnits = String("s");
    ThrowIf(
        ! pC->setWorldAxisUnits(axisUnits),
        "Failed to set TimeCoordinate units to seconds because "
        + pC->errorMessage()
    );
    // Find factor to convert from time (s) to rad/m/m
    auto inc = pC->increment();
    const auto ff = f.getValue(Unit("Hz"));
    const auto lambda = QC::c( ).getValue(Unit("m/s")) / ff;
    const auto fac = -C::pi * ff / 2.0 / lambda / lambda;
    inc *= fac;
    Vector<String> axisNames(1);
    axisNames = String("RotationMeasure");
    axisUnits = String("rad/m/m");
    Vector<Double> refVal(1,0.0);
    LinearCoordinate lC(
        axisNames, axisUnits, refVal, inc,
        pC->linearTransform().copy(), pC->referencePixel().copy()
    );
    cSys.replaceCoordinate(lC, coord);
    ie.setCoordinateInfo(cSys);
}

Quantum<Double> ImagePolarimetry::_findCentralFrequency(
    const Coordinate& coord, Int shape
) const {
    AlwaysAssert(coord.nPixelAxes()==1,AipsError);
    Vector<Double> pixel(1);
    Vector<Double> world;
    pixel(0) = Double(shape - 1) / 2.0;
    ThrowIf(
        ! coord.toWorld(world, pixel),
        "Failed to convert pixel to world for SpectralCoordinate because "
        + coord.errorMessage()
    );
    const auto units = coord.worldAxisUnits();
    return Quantum<Double>(world(0), units(0));
}

Int ImagePolarimetry::_findSpectralCoordinate(
    const CoordinateSystem& cSys, LogIO& os, Bool fail
) const {
    Int afterCoord = -1;
    Int coord = cSys.findCoordinate(Coordinate::SPECTRAL, afterCoord);
    ThrowIf(coord < 0 && fail, "No spectral coordinate in this image");
    if (afterCoord>0) {
        os << LogIO::WARN << "This image has more than one spectral "
            << "coordinate; only first used" << LogIO::POST;
    }
    return coord;
}

Bool ImagePolarimetry::_findRotationMeasure(
    Float& rmFitted, Float& rmErrFitted, Float& pa0Fitted, Float& pa0ErrFitted,
    Float& rChiSqFitted, Float& nTurns, const Vector<uInt>& sortidx,
    const Vector<Float>& wsq2, const Vector<Float>& pa2,
    const Array<Bool>& paMask2, const Array<Float>& paerr2, Float rmFg,
    Float rmMax, Float maxPaErr, const String& posString
) {
    // wsq is lambda squared in m**2 in increasing wavelength order
    // pa is position angle in radians
    // paerr is pa error in radians
    // maxPaErr is maximum tolerated error in position angle
    // rmfg is a user specified foreground RM rad/m/m
    // rmmax is a user specified maximum RM
    static Vector<Float> paerr;
    static Vector<Float> pa;
    static Vector<Float> wsq;
    // Abandon if less than 2 points
    uInt n = sortidx.size();
    if (n<2) {
        return false;
    }
    rmFitted = rmErrFitted = pa0Fitted = pa0ErrFitted = rChiSqFitted = 0.0;
    // Sort into decreasing frequency order and correct for foreground rotation
    // Remember wsq already sorted.  Discard points that are too noisy or masked
    const Vector<Float>& paerr1(paerr2.nonDegenerate(0));
    const Vector<Bool>& paMask1(paMask2.nonDegenerate(0));
    paerr.resize(n);
    pa.resize(n);
    wsq.resize(n);
    uInt j = 0;
    for (uInt i=0; i<n; ++i) {
        if (abs(paerr1(sortidx(i))) < maxPaErr && paMask1(sortidx(i))) {
            pa(j) = pa2(sortidx(i)) - rmFg*wsq2(i);
            paerr(j) = paerr1(sortidx(i));
            wsq(j) = wsq2(i);
            ++j;
        }
    }
    n = j;
    if (n<=1) {
        return false;
    }
    pa.resize(n, true);
    paerr.resize(n, true);
    wsq.resize(n, true);
    // Treat supplementary and primary points separately
    Bool ok = n == 2
        ? _rmSupplementaryFit(
            nTurns, rmFitted, rmErrFitted, pa0Fitted, pa0ErrFitted,
            rChiSqFitted, wsq, pa, paerr
        )
        : _rmPrimaryFit(
            nTurns, rmFitted, rmErrFitted, pa0Fitted, pa0ErrFitted,
            rChiSqFitted, wsq, pa, paerr, rmMax, /*plotter,*/ posString
        );
    // Put position angle into the range 0->pi
    static MVAngle tmpMVA1;
    if (ok) {
        MVAngle tmpMVA0(pa0Fitted);
        tmpMVA1 = tmpMVA0.binorm(0.0);
        pa0Fitted = tmpMVA1.radian();
        // Add foreground back on
        rmFitted += rmFg;
    }
    return ok;
}

void ImagePolarimetry::_hasQU () const {
   ThrowIf(
       ! (_stokes[Q] && _stokes[U]),
       "This image does not have Stokes Q and U which are "
       "required for this function"
   );
}

ImageExpr<Float> ImagePolarimetry::_makeStokesExpr(
    ImageInterface<Float>* imPtr, Stokes::StokesTypes type, const String& name
) const {
    ThrowIf(! imPtr, "This image does not have Stokes " + Stokes::name(type));
    LatticeExprNode node(*imPtr);
    LatticeExpr<Float> le(node);
    ImageExpr<Float> ie(le, name);
    ie.setUnits(_image->units());
    _fiddleStokesCoordinate(ie, type);
    return ie;
}

ImageInterface<Float>* ImagePolarimetry::_makeSubImage(
    IPosition& blc, IPosition& trc, Int axis, Int pix
) const {
    blc[axis] = pix;
    trc[axis] = pix;
    LCSlicer slicer(blc, trc, RegionType::Abs);
    ImageRegion region(slicer);
    return new SubImage<Float>(*_image, region);
}

LatticeExprNode ImagePolarimetry::_makePolIntNode(
    LogIO& os, Bool debias, Float clip, Float sigma, Bool doLin, Bool doCirc
) {
    LatticeExprNode linNode, circNode, node;
    Float sigma2 = debias ? (sigma > 0.0 ? sigma : this->sigma(clip)) : 0.0;
    if (doLin) {
        linNode = LatticeExprNode(pow(*_stokes[U], 2) + pow(*_stokes[Q], 2));
    }
    if (doCirc) {
        circNode = LatticeExprNode(pow(*_stokes[V], 2));
    }
    Float sigmasq = sigma2 * sigma2;
    if (doLin && doCirc) {
        node = linNode + circNode;
        if (debias) {
            node = node - LatticeExprNode(sigmasq);
            os << LogIO::NORMAL << "Debiasing with sigma = " << sqrt(sigmasq)
                << LogIO::POST;
        }
    }
    else if (doLin) {
        node = linNode;
        if (debias) {
            node = node - LatticeExprNode(sigmasq);
            os << LogIO::NORMAL << "Debiasing with sigma  = " << sqrt(sigmasq)
                << LogIO::POST;
        }
    }
    else if (doCirc) {
        node = circNode;
        if (debias) {
            node = node - LatticeExprNode(sigmasq);
            os << LogIO::NORMAL << "Debiasing with sigma = " << sqrt(sigmasq)
                << LogIO::POST;
        }
    }
    return LatticeExprNode(sqrt(node));
}

Bool ImagePolarimetry::_rmPrimaryFit(
    Float& nTurns, Float& rmFitted, Float& rmErrFitted, Float& pa0Fitted,
    Float& pa0ErrFitted, Float& rChiSqFitted, const Vector<Float>& wsq,
    const Vector<Float>& pa, const Vector<Float>& paerr, Float rmMax,
    const String&
) {
    static Vector<Float> plotPA;
    static Vector<Float> plotPAErr;
    static Vector<Float> plotPAErrY1;
    static Vector<Float> plotPAErrY2;
    static Vector<Float> plotPAFit;
    // Assign position angle to longest wavelength consistent with RM < RMMax
    const uInt n = wsq.size();
    Double dwsq = wsq(n-1) - wsq(0);
    Float ppa = abs(rmMax)*dwsq + pa(0);
    Float diff = ppa - pa(n-1);
    Float t = diff >= 0 ? 0.5 : -0.5;
    Int maxnpi = Int(diff/C::pi + t);
    ppa = -abs(rmMax)*dwsq + pa(0);
    diff = ppa - pa(n-1);
    t = diff >= 0 ? 0.5 : -0.5;
    Int minnpi = Int(diff/C::pi + t);
    // Loop over range of n*pi ambiguity
    Vector<Float> fitpa(n);
    Vector<Float> pars;
    Float chiSq = 1e30;
    for (Int h=minnpi; h<=maxnpi; ++h) {
        fitpa[n-1] = pa[n-1] + C::pi*h;
        Float rm0 = (fitpa(n-1) - pa(0))/dwsq;
        // Assign position angles to remaining wavelengths
        for (uInt k=1; k<n-1; ++k) {
            ppa = pa[0] + rm0*(wsq[k]-wsq[0]);
            diff = ppa - pa[k];
            t = diff >= 0 ? 0.5 : -0.5;
            Int npi = Int(diff/C::pi + t);
            fitpa[k] = pa[k] + npi*C::pi;
        }
        fitpa[0] = pa[0];
        // Do least squares fit
        if (!_rmLsqFit (pars, wsq, fitpa, paerr)) {
            return false;
        }
        if (pars[4] < chiSq) {
            chiSq = pars[4];
            nTurns = h;                   // Number of turns
            rmFitted = pars[0];           // Fitted RM
            rmErrFitted = pars[1];        // Error in RM
            pa0Fitted = pars[2];          // Fitted intrinsic angle
            pa0ErrFitted = pars[3];       // Error in angle
            rChiSqFitted = pars[4];       // Recued chi squared
            if (n > 2) {
                rChiSqFitted /= Float(n - 2);
            }
        }
    }
    return true;
}

Bool ImagePolarimetry::_rmSupplementaryFit(
    Float& nTurns, Float& rmFitted, Float& rmErrFitted, Float& pa0Fitted,
    Float& pa0ErrFitted, Float& rChiSqFitted,  const Vector<Float>& wsq,
    const Vector<Float>& pa, const Vector<Float>& paerr
) {
    // For supplementary points find lowest residual RM
    const uInt n = wsq.size();
    Float absRM = 1e30;
    Vector<Float> fitpa(pa.copy());
    Vector<Float> pars;
    for (Int i=-2; i<3; ++i) {
        fitpa[n-1] = pa[n-1] + C::pi*i;
        // Do least squares fit
        if (! _rmLsqFit (pars, wsq, fitpa, paerr)) {
            return false;
        }
        // Save solution  with lowest absolute RM
        if (abs(pars[0]) < absRM) {
            absRM = abs(pars[0]);
            nTurns = i;                        // nTurns
            rmFitted = pars(0);                // Fitted RM
            rmErrFitted = pars[1];             // Error in RM
            pa0Fitted = pars[2];               // Fitted intrinsic angle
            pa0ErrFitted = pars[3];            // Error in angle
            rChiSqFitted = pars[4];            // Reduced chi squared
            if (n > 2) {
                rChiSqFitted /= Float(n - 2);
            }
        }
    }
    return true;
}

Bool ImagePolarimetry::_rmLsqFit(
    Vector<Float>& pars, const Vector<Float>& wsq, const Vector<Float> pa,
    const Vector<Float>& paerr
) const {
    // Perform fit on unmasked data
    static Vector<Float> solution;
    try {
        solution = _fitter->fit(wsq, pa, paerr);
    } catch (AipsError x) {
        return false;
    }
    const Vector<Double>& cv = _fitter->compuCovariance().diagonal();
    pars.resize(5);
    pars[0] = solution[1];
    pars[1] = sqrt(cv[1]);
    pars[2] = solution[0];
    pars[3] = sqrt(cv[0]);
    pars[4] = _fitter->chiSquare();
    return true;
}

String ImagePolarimetry::_stokesName(
    ImagePolarimetry::StokesTypes index
) const {
    const auto iter = polMap.find(index);
    return (iter == polMap.end()) ? "??" : iter->second;
}

Stokes::StokesTypes ImagePolarimetry::_stokesType (ImagePolarimetry::StokesTypes index) const
{
   if (index==ImagePolarimetry::I) {
      return Stokes::I;
   } else if (index==ImagePolarimetry::Q) {
      return Stokes::Q;
   } else if (index==ImagePolarimetry::U) {
      return Stokes::U;
   } else if (index==ImagePolarimetry::V) {
      return Stokes::V;
   } else {
      return Stokes::Undefined;
   }
}


Float ImagePolarimetry::_sigma(
    ImagePolarimetry::StokesTypes index, Float clip
) {
    Float clip2 = clip == 0 ? 10.0 : abs(clip);
    if (clip2 != _oldClip && _stokesStats[index]) {
        delete _stokesStats[index];
        _stokesStats[index] = 0;
    }
    if (! _stokesStats[index]) {
        // Find sigma for all points inside +/- clip-sigma of the mean
        // More joys of LEL
        const ImageInterface<Float>* p = _stokes[index];
        LatticeExprNode n1 (*p);
        LatticeExprNode n2 (n1[abs(n1-mean(n1)) < clip2*stddev(n1)]);
        LatticeExpr<Float> le(n2);
        _stokesStats[index] = new LatticeStatistics<Float>(le, false, false);
    }
    Array<Float> sigmaA;
    _stokesStats[index]->getConvertedStatistic(sigmaA, LatticeStatsBase::SIGMA);
    ThrowIf(
        sigmaA.empty(),
        "No good points in clipped determination of the noise for the Stokes "
        + _stokesName(index) + " image"
    );
    _oldClip = clip2;
    return sigmaA(IPosition(1,0));
}

void ImagePolarimetry::_subtractProfileMean(
    ImageInterface<Float>& im, uInt axis
) const {
    const IPosition tileShape = im.niceCursorShape();
    TiledLineStepper ts(im.shape(), tileShape, axis);
    LatticeIterator<Float> it(im, ts);
    Float dMean;
    if (im.isMasked()) {
        const Lattice<Bool>& mask = im.pixelMask();
        for (it.reset(); !it.atEnd(); it++) {
            const auto& p = it.cursor();
            const auto& m = mask.getSlice(it.position(), it.cursorShape());
            const MaskedArray<Float> ma(p, m, true);
            dMean = mean(ma);
            it.rwVectorCursor() -= dMean;
        }
    }
    else {
        for (it.reset(); !it.atEnd(); it++) {
            dMean = mean(it.vectorCursor());
            it.rwVectorCursor() -= dMean;
        }
    }
}

Bool ImagePolarimetry::_dealWithMask(
    Lattice<Bool>*& pMask, ImageInterface<Float>*& pIm, LogIO& os,
    const String& type
) const {
    auto isMasked = pIm->isMasked();
    if (! isMasked) {
        if (pIm->canDefineRegion()) {
            pIm->makeMask("mask0", true, true, true, true);
            isMasked = true;
        }
        else {
            os << LogIO::WARN << "Could not create a mask for the output "
                << type << " image" << LogIO::POST;
        }
    }
    if (isMasked) {
        pMask = &(pIm->pixelMask());
        if (! pMask->isWritable()) {
            isMasked = false;
            os << LogIO::WARN << "The output " << type
                << " image has a mask but it's not writable" << LogIO::POST;
        }
    }
    return isMasked;
}

void ImagePolarimetry::_createBeamsEqMat() {
	_beamsEqMat.assign(Matrix<Bool>(4, 4, false));
	Bool hasMultiBeams = _image->imageInfo().hasMultipleBeams();
	for (uInt i=0; i<4; ++i) {
		for (uInt j=i; j<4; ++j) {
			if (! (_stokes[i] &&  _stokes[j])) {
				_beamsEqMat(i, j) = false;
			}
			else if (i == j) {
				_beamsEqMat(i, j) = true;
			}
			else if (hasMultiBeams) {
				_beamsEqMat(i, j) = (
					_stokes[i]->imageInfo().getBeamSet()
					== _stokes[j]->imageInfo().getBeamSet()
				);
			}
			else {
				_beamsEqMat(i, j) = true;
			}
			_beamsEqMat(j, i) = _beamsEqMat(i, j);

		}
	}
}

Bool ImagePolarimetry::_checkBeams(
    const vector<StokesTypes>& stokes, Bool requireChannelEquality, Bool throws
) const {
	for (
		auto iter = stokes.cbegin(); iter != stokes.cend(); ++iter
	) {
		for (
			auto jiter=iter; jiter!=stokes.cend(); ++jiter
		) {
			if (iter == jiter) {
				continue;
			}
			if (! _beamsEqMat(*iter, *jiter)) {
				ThrowIf(
				    throws,
				    "Input image has multiple beams and the corresponding "
				    "beams for the stokes planes necessary for this "
				    "computation are not equal."
				);
				return False;
			}
		}
	}
	if (
		requireChannelEquality
		&& _stokes[stokes[0]]->coordinates().hasSpectralAxis()
		&& _stokes[stokes[0]]->imageInfo().hasMultipleBeams()
	) {
		const auto& beamSet
		    = _stokes[stokes[0]]->imageInfo().getBeamSet().getBeams();
		auto start = beamSet.cbegin();
		++start;
		for (auto iter=start; iter!=beamSet.cend(); ++iter) {
			if (*iter != *(beamSet.begin())) {
				ThrowIf(
				    throws,
					"At least one beam in this image is not equal to all the "
				    "others along its spectral axis so this computation cannot "
				    "be performed"
				);
				return False;
			}
		}
	}
	return true;
}

Bool ImagePolarimetry::_checkIQUBeams(
    Bool requireChannelEquality, Bool throws
) const {
	static const vector<StokesTypes> types = {I, Q, U};
	return _checkBeams(types, requireChannelEquality, throws);
}

Bool ImagePolarimetry::_checkIVBeams(
    Bool requireChannelEquality, Bool throws
) const {
	static const vector<StokesTypes> types = {I, V};
	return _checkBeams(types, requireChannelEquality, throws);
}

Bool ImagePolarimetry::_checkQUBeams(
    Bool requireChannelEquality, Bool throws
) const {
	static const vector<StokesTypes> types = {Q, U};
	return _checkBeams(types, requireChannelEquality, throws);
}

void ImagePolarimetry::_checkBeams(
	const ImagePolarimetry& im1, const ImagePolarimetry& im2,
	const Vector<ImagePolarimetry::StokesTypes>& stokes
) {
	for (auto iter=stokes.cbegin(); iter!=stokes.cend(); iter++) {
		ThrowIf(
			im1.stokes(*iter).imageInfo().getBeamSet()
			!= im2.stokes(*iter).imageInfo().getBeamSet(),
			"Beams or beamsets are not equal between the two images, caonnot "
			"perform calculation"
		);
	}
}

}
