#include <imageanalysis/ImageAnalysis/ImageFFTer.h>

#include <imageanalysis/ImageAnalysis/ImageFFT.h>

namespace casa {

template<class T> ImageFFTer<T>::ImageFFTer(
	const SPCIIT image,
	const casacore::Record *const region,
	const casacore::String& maskInp, const casacore::Vector<casacore::uInt>& axes
) : ImageTask<T>(
		image, "", region, "", "", "",
		maskInp, "", false
	),
	_axes(axes), _real(), _imag(), _amp(),
	_phase(), _complex() {
	this->_construct();
}

template<class T> void ImageFFTer<T>::fft() const {
	*this->_getLog() << casacore::LogOrigin(getClass(), __FUNCTION__);
	ThrowIf(
		_real.empty() && _imag.empty() && _amp.empty()
		&& _phase.empty() && _complex.empty(),
		"No output image names have been specified"
	);
	_checkExists(_real);
	_checkExists(_imag);
	_checkExists(_amp);
	_checkExists(_phase);
	_checkExists(_complex);
	auto subImage = SubImageFactory<T>::createSubImageRO(
		*this->_getImage(), *this->_getRegion(), this->_getMask(),
		this->_getLog().get(), casacore::AxesSpecifier(), this->_getStretch()
	);
	ImageFFT<T> fft;
	if (_axes.size() == 0) {
		*this->_getLog() << casacore::LogIO::NORMAL
		    << "FFT the direction coordinate" << casacore::LogIO::POST;
		fft.fftsky(*subImage);
	}
	else {
		// Set vector of bools specifying axes
		casacore::Vector<casacore::Bool> which(subImage->ndim(), false);
		for(casacore::uInt i: _axes) {
			which(_axes(i)) = true;
		}
		*this->_getLog() << casacore::LogIO::NORMAL << "FFT zero-based axes "
		    << _axes << casacore::LogIO::POST;
		fft.fft(*subImage, which);
	}

	// casacore::String maskName("");
    _createOutputImages(*subImage, fft);
}

template <class T>
void ImageFFTer<T>::_createOutputImages(
    const casacore::SubImage<T>& subImage, const ImageFFT<T>& fft
) const {
    if (
        ! (_real.empty() && _imag.empty() && _amp.empty() && _phase.empty())
    ) {
        using RealType = typename NumericTraits<T>::BaseType;
        SPIIRT realImage;
        if (! _real.empty()) {
            _createImage(realImage, _real, subImage);
            fft.getReal(*realImage);
            this->_doHistory(realImage);
        }
        if (! _imag.empty()) {
            _createImage(realImage, _imag, subImage);
            fft.getImaginary(*realImage);
            this->_doHistory(realImage);
        }
        if (! _amp.empty()) {
            _createImage(realImage, _amp, subImage);
            fft.getAmplitude(*realImage);
            this->_doHistory(realImage);
        }
        if (! _phase.empty()) {
            _createImage(realImage, _phase, subImage);
            fft.getPhase(*realImage);
            this->_doHistory(realImage);
        }
    }
    if (! _complex.empty()) {
        using ComplexType = std::complex<typename NumericTraits<T>::BaseType>;
        SPIICT complexImage;
        _createImage(complexImage, _complex, subImage);
        fft.getComplex(*complexImage);
        this->_doHistory(complexImage);
    }
}

template<class T> template <class U> void ImageFFTer<T>::_createImage(
    SPIIU& out, const casacore::String& name,
    const casacore::SubImage<T>& subImage
) const {
	*this->_getLog() << casacore::LogIO::NORMAL << "Creating image '"
		<< name << "'" << casacore::LogIO::POST;
	out.reset(
	    new casacore::PagedImage<U>(
	        subImage.shape(), subImage.coordinates(), name
	    )
	);
	if (subImage.isMasked()) {
		casacore::String x;
		ImageMaskAttacher::makeMask(
			*out, x, false, true, *this->_getLog(), true
		);
	}
}

template<class T> void ImageFFTer<T>::_checkExists(
	const casacore::String& name
) {
	if (! name.empty()) {
		casacore::File f(name);
		ThrowIf (
			f.exists(), "File " + name + " already exists"
		);
	}
}
}
