#include <iostream>

#include <libsakura/sakura.h>
//#include <libsakura/config.h>

#include <casa_sakura/SakuraUtils.h>
#include <casa_sakura/SakuraArrayConverter.h>

// AIPS++
#include <casacore/casa/aips.h>
#include <casacore/casa/Utilities/CountedPtr.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
using namespace casacore;
namespace casa {

template<> void SakuraArrayConverter::CASAToSakura<Complex, float>(
		uInt const index_in, uInt const index_out, const Complex* pin,
		float *output_data) {
	output_data[index_out] = casacore::real(pin[index_in]);
}

template<> void SakuraArrayConverter::SakuraToCASA<float, Complex>(
		uInt const index_in, uInt const index_out, float const *input_data,
		Complex* pout) {
	pout[index_out] = Complex(input_data[index_in],0.0);
}

template<> void SakuraArrayConverter::CASAToSakura<Bool, bool>(
		uInt const index_in, uInt const index_out, const Bool* pin,
		bool *output_data) {
	output_data[index_out] = !pin[index_in];
}

template<> void SakuraArrayConverter::SakuraToCASA<bool,Bool>(
		uInt const index_in, uInt const index_out, bool const *input_data,
		Bool* pout) {
	pout[index_out] = !input_data[index_in];
}
}
// End of casa namespace.
