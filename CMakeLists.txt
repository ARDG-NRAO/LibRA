# LibRA Main CMakeLists.txt - Clean modular approach
# Copyright 2025 Associated Universities, Inc. Washington DC, USA.

cmake_minimum_required(VERSION 3.18)
project("LibRA Apps"
    DESCRIPTION "LibRA CL, C/C++, Py Interfaces"
    HOMEPAGE_URL https://github.com/ARDG-NRAO/LibRA
    LANGUAGES CXX Fortran C)



# Path and directory setup
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.cmake)
message("================ Top-level CMAKE_MODULE_PATH after paths.cmake ================")
message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message("Testing if FindWCSLIB.cmake can be found...")
find_file(WCSLIB_FIND_MODULE FindWCSLIB.cmake PATHS ${CMAKE_MODULE_PATH})
message("FindWCSLIB.cmake found at: ${WCSLIB_FIND_MODULE}")
message("Testing find_package(WCSLIB) at top level...")
find_package(WCSLIB QUIET)
message("Top-level WCSLIB_FOUND: ${WCSLIB_FOUND}")
message("================ End MODULE_PATH debug ================")

# Load project configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libraconfig.cmake)

# Use spack environment if set
if(LIBRA_USE_SPACK)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Findspackenv.cmake)
endif()

# Print summary of machine and LIBRA configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PrintLibraSummary.cmake)
print_libra_summary()

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
