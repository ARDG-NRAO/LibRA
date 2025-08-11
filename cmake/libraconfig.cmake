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

# Option for Kokkos CUDA architecture
set(Kokkos_CUDA_ARCH_NAME "" CACHE STRING "Kokkos CUDA architecture name, e.g. Kokkos_ARCH_AMPERE80")

# LibRA project configuration and global variables

set(LIBRA_ID "LibRA INFO: ")
set(CMAKE_CXX_STANDARD 17)
set(BUILD_TYPE "RelWithDebInfo")
set(CASA_BUILD_TYPE "RelWithDebInfo")

# Path and directory setup moved to paths.cmake
# include(${CMAKE_CURRENT_SOURCE_DIR}/paths.cmake)

# External project repositories and tags

set(LIBRA_KOKKOS_GIT_REPOSITORY   "https://github.com/kokkos/kokkos.git")
set(LIBRA_KOKKOS_GIT_TAG          "4.6.01")
set(LIBRA_HPG_GIT_REPOSITORY      "https://gitlab.nrao.edu/mpokorny/hpg.git")
set(LIBRA_HPG_GIT_TAG             "v3.0.3")
set(LIBRA_CASACORE_GIT_REPOSITORY "https://github.com/casacore/casacore.git")
set(LIBRA_CASACORE_GIT_TAG        "v3.7.1")
set(LIBRA_PARAFEED_GIT_REPOSITORY "https://github.com/sanbee/parafeed.git")
set(LIBRA_PARAFEED_GIT_TAG        "v1.1.44")
set(LIBRA_SAKURA_GIT_REPOSITORY   "https://github.com/tnakazato/sakura.git")
set(LIBRA_SAKURA_GIT_TAG          "libsakura-5.1.3")
set(LIBRA_PYBIND11_GIT_REPOSITORY "https://github.com/pybind/pybind11.git")
set(LIBRA_PYBIND11_GIT_TAG        "v2.10.2")
set(LIBRA_FFTW_URL                "http://www.fftw.org/fftw-3.3.10.tar.gz")
set(LIBRA_GTEST_GIT_REPOSITORY    "https://github.com/google/googletest.git")
set(LIBRA_GTEST_GIT_TAG           "v1.14.0")
set(LIBRA_MEASURES_FTP            "ftp://ftp.astron.nl/outgoing/Measures/WSRT_Measures.ztar")
set(LIBRA_MEASURES_HTTPS          "https://www.astron.nl/iers/WSRT_Measures.ztar")
set(LIBRA_EXODUS_GIT_REPOSITORY   "https://github.com/sanbee/exodus_sbfork.git")
set(LIBRA_EXODUS_GIT_TAG          "master")
set(LIBRA_EXODUS_BUNDLE_NAME      "libra_apps.sh")
set(LIBRA_EXODUS_BUNDLE_PATH      "")

# Options
option(USE_CCACHE "Enable use of ccache for compilation" OFF)
option(LIBRA_USE_LIBSAKURA "Enable use of Sakura library" OFF)
option(LIBRA_USE_EXODUS "Enable use of Exodus" OFF)
option(CASACORE_DATA_DOWNLOAD "Enable download of casacore data" ON)

# System info

# Set NCORES to total logical cores minus two (minimum 1)
if (NOT DEFINED NCORES)
  cmake_host_system_information(RESULT NCORES QUERY NUMBER_OF_LOGICAL_CORES)
  math(EXPR NCORES "${NCORES} - 2")
  if (NCORES LESS 1)
    set(NCORES 1)
  endif()
endif()




