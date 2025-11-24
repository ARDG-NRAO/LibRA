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
find_package(GSL REQUIRED)
find_package(WCSLIB REQUIRED)
include(ExternalProject)

if (LIBRA_USE_LIBSAKURA)
  set(SAKURA_DEPENDENCE "Sakura")
else()
  set(SAKURA_DEPENDENCE "")
endif()

# Debug output
message(STATUS "Casacpp.cmake: WCSLIB_FOUND = ${WCSLIB_FOUND}")
message(STATUS "Casacpp.cmake: WCSLIB_INCLUDE_DIRS = ${WCSLIB_INCLUDE_DIRS}")
message(STATUS "Casacpp.cmake: WCSLIB_LIBRARIES = ${WCSLIB_LIBRARIES}")

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
    -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE
    -DCMAKE_BUILD_RPATH_USE_ORIGIN=TRUE
    -DCMAKE_INSTALL_RPATH=${INSTALL_DIR}/lib
    -DHpg_DIR=${INSTALL_DIR}/lib/cmake/Hpg/
    -Dparafeed_DIR=${INSTALL_DIR}/lib/cmake/parafeed/
    ${KOKKOS_WRAPPER_FLAGS}
    -DCMAKE_MODULE_PATH=<SOURCE_DIR>/../cmake/modules
    -DCMAKE_PREFIX_PATH=${INSTALL_DIR}:${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos
    -DCASACORE_ROOT_DIR=${INSTALL_DIR}
    -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
    -DGSL_INCLUDE_DIR=${GSL_INCLUDE_DIRS}
    -DGSL_LIBRARY=${GSL_LIBRARIES}
    -DGSL_CBLAS_LIBRARY=${GSL_LIBRARIES}
    -DWCSLIB_INCLUDE_DIRS=${WCSLIB_INCLUDE_DIRS}
    -DWCSLIB_LIBRARIES=${WCSLIB_LIBRARIES}
    -DWCSLIB_FOUND=${WCSLIB_FOUND}
    ${SPACK_FLAGS}
  BUILD_COMMAND   ${CMAKE_COMMAND} -E env PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig make -j${NCORES}
  INSTALL_COMMAND make install
)