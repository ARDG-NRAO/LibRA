#ifndef IMAGES_IMAGEEDECIMATORDATA_H
#define IMAGES_IMAGEEDECIMATORDATA_H

#include <casa/namespace.h>

namespace casa {

class ImageDecimatorData {
	// <summary>
	// Holds data for ImageDecimator class.
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

	// Put new functions before NFUNCS.
	enum Function {
		// just copy the nth plane as is, no function applied
		COPY,
		MEAN,
		NFUNCS
	};
};
}

#endif
