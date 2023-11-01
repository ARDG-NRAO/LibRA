#include <imageanalysis/ImageAnalysis/ImageHanningSmoother.h>


namespace casa {

template<class T> ImageHanningSmoother<T>::ImageHanningSmoother(
    const SPCIIT image,
    const casacore::Record *const region,
    const casacore::String& mask,
    const casacore::String& outname, casacore::Bool overwrite
) : Image1DSmoother<T>(
        image, region, mask, outname, overwrite
    ) {
    this->_construct();
    this->_setNMinPixels(3);
}

template<class T> SPIIT ImageHanningSmoother<T>::_smooth(
    const casacore::ImageInterface<T>& image
) const {
    casacore::IPosition inTileShape = image.niceCursorShape();
    casacore::uInt axis = this->_getAxis();
    casacore::TiledLineStepper inNav(image.shape(), inTileShape, axis);
    casacore::RO_MaskedLatticeIterator<T> inIter(image, inNav);
    casacore::IPosition sliceShape(image.ndim(), 1);
    sliceShape[axis] = image.shape()[axis];
    casacore::Array<T> slice(sliceShape);
    casacore::String empty;
    casacore::Record emptyRecord;
    SPIIT out(
        SubImageFactory<T>::createImage(
            image, empty, emptyRecord, empty,
            false, false, false, false
        )
    );

    while (!inIter.atEnd()) {
        slice = _hanningSmooth(inIter.cursor());
        out->putSlice(slice, inIter.position());
        inIter++;
    }
    if (this->_getDecimate()) {
        // remove the first plane from _axis
        casacore::IPosition shape = out->shape();
        casacore::IPosition blc(shape.size(), 0);
        blc[axis] = 1;
        ImageDecimatorData::Function f = this->_getDecimationFunction();
        casacore::IPosition trc = shape - 1;
        if (shape[axis] % 2 == 0) {
            trc[axis]--;
        }
        casacore::LCBox lcbox(blc, trc, shape);
        casacore::Record x = lcbox.toRecord("");
        ImageDecimator<T> decimator(
            SPIIT(out->cloneII()), &x,
            casacore::String(""), casacore::String(""), false
        );
        decimator.setFunction(f);
        decimator.setAxis(axis);
        decimator.setFactor(2);
        decimator.suppressHistoryWriting(true);
        out = decimator.decimate();
        this->addHistory(decimator.getHistory());
    }
    return out;
}

template<class T> casacore::Array<T> ImageHanningSmoother<T>::_hanningSmooth(
    const casacore::Array<T>& in
) const {
    // although the passed in array may have more than one
    // dimensions, only one of those will have length > 1
    casacore::uInt size = in.size();
    casacore::Array<T> out(in.shape(), T(0.0));

    casacore::uInt count = 1;
    casacore::uInt end = size - 1;
    typename casacore::Array<T>::const_iterator prev = in.begin();
    typename casacore::Array<T>::const_iterator cur = in.begin();
    cur++;
    typename casacore::Array<T>::const_iterator next = in.begin();
    next++;
    next++;
    casacore::Bool skip = this->_getDecimate()
        && this->_getDecimationFunction() == ImageDecimatorData::COPY;
    casacore::uInt inc = skip ? 2 : 1;
    if (skip && size % 2 == 0) {
        end--;
    }
    typename casacore::Array<T>::iterator outIter = out.begin();
    if (! skip) {
        *outIter = 0.5*(*cur + *prev);
    }
    outIter++;
    while (count < end) {
        *outIter = 0.25*(*prev + *next)
            + 0.5 * (*cur);
        for (casacore::uInt i=0; i<inc; i++) {
            outIter++;
            prev++;
            cur++;
            next++;
        }
        count += inc;
    }
    if (! skip) {
         *outIter = 0.5 * (*cur + *prev);
    }
    return out;
}

}
