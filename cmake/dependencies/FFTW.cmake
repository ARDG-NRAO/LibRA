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


# Download and extract FFTW only once
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.25")
  ExternalProject_Add(
    FFTW_source
    URL             ${LIBRA_FFTW_URL}
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
    CONFIGURE_COMMAND ""
    BUILD_COMMAND    ""
    INSTALL_COMMAND  ""
  )
else()
  ExternalProject_Add(
    FFTW_source
    URL             ${LIBRA_FFTW_URL}
    SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
    CONFIGURE_COMMAND ""
    BUILD_COMMAND    ""
    INSTALL_COMMAND  ""
  )
endif()

# Build double precision (default)
ExternalProject_Add(
  FFTW_double
  DOWNLOAD_COMMAND ""
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
  BINARY_DIR      ${BUILD_DIR}/FFTW_double
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E sleep 2 && <SOURCE_DIR>/configure --enable-shared --enable-threads --enable-openmp --prefix=${INSTALL_DIR} --bindir=${INSTALL_DIR}/bin/fftw --libdir=${INSTALL_DIR}/lib
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
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E sleep 2 && <SOURCE_DIR>/configure --enable-shared --enable-threads --enable-openmp --enable-float --prefix=${INSTALL_DIR} --bindir=${INSTALL_DIR}/bin/fftw --libdir=${INSTALL_DIR}/lib
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
  DEPENDS         FFTW_double
)
