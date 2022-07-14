#ifndef IMAGEANALYSIS_IMAGEBOXCARSMOOTHER_H
#define IMAGEANALYSIS_IMAGEBOXCARSMOOTHER_H

#include <imageanalysis/ImageAnalysis/Image1DSmoother.h>

#include <casacore/casa/namespace.h>

namespace casa {

template <class T> class ImageBoxcarSmoother : public Image1DSmoother<T> {
	// <summary>
	// Top level interface for 1-D boxcar smoothing of images.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// Boxcar smooths an image in one dimension.
	// </etymology>

	// <synopsis>
	// </synopsis>

public:

	ImageBoxcarSmoother(
		const SPCIIT image,
		const casacore::Record *const region,
		const casacore::String& maskInp,
		const casacore::String& outname, casacore::Bool overwrite
	);

	// destructor
	~ImageBoxcarSmoother() {}

	casacore::String getClass() const { const static casacore::String s = "ImageBoxcarSmoother"; return s; }

    // set witdth of boxcar
    void setWidth(casacore::uInt w);

protected:

	SPIIT _smooth(
		const casacore::ImageInterface<T>& image
	) const;

private:

    casacore::uInt _width;

	// disallow default constructor
	ImageBoxcarSmoother();

	void _boxcarSmooth(casacore::Array<T>& out, const casacore::Array<T>& in) const ;
};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageBoxcarSmoother.tcc>
#endif

#endif
