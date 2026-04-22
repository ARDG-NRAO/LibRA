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
# FFTW external project
include(ExternalProject)

set(LIBRA_FFTW_AUTOTOOLS_ENV ${LIBRA_EXTERNAL_AUTOTOOLS_COMPILER_ENV})
set(LIBRA_FFTW_OPENMP_OPTION --enable-openmp)

if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Apple Clang can use libomp for CMake-based projects, but FFTW's autotools
  # build leaks bare -fopenmp into libtool link lines, which fails on macOS.
  # Build the pthread-enabled FFTW variants instead; those are the ones used by
  # casacore/synthesis in this tree.
  set(LIBRA_FFTW_AUTOTOOLS_ENV "")
  if(CMAKE_C_COMPILER)
    list(APPEND LIBRA_FFTW_AUTOTOOLS_ENV "CC=${CMAKE_C_COMPILER}")
  endif()
  if(CMAKE_CXX_COMPILER)
    list(APPEND LIBRA_FFTW_AUTOTOOLS_ENV "CXX=${CMAKE_CXX_COMPILER}")
  endif()
  if(CMAKE_Fortran_COMPILER)
    list(APPEND LIBRA_FFTW_AUTOTOOLS_ENV "FC=${CMAKE_Fortran_COMPILER}")
  endif()
  set(LIBRA_FFTW_OPENMP_OPTION "")
endif()


# Download and extract FFTW only once
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.25")
  set(DOWNLOAD_EXTRACT_TIMESTAMP_KW DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
else()
  set(DOWNLOAD_EXTRACT_TIMESTAMP_KW)
endif()

ExternalProject_Add(
  FFTW_source
  URL             ${LIBRA_FFTW_URL}
  ${DOWNLOAD_EXTRACT_TIMESTAMP_KW}
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
  CONFIGURE_COMMAND ""
  BUILD_COMMAND    ""
  INSTALL_COMMAND  ""
)

# Build double precision (default)
ExternalProject_Add(
  FFTW_double
  DOWNLOAD_COMMAND ""
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
  BINARY_DIR      ${BUILD_DIR}/FFTW_double
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E sleep 2 && ${CMAKE_COMMAND} -E env ${LIBRA_FFTW_AUTOTOOLS_ENV} <SOURCE_DIR>/configure --enable-shared --enable-threads ${LIBRA_FFTW_OPENMP_OPTION} --prefix=${INSTALL_DIR} --bindir=${INSTALL_DIR}/bin/fftw --libdir=${INSTALL_DIR}/lib
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
  DEPENDS         FFTW_source
)

# Build single precision (float)
ExternalProject_Add(
  FFTW_float
  DOWNLOAD_COMMAND ""
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
  BINARY_DIR      ${BUILD_DIR}/FFTW_float
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E sleep 2 && ${CMAKE_COMMAND} -E env ${LIBRA_FFTW_AUTOTOOLS_ENV} <SOURCE_DIR>/configure --enable-shared --enable-threads ${LIBRA_FFTW_OPENMP_OPTION} --enable-float --prefix=${INSTALL_DIR} --bindir=${INSTALL_DIR}/bin/fftw --libdir=${INSTALL_DIR}/lib
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
  DEPENDS         FFTW_double
)
