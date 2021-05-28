#ifndef IMAGEANALYSIS_IMAGEREBINNER_H
#define IMAGEANALYSIS_IMAGEREBINNER_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <casa/namespace.h>

namespace casa {

template <class T> class ImageRebinner : public ImageTask<T> {
	// <summary>
	// Top level interface for image rebinning
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// Rebins an image to larger pixels.
	// </etymology>

	// <synopsis>
	// Top level interface for rebinning images.
	// </synopsis>

public:


	ImageRebinner(
		const SPCIIT image, const casacore::Record *const region,
		const casacore::String& maskInp,
		const casacore::String& outname, casacore::Bool overwrite
	);

	// destructor
	virtual ~ImageRebinner() {}

	SPIIT rebin() const;

	virtual casacore::String getClass() const { const static casacore::String s = "ImageRebinner"; return s; }

	// Set the factors (1 for each axis) to rebin by.
	void setFactors(const casacore::Vector<casacore::Int>& f);

	void setCrop(casacore::Bool c) { _crop = c; }

protected:

	CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_ALL_STOKES;
	}

	std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>();
	}

private:
	casacore::IPosition _factors;
	casacore::Bool _crop;

	// disallow default constructor
	ImageRebinner();
};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageRebinner.tcc>
#endif

#endif
