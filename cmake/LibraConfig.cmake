# LibraConfig.cmake - Essential configuration for LibRA
# Copyright 2025 Associated Universities, Inc. Washington DC, USA.

# Project identification
set(LIBRA_ID "LibRA INFO: ")

# Build configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_TYPE "RelWithDebInfo")
set(CASA_BUILD_TYPE "RelWithDebInfo")

# Directory configuration
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install)
set(TOP_LEVEL_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Standard CMake install directories
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

# Build options
option(USE_CCACHE "Enable ccache for faster builds" OFF)
option(LIBRA_USE_LIBSAKURA "Enable Sakura library support" OFF)
option(LIBRA_USE_EXODUS "Enable Exodus bundling support" OFF)
option(CASACORE_DATA_DOWNLOAD "Download CASACORE measures data" ON)
option(Apps_BUILD_TESTS "Build application tests" OFF)

# Dependency versions
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

# System detection
if(NOT DEFINED NCORES)
    cmake_host_system_information(RESULT NCORES QUERY NUMBER_OF_PHYSICAL_CORES)
    math(EXPR NCORES "${NCORES} - 2")
    if(NCORES LESS 1)
        set(NCORES 1)
    endif()
endif()

# SPACK environment detection
find_package(SPACKENV QUIET)
if(SPACKENV_FOUND)
    set(SPACK_FLAGS "-DCMAKE_CXX_FLAGS=-I${SPACK_VIEW_ROOT}/include -L${SPACK_VIEW_ROOT}/lib64 -L${SPACK_VIEW_ROOT}/lib")
    message("${LIBRA_ID} SPACK environment detected")
else()
    set(SPACK_FLAGS "")
endif()

# CCACHE setup
if(USE_CCACHE)
    include(cmake/EnableCcache.cmake)
    if(CCACHE_PROGRAM)
        set(CCACHE_LAUNCHER "-DCMAKE_CXX_COMPILER_LAUNCHER=${CCACHE_PROGRAM} -DCMAKE_C_COMPILER_LAUNCHER=${CCACHE_PROGRAM}")
        message("${LIBRA_ID} CCACHE enabled: ${CCACHE_PROGRAM}")
    else()
        set(CCACHE_LAUNCHER "")
    endif()
else()
    set(CCACHE_LAUNCHER "")
endif()

# Environment setup for CUDA
set(ENV{PATH} $ENV{PATH}:/usr/local/cuda/bin)
list(APPEND CMAKE_PROGRAM_PATH "/usr/local/cuda/bin")

# PKG_CONFIG_PATH setup - ensure our packages take precedence
set(ENV{PKG_CONFIG_PATH} "${INSTALL_DIR}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

message("${LIBRA_ID} Build directory: ${BUILD_DIR}")
message("${LIBRA_ID} Install directory: ${INSTALL_DIR}")
message("${LIBRA_ID} Using ${NCORES} cores for parallel builds")