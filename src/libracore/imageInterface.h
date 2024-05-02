// Copyright 2021 Associated Universities, Inc. Washington DC, USA.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef IMAGEINTERFACE_FUNC_H
#define IMAGEINTERFACE_FUNC_H

#include <iostream>

#include <mdspan.hpp>
#include <casacore/casa/namespace.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Arrays/Matrix.h>

namespace libracore {

using extents_type = Kokkos::dextents<size_t, 2>;

template <typename T>
using row_major_mdspan = Kokkos::mdspan<T, extents_type, Kokkos::layout_right>;
// p.s. casa matrix is col major
template <typename T>
using col_major_mdspan = Kokkos::mdspan<T, extents_type, Kokkos::layout_left>;


template <typename T>
int mdspan2casamatrix(
  libracore::col_major_mdspan<T> md,
  casacore::Matrix<T>& matrix)
{
    size_t nx = md.extent(0);
    size_t ny = md.extent(1);

    if (size_t(matrix.shape()(0)) != nx || size_t(matrix.shape()(1)) != ny)
    {
        std::cerr << "Sizes of mdspan and casa matrix do not match.\n";
        return EXIT_FAILURE;
    }

    
    for (std::size_t j = 0; j < ny; ++j)
        for (std::size_t i = 0; i < nx; ++i)
        #if MDSPAN_USE_BRACKET_OPERATOR
            matrix(i,j) = md[i, j];
        #else
            matrix(i,j) = md(i, j);
        #endif

    return(0);
}

template <typename T>
int casamatrix2mdspan(casacore::Matrix<T>& matrix,
    libracore::col_major_mdspan<T> md)
{
    const size_t nx = size_t(matrix.shape()(0));
    const size_t ny = size_t(matrix.shape()(1));

    if (md.extent(0) != nx || md.extent(1) != ny)
    {
        std::cerr << "Sizes of mdspan and casa matrix do not match.\n";
        return EXIT_FAILURE;
    }

    for (std::size_t j = 0; j < ny; ++j)
        for (std::size_t i = 0; i < nx; ++i)
        #if MDSPAN_USE_BRACKET_OPERATOR
            md[i, j] = matrix(i, j);
        #else
            md(i, j) = matrix(i, j);
        #endif

    return(0);

}

void testing();

} // end namespace 

#endif
