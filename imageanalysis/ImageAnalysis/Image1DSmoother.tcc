#include <imageanalysis/ImageAnalysis/Image1DSmoother.h>

#include <imageanalysis/ImageAnalysis/ImageDecimator.h>
#include <imageanalysis/ImageAnalysis/ImageMaskedPixelReplacer.h>

namespace casa {

template<class T> Image1DSmoother<T>::Image1DSmoother(
    const SPCIIT image,
    const casacore::Record *const region,
    const casacore::String& maskInp,
    const casacore::String& outname, casacore::Bool overwrite
) : ImageTask<T>(
        image, "", region, "", "", "",
        maskInp, outname, overwrite
    ),
    _axis(0), _nMinPixels(2), _decimate(false),
    _decimationFunction(ImageDecimatorData::COPY) {}

template<class T> void Image1DSmoother<T>::setAxis(casacore::uInt n) {
    casacore::uInt ndim = this->_getImage()->ndim();
    ThrowIf(
        n >= ndim,
        "The axis number along which the smoothing "
        "is to occur must be less than the number of "
        "axes in the image which is " + casacore::String::toString(ndim)
    );
    _axis = n;
}

template<class T> SPIIT Image1DSmoother<T>::smooth() const {
    *this->_getLog() << casacore::LogOrigin(getClass(), __func__);
    SPIIT subImage(
        SubImageFactory<T>::createImage(
            *this->_getImage(), "", *this->_getRegion(),
            this->_getMask(), false, false, false, this->_getStretch()
        )
    );
    ThrowIf(
        subImage->shape()[_axis] < _nMinPixels,
        "The selected region of the image must have at least "
        + casacore::String::toString(_nMinPixels)
    	+ " pixels along the axis to be smoothed."
    );
    if (subImage->isMasked() || subImage->hasPixelMask()) {
    	ImageMaskedPixelReplacer<T> impr(
    		subImage, 0, ""
    	);
    	impr.replace("0", false, false);
    }
    SPIIT smoothed(_smooth(*subImage));
    this->_reportOldNewImageShapes(*smoothed);
    return this->_prepareOutputImage(*smoothed);
}

}
