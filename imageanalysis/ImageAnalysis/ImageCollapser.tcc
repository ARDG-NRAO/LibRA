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

#include <imageanalysis/ImageAnalysis/ImageCollapser.h>

#include <casa/Arrays/ArrayLogical.h>
#include <casa/BasicSL/STLIO.h>
#include <casacore/scimath/StatsFramework/ClassicalStatistics.h>
#include <casacore/scimath/Mathematics/NumericTraits.h>
#include <images/Images/ImageStatistics.h>
#include <images/Images/ImageUtilities.h>
#include <images/Images/PagedImage.h>
#include <imageanalysis/ImageAnalysis/SubImageFactory.h>
#include <images/Images/TempImage.h>
#include <lattices/Lattices/LatticeUtilities.h>
#include <lattices/LatticeMath/LatticeMathUtil.h>

#include <imageanalysis/ImageAnalysis/ImageMaskedPixelReplacer.h>

#include <memory>

namespace casa {

template<class T> ImageCollapser<T>::ImageCollapser(
    const casacore::String & aggString, const SPCIIT image,
    const casacore::Record * const regionRec,
    const casacore::String & maskInp, const casacore::IPosition & axes,
    casacore::Bool invertAxesSelection,
    const casacore::String & outname, casacore::Bool overwrite
) : ImageTask<T>(
        image, "", regionRec, "", "", "",
        maskInp, outname, overwrite
    ), _invertAxesSelection(invertAxesSelection),
    _axes(axes), _aggType(ImageCollapserData::UNKNOWN) {
    _aggType = ImageCollapserData::aggregateType(aggString);
    this->_construct();
    _finishConstruction();
}

template<class T> ImageCollapser<T>::ImageCollapser(
    const SPCIIT image,
    const casacore::IPosition & axes, const casacore::Bool invertAxesSelection,
    const ImageCollapserData::AggregateType aggregateType,
    const casacore::String & outname, const casacore::Bool overwrite
) : ImageTask<T>(image, "", 0, "", "", "", "", outname, overwrite),
    _invertAxesSelection(invertAxesSelection),
    _axes(axes), _aggType(aggregateType) {
    ThrowIf (
        _aggType == ImageCollapserData::UNKNOWN,
        "UNKNOWN aggregateType not allowed"
    );
    ThrowIf(
        ! image,
        "Cannot use a null image pointer with this constructor"
    );
    this->_construct();
    _finishConstruction();
}

template<class T> SPIIT ImageCollapser<T>::collapse() const {
    auto subImage = SubImageFactory<T>::createSubImageRO(
        *this->_getImage(), *this->_getRegion(), this->_getMask(),
        this->_getLog().get(), AxesSpecifier(), this->_getStretch()
    );
    *this->_getLog() << LogOrigin(getClass(), __func__);
    ThrowIf(
        ImageMask::isAllMaskFalse(*subImage),
        "All selected pixels are masked"
    );
    auto outCoords = subImage->coordinates();
    auto hasDir = outCoords.hasDirectionCoordinate();
    auto inShape = subImage->shape();
    if (_aggType == ImageCollapserData::FLUX) {
        _checkFlux(subImage);
    }
    // Set the compressed axis reference pixel and reference value
    Vector<Double> blc, trc;
    IPosition pixblc(inShape.nelements(), 0);
    auto pixtrc = inShape - 1;
    ThrowIf(
        ! outCoords.toWorld(blc, pixblc)
        || ! outCoords.toWorld(trc, pixtrc),
        "Could not set new coordinate values"
    );
    auto refValues = outCoords.referenceValue();
    auto refPixels = outCoords.referencePixel();
    auto outShape = inShape;
    auto degenTypes = ImageCollapserData::aggTypesSupportedDegenAxes();
    auto useDegenCase = degenTypes->find(_aggType) != degenTypes->end();
    for (const auto& axis: _axes) {
        refValues[axis] = (blc[axis] + trc[axis])/2;
        refPixels[axis] = 0;
        outShape[axis] = 1;
        useDegenCase = useDegenCase && inShape[axis] == 1;
    }
    ThrowIf(
        ! outCoords.setReferenceValue(refValues),
        "Unable to set reference value"
    );
    ThrowIf(
        ! outCoords.setReferencePixel(refPixels),
        "Unable to set reference pixel"
    );
    TempImage<T> tmpIm(outShape, outCoords);
    if (_aggType == ImageCollapserData::ZERO) {
        tmpIm.set(0.0);
    }
    else if (useDegenCase) {
        _doDegenerateAxesCase(tmpIm, subImage);
    }
    else if (
        _aggType == ImageCollapserData::MEDIAN
        || _aggType == ImageCollapserData::MADM
        || _aggType == ImageCollapserData::XMADM       
    ) {
        _doHighPerf(subImage, tmpIm);
    }
    else {
        _doOtherStats(tmpIm, subImage);
    }
    auto copied = subImage->imageInfo().hasMultipleBeams()
        ? _doMultipleBeams(tmpIm, subImage, hasDir, outCoords)
        : false;
    if (! copied) {
        ImageUtilities::copyMiscellaneous(tmpIm, *subImage, true);
    }
    if (_aggType == ImageCollapserData::FLUX) {
        _doFluxUnits(tmpIm, subImage);
    }
    return this->_prepareOutputImage(tmpIm);
}

template<class T> void ImageCollapser<T>::_checkFlux(
    SPCIIT subImage
) const {
    String cant = " Cannot do flux density calculation";
    const auto& outCoords = subImage->coordinates();
    ThrowIf(
        ! outCoords.hasDirectionCoordinate(),
        "Image has no direction coordinate." + cant
    );
    ThrowIf(
        subImage->units().getName().contains("beam")
        && ! subImage->imageInfo().hasBeam(),
        "Image has no beam." + cant
    );
    auto dirAxes = outCoords.directionAxesNumbers();
    const auto naxes = _axes.size();
    for (uInt i = 0; i < naxes; ++i) {
        Int axis = _axes[i];
        ThrowIf(
            ! anyTrue(dirAxes == axis)
            && subImage->shape()[axis] > 1,
            "Specified axis " + String::toString(axis)
            + " is not a direction axis but has length > 1." + cant
        );
    }
}

template<class T> void ImageCollapser<T>::_doDegenerateAxesCase(
    TempImage<T>& tmpIm, SPCIIT subImage
) const {
    *this->_getLog() << LogOrigin(getClass(), __func__);
    *this->_getLog() << LogIO::NORMAL << "All subimage axes to be "
        << "collapsed are degenerate, using algorithm optimized for "
        << "that case." << LogIO::POST;
    ThrowIf(
        _aggType == ImageCollapserData::STDDEV
        || _aggType == ImageCollapserData::VARIANCE,
        "Cannot compute "
        + ImageCollapserData::funcNameMap()->find(_aggType)->second
        + " for single sample data sets"
    );
    if (
        _aggType == ImageCollapserData::MAX
        || _aggType == ImageCollapserData::MEAN
        || _aggType == ImageCollapserData::MEDIAN
        || _aggType == ImageCollapserData::MIN
        || _aggType == ImageCollapserData::SUM
    ) {
        // Straight copy
        this->_copyData(tmpIm, *subImage);
    }
    else if (_aggType == ImageCollapserData::NPTS) {
        tmpIm.set(1.0);
    }
    else if (
        _aggType == ImageCollapserData::MADM
        || _aggType == ImageCollapserData::XMADM
    ) {
        tmpIm.set(0.0);
    }
    else if (_aggType == ImageCollapserData::RMS) {
        this->_copyData(tmpIm, LatticeExpr<T>(abs(*subImage)));
    }
    else {
        ThrowCc(
            "Logic error: "
            + ImageCollapserData::funcNameMap()->find(_aggType)->second
            + " erroneously not supported for degenerate axis case. Please "
            + "file a bug report and include this message"
        );
    }
    if (subImage->isMasked() && ! ImageMask::isAllMaskTrue(*subImage)) {
        if (! tmpIm.isMasked()) {
            TempLattice<Bool> mask(tmpIm.shape());
            this->_copyMask(mask, *subImage);
            tmpIm.attachMask(mask);
        }
        // This works because the underlying pixel data are cloned by reference
        SPIIT myclone(tmpIm.cloneII());
        ImageMaskedPixelReplacer<T> impr(myclone);
        impr.replace("0", False, False);
    }
}

template<class T> void ImageCollapser<T>::_doFluxUnits(
    TempImage<T>& tmpIm, const std::shared_ptr<const SubImage<T>> subImage
) const {
    // get the flux units right
     auto sbunit = subImage->units().getName();
     String unit;
     if (sbunit.contains("K")) {
         casacore::String areaUnit = "arcsec2";
         unit = sbunit + "." + areaUnit;
     }
     else {
         unit = "Jy";
         if (sbunit.contains("/beam")) {
             uInt iBeam = sbunit.find("/beam");
             unit = sbunit.substr(0, iBeam) + sbunit.substr(iBeam + 5);
         }
     }
     tmpIm.setUnits(unit);
}

template<class T> void ImageCollapser<T>::_doHighPerf(
    SPCIIT image, casacore::TempImage<T>& outImage
) const {
    auto doMedian = _aggType == ImageCollapserData::MEDIAN;
    auto doMADM = _aggType == ImageCollapserData::MADM
        || _aggType == ImageCollapserData::XMADM;
    ThrowIf(
        ! doMedian && ! doMADM,
        "Logic error, unsupported aggregate type "
        + String(ImageCollapserData::funcNameMap()->at((uInt)_aggType)) + " for method "
        + String(__func__)
    );
    IPosition cursorShape(image->ndim(), 1);
    for (uInt i = 0; i < cursorShape.size(); ++i) {
        for (uInt j = 0; j < _axes.size(); ++j) {
            if (_axes[j] == i) {
                cursorShape[i] = image->shape()[i];
                break;
            }
        }
    }
    LatticeStepper stepper(image->shape(), cursorShape);
    std::unique_ptr<Array<Bool>> outMask;
    // accumtype being the same precision as the input data type is ok here,
    // since we are only computing the median/madm and not actually accumulating
    ClassicalStatistics<
        T, typename Array<T>::const_iterator, Array<Bool>::const_iterator
    > stats;
    auto hasMaskedPixels = ! ImageMask::isAllMaskTrue(*image);
    for (stepper.reset(); !stepper.atEnd(); stepper++) {
        Slicer slicer(
            stepper.position(), stepper.endPosition(), casacore::Slicer::endIsLast
        );
        auto data = image->getSlice(slicer);
        Bool isMasked = False;
        Array<Bool> maskSlice;
        if (hasMaskedPixels) {
            maskSlice = image->getMaskSlice(slicer);
            isMasked = ! allTrue(maskSlice);
        }
        if (isMasked) {
            if (! anyTrue(maskSlice)) {
                if (! outMask) {
                    outMask.reset(new Array<Bool>(outImage.shape(), true));
                }
                (*outMask)(stepper.position()) = false;
                outImage.putAt(0, stepper.position());
            }
            else if (! allTrue(maskSlice)) {
                stats.setData(data.begin(), maskSlice.begin(), data.size());
                if (doMedian) {
                    outImage.putAt(stats.getMedian(), stepper.position());
                }
                else if (doMADM) {
                    auto x = stats.getMedianAbsDevMed();
                    if (_aggType == ImageCollapserData::XMADM) {
                        x *= C::probit_3_4;
                    }
                    outImage.putAt(x, stepper.position());
                }
            }
        }
        else {
            stats.setData(data.begin(), data.size());
            if (doMedian) {
                outImage.putAt(stats.getMedian(), stepper.position());
            }
            else if (doMADM) {
                auto x = stats.getMedianAbsDevMed();
                if (_aggType == ImageCollapserData::XMADM) {
                    x *= C::probit_3_4;
                }
                outImage.putAt(x, stepper.position());
            }
        }
    }
    if (outMask) {
        outImage.attachMask(ArrayLattice<Bool>(*outMask));
    }
}

template<class T> Bool ImageCollapser<T>::_doMultipleBeams(
    TempImage<T>& tmpIm, SPCIIT subImage, Bool hasDir,
    const CoordinateSystem & outCoords
) const {
    auto naxes = _axes.size();
    auto dirAxesOnlyCollapse = hasDir && naxes == 2;
    if (dirAxesOnlyCollapse) {
        auto dirAxes = outCoords.directionAxesNumbers();
        dirAxesOnlyCollapse = (_axes[0] == dirAxes[0] && _axes[1] == dirAxes[1])
                              || (_axes[1] == dirAxes[0] && _axes[0] == dirAxes[1]);
    }
    if (! dirAxesOnlyCollapse) {
        // check for degeneracy of spectral or polarization axes
        auto specAxis = outCoords.spectralAxisNumber(false);
        auto polAxis = outCoords.polarizationAxisNumber(false);
        dirAxesOnlyCollapse = true;
        auto shape = subImage->shape();
        for (uInt i = 0; i < naxes; ++i) {
            auto axis = _axes[i];
            if (
                (axis == specAxis || axis == polAxis)
                && shape[axis] > 1
            ) {
                dirAxesOnlyCollapse = false;
                break;
            }
        }
    }
    if (! dirAxesOnlyCollapse) {
        LogOrigin lor(getClass(), __func__);
        String msg = "Input image has per plane beams "
            "but the collapse is not done exclusively along the direction axes. "
            "The output image will arbitrarily have a single beam which "
            "is the first beam available in the subimage."
            "Thus, the image planes will not be convolved to a common "
            "restoring beam before collapsing. If, however, this is desired, "
            "then run the task imsmooth or the tool method ia.convolve2d() first, "
            "and use the output image of that as the input for collapsing.";
        *this->_getLog() << lor << LogIO::WARN << msg << LogIO::POST;
        this->addHistory(lor, msg);
        ImageUtilities::copyMiscellaneous(tmpIm, *subImage, false);
        auto info = subImage->imageInfo();
        auto beam = *(info.getBeamSet().getBeams().begin());
        info.removeRestoringBeam();
        info.setRestoringBeam(beam);
        tmpIm.setImageInfo(info);
        return true;
    }
    return false;
}

template<class T> void ImageCollapser<T>::_doOtherStats(
    TempImage<T>& tmpIm, SPCIIT subImage
) const {
    T npixPerBeam = 1;
    if (_aggType == ImageCollapserData::SQRTSUM_NPIX_BEAM) {
        const auto& info = subImage->imageInfo();
        if (! info.hasBeam()) {
            *this->_getLog() << casacore::LogIO::WARN
                << "Image has no beam, will use sqrtsum method"
                << casacore::LogIO::POST;
        }
        else if (info.hasMultipleBeams()) {
            *this->_getLog() << casacore::LogIO::WARN
                << "Function sqrtsum_npix_beam does not support multiple beams, will"
                << "use sqrtsum method instead"
                << casacore::LogIO::POST;
        }
        else {
            npixPerBeam = info.getBeamAreaInPixels(
                -1, -1, subImage->coordinates().directionCoordinate()
            );
        }
    }
    _doLowPerf(tmpIm, subImage, npixPerBeam);
}

template<class T> void ImageCollapser<T>::_doLowPerf(
    TempImage<T>& tmpIm, SPCIIT subImage, T npixPerBeam
) const {
    // flux or mask with one or more false values, must use lower performance methods
    auto lattStatType = _getStatsType();
    Array<T> data;
    Array<Bool> mask;
    if (_aggType == ImageCollapserData::FLUX) {
        ImageStatistics<T> stats(*subImage, false);
        stats.setAxes(_axes.asVector());
        if (
            ! stats.getConvertedStatistic(data, lattStatType, false)
        ) {
            ostringstream oss;
            oss << "Unable to calculate flux density: "
                << stats.getMessages();
            ThrowCc(oss.str());
        }
        mask.resize(data.shape());
        mask.set(true);
    }
    else {
        LatticeMathUtil::collapse(
            data, mask, _axes, *subImage, false,
            true, true, lattStatType
        );
        if (
            _aggType == ImageCollapserData::SQRTSUM
            || _aggType == ImageCollapserData::SQRTSUM_NPIX
            || _aggType == ImageCollapserData::SQRTSUM_NPIX_BEAM
        ) {
            _zeroNegatives(data);
            data = sqrt(data);
            if (_aggType == ImageCollapserData::SQRTSUM_NPIX) {
                auto npts = data.copy();
                LatticeMathUtil::collapse(
                    npts, mask, _axes, *subImage, false,
                    true, true, LatticeStatsBase::NPTS
                );
                data /= npts;
            }
            else if (_aggType == ImageCollapserData::SQRTSUM_NPIX_BEAM) {
                data /= npixPerBeam;
            }
        }
    }
    auto dataCopy = (_axes.size() <= 1)
        ? data : data.addDegenerate(_axes.size() - 1);
    IPosition newOrder(tmpIm.ndim(), -1);
    auto nAltered = _axes.size();
    auto nUnaltered = tmpIm.ndim() - nAltered;
    auto alteredCount = nUnaltered;
    auto unAlteredCount = 0;
    const auto ndim = tmpIm.ndim();
    const auto naxes = nAltered;
    for (uInt i = 0; i < ndim; ++i) {
        for (uInt j = 0; j < naxes; ++j) {
            if (i == _axes[j]) {
                newOrder[i] = alteredCount;
                alteredCount++;
                break;
            }
        }
        if (newOrder[i] < 0) {
            newOrder[i] = unAlteredCount;
            ++unAlteredCount;
        }
    }
    tmpIm.put(reorderArray(dataCopy, newOrder));
    if (! allTrue(mask)) {
        auto maskCopy = (
            _axes.size() <= 1) ? mask
                : mask.addDegenerate(_axes.size() - 1
        );
        auto mCopy = reorderArray(maskCopy, newOrder);
        tmpIm.attachMask(ArrayLattice<Bool>(mCopy));
    }
}

template<class T> LatticeStatsBase::StatisticsTypes ImageCollapser<T>::_getStatsType() const {
    auto lattStatType = LatticeStatsBase::NACCUM;
    switch (_aggType) {
    case ImageCollapserData::FLUX:
        lattStatType = LatticeStatsBase::FLUX;
        break;
    case ImageCollapserData::MAX:
        lattStatType = LatticeStatsBase::MAX;
        break;
    case ImageCollapserData::MEAN:
        lattStatType = LatticeStatsBase::MEAN;
        break;
    case ImageCollapserData::MIN:
        lattStatType = LatticeStatsBase::MIN;
        break;
    case ImageCollapserData::NPTS:
        lattStatType = LatticeStatsBase::NPTS;
        break;
    case ImageCollapserData::RMS:
        lattStatType = LatticeStatsBase::RMS;
        break;
    case ImageCollapserData::STDDEV:
        lattStatType = LatticeStatsBase::SIGMA;
        break;
    case ImageCollapserData::SQRTSUM:
    case ImageCollapserData::SQRTSUM_NPIX:
    case ImageCollapserData::SQRTSUM_NPIX_BEAM:
    case ImageCollapserData::SUM:
        lattStatType = LatticeStatsBase::SUM;
        break;
    case ImageCollapserData::VARIANCE:
        lattStatType = LatticeStatsBase::VARIANCE;
        break;
    case ImageCollapserData::MEDIAN:
    case ImageCollapserData::ZERO:
    case ImageCollapserData::UNKNOWN:
    default:
        ThrowCc(
            "Logic error. Should never have gotten the the bottom of the switch statement"
        );
        break;
    }
    return lattStatType;
}

template<class T> void ImageCollapser<T>::_zeroNegatives(Array<T>& arr) {
    auto iter = arr.begin();
    if (isComplex(whatType(&(*iter))) || allGE(arr, (T)0)) {
        return;
    }
    auto end = arr.end();
    for (; iter != end; ++iter) {
        if (*iter < 0) {
            *iter = 0;
        }
    }
}

template<class T> void ImageCollapser<T>::_finishConstruction() {
    for (
        casacore::IPosition::const_iterator iter = _axes.begin();
        iter != _axes.end(); iter++
    ) {
        ThrowIf(
            *iter >= this->_getImage()->ndim(),
            "Specified zero-based axis (" + casacore::String::toString(*iter)
            + ") must be less than the number of axes in " + this->_getImage()->name()
            + "(" + casacore::String::toString(this->_getImage()->ndim()) + ")"
        );
    }
    _invert();
}

template<class T> void ImageCollapser<T>::_invert() {
    if (_invertAxesSelection) {
        casacore::IPosition x = casacore::IPosition::otherAxes(this->_getImage()->ndim(), _axes);
        _axes.resize(x.size());
        _axes = x;
    }
}

}
