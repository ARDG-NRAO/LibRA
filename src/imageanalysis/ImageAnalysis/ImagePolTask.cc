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

#include <imageanalysis/ImageAnalysis/ImagePolTask.h>

#include <casacore/lattices/LRegions/LCSlicer.h>

using namespace casacore;
namespace casa {

const String ImagePolTask::CLASS_NAME = "ImagePolTask";

ImagePolTask::ImagePolTask(
    const SPCIIF image, const String& outname, Bool overwrite
) : ImageTask<Float>(image, nullptr, "", outname, overwrite) {
    _findStokes();
    _createBeamsEqMat();
}

ImagePolTask::~ImagePolTask() {}

String ImagePolTask::getClass() const {
    return CLASS_NAME;
}

Float ImagePolTask::sigmaLinPolInt(Float clip, Float sigma) {
    *_getLog() <<LogOrigin(CLASS_NAME, __func__, WHERE);
    ThrowIf(
        ! _stokesImage[Q] && ! _stokesImage[U]==0,
        "This image does not have Stokes Q and U so cannot provide linear polarization"
    );
    _checkQUBeams(False);
    Float sigma2 = 0.0;
    if (sigma > 0) {
        sigma2 = sigma;
    }
    else {
       Float sq = _sigma(Q, clip);
       Float su = _sigma(U, clip);
       sigma2 = (sq+su)/2.0;
       *_getLog() << LogIO::NORMAL << "Determined noise from Q&U images to be "
           << sigma2 << LogIO::POST;
   }
   return sigma2;
}

Bool ImagePolTask::_checkBeams(
    const std::vector<StokesTypes>& stokes,
    Bool requireChannelEquality, Bool throws
) const {
    for (const auto i: stokes) {
        for (const auto j: stokes) {
            if (i == j) {
                continue;
            }
            if (! _beamsEqMat(i, j)) {
                ThrowIf(
                    throws,
                    "Input image has multiple beams and the corresponding "
                    "beams for the stokes planes necessary for this computation "
                    "are not equal."
                );
                return False;
            }
        }
    }
    auto image0 = _stokesImage[stokes[0]];
    if (
        requireChannelEquality
        && image0->coordinates().hasSpectralAxis()
        && image0->imageInfo().hasMultipleBeams()
    ) {
        const auto& beamSet = image0->imageInfo().getBeamSet().getBeams();
        auto firstBeam = *(beamSet.begin());
        for (const auto& beam: beamSet) {
            if (beam != firstBeam) {
                ThrowIf(
                    throws, "At least one beam in this image is not equal "
                    "to all the others along its spectral axis so this "
                    "computation cannot be performed"
                );
                return False;
            }
        }
    }
    return True;
}

void ImagePolTask::_createBeamsEqMat() {
    const auto hasMultiBeams = _getImage()->imageInfo().hasMultipleBeams();
    for (uInt i=0; i<4; ++i) {
        for (uInt j=i; j<4; ++j) {
            if (! _stokesImage[i] || ! _stokesImage[j]) {
                _beamsEqMat(i, j) = False;
            }
            else if (i == j) {
                _beamsEqMat(i, j) = True;
            }
            else if (hasMultiBeams) {
                _beamsEqMat(i, j) = (
                    _stokesImage[i]->imageInfo().getBeamSet()
                    == _stokesImage[j]->imageInfo().getBeamSet()
                );
            }
            else {
                _beamsEqMat(i, j) = True;
            }
            _beamsEqMat(j, i) = _beamsEqMat(i, j);
        }
    }
}

casacore::Bool ImagePolTask::_checkIQUBeams(
    Bool requireChannelEquality, Bool throws
) const {
    std::vector<StokesTypes> types { I, Q, U };
    return _checkBeams(types, requireChannelEquality, throws);
}

Bool ImagePolTask::_checkIVBeams(
    Bool requireChannelEquality, Bool throws
) const {
    std::vector<StokesTypes> types {I, V};
    return _checkBeams(types, requireChannelEquality, throws);
}

Bool ImagePolTask::_checkQUBeams(
    Bool requireChannelEquality, Bool throws
) const {
    std::vector<StokesTypes> types {Q, U};
    return _checkBeams(types, requireChannelEquality, throws);
}

void ImagePolTask::_fiddleStokesCoordinate(
    ImageInterface<Float>& im, Stokes::StokesTypes type
) const {
   CoordinateSystem cSys = im.coordinates();
   _fiddleStokesCoordinate(cSys, type);
   im.setCoordinateInfo(cSys);
}

void ImagePolTask::_fiddleStokesCoordinate(
    CoordinateSystem& cSys, Stokes::StokesTypes type
) const {
    Int afterCoord = -1;
    Int iStokes = cSys.findCoordinate(Coordinate::STOKES, afterCoord);
    Vector<Int> which(1);
    which(0) = Int(type);
    StokesCoordinate stokes(which);
    cSys.replaceCoordinate(stokes, iStokes);
}


void ImagePolTask::_findStokes() {
    *_getLog() << LogOrigin(CLASS_NAME, __func__, WHERE);
    // Do we have any Stokes ?
    const auto& csys = _getImage()->coordinates();
    ThrowIf(
        ! csys.hasPolarizationCoordinate(),
        "There is no Stokes Coordinate in this image"
    );
    // Find the pixel axis of the image which is Stokes
    auto stokesAxis = csys.polarizationAxisNumber();
    // Make the regions
    const auto& stokes = csys.stokesCoordinate();
    const auto ndim = _getImage()->ndim();
    const auto shape = _getImage()->shape();
    IPosition blc(ndim, 0);
    auto trc = shape - 1;
    Int pix;
    if (stokes.toPixel(pix, Stokes::I)) {
        _stokesImage[I] = _makeSubImage(blc, trc, stokesAxis, pix);
    }
    if (stokes.toPixel(pix, Stokes::Q)) {
        _stokesImage[Q] = _makeSubImage(blc, trc, stokesAxis, pix);
    }
    if (stokes.toPixel(pix, Stokes::U)) {
        _stokesImage[U] = _makeSubImage(blc, trc, stokesAxis, pix);
    }
    if (stokes.toPixel(pix, Stokes::V)) {
        _stokesImage[V] = _makeSubImage(blc, trc, stokesAxis, pix);
    }
    ThrowIf (
        (_stokesImage[Q] && ! _stokesImage[U])
        || (! _stokesImage[Q] && _stokesImage[U]),

        "This Stokes coordinate has only one of Q and U. This is not useful"
    );
    ThrowIf(
        ! _stokesImage[Q] && ! _stokesImage[U] && ! _stokesImage[V],
        "This image has no Stokes Q, U, or V.  This is not useful"
    );
}

SPCIIF ImagePolTask::_getStokesImage(StokesTypes type) const {
    return _stokesImage[type];
}

LatticeExprNode ImagePolTask::_makePolIntNode(
    Bool debias, Float clip, Float sigma,
    Bool doLin, Bool doCirc
) {
    LatticeExprNode linNode, circNode;
    if (doLin) {
        linNode = LatticeExprNode(
            pow(*_stokesImage[U],2) + pow(*_stokesImage[Q],2)
        );
    }
    if (doCirc) {
        circNode = LatticeExprNode(pow(*_stokesImage[V],2));
    }
    auto node = (doLin && doCirc) ? linNode + circNode
        : doLin ? linNode : circNode;
    if (debias) {
        auto sigma2 = sigma > 0 ? sigma : _sigma(clip);
        node = node - LatticeExprNode(sigma2*sigma2);
        // node = iif(node >= 0, node, 0);
        *_getLog() << LogIO::NORMAL << "Debiasing with sigma = "
            << sigma2 << LogIO::POST;
    }
    return sqrt(node);
}

SPIIF ImagePolTask::_makeSubImage(
    IPosition& blc, IPosition& trc, Int axis, Int pix
) const {
    blc(axis) = pix;
    trc(axis) = pix;
    LCSlicer slicer(blc, trc, RegionType::Abs);
    ImageRegion region(slicer);
    return SPIIF(new SubImage<Float>(*_getImage(), region));
}

void ImagePolTask::_maskAndZeroNaNs(SPIIF out) {
    auto isnan = isNaN(*out);
    if (any(isnan).getBool()) {
        LatticeExpr<Bool> mask(! isnan);
        if (! out->hasPixelMask()) {
            String x;
            ImageMaskAttacher::makeMask(*out, x, True, True, *_getLog(), False);
        }
        auto& pixelMask = out->pixelMask();
        LatticeExpr<Bool> newMask(mask && LatticeExpr<Bool>(out->pixelMask()));
        pixelMask.copyData(newMask);
        out->copyData(LatticeExpr<Float>(iif(isnan, 0, *out)));
    }
}

void ImagePolTask::_setDoLinDoCirc(Bool& doLin, Bool& doCirc, Bool requireI) const {
    *_getLog() << LogOrigin(CLASS_NAME, __func__, WHERE);
    doLin = _stokesImage[Q] && _stokesImage[U];
    doCirc = Bool(_stokesImage[V]);
    AlwaysAssert((doLin||doCirc), AipsError);    // Should never happen
    if (requireI && ! _stokesImage[I]) {
        *_getLog() << "This image does not have Stokes I so this calculation cannot be carried out"
            << LogIO::EXCEPTION;
    }
    if (doLin) {
        if (_stokesImage[I]) {
            if (! _checkIQUBeams(False, False)) {
                *_getLog() << LogIO::WARN
                    << "I, Q, and U beams are not the same, cannot do linear portion"
                    << LogIO::POST;
                doLin = False;
            }
        }
        else {
            if (! _checkQUBeams(False, False)) {
                *_getLog() << LogIO::WARN
                    << "Q, and U beams are not the same, cannot do linear portion"
                    << LogIO::POST;
                doLin = False;
            }
        }
    }
    if (doCirc) {
        if (_stokesImage[I] && ! _checkIVBeams(False, False)) {
            *_getLog() << LogIO::WARN
                << "I and V beams are not the same, cannot do circular portion"
                << LogIO::POST;
            doCirc = False;
        }
    }
    if (! doLin && ! doCirc) {
        throw AipsError("Can do neither linear nor circular portions");
    }
}

void ImagePolTask::_setInfo(ImageInterface<Float>& im, const StokesTypes stokes) const {
    ImageInfo info = _getImage()->imageInfo();
    if (info.hasMultipleBeams()) {
        info.setBeams(_stokesImage[stokes]->imageInfo().getBeamSet());
    }
    im.setImageInfo(info);
}

Float ImagePolTask::_sigma(Float clip) {
    *_getLog() << LogOrigin(CLASS_NAME, __func__, WHERE);
    Float sigma2 = 0.0;
    if (_stokesImage[V]) {
        *_getLog() << LogIO::NORMAL << "Determined noise from V image to be ";
        sigma2 = _sigma(V, clip);
    }
    else if (
        _stokesImage[Q] && _stokesImage[U] && _checkQUBeams(False, False)
    ) {
        sigma2 = sigmaLinPolInt(clip);
    }
    else if (_stokesImage[Q]) {
        *_getLog() << LogIO::NORMAL << "Determined noise from Q image to be " << LogIO::POST;
        sigma2 = _sigma(Q, clip);
    }
    else if (_stokesImage[U]) {
         *_getLog() << LogIO::NORMAL << "Determined noise from U image to be " << LogIO::POST;
         sigma2 = _sigma(U, clip);
    }
    else if (_stokesImage[I]) {
        *_getLog() << LogIO::NORMAL << "Determined noise from I image to be " << LogIO::POST;
        sigma2 = _sigma(I, clip);
    }
    *_getLog() << sigma2 << LogIO::POST;
    return sigma2;
}

Float ImagePolTask::_sigma (StokesTypes index, Float clip) {
    auto clip2 = abs(clip);
    if (clip2==0.0) {
        clip2 = 10.0;
    }
    if (clip2 != _oldClip) {
        _stokesStats[index].reset();
    }
    if (! _stokesStats[index]) {
        // Find sigma for all points inside +/- clip-sigma of the mean
        // More joys of LEL
        const auto p = _stokesImage[index];
        LatticeExprNode n1 (*p);
        LatticeExprNode n2 (n1[abs(n1-mean(n1)) < clip2*stddev(n1)]);
        LatticeExpr<Float> le(n2);
        _stokesStats[index].reset(new LatticeStatistics<Float>(le, false, false));
    }
    Array<Float> sigmaA;
    _stokesStats[index]->getConvertedStatistic(sigmaA, LatticeStatsBase::SIGMA);
    ThrowIf(
        sigmaA.empty(), "No good points in clipped determination of the noise "
        "for the Stokes " + _stokesName(index) + " image"
    );
    _oldClip = clip2;
    return sigmaA(IPosition(1,0));
}

String ImagePolTask::_stokesName (StokesTypes index) const {
    switch(index) {
    case I: return "I";
    case Q: return "Q";
    case U: return "U";
    case V: return "V";
    default:
        ThrowCc("Unsupported stokes index " + String::toString(index));
    }
}

}

