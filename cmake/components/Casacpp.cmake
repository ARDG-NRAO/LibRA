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
# Casacpp external project
include(FindPkgConfig)
pkg_check_modules(GSL REQUIRED gsl)
include(ExternalProject)

if (LIBRA_USE_LIBSAKURA)
  set(SAKURA_DEPENDENCE "Sakura")
else()
  set(SAKURA_DEPENDENCE "")
endif()

ExternalProject_Add(
  Casacpp
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/src/
  DOWNLOAD_COMMAND ""
  BINARY_DIR      ${BUILD_DIR}/Libra/src
  DEPENDS         Casacore ${SAKURA_DEPENDENCE} Hpg Parafeed Pybind11
  CONFIGURE_COMMAND
    PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig
    ${CMAKE_COMMAND} <SOURCE_DIR>
    -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_INSTALL_BINDIR=bin/casacpp
    -DCMAKE_CXX_FLAGS=-I${INSTALL_DIR}/include
    -DHpg_DIR=${INSTALL_DIR}/lib/cmake/Hpg/
    -Dparafeed_DIR=${INSTALL_DIR}/lib/cmake/parafeed/
    -DKokkos_COMPILE_LAUNCHER=${INSTALL_DIR}/bin/kokkos/kokkos_launch_compiler
    -DKokkos_NVCC_WRAPPER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper
    -DCMAKE_MODULE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/src/cmake
    -DCMAKE_PREFIX_PATH=${INSTALL_DIR}:${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos
    -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
    -DGSL_INCLUDE_DIR=${GSL_INCLUDEDIR}
    -DGSL_LIBRARY=${GSL_LIBRARY_DIRS}
    -DGSL_CBLAS_LIBRARY=${GSL_LIBRARY_DIRS}
    ${SPACK_FLAGS}
  BUILD_COMMAND   ${CMAKE_COMMAND} -E env PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig make -j${NCORES}
  INSTALL_COMMAND make install
)
