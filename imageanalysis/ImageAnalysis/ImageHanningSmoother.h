#ifndef IMAGEANALYSIS_IMAGEHANNINGSMOOTHER_H
#define IMAGEANALYSIS_IMAGEHANNINGSMOOTHER_H

#include <imageanalysis/ImageAnalysis/Image1DSmoother.h>

#include <casa/namespace.h>

namespace casa {

template <class T> class ImageHanningSmoother : public Image1DSmoother<T> {
	// <summary>
	// Top level interface for 1-D hanning smoothing of images.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// Hanning smooths an image in one dimension.
	// </etymology>

	// <synopsis>
	// Top level interface for removing image planes.
	// </synopsis>

public:

	ImageHanningSmoother(
		const SPCIIT image,
		const casacore::Record *const region,
		const casacore::String& maskInp,
		const casacore::String& outname, casacore::Bool overwrite
	);

	// destructor
	~ImageHanningSmoother() {}

	casacore::String getClass() const { const static casacore::String s = "HanningSmoother"; return s; }

protected:

	SPIIT _smooth(
		const casacore::ImageInterface<T>& image
	) const;

private:

	// disallow default constructor
	ImageHanningSmoother();

	casacore::Array<T> _hanningSmooth(const casacore::Array<T>& in) const ;
};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageHanningSmoother.tcc>
#endif

#endif
