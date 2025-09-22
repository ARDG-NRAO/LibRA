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
# Path and directory setup for LibRA

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
  message("   Project: ${PROJECT_NAME}")
  message("   Description: ${PROJECT_DESCRIPTION}")
  message("   Home: ${PROJECT_HOMEPAGE_URL}")
  message("   Build Dir: ${BUILD_DIR}")
  message("   Install Dir: ${INSTALL_DIR}")
  message("   C++ Standard: ${CMAKE_CXX_STANDARD}")
  message("   Build Type: ${BUILD_TYPE}")
  message("   CASA Build Type: ${CASA_BUILD_TYPE}")
  message("   Number of Cores: ${NPROCS}")
  message("")
  message("   OS Name: ${OS_NAME}")
  message("   OS Release: ${OS_RELEASE}")
  message("   OS Version: ${OS_VERSION}")
  message("   OS Platform: ${OS_PLATFORM}")
  message("   Available Physical Memory: ${AVAILABLE_PHYSICAL_MEMORY} BYTES")
  message("   Total Physical Memory: ${TOTAL_PHYSICAL_MEMORY} BYTES")
  message("   Total Virtual Memory: ${TOTAL_VIRTUAL_MEMORY} BYTES")
  message("")
  message("   Kokkos: ${LIBRA_KOKKOS_GIT_REPOSITORY} @ ${LIBRA_KOKKOS_GIT_TAG}")
  message("   HPG: ${LIBRA_HPG_GIT_REPOSITORY} @ ${LIBRA_HPG_GIT_TAG}")
  message("   Casacore: ${LIBRA_CASACORE_GIT_REPOSITORY} @ ${LIBRA_CASACORE_GIT_TAG}")
  message("   Parafeed: ${LIBRA_PARAFEED_GIT_REPOSITORY} @ ${LIBRA_PARAFEED_GIT_TAG}")
  message("   Pybind11: ${LIBRA_PYBIND11_GIT_REPOSITORY} @ ${LIBRA_PYBIND11_GIT_TAG}")
  message("   FFTW: ${LIBRA_FFTW_URL}")
  message("   GTest: ${LIBRA_GTEST_GIT_REPOSITORY} @ ${LIBRA_GTEST_GIT_TAG}")
  message("   Exodus: ${LIBRA_EXODUS_GIT_REPOSITORY} @ ${LIBRA_EXODUS_GIT_TAG}")
  message("   Measures FTP: ${LIBRA_MEASURES_FTP}")
  message("   Measures HTTPS: ${LIBRA_MEASURES_HTTPS}")
  message("")
  if("${Kokkos_CUDA_ARCH_NAME}" STREQUAL "")
    message("   Kokkos_CUDA_ARCH_NAME: (no arch provided, will autodetect current CUDA arch)")
  else()
    message("   Kokkos_CUDA_ARCH_NAME: ${Kokkos_CUDA_ARCH_NAME}")
  endif()
  message("   USE_CCACHE: ${USE_CCACHE}")
  message("   LIBRA_USE_LIBSAKURA: ${LIBRA_USE_LIBSAKURA}")
  message("   LIBRA_USE_EXODUS: ${LIBRA_USE_EXODUS}")
  message("   LIBRA_USE_SPACK: ${LIBRA_USE_SPACK}")
  message("   LIBRA_DISABLE_CUDA: ${LIBRA_DISABLE_CUDA}")
  message("   CASACORE_DATA_DOWNLOAD: ${CASACORE_DATA_DOWNLOAD}")
  message("   Apps_BUILD_TESTS: ${Apps_BUILD_TESTS}")
  message("=====================================================================")
  message("")
endmacro()
