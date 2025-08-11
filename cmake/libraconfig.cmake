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
# Macro to print LIBRA and machine configuration summary
macro(print_libra_summary)
  cmake_host_system_information(RESULT OS_NAME QUERY OS_NAME)
  cmake_host_system_information(RESULT OS_RELEASE QUERY OS_RELEASE)
  cmake_host_system_information(RESULT OS_VERSION QUERY OS_VERSION)
  cmake_host_system_information(RESULT OS_PLATFORM QUERY OS_PLATFORM)
  cmake_host_system_information(RESULT NPROCS QUERY NUMBER_OF_LOGICAL_CORES)
  cmake_host_system_information(RESULT AVAILABLE_PHYSICAL_MEMORY QUERY AVAILABLE_PHYSICAL_MEMORY)
  cmake_host_system_information(RESULT TOTAL_PHYSICAL_MEMORY QUERY TOTAL_PHYSICAL_MEMORY)
  cmake_host_system_information(RESULT TOTAL_VIRTUAL_MEMORY QUERY TOTAL_VIRTUAL_MEMORY)

  message("")
  message("==================== LIBRA CONFIGURATION SUMMARY ====================")
  message("  Project: ${PROJECT_NAME}")
  message("  Description: ${PROJECT_DESCRIPTION}")
  message("  Home: ${PROJECT_HOMEPAGE_URL}")
  message("  Build Dir: ${BUILD_DIR}")
  message("  Install Dir: ${INSTALL_DIR}")
  message("  C++ Standard: ${CMAKE_CXX_STANDARD}")
  message("  Build Type: ${BUILD_TYPE}")
  message("  CASA Build Type: ${CASA_BUILD_TYPE}")
  message("  Number of Cores: ${NPROCS}")
  message("")
  message("  OS Name: ${OS_NAME}")
  message("  OS Release: ${OS_RELEASE}")
  message("  OS Version: ${OS_VERSION}")
  message("  OS Platform: ${OS_PLATFORM}")
  message("  Available Physical Memory: ${AVAILABLE_PHYSICAL_MEMORY} BYTES")
  message("  Total Physical Memory: ${TOTAL_PHYSICAL_MEMORY} BYTES")
  message("  Total Virtual Memory: ${TOTAL_VIRTUAL_MEMORY} BYTES")
  message("")
  message("  Kokkos: ${LIBRA_KOKKOS_GIT_REPOSITORY} @ ${LIBRA_KOKKOS_GIT_TAG}")
  message("  HPG: ${LIBRA_HPG_GIT_REPOSITORY} @ ${LIBRA_HPG_GIT_TAG}")
  message("  Casacore: ${LIBRA_CASACORE_GIT_REPOSITORY} @ ${LIBRA_CASACORE_GIT_TAG}")
  message("  Parafeed: ${LIBRA_PARAFEED_GIT_REPOSITORY} @ ${LIBRA_PARAFEED_GIT_TAG}")
  message("  Pybind11: ${LIBRA_PYBIND11_GIT_REPOSITORY} @ ${LIBRA_PYBIND11_GIT_TAG}")
  message("  FFTW: ${LIBRA_FFTW_URL}")
  message("  GTest: ${LIBRA_GTEST_GIT_REPOSITORY} @ ${LIBRA_GTEST_GIT_TAG}")
  message("  Measures FTP: ${LIBRA_MEASURES_FTP}")
  message("  Measures HTTPS: ${LIBRA_MEASURES_HTTPS}")
  message("")
  message("  Kokkos_CUDA_ARCH_NAME: ${Kokkos_CUDA_ARCH_NAME}")
  message("  USE_CCACHE: ${USE_CCACHE}")
  message("  LIBRA_USE_LIBSAKURA: ${LIBRA_USE_LIBSAKURA}")
  message("  LIBRA_USE_EXODUS: ${LIBRA_USE_EXODUS}")
  message("  CASACORE_DATA_DOWNLOAD: ${CASACORE_DATA_DOWNLOAD}")
  message("  Apps_BUILD_TESTS: ${Apps_BUILD_TESTS}")
  message("=====================================================================")
  message("")
endmacro()
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




