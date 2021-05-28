#ifndef IMAGEANALYSIS_TWOPOINTCORRELATOR_H
#define IMAGEANALYSIS_TWOPOINTCORRELATOR_H

#include <lattices/LatticeMath/LatticeTwoPtCorr.h>

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <casa/namespace.h>

namespace casa {

template <class T> class TwoPointCorrelator : public ImageTask<T> {
	// <summary>
	// Top level interface for two point correlation of images.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// Smooths an image in one dimension.
	// </etymology>

	// <synopsis>
	// </synopsis>

public:

	TwoPointCorrelator() = delete;

	TwoPointCorrelator(
		SPCIIT image, const casacore::Record *const region, const casacore::String& mask,
		const casacore::String& outname, casacore::Bool overwrite
	);

	// destructor
	~TwoPointCorrelator() {}

	SPIIT correlate();

	casacore::String getClass() const { const static casacore::String s = "TwoPointCorrelator"; return s; }

	// Set the pixel axes over which to do correlation
	void setAxes(const casacore::IPosition& axes);

	void setMethod(const casacore::String& method) {
		_method = casacore::LatticeTwoPtCorr<T>::fromString(method);
	}

protected:

	inline  CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_ALL_STOKES;
	}

	inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>();
	}

    inline casacore::Bool _supportsMultipleBeams() const {return true;}

private:
	casacore::IPosition _axes = casacore::IPosition();
	typename casacore::LatticeTwoPtCorr<T>::Method _method = casacore::LatticeTwoPtCorr<T>::STRUCTUREFUNCTION;

};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/TwoPointCorrelator.tcc>
#endif

#endif
