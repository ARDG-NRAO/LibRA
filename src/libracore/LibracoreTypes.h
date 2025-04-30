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

#ifndef LIBRACORETYPES_FUNC_H
#define LIBRACORETYPES_FUNC_H

#include <iostream>

#include <mdspan.hpp>
#include <casacore/casa/namespace.h>

namespace libracore {

using extents_type = Kokkos::dextents<size_t, 2>;

template <typename T>
using row_major_mdspan = Kokkos::mdspan<T, extents_type, Kokkos::layout_right>;
// p.s. casa matrix is col major
template <typename T>
using col_major_mdspan = Kokkos::mdspan<T, extents_type, Kokkos::layout_left>;

// Define mdspan types for 3D arrays, both row-major and column-major
using extents_type_3d = Kokkos::dextents<size_t, 3>;

template <typename T>
using row_major_mdspan_3d = Kokkos::mdspan<T, extents_type_3d, Kokkos::layout_right>;

template <typename T>
using col_major_mdspan_3d = Kokkos::mdspan<T, extents_type_3d, Kokkos::layout_left>;

// Define mdspan types for 1D arrays
using extents_type_1d = Kokkos::dextents<size_t, 1>;

template <typename T>
using col_major_mdspan_1d = Kokkos::mdspan<T, extents_type_1d, Kokkos::layout_left>;

} // end namespace 

#endif