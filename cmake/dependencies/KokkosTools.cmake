#-*- cmake -*-
# Copyright (C) 2025
# Associated Universities, Inc. Washington DC, USA.
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.is
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning this should be addressed as follows:
#        Postal address: National Radio Astronomy Observatory
#                        1003 Lopezville Road,
#                        Socorro, NM - 87801, USA
#

# KokkosTools external project for profiling support
include(ExternalProject)

if(NOT LIBRA_BUILD_KOKKOSTOOLS)
  return()
endif()

# KokkosTools requires dl library for dynamic loading
find_library(DL_LIBRARY dl)
if(NOT DL_LIBRARY)
  message(WARNING "${LIBRA_ID}libdl not found - KokkosTools may not function properly")
endif()

ExternalProject_Add(
  KokkosTools
  DEPENDS        Kokkos
  GIT_REPOSITORY ${LIBRA_KOKKOSTOOLS_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_KOKKOSTOOLS_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/kokkos-tools
  BINARY_DIR     ${BUILD_DIR}/KokkosTools
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                 -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                 -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
                 -DCMAKE_CXX_STANDARD=17
                 -DKokkos_ROOT=${INSTALL_DIR}
                 -DKokkos_NVCC_WRAPPER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper
                 -DKokkos_COMPILE_LAUNCHER=${INSTALL_DIR}/bin/kokkos/kokkos_launch_compiler
                 -DCMAKE_CXX_EXTENSIONS=OFF
                 -DKokkosTools_ENABLE_EXAMPLES=ON
                 -DKokkosTools_ENABLE_TESTS=ON
                 -DKokkosTools_ENABLE_BENCHMARKS=ON
                 -DBUILD_SHARED_LIBS=ON
                 ${SPACK_FLAGS}
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
)

# Set environment variables for runtime use (KokkosTools installs to lib directory)
set(LIBRA_KOKKOSTOOLS_PATH "${INSTALL_DIR}/lib" CACHE PATH "Path to KokkosTools installation")

# Common profiling tools that will be available (actual names may vary)
set(LIBRA_KOKKOSTOOLS_KERNEL_TIMER    "${LIBRA_KOKKOSTOOLS_PATH}/libkp_kernel_timer.so")
set(LIBRA_KOKKOSTOOLS_MEMORY_EVENTS   "${LIBRA_KOKKOSTOOLS_PATH}/libkp_memory_events.so")
set(LIBRA_KOKKOSTOOLS_MEMORY_USAGE    "${LIBRA_KOKKOSTOOLS_PATH}/libkp_memory_usage.so")
set(LIBRA_KOKKOSTOOLS_NVTX_CONNECTOR  "${LIBRA_KOKKOSTOOLS_PATH}/libkp_nvtx_connector.so")
set(LIBRA_KOKKOSTOOLS_SPACE_TIME_STACK "${LIBRA_KOKKOSTOOLS_PATH}/libkp_space_time_stack.so")