#include <imageanalysis/ImageAnalysis/PixelValueManipulatorData.h>

#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>

using namespace casacore;
namespace casa {
PixelValueManipulatorData::SpectralType PixelValueManipulatorData::spectralType(
	const String& specString
) {

	String spec = specString;
	spec.downcase();
	static const String def = "default";
	static const String optical = "optical velocity";
	static const String radio = "radio velocity";
	static const String wave = "wavelength";
	static const String air = "air wavelength";
	static const String rel = "relativistic velocity";
	static const String beta = "beta";
	ThrowIf (
		spec == "r",
		"Ambiguous, could be either radio or relativistic"
	);
	if (spec.empty() || def.startsWith(spec)) {
		return DEFAULT;
	}
	else if (optical.startsWith(spec)) {
		return OPTICAL_VELOCITY;
	}
	else if (radio.startsWith(spec)) {
		return RADIO_VELOCITY;
	}
	else if (rel.startsWith(spec) || beta.startsWith(spec)) {
		return RELATIVISTIC;
	}
	else if (wave.startsWith(spec)) {
		return WAVELENGTH;
	}
	else if (air.startsWith(spec)) {
		return AIR_WAVELENGTH;
	}
	else {
		ThrowCc(specString + "does not identify a spectral type");
	}
}

}
