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

cmake_minimum_required(VERSION 3.18)
project("LibRA Apps"
  DESCRIPTION "LibRA CL, C/C++, Py Interfaces"
  HOMEPAGE_URL https://github.com/ARDG-NRAO/LibRA
  LANGUAGES CXX Fortran C)



# Path and directory setup
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.cmake)

# Load project configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libraconfig.cmake)

# Use spack environment if set
if(LIBRA_USE_SPACK)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Findspackenv.cmake)
endif()

<<<<<<< HEAD
option(USE_CCACHE "Enable use of ccache for compilation" OFF)
if (USE_CCACHE)
  # Enable ccache for faster builds
  include(cmake/EnableCcache.cmake)
  set(CCACHE_LAUNCHER "-DCMAKE_CXX_COMPILER_LAUNCHER=${CCACHE_PROGRAM} -DCMAKE_C_COMPILER_LAUNCHER=${CCACHE_PROGRAM}")
  message("${LIBRA_ID} Adding CCACHE_LAUNCHER flag ${CCACHE_LAUNCHER}")
endif()
#
#------------------------------------------------------------------------------
# Various repositories and the tags we dependend on
#
set(LIBRA_KOKKOS_GIT_REPOSITORY   "https://github.com/kokkos/kokkos.git")
set(LIBRA_KOKKOS_GIT_TAG          "4.6.01")

set(LIBRA_HPG_GIT_REPOSITORY      "https://gitlab.nrao.edu/mpokorny/hpg.git")
#set(LIBRA_HPG_GIT_REPOSITORY      "https://gitlab.com/dsa-2000/rcp/hpg")
set(LIBRA_HPG_GIT_TAG             "v2.0.2")

set(LIBRA_CASACORE_GIT_REPOSITORY "https://github.com/casacore/casacore.git")
set(LIBRA_CASACORE_GIT_TAG	  "v3.7.1")
=======
# Print summary of machine and LIBRA configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PrintLibraSummary.cmake)
print_libra_summary()
>>>>>>> main


# Include dependencies
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/Kokkos.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/FFTW.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/HPG.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/Casacore.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/Parafeed.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/Pybind11.cmake)

# Build gtest on if tests are enabled
if(Apps_BUILD_TESTS)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/GTest.cmake)
endif()

# Include components
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components/Casacpp.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components/Apps.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components/Frameworks.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components/Scripts.cmake)

# Exodus - Needs to be here so Apps is created first.
if(LIBRA_USE_EXODUS)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/exodus.cmake)
endif()
