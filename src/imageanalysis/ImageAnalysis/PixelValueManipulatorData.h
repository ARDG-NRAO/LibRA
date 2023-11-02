#ifndef IMAGES_PIXELVALUEMANIPULATORDATA_H
#define IMAGES_PIXELVALUEMANIPULATORDATA_H

#include <casacore/casa/typeinfo.h>

namespace casacore{

class String;
}

namespace casa {


class PixelValueManipulatorData {
	// <summary>
	// Non-templated data related bits for PixelValueManipulator.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// casacore::Data for PixelValueManipulator.
	// </etymology>

	// <synopsis>
	// </synopsis>

	// <example>
	// </example>

public:

	enum SpectralType {
		DEFAULT,
		RELATIVISTIC,
		RADIO_VELOCITY,
		OPTICAL_VELOCITY,
		AIR_WAVELENGTH,
		WAVELENGTH,
	};

	PixelValueManipulatorData() = delete;

	// destructor
	~PixelValueManipulatorData() {}

	// convert a string to a spectral type. Acceptable values are (case insensitive,
	// minimum match supported): "default", "optical velocity", "radio velocity",
	// "wavelength", "air wavelength", and "" (which returns DEFAULT)
	static SpectralType spectralType(const casacore::String& specString);

};
}

#endif
