#include <imageanalysis/ImageAnalysis/ImageBoxcarSmoother.h>

namespace casa {

template<class T> ImageBoxcarSmoother<T>::ImageBoxcarSmoother(
    const SPCIIT image,
    const casacore::Record *const region,
    const casacore::String& mask,
    const casacore::String& outname, casacore::Bool overwrite
) : Image1DSmoother<T>(
        image, region, mask, outname, overwrite
    ), _width(2) {
    this->_construct();
    this->_setNMinPixels(_width);
}

template<class T> SPIIT ImageBoxcarSmoother<T>::_smooth(
    const casacore::ImageInterface<T>& image
) const {
    auto shape = image.shape();
    auto axis = this->_getAxis();
    ThrowIf(
        this->_getDecimate()
        && this->_getDecimationFunction() == ImageDecimatorData::MEAN
        && shape[axis] < 2*_width,
        "The convolution axis of selected image region must have length "
        "of at least 2*width when using the mean decimation function"
    );
    auto inTileShape = image.niceCursorShape();
    casacore::TiledLineStepper inNav(image.shape(), inTileShape, axis);
    casacore::RO_MaskedLatticeIterator<T> inIter(image, inNav);
    casacore::IPosition sliceShape(image.ndim(), 1);
    sliceShape[axis] = shape[axis];
    casacore::String empty;
    casacore::Record emptyRecord;
    casacore::IPosition blc(image.ndim(), 0);
    auto trc = shape - 1;
    // integer division
    trc[axis] = this->_getDecimate()
        && this->_getDecimationFunction() == ImageDecimatorData::COPY
        ? shape[axis]/_width*_width - _width
        : shape[axis] - _width;
    casacore::LCBox lcbox(blc, trc, shape);
    auto region = lcbox.toRecord("");
    SPIIT out(
        SubImageFactory<T>::createImage(
            image, empty, region, empty,
            false, false, false, false
        )
    );
    // fix up smoothed axis metadata
    auto csys = out->coordinates();
    auto shift = csys.increment()[axis]*(_width - 1)/2.0;
    auto rval = csys.referenceValue();
    rval[axis] += shift;
    csys.setReferenceValue(rval);
    out->setCoordinateInfo(csys);
    out->set(T(0));
    sliceShape[axis] = out->shape()[axis];
    casacore::Array<T> slice(sliceShape);
    while (!inIter.atEnd()) {
        _boxcarSmooth(slice, inIter.cursor());
        out->putSlice(slice, inIter.position());
        inIter++;
    }
    if (this->_getDecimate() && out->shape()[axis] > 1) {
        ImageDecimator<T> decimator(
            SPIIT(out->cloneII()), 0,
            casacore::String(""), casacore::String(""), false
        );
        decimator.setFunction(this->_getDecimationFunction());
        decimator.setAxis(axis);
        decimator.setFactor(_width);
        decimator.suppressHistoryWriting(true);
        out = decimator.decimate();
        this->addHistory(decimator.getHistory());
    }
    return out;
}

template<class T> void ImageBoxcarSmoother<T>::setWidth(casacore::uInt w) {
    ThrowIf(w == 0, "Boxcar width must be positive");
    _width = w;
    this->_setNMinPixels(_width);
}

template<class T> void ImageBoxcarSmoother<T>::_boxcarSmooth(
    casacore::Array<T>& out, const casacore::Array<T>& in
) const {
	// although the passed in array may have more than one
	// dimension, only one of those will have length > 1

    out.set(T(0.0));
    typename casacore::Array<T>::const_iterator cur = in.begin();
    typename casacore::Array<T>::const_iterator end = in.end();
    auto skip = this->_getDecimate()
        && this->_getDecimationFunction() == ImageDecimatorData::COPY;
    typename casacore::Array<T>::iterator outIter = out.begin();
    if (skip) {
	    casacore::uInt count = 0;
	    auto nelem = in.size();
        while (count < nelem - _width + 1) {
            casacore::uInt intraCount = 0;
            while (intraCount < _width) {
                *outIter += *cur;
                cur++;
                intraCount++;
            }
            *outIter /= _width;
            for (casacore::uInt i=0; i<_width; i++) {
                outIter++;
            }
            count += _width;
        }
    }
    else {
        auto windowEnd = cur;
        T windowSum(0.0);
        for (casacore::uInt i=0; i<_width; i++) {
            windowSum += *windowEnd;
            windowEnd++;
        }
        casacore::Float width = _width;
        *outIter = windowSum/width;
        *outIter++;
        while (windowEnd != end) {
            windowSum += (*windowEnd - *cur);
            *outIter = windowSum/width;
            ++cur;
            ++windowEnd;
            ++outIter;
        }
    }
}

}
