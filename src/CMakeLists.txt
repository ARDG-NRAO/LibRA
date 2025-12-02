# -*- mode: cmake -*-
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2022 CASA project
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# Correspondence concerning CASA should be addressed as follows:
#        Internet email: casa-feedback@nrao.edu.

## Set general cmake configuration

# Minimum cmake required version
cmake_minimum_required(VERSION 3.12)

project(
  casacpp
  VERSION 6.5.2.0
  DESCRIPTION "CASA C++ libraries"
  HOMEPAGE_URL https://casa.nrao.edu/
  LANGUAGES CXX Fortran C)

# Add directory with additional macros
# Preserve any MODULE_PATH passed in from parent, then append local cmake dir
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(DeclareCasacppComponent)
include(GNUInstallDirs)
include(CheckCXXCompilerFlag)
include(FindPkgConfig)
include(CTest)

# Set a default build type if none was specified
# This should have be set at the top-level already
# set(CMAKE_BUILD_TYPE "RelWithDebInfo")

# Variables to control dependency graph
set(CMAKE_USE_GRPC OFF)         # Remove GRPC and protobuf dependency
set(CMAKE_PYTHON_INTERFACE OFF) # Remove xml++ and xslt depdnency
set(CMAKE_LIBIMAGING_ONLY ON)   # For future use (post code/synthesis refactoring)

# Set C++ standard to c++11
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Check for dependencies

# Find MPI compiler
find_package(MPI)

# casacore dependency
find_package(Casacore REQUIRED COMPONENTS casa coordinates derivedmscal fits images lattices meas measures mirlib ms msfits scimath scimath_f tables)
include_directories(${CASACORE_INCLUDE_DIRS})

# Find BISON, FLEX
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# protobuf/grpc dependency
if (CMAKE_USE_GRPC)
	find_package(Protobuf REQUIRED)
	find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
	# Try to detect GRPC using native cmake support (for distributions like Fedora or source compilation)
	find_package(gRPC QUIET)
	# If that fails, try detecting GRPC with pkgconfig (for distributions like Ubuntu, Rocky Linux)
	if(NOT DEFINED gRPC_FOUND OR gRPC_FOUND EQUAL 0libprotobuf-dev:amd64)
		pkg_check_modules(GRPC REQUIRED IMPORTED_TARGET grpc++)
		set(gRPC_USES_FIND_PACKAGE OFF)
	else()
		set(gRPC_USES_FIND_PACKAGE ON)
	endif()
	add_definitions(-DUSE_GRPC)
endif()

# GSL dependency
find_package(GSL REQUIRED)

if (CMAKE_PYTHON_INTERFACE)
  # LIBXML dependecy
  find_package(LibXML++ REQUIRED)

  # LIBXSLT dependecy
  find_package(LibXSLT REQUIRED)
endif()

# CFITSIO dependecy
find_package(CFITSIO REQUIRED)

# sqllite3 dependency
#find_package(SQLite3 REQUIRED)

# OpenMP dependency
find_package(OpenMP REQUIRED)

# SAKURA dependency
if (LIBRA_USE_LIBSAKURA)
  pkg_check_modules(SAKURA REQUIRED IMPORTED_TARGET libsakura)
endif()
# HPG dependency
find_package(Hpg CONFIG)
if(DEFINED Hpg_FOUND AND ${Hpg_FOUND})
  add_definitions(-DUSE_HPG)
endif()

#parafeed dependency
find_package(parafeed REQUIRED)

# if defined(parafeed_FOUND AND ${parafeed_FOUND})
  # add_definitions(-DUSE_PARAFEED)
# endif()

#pkg_check_modules(readline REQUIRED IMPORTED_TARGET readline)

# WCSLIB Check
# Check if WCSLIB variables were passed from parent
if(DEFINED WCSLIB_INCLUDE_DIRS AND DEFINED WCSLIB_LIBRARIES)
  message(STATUS "Using WCSLIB from parent: ${WCSLIB_INCLUDE_DIRS}")
  include_directories(${WCSLIB_INCLUDE_DIRS})
else()
  # Try to find it ourselves
  find_package(WCSLIB REQUIRED)
  if(WCSLIB_FOUND)
    include_directories(${WCSLIB_INCLUDE_DIRS})
  endif()
endif()

if (LIBRA_USE_LIBSAKURA)
  set(CASA_SAKURA "casa_sakura")
else()
  set(CASA_SAKURA "")
endif()
# Create a list with all the components
if (CMAKE_LIBIMAGING_ONLY)
  set(casacpp_all_components
    asdmstman
    atmosphere
    casatools
    ${CASA_SAKURA}
    components
    imageanalysis
    libracore
    librautils
    mstransform
    msvis
    nrao
    stdcasa
    synthesis
    )
else()
  set(casacpp_all_components
    alma
    asdmstman
    atmosphere
    atnf
    calanalysis
    casadbus
    casatools
    ${CASA_SAKURA}
    components
    imageanalysis
    flagging
    graphics
    libracore
    librautils
    miriad
    mstransform
    msvis
    nrao
    parallel
    protobuf_generated
    singledish
    singledishfiller
    spectrallines
    stdcasa
    synthesis
    )
endif()

# Add a subdirectory per casacpp component
foreach(_component IN LISTS casacpp_all_components)
  add_subdirectory(${_component})
endforeach()


# install the casacpp.pc.in file
configure_file(casacpp.pc.in casacpp.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/casacpp.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
message(STATUS "CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}") 
