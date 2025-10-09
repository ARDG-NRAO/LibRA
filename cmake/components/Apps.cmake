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
# Apps component
include(ExternalProject)

# Find WCSLIB for passing to Apps
find_package(WCSLIB REQUIRED)

# Build dependency list based on enabled features
set(APPS_DEPENDS Casacpp Hpg Parafeed Casacore Pybind11)
set(APPS_CMAKE_PREFIX_PATH ${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos)
if(LIBRA_BUILD_TESTING)
  list(APPEND APPS_DEPENDS GTest)
  set(APPS_CMAKE_PREFIX_PATH ${APPS_CMAKE_PREFIX_PATH}:${INSTALL_DIR}/lib/cmake/GTest)
endif()

# Debug output
message(STATUS "Apps.cmake: WCSLIB_FOUND = ${WCSLIB_FOUND}")
message(STATUS "Apps.cmake: WCSLIB_INCLUDE_DIRS = ${WCSLIB_INCLUDE_DIRS}")
message(STATUS "Apps.cmake: WCSLIB_LIBRARIES = ${WCSLIB_LIBRARIES}")

ExternalProject_Add(
  Apps
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/apps
  DOWNLOAD_COMMAND ""
  BINARY_DIR      ${BUILD_DIR}/Libra/apps
  DEPENDS         ${APPS_DEPENDS}
  CONFIGURE_COMMAND
    PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig
    CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:${APPS_CMAKE_PREFIX_PATH}
    ${CMAKE_COMMAND} <SOURCE_DIR>
      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
      -DCMAKE_INSTALL_LIBDIR=lib
      -DCMAKE_CXX_FLAGS=-I${INSTALL_DIR}/include
      -Dcasacore_DIR=${INSTALL_DIR}/lib/cmake/casacore
      -DKokkos_DIR=${INSTALL_DIR}/lib/cmake/Kokkos
      -DCMAKE_MODULE_PATH=<SOURCE_DIR>/../cmake/modules
      -DCMAKE_PREFIX_PATH=${APPS_CMAKE_PREFIX_PATH}
      -DApps_BUILD_TESTS=${Apps_BUILD_TESTS}
      -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
      -DWCSLIB_INCLUDE_DIRS=${WCSLIB_INCLUDE_DIRS}
      -DWCSLIB_LIBRARIES=${WCSLIB_LIBRARIES}
      -DWCSLIB_FOUND=${WCSLIB_FOUND}
      ${KOKKOS_WRAPPER_FLAGS}
      ${SPACK_FLAGS}
  BUILD_COMMAND   make -j${NCORES}
  INSTALL_COMMAND make install
)