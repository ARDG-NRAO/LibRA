#ifndef IMAGESANALYSIS_IMAGEMASKATTACHER_H
#define IMAGESANALYSIS_IMAGEMASKATTACHER_H

#include <casacore/images/Images/ImageInterface.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// Contains tatic method to attach a mask to an image
// </summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="" tests="">
// </reviewed>
//
// <prerequisite>
// </prerequisite>
//
// <synopsis> 
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="2013/02/24">
// </todo>

class ImageMaskAttacher {
public: 
	// Moved from ImageAnalysis
	// Make a mask and define it in the image.
	template <class T> static casacore::Bool makeMask(
		casacore::ImageInterface<T>& out,
		casacore::String& maskName,
		casacore::Bool init, casacore::Bool makeDefault,
		casacore::LogIO& os, casacore::Bool list
	);

private:
    ImageMaskAttacher() {};

    ImageMaskAttacher operator=(ImageMaskAttacher);
};

} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageMaskAttacher.tcc>
#endif

#endif
