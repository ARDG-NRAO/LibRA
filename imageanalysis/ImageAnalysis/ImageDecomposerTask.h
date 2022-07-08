#ifndef IMAGEANALYSIS_IMAGEDECOMPOSERTASK_H
#define IMAGEANALYSIS_IMAGEDECOMPOSERTASK_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <casacore/casa/namespace.h>

namespace casa {

template <class T> class ImageDecomposerTask : public ImageTask<T> {
	// <summary>
	// Top level interface for decomposing of images.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// </etymology>

	// <synopsis>
	// </synopsis>

public:

	ImageDecomposerTask() = delete;

	ImageDecomposerTask(
		SPCIIT image, const casacore::Record *const region, const casacore::String& mask
	);

	// destructor
	~ImageDecomposerTask() {}

	casacore::Matrix<T> decompose(casacore::Matrix<casacore::Int>& blcs, casacore::Matrix<casacore::Int>& trcs);

	casacore::String getClass() const { const static casacore::String s = "ImageDecomposerTask"; return s; }

	void setSimple(casacore::Bool b) { _simple = b; }

	void setDeblendOptions(casacore::Double threshold, casacore::Int nContour, casacore::Int minRange, casacore::Int nAxis);

	void setFit(casacore::Bool b) { _fit = b; }

	void setFitOptions(casacore::Double maxrms, casacore::Int maxRetry, casacore::Int maxIter, casacore::Double convCriteria);

protected:

	inline  CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_ALL_STOKES;
	}

	inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>();
	}

    inline casacore::Bool _supportsMultipleBeams() const {return true;}

private:
    casacore::Bool _simple = false;
    casacore::Double _threshold = -1;
    casacore::Int _ncontour = 11;
    casacore::Int _minrange = 1;
    casacore::Int _naxis = 2;
    casacore::Bool _fit = true;
    casacore::Double _maxrms = -1;
    casacore::Int _maxretry = -1;
    casacore::Int _maxiter = 256;
    casacore::Double _convcriteria = 0.0001;
};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageDecomposerTask.tcc>
#endif

#endif
