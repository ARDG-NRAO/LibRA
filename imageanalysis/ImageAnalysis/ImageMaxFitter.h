#ifndef IMAGEANALYSIS_IMAGEMAXFITTER_H
#define IMAGEANALYSIS_IMAGEMAXFITTER_H

#include <casa/namespace.h>

namespace casa {

template <class T> class ImageMaxFitter: public ImageTask<T> {
	// <summary>
	// Top level interface source max fitting
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

	ImageMaxFitter(SPCIIT image, const casacore::Record *const &region);

	// destructor
	~ImageMaxFitter() {}

	casacore::String getClass() const { const static casacore::String s = "ImageMaxFitter"; return s; }

	casacore::Record fit(casacore::Bool point, casacore::Int width=5, casacore::Bool negfind=false, casacore::Bool list=true) const;

protected:

	inline  CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_FIRST_STOKES;
	}

	inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>(1, casacore::Coordinate::DIRECTION);
	}

    inline casacore::Bool _supportsMultipleBeams() const {return true;}


};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageMaxFitter.tcc>
#endif

#endif
