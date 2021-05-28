#ifndef _CASA_SAKURA_ARRAY_CONVERTER_H_
#define _CASA_SAKURA_ARRAY_CONVERTER_H_

#include <iostream>
#include <string>
#include <stdexcept>
//#include <libsakura/sakura.h>
#include <casa/aipstype.h>

// AIPS++
#include <casa/aips.h>
#include <casa/Utilities/CountedPtr.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/BasicSL/Complex.h>

using namespace std;

namespace casa { //# NAMESPACE CASA - BEGIN

class SakuraArrayConverter {
protected:

public:
	template<class T, class S> static void ConvertCASAToSakura(
			casacore::uInt const pol_index, casacore::uInt const num_data,
			const casacore::Matrix<T> &input_matrix, S *output_data) {
		casacore::uInt const max_pol = input_matrix.nrow();
		if (!(pol_index < max_pol)) {
			throw std::invalid_argument(
					"pol_index must less than input_matrix.nrow()");
		}
		if (!(num_data == input_matrix.ncolumn())) {
			throw std::invalid_argument(
					"num_data must equal to input_matrix.ncolumn()");
		}
		casacore::Bool deleteIn;
		const T* pin = input_matrix.getStorage(deleteIn);
		for (casacore::uInt i = 0; i < num_data; ++i) {
			CASAToSakura(max_pol * i + pol_index, i, pin, output_data);
		}
		input_matrix.freeStorage(pin, deleteIn);
	}
	template<class S, class T> static void ConvertSakuraToCASA(
			casacore::uInt const pol_index, casacore::uInt const num_data, S const *input_data,
			casacore::Matrix<T> &output_matrix) {
		casacore::uInt const max_pol = output_matrix.nrow();
		if (!(pol_index < max_pol)) {
			throw std::invalid_argument(
					"pol_index must less than input_matrix.nrow()");
		}
		if (!(num_data == output_matrix.ncolumn())) {
			throw std::invalid_argument(
					"num_data must equal to output_matrix.ncolumn()");
		}
		casacore::Bool deleteOut;
		T* pout = output_matrix.getStorage(deleteOut);
		for (casacore::uInt i = 0; i < num_data; ++i) {
			SakuraToCASA(i, max_pol * i + pol_index, input_data, pout);
		}
		output_matrix.putStorage(pout, deleteOut);
	}

private:
	template<class T, class S> static void CASAToSakura(casacore::uInt const index_in,
			casacore::uInt const index_out, const T* pin, S *output_data) {
		output_data[index_out] = pin[index_in];
	}
	template<class S, class T> static void SakuraToCASA(casacore::uInt const index_in,
			casacore::uInt const index_out, S const *input_data, T* pout) {
		pout[index_out] = input_data[index_in];
	}
};

template<> void SakuraArrayConverter::CASAToSakura<casacore::Complex, float>(
		casacore::uInt const index_in, casacore::uInt const index_out, const casacore::Complex* pin,
		float *output_data);
template<> void SakuraArrayConverter::SakuraToCASA<float, casacore::Complex>(
		casacore::uInt const index_in, casacore::uInt const index_out, float const *input_data,
		casacore::Complex* pout);
template<> void SakuraArrayConverter::CASAToSakura<casacore::Bool, bool>(
		casacore::uInt const index_in, casacore::uInt const index_out, const casacore::Bool* pin,
		bool *output_data);
template<> void SakuraArrayConverter::SakuraToCASA<bool, casacore::Bool>(
		casacore::uInt const index_in, casacore::uInt const index_out, bool const *input_data,
		casacore::Bool* pout);
} //# NAMESPACE CASA - END

#endif /* _CASA_SINGLEDISH_DUMMY_H_ */
