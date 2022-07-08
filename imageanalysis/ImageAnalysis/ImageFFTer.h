#ifndef IMAGES_IMAGEFFTER_H
#define IMAGES_IMAGEFFTER_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <casacore/casa/namespace.h>

namespace casa {

template <class T> class ImageFFT;

template <class T> class ImageFFTer : public ImageTask<T> {
	// <summary>
	// Top level interface which allows FFTing of images.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// FFTs image.
	// </etymology>

	// <synopsis>
	// High level interface for FFTing an image.
	// </synopsis>

public:

	ImageFFTer(
		const SPCIIT image, const casacore::Record *const region,
		const casacore::String& maskInp,
		const casacore::Vector<casacore::uInt>& axes
	);

	// destructor
	~ImageFFTer() {}

	void fft() const;

	void setReal(const casacore::String& name) { _real = name; }
	void setImag(const casacore::String& name) { _imag = name; }
	void setAmp(const casacore::String& name) { _amp = name; }
	void setPhase(const casacore::String& name) { _phase = name; }
	void setComplex(const casacore::String& name) { _complex = name; }

	casacore::String getClass() const {
	    const static casacore::String s = "ImageFFTer";
	    return s;
	}

protected:
	CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_ALL_STOKES;
	}

	std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>();
	}

private:
	casacore::Vector<casacore::uInt> _axes;
	casacore::String _real, _imag, _amp, _phase, _complex;

	// disallow default constructor
	ImageFFTer();

	void _createOutputImages(
	    const casacore::SubImage<T>& subImage, const ImageFFT<T>& fft
	) const;

	template <class U> void _createImage(
	    SPIIU& out, const casacore::String& name,
	    const casacore::SubImage<T>& subimage
	) const;

	static void _checkExists(const casacore::String& name);
};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageFFTer.tcc>
#endif

#endif
