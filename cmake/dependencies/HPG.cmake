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
# HPG external project
include(ExternalProject)

ExternalProject_Add(
  Hpg
  GIT_REPOSITORY ${LIBRA_HPG_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_HPG_GIT_TAG}
  DEPENDS        Kokkos FFTW_double
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/HPG
  BINARY_DIR     ${BUILD_DIR}/HPG
  CONFIGURE_COMMAND PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH} CMAKE_PREFIX_PATH=${INSTALL_DIR}:${CMAKE_PREFIX_PATH}:${INSTALL_DIR}/lib/cmake/Kokkos
                    ${CMAKE_COMMAND} <SOURCE_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_BINDIR=bin/Hpg -DCMAKE_BUILD_TYPE=Release
                    -DFFTW_LIBRARIES=${INSTALL_DIR}/lib/ -DFFTW_ROOT=${INSTALL_DIR} -DHpg_ENABLE_MIXED_PRECISION=OFF -DHpg_ENABLE_EXPERIMENTAL_IMPLEMENTATIONS=OFF -DHpg_API=17
                    ${HPG_CUDA_COMPILER_FLAGS}
                    -DHpg_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=ON ${HPG_CUDA_ENABLE_FLAGS}
                    ${KOKKOS_WRAPPER_FLAGS}
                    -DKokkos_DIR=${INSTALL_DIR}/lib/cmake/Kokkos ${CCACHE_LAUNCHER}
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
)
