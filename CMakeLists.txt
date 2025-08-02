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
#if cmake is greater than 3.24.0, set policy to old
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 OLD)
endif()

project("LibRA Apps"
  DESCRIPTION "LibRA CL, C/C++, Py Interfaces"
  HOMEPAGE_URL https://github.com/ARDG-NRAO/LibRA
  LANGUAGES CXX Fortran C)


set(LIBRA_ID "LibRA INFO: ")
set(CMAKE_CXX_STANDARD 17)
set(BUILD_TYPE "RelWithDebInfo")
set(CASA_BUILD_TYPE "RelWithDebInfo")

# Set the top-level build and install directories
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install)
set(TOP_LEVEL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
 set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
# LibRA location to find Find<PACKAGE>.cmake files 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

message("${LIBRA_ID} CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message("${LIBRA_ID} BUILD_DIR: ${BUILD_DIR}")
message("${LIBRA_ID} INSTALL_DIR: ${INSTALL_DIR}")
message("${LIBRA_ID} Installing all libraries to ${INSTALL_DIR}/lib")
message("${LIBRA_ID} Installing all headers to ${INSTALL_DIR}/include")
message("${LIBRA_ID} Installing all binaries to ${INSTALL_DIR}/bin")

# Include the ExternalProject module
include(ExternalProject)
include(FindPkgConfig)


set(ENV{PATH} $ENV{PATH}:/usr/local/cuda/bin)
message("${LIBRA_ID} PATH=$ENV{PATH}")
LIST(APPEND CMAKE_PROGRAM_PATH  "/usr/local/cuda/bin" ...)
# pkg_check_modules(WCSLIB REQUIRED wcslib) # we are finding wcs in the libracore CMakeLists.txt

#Check if it's an SPACK env. If it is, use env root as /usr and / for
#finding libs and include files.
find_package(SPACKENV)
if (SPACKENV_FOUND)
  set(SPACK_FLAGS "-DCMAKE_CXX_FLAGS=-I${SPACK_VIEW_ROOT}/include  -L${SPACK_VIEW_ROOT}/lib64 -L${SPACK_VIEW_ROOT}/lib ")
  message("${LIBRA_ID} Adding SPACK flags ${SPACK_FLAGS}")
endif()

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
set(LIBRA_HPG_GIT_TAG             "v3.0.3")

set(LIBRA_CASACORE_GIT_REPOSITORY "https://github.com/casacore/casacore.git")
set(LIBRA_CASACORE_GIT_TAG	  "v3.7.1")


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
set(LIBRA_EXODUS_BUNDLE_PATH       "")
#------------------------------------------------------------------------------
#


if(NOT DEFINED LIBRA_USE_LIBSAKURA)
  set(LIBRA_USE_LIBSAKURA OFF)
endif()

if(NOT DEFINED LIBRA_USE_EXODUS)
  set(LIBRA_USE_EXODUS OFF)
endif()

if(NOT DEFINED CASACORE_DATA_DOWNLOAD)
  set(CASACORE_DATA_DOWNLOAD ON)
endif()


# Find system information
if (NOT DEFINED NCORES)
	cmake_host_system_information(RESULT NCORES QUERY NUMBER_OF_PHYSICAL_CORES)
	cmake_host_system_information(RESULT NTHREADS QUERY NUMBER_OF_LOGICAL_CORES)
	message("${LIBRA_ID} Number of physical cores: ${NCORES}")
	message("${LIBRA_ID} Number of logical cores: ${NTHREADS}")
  	math(EXPR NCORES "${NCORES} - 2")
endif()

cmake_host_system_information(RESULT OS_NAME QUERY OS_NAME)
cmake_host_system_information(RESULT OS_RELEASE QUERY OS_RELEASE)
cmake_host_system_information(RESULT OS_VERSION QUERY OS_VERSION)
cmake_host_system_information(RESULT OS_PLATFORM QUERY OS_PLATFORM)
cmake_host_system_information(RESULT AVAILABLE_PHYSICAL_MEMORY QUERY AVAILABLE_PHYSICAL_MEMORY)
cmake_host_system_information(RESULT TOTAL_PHYSICAL_MEMORY QUERY TOTAL_PHYSICAL_MEMORY)
cmake_host_system_information(RESULT TOTAL_VIRTUAL_MEMORY QUERY TOTAL_VIRTUAL_MEMORY)


message("${LIBRA_ID} OS Name: ${OS_NAME}")
message("${LIBRA_ID} OS Release: ${OS_RELEASE}")
message("${LIBRA_ID} OS Version: ${OS_VERSION}")
message("${LIBRA_ID} OS Platform: ${OS_PLATFORM}")
message("${LIBRA_ID} Available Physical Memory: ${AVAILABLE_PHYSICAL_MEMORY} BYTES")
message("${LIBRA_ID} Total Physical Memory: ${TOTAL_PHYSICAL_MEMORY} BYTES")
if ($NCORES LESS 1)
  set(NCORES 1)
endif()
message(${LIBRA_ID} "Setting number of cores to ${NCORES}")

message("${LIBRA_ID} LIBRA_USE_EXODUS: ${LIBRA_USE_EXODUS}")  

# Set the PKG_CONFIG_PATH environment variable
set(ENV{PKG_CONFIG_PATH} "${CMAKE_SOURCE_DIR}/install/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

# Add Kokkos as an external project
if(NOT "${Kokkos_CUDA_ARCH_NAME}" STREQUAL "")
  message("${LIBRA_ID} Kokkos_CUDA_ARCH_NAME: ${Kokkos_CUDA_ARCH_NAME}")
  set(Kokkos_CUDA_ARCH_NAME_OPT "-D${Kokkos_CUDA_ARCH_NAME}=ON")
else()
  message("${LIBRA_ID} Kokkos_CUDA_ARCH_NAME is empty. Will automatically detect the CUDA architecture")
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
                   -DCMAKE_CXX_COMPILER=${CMAKE_SOURCE_DIR}/dependencies/Kokkos/bin/nvcc_wrapper
                   -DKokkos_ENABLE_CUDA_LAMBDA=ON
  	           -DKokkos_ENABLE_CUDA=ON
	           ${Kokkos_CUDA_ARCH_NAME_OPT}
  	           -DKokkos_ENABLE_OPENMP=ON
  	           -DKokkos_ENABLE_SERIAL=ON
  	           -DBUILD_SHARED_LIBS=ON
                   ${SPACK_FLAGS}
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
  )

# Add FFTW as an external project
ExternalProject_Add(
  FFTW
  URL             ${LIBRA_FFTW_URL}
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
  BINARY_DIR      ${BUILD_DIR}/FFTW
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --enable-shared --enable-threads --enable-openmp --prefix=${INSTALL_DIR} --bindir=${INSTALL_DIR}/bin/fftw --libdir=${INSTALL_DIR}/lib
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
)

# Get the install directory of Kokkos
ExternalProject_Get_Property(Kokkos install_dir)

# Add HPG as an external project
ExternalProject_Add(
  Hpg
  GIT_REPOSITORY ${LIBRA_HPG_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_HPG_GIT_TAG}
  DEPENDS        Kokkos FFTW
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/HPG
  BINARY_DIR     ${BUILD_DIR}/HPG
  CONFIGURE_COMMAND PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH} CMAKE_PREFIX_PATH=${INSTALL_DIR}:${CMAKE_PREFIX_PATH}:${INSTALL_DIR}/lib/cmake/Kokkos
                    ${CMAKE_COMMAND} <SOURCE_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_BINDIR=bin/Hpg -DCMAKE_BUILD_TYPE=Release
		    -DFFTW_LIBRARIES=${INSTALL_DIR}/lib/ -DFFTW_ROOT=${INSTALL_DIR} -DHpg_ENABLE_MIXED_PRECISION=OFF -DHpg_ENABLE_EXPERIMENTAL_IMPLEMENTATIONS=OFF -DHpg_API=17
		    -DHpg_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_CXX_COMPILER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper
		    -DKokkos_DIR=${INSTALL_DIR}/lib/cmake/Kokkos ${CCACHE_LAUNCHER}
  BUILD_COMMAND   make -j ${NCORES};
  INSTALL_COMMAND make install
)

# Add parafeed as an external project
ExternalProject_Add(
  Parafeed
  GIT_REPOSITORY ${LIBRA_PARAFEED_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_PARAFEED_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/parafeed
  BINARY_DIR     ${BUILD_DIR}/parafeed
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} 
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/parafeed
		 ${CCACHE_LAUNCHER}
                 -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
                 -DBUILD_SHARED_LIBS=ON
		 -DGNUREADLINE=ON
                  ${SPACK_FLAGS}
  BUILD_COMMAND make -j ${NCORES}
  INSTALL_COMMAND make install
)

# Create the path ${CMAKE_SOURCE_DIR}/install/data and download the data into it
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/install/data)
# Create an external project for the data download
if(CASACORE_DATA_DOWNLOAD)
  message("${LIBRA_ID} CASACORE_DATA_DOWNLOAD is ON. Downloading measures data...")
  ExternalProject_Add(
    DataDownload
    DOWNLOAD_COMMAND bash -c "curl ${LIBRA_MEASURES_FTP} | tar -C ${CMAKE_SOURCE_DIR}/install/data -xzf - || curl ${LIBRA_MEASURES_HTTPS} | tar -C ${CMAKE_SOURCE_DIR}/install/data -xzf -"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
else()
  message("${LIBRA_ID} CASACORE_DATA_DOWNLOAD is OFF. Skipping measures data download.")
  # Create a dummy DataDownload target
  ExternalProject_Add(
    DataDownload
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
endif()

# Create the Casacore external project
ExternalProject_Add(
  Casacore
  GIT_REPOSITORY ${LIBRA_CASACORE_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_CASACORE_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/casacore
  BINARY_DIR     ${BUILD_DIR}/casacore
  CONFIGURE_COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/dependencies/casacore
                    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/install
                    -DCMAKE_INSTALL_LIBDIR=lib
                    -DCMAKE_INSTALL_BINDIR=bin/casacore
                    -DDATA_DIR=${CMAKE_SOURCE_DIR}/install/data
                    -DUSE_OPENMP=ON
                    -DUSE_THREADS=ON
                    -DCMAKE_CXX_FLAGS="-fno-tree-vrp"
                    -DBUILD_FFTPACK_DEPRECATED=ON
                    -DBUILD_TESTING=OFF
                    -DBUILD_PYTHON=OFF
                    -DBUILD_PYTHON3=OFF
                    -DBUILD_DYSCO=ON
                    -DUseCcache=0 
                    -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
		    ${CCACHE_LAUNCHER}
                    ${SPACK_FLAGS}
  BUILD_COMMAND  make -j${NCORES}
  INSTALL_COMMAND make install
  DEPENDS DataDownload
)


if (LIBRA_USE_LIBSAKURA)
  #Add Sakura as an external project
  ExternalProject_Add(
    Sakura
    GIT_REPOSITORY ${LIBRA_SAKURA_GIT_REPOSITORY}
    GIT_TAG        4{LIBRA_SAKURA_GIT_TAG}
    SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/sakura
    BINARY_DIR     ${BUILD_DIR}/sakura
    SOURCE_SUBDIR  libsakura
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                   -DCMAKE_INSTALL_LIBDIR=lib
                   -DCMAKE_INSTALL_BINDIR=bin/sakura
                   -DBUILD_DOC:BOOL=OFF
                   -DPYTHON_BINDING:BOOL=OFF
                   -DSIMD_ARCH=GENERIC
                   -DENABLE_TEST:BOOL=OFF
    BUILD_COMMAND   make -j ${NCORES};
    INSTALL_COMMAND make install
    )
else()
  message("${LIBRA_ID} LIBRA_USE_LIBSAKURA is ${LIBRA_USE_LIBSAKURA}.")
endif()
# Add pybind11 as an external project
ExternalProject_Add(
  Pybind11
  GIT_REPOSITORY ${LIBRA_PYBIND11_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_PYBIND11_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/pybind11
  BINARY_DIR     ${BUILD_DIR}/pybind11
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/pybind11
		 ${CCACHE_LAUNCHER}
                 -DPYBIND11_TEST=OFF
  BUILD_COMMAND   make -j ${NCORES}; 
  INSTALL_COMMAND make install
)

#if(Apps_BUILD_TESTS)
# Add GTest as an external project
ExternalProject_Add(
  GTest
  GIT_REPOSITORY ${LIBRA_GTEST_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_GTEST_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/gtest
  BINARY_DIR     ${BUILD_DIR}/gtest
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/gtest
		 ${CCACHE_LAUNCHER}
                 -DBUILD_GMOCK=OFF
                 -DBUILD_GTEST=ON
                 -DBUILD_SHARED_LIBS=ON
  BUILD_COMMAND   make -j ${ncores};
  INSTALL_COMMAND make install
)
#endif()
# Add Libra/src as an external project
if (LIBRA_USE_LIBSAKURA)
  set(SAKURA_DEPENDENCE "Sakura")
else()
  set(SAKURA_DEPENDENCE "")
endif()

include(FindPkgConfig)
pkg_check_modules(GSL REQUIRED gsl)
ExternalProject_Add(
  Casacpp
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/src/
  DOWNLOAD_COMMAND ""
  BINARY_DIR      ${BUILD_DIR}/Libra/src
  DEPENDS         Casacore ${SAKURA_DEPENDENCE} Hpg Parafeed Pybind11
  #DEPENDS         Casacore Hpg Parafeed Pybind11
  CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                  -DCMAKE_INSTALL_LIBDIR=lib
                  -DCMAKE_INSTALL_BINDIR=bin/casacpp
                  -DCMAKE_CXX_FLAGS=-I${INSTALL_DIR}/include
                  -DHpg_DIR=${INSTALL_DIR}/lib/cmake/Hpg/
	                -Dparafeed_DIR=${INSTALL_DIR}/lib/cmake/parafeed/
                  -DKokkos_COMPILE_LAUNCHER=${INSTALL_DIR}/bin/kokkos/kokkos_launch_compiler
                  -DKokkos_NVCC_WRAPPER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper
                  -DCMAKE_PREFIX_PATH=${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos
		              -DGSL_INCLUDE_DIR=${GSL_INCLUDEDIR}
		              -DGSL_LIBRARY=${GSL_LIBRARY_DIRS}
		              -DGSL_CBLAS_LIBRARY=${GSL_LIBRARY_DIRS}
			      ${SPACK_FLAGS}
  CONFIGURE_COMMAND PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig
  CMAKE_PREFIX_PATH=${INSTALL_DIR}:${CMAKE_PREFIX_PATH}:${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos
                                    ${CMAKE_COMMAND} <SOURCE_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_BINDIR=bin/casacpp
  -DCMAKE_CXX_FLAGS=-I${INSTALL_DIR}/include
                                    ${CCACHE_LAUNCHER} -DCMAKE_MODULE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/src/cmake -DHpg_DIR=${INSTALL_DIR}/lib/cmake/Hpg/
  -Dparafeed_DIR=${INSTALL_DIR}/lib/cmake/parafeed/ -DKokkos_COMPILE_LAUNCHER=${INSTALL_DIR}/bin/kokkos/kokkos_launch_compiler
                                    -DKokkos_NVCC_WRAPPER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper -DFFTW_ROOT=${INSTALL_DIR}
                                    -DCMAKE_PREFIX_PATH=${INSTALL_DIR}:${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos
  -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
                                    -DGSL_INCLUDE_DIR=${GSL_INCLUDEDIR}
                                    -DGSL_LIBRARY=${GSL_LIBRARY_DIRS}
                                    -DGSL_CBLAS_LIBRARY=${GSL_LIBRARY_DIRS} ${SPACK_FLAGS}

  BUILD_COMMAND   ${CMAKE_COMMAND} -E env PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig make -j${NCORES}
  INSTALL_COMMAND make install
)

# If not defined on the command line, switch off apps testing
if(NOT DEFINED Apps_BUILD_TESTS)
  set(Apps_BUILD_TESTS OFF)
else()
  # Update git submodules
  execute_process(
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif()
message ("${LIBRA_ID} Apps_BUILD_TESTS=${Apps_BUILD_TESTS}")
message ("Pulling in the tests data from libra-tests as a submodule")
# Add Libra/apps as an external project
ExternalProject_Add(
    Apps
    SOURCE_DIR      ${CMAKE_SOURCE_DIR}/apps
    DOWNLOAD_COMMAND ""
    BINARY_DIR      ${BUILD_DIR}/Libra/apps
    DEPENDS         Casacpp Hpg Parafeed Casacore Pybind11 GTest
    CONFIGURE_COMMAND PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos 
                                      ${CMAKE_COMMAND} <SOURCE_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_CXX_FLAGS=-I${INSTALL_DIR}/include -Dcasacore_DIR=${INSTALL_DIR}/lib/cmake/casacore 
		                                  -DCMAKE_PREFIX_PATH=${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos:${INSTALL_DIR}/lib/cmake/GTest -DApps_BUILD_TESTS=${Apps_BUILD_TESTS} 
                                      -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE} -DKokkos_COMPILE_LAUNCHER=${INSTALL_DIR}/bin/kokkos/kokkos_launch_compiler -DKokkos_NVCC_WRAPPER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper ${SPACK_FLAGS}
    BUILD_COMMAND   make -j${NCORES}
    INSTALL_COMMAND make install
)

# Add Libra/frameworks as an external project
ExternalProject_Add(
  Frameworks
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/frameworks
  DOWNLOAD_COMMAND ""
  BINARY_DIR      ${BUILD_DIR}/Libra/frameworks
  DEPENDS         Apps
  CONFIGURE_COMMAND ${CMAKE_COMMAND} <SOURCE_DIR> -B <BINARY_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DTOP_LEVEL_DIR=${TOP_LEVEL_DIR} -DINSTALL_DIR=${INSTALL_DIR}
  BUILD_COMMAND   make -j${NCORES} -C <BINARY_DIR>
  INSTALL_COMMAND make install -C <BINARY_DIR>
)

# Add Libra/scripts as an external project
ExternalProject_Add(
  Scripts
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/scripts
  DOWNLOAD_COMMAND ""
  BINARY_DIR      ${BUILD_DIR}/Libra/scripts
  DEPENDS         Frameworks
  CONFIGURE_COMMAND ${CMAKE_COMMAND} <SOURCE_DIR> -B <BINARY_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DTOP_LEVEL_DIR=${TOP_LEVEL_DIR} -DINSTALL_DIR=${INSTALL_DIR}
  BUILD_COMMAND   make -j${NCORES} -C <BINARY_DIR>
  INSTALL_COMMAND make install -C <BINARY_DIR>
)

if (LIBRA_USE_EXODUS)
  # setup a python vitual environment inside the install directory
  # after checking if python3 is installed
  find_package(Python3 COMPONENTS Interpreter Development)
  #if (NOT Python3_FOUND)
  #  message(FATAL_ERROR "Python3 not found. Please install python3 to use exodus")
  # else()
  #  message("${LIBRA_ID} Python3 found at ${Python3_EXECUTABLE}")
  #  message("${LIBRA_ID} Python3 version: ${Python3_VERSION}")
  message("Setting up librapython venv")
    execute_process(
      COMMAND python3 -m venv ${CMAKE_SOURCE_DIR}/install/librapython
      COMMAND source ${CMAKE_SOURCE_DIR}/install/librapython/bin/activate
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/install
     
    ) 
  # Add exodus as an external project install the python environment
  if (LIBRA_EXODUS_BUNDLE_PATH STREQUAL "")
    set(LIBRA_EXODUS_BUNDLE_PATH ${CMAKE_SOURCE_DIR}/install/bin)
  endif()
  set(EXODUS_APPS "roadrunner,hummbee,coyote,chip,dale,dataiter,mssplit,subms,tableinfo,acme")
    ExternalProject_Add(
      Exodus
      GIT_REPOSITORY ${LIBRA_EXODUS_GIT_REPOSITORY}
      GIT_TAG        ${LIBRA_EXODUS_GIT_TAG}
      SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/exodus
      CONFIGURE_COMMAND python3 -m venv ${CMAKE_SOURCE_DIR}/install/librapython && source ${CMAKE_SOURCE_DIR}/install/librapython/bin/activate
      BUILD_COMMAND PYTHONUSERBASE=${CMAKE_SOURCE_DIR}/install/librapython ${CMAKE_SOURCE_DIR}/install/librapython/bin/pip3 install --user ${CMAKE_SOURCE_DIR}/dependencies/exodus/
      INSTALL_COMMAND ${CMAKE_SOURCE_DIR}/install/librapython/bin/exodus ${CMAKE_SOURCE_DIR}/install/bin/{${EXODUS_APPS}} -o ${LIBRA_EXODUS_BUNDLE_PATH}/${LIBRA_EXODUS_BUNDLE_NAME}
    )
  # pip install exodus into the virtual environment
    #set a list of exodus apps in a bash variable to install
    execute_process(
      COMMAND ${CMAKE_SOURCE_DIR}/install/librapython/bin/exodus 
    )
#  endif()
endif()

include_directories(
  ${INSTALL_DIR}/include
  ${INSTALL_DIR}/include/casacpp
  /usr/include/cfitsio
  ${CMAKE_SOURCE_DIR}/apps/src
  ${CMAKE_SOURCE_DIR}/src
  ${INSTALL_DIR}/include/pybind11)

link_directories(
  ${INSTALL_DIR}/lib
  /usr/local/lib
  /usr/lib64
  /usr/lib
  /usr/lib/x86_64-linux-gnu)
