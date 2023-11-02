#include <imageanalysis/ImageAnalysis/ImageDecimator.h>

#include <imageanalysis/ImageAnalysis/ImageCollapser.h>
#include <imageanalysis/ImageAnalysis/ImageHistory.h>

namespace casa {

template<class T> ImageDecimator<T>::ImageDecimator(
    const SPCIIT image,
    const casacore::Record *const region,
    const casacore::String& maskInp,
    const casacore::String& outname, casacore::Bool overwrite
) : ImageTask<T>(
        image, "", region, "", "", "",
        maskInp, outname, overwrite
    ),
    _axis(0), _factor(1), _function(ImageDecimatorData::COPY) {
    this->_construct();
}

template<class T> void ImageDecimator<T>::setFactor(casacore::uInt n) {
    ThrowIf(n == 0, "The decimation factor must be positive");
    _factor = n;
}

template<class T> void ImageDecimator<T>::setFunction(
    ImageDecimatorData::Function f
) {
    ThrowIf(
        f == ImageDecimatorData::NFUNCS,
        "Setting decimation function to NFUNCS is not allowed"
    );
    _function = f;
}

template<class T> void ImageDecimator<T>::setAxis(casacore::uInt n) {
    casacore::uInt ndim = this->_getImage()->ndim();
    ThrowIf(
        n >= ndim,
        "The axis number along which the decimation "
        "is to occur must be less than the number of "
        "axes in the image which is " + casacore::String::toString(ndim)
    );
    _axis = n;
}

template<class T> SPIIT ImageDecimator<T>::decimate() const {
    ThrowIf(
        _factor > this->_getImage()->shape()[_axis],
        "The value of factor cannot be greater than the "
        "number of pixels along the specified axis"
    );
    casacore::LogOrigin lor(getClass(), __func__);
    *this->_getLog() << lor;
    auto subImage = SubImageFactory<T>::createSubImageRO(
        *this->_getImage(), *this->_getRegion(), this->_getMask(), 0,
        casacore::AxesSpecifier(), this->_getStretch()
    );
    if (_factor == 1) {
        *this->_getLog() << casacore::LogIO::WARN << "A decimation factor "
            << "of 1 has been specified which means no planes will "
            << "be removed. The resulting image will be a straight "
            << "copy of the selected image." << casacore::LogIO::POST;
        return this->_prepareOutputImage(*subImage);
    }
    auto csys = subImage->coordinates();
    auto refPix = csys.referencePixel();
    refPix[_axis] /= _factor;
    csys.setReferencePixel(refPix);
    casacore::Vector<casacore::Double> inc = csys.increment();
    inc[_axis] *= _factor;
    csys.setIncrement(inc);
    auto subShape = subImage->shape();
    auto shape = subShape;
    // integer division
    shape[_axis] = shape[_axis]/_factor;
    if (
        _function == ImageDecimatorData::COPY
        && subShape[_axis] % _factor != 0
    ) {
        shape[_axis]++;
    }
    casacore::TempImage<T> out(shape, csys);
    auto cursorShape = shape;
    cursorShape[_axis] = _factor;
    casacore::LatticeStepper stepper(subShape, cursorShape);
    casacore::RO_MaskedLatticeIterator<T> inIter(*subImage, stepper);
    inIter.reset();
    auto isMasked = subImage->isMasked();
    auto ndim = subImage->ndim();
    casacore::IPosition begin(ndim, 0);
    auto end = cursorShape - 1;
    std::shared_ptr<casacore::ArrayLattice<casacore::Bool>> outMask(
        isMasked ? new casacore::ArrayLattice<casacore::Bool>(out.shape()) : nullptr
    );
    auto outPos = begin;
    if (_function == ImageDecimatorData::COPY) {
        end[_axis] = 0;
        while(! inIter.atEnd() && outPos[_axis]<shape[_axis]) {
            if (isMasked) {
                const casacore::Array<casacore::Bool> mask = inIter.getMask();
                casacore::Array<casacore::Bool> maskSlice = mask(begin, end);
                outMask->putSlice(maskSlice, outPos);
            }
            const casacore::Array<T> data = inIter.cursor();
            casacore::Array<T> dataSlice = data(begin, end);
            out.putSlice(dataSlice, outPos);
            inIter++;
            outPos[_axis]++;
        }
    }
    else if (_function == ImageDecimatorData::MEAN) {
        casacore::String comment;
        ImageCollapser<T> collapser(
            subImage, casacore::IPosition(1, _axis), false,
            ImageCollapserData::MEAN, "", false
        );
        std::unique_ptr<casacore::Record> reg;
        casacore::Vector<casacore::Double> start(ndim);
        casacore::Vector<casacore::Int> lattShape(ndim);
        casacore::Vector<casacore::Double> stop(ndim);
        for (casacore::uInt i=0; i<ndim; i++) {
            start[i] = begin[i];
            stop[i] = begin[i] + cursorShape[i] - 1;
            lattShape[i] = subShape[i];
        }
        SPIIT collapsed;
        while(! inIter.atEnd() && (outPos[_axis] < shape[_axis])) {
            reg.reset(
                casacore::RegionManager::box(
                    start, stop, lattShape, comment
                )
            );
            collapser.setRegion(*reg);
            collapsed = collapser.collapse();
            if (isMasked) {
                if (collapsed->isMasked()) {
                    casacore::Array<casacore::Bool> mask = collapsed->pixelMask().get();
                    outMask->putSlice(mask, outPos);
                }
                else {
                    casacore::Array<casacore::Bool> mask(collapsed->shape(), true);
                    outMask->putSlice(mask, outPos);
                }
            }
            const auto data = collapsed->get();
            out.putSlice(data, outPos);
            inIter++;
            outPos[_axis]++;
            start[_axis] += _factor;
            stop[_axis] += _factor;
        }
    }
    if (! this->_getSuppressHistory()) {
        ostringstream os;
        os << "Decimated axis " << _axis << " by keeping only every nth plane, "
            << "where n=" << _factor << ". ";
        if (_function == ImageDecimatorData::COPY) {
            os << "Directly copying every i*nth plane "
                << "in input to plane i in output.";
        }
        else if (_function == ImageDecimatorData::MEAN) {
            os << "Averaging every i to i*(n-1) planes in the input "
                << "image to form plane i in the output image.";
        }
        this->addHistory(lor, os.str());
        this->_reportOldNewImageShapes(out);
    }
    // FIXME decimating multiple beams not yet supported
    casacore::ImageUtilities::copyMiscellaneous(
        out, *subImage, ! subImage->imageInfo().hasMultipleBeams()
    );
    return this->_prepareOutputImage(out, nullptr, outMask.get());
}

}
