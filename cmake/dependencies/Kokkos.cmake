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
# Kokkos external project
include(ExternalProject)

if(NOT "${Kokkos_CUDA_ARCH_NAME}" STREQUAL "")
  set(Kokkos_CUDA_ARCH_NAME_OPT "-D${Kokkos_CUDA_ARCH_NAME}=ON")
else()
  set(Kokkos_CUDA_ARCH_NAME_OPT "")
endif()
ExternalProject_Add(
  Kokkos
  GIT_REPOSITORY ${LIBRA_KOKKOS_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_KOKKOS_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/Kokkos
  BINARY_DIR     ${BUILD_DIR}/Kokkos
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/kokkos
                 ${KOKKOS_CUDA_COMPILER_FLAGS}
                 ${KOKKOS_CUDA_ENABLE_FLAGS}
                 ${Kokkos_CUDA_ARCH_NAME_OPT}
                 -DKokkos_ENABLE_OPENMP=ON
                 -DKokkos_ENABLE_SERIAL=ON
                 -DBUILD_SHARED_LIBS=ON
                 ${SPACK_FLAGS}
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
)
