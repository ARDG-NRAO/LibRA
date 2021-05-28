#ifndef IMAGEANALYSIS_IMAGE1DSMOOTHER_H
#define IMAGEANALYSIS_IMAGE1DSMOOTHER_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <imageanalysis/ImageAnalysis/ImageDecimatorData.h>

#include <casa/namespace.h>

namespace casa {

template <class T> class Image1DSmoother : public ImageTask<T> {
	// <summary>
	// Top level interface for 1-D smoothing of images.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// Smooths an image in one dimension.
	// </etymology>

	// <synopsis>
	// Top level interface for removing image planes.
	// </synopsis>

public:
	Image1DSmoother() = delete;

	// destructor
	virtual ~Image1DSmoother() {}

	SPIIT smooth() const;

	virtual casacore::String getClass() const { const static casacore::String s = "Image1DSmoother"; return s; }

	// Keep only every nth plane
	void setDecimate(casacore::Bool b) {_decimate = b; }

	// Set the pixel axis number along which the smoothing will occur
	void setAxis(casacore::uInt n);

	// Set the decimation function.
	inline void setDecimationFunction(ImageDecimatorData::Function f) {
		_decimationFunction = f;
	}

protected:

	Image1DSmoother(
		const SPCIIT image,
		const casacore::Record *const region,
		const casacore::String& maskInp,
		const casacore::String& outname, casacore::Bool overwrite
	);

	virtual SPIIT _smooth(const casacore::ImageInterface<T>& image) const = 0;

	inline  CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_ALL_STOKES;
	}

	inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>();
	}

    inline casacore::Bool _supportsMultipleBeams() const {return false;}

    inline casacore::Bool _getDecimate() const { return _decimate; }

    inline ImageDecimatorData::Function _getDecimationFunction() const {
    	return _decimationFunction;
    }

    inline casacore::uInt _getAxis() const { return _axis; }

    inline void _setNMinPixels(casacore::uInt n) { _nMinPixels = n; }
private:
	casacore::uInt _axis, _nMinPixels;
	casacore::Bool _decimate;
	ImageDecimatorData::Function _decimationFunction;

};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/Image1DSmoother.tcc>
#endif

#endif
