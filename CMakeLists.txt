# Copyright 2021 Associated Universities, Inc. Washington DC, USA.
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

## Set general cmake configuration

# Minimum cmake required version
cmake_minimum_required(VERSION 3.12)

project(
  casacpp
  VERSION 6.5.2.0
  DESCRIPTION "CASA C++ libraries"
  HOMEPAGE_URL https://casa.nrao.edu/
  LANGUAGES CXX Fortran C)

include(GNUInstallDirs)
include(CheckCXXCompilerFlag)
include(FindPkgConfig)

# Set a default build type if none was specified
set(default_build_type RelWithDebInfo)

# Set C++ standard to c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Check for dependencies

# casacore dependency
pkg_check_modules(CASACORE REQUIRED IMPORTED_TARGET casacore)

# Find BISON, FLEX
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# protobuf/grpc dependency
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

# GSL dependency
find_package(GSL REQUIRED)

# LIBXML dependecy
pkg_check_modules(LibXML++ REQUIRED IMPORTED_TARGET libxml++-2.6)

# LIBXSLT dependecy
pkg_check_modules(XSLT REQUIRED IMPORTED_TARGET libxslt)

# CFITSIO dependecy
pkg_check_modules(CFITSIO REQUIRED IMPORTED_TARGET cfitsio)

# sqllite3 dependency
find_package(SQLite3 REQUIRED)

# OpenMP dependency
find_package(OpenMP REQUIRED)

# SAKURA dependency
pkg_check_modules(SAKURA REQUIRED IMPORTED_TARGET libsakura)

# Function to help define a casacpp component
macro(declare_casacpp_component name)

  # List the files that are going to be compiled. It is a recursive search
  file(GLOB_RECURSE ${name}_library_sources "*.cc" "*.c" "*.f")

  # List the header files and template header files, also recursive
  file(GLOB_RECURSE ${name}_public_headers CONFIGURE_DEPENDS "*.h" "*.tcc")

  # Add the library target to cmake
  add_library(casacpp_${name} SHARED)

  # Specify which files belong to the target
  target_sources(casacpp_${name}
    PRIVATE 
      ${${name}_library_sources}
    PUBLIC 
      ${${name}_public_headers})

  # Set the version property of the shared library
  set_target_properties(casacpp_${name} PROPERTIES VERSION ${PROJECT_VERSION})

  # Set the shared object version property to the project's major version.
  set_target_properties(casacpp_${name} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

  # Set -Wall compiler option for supported compilers
  target_compile_options(casacpp_${name} PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall>)

  # Install the library
  install(TARGETS casacpp_${name}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

  # Install the header and template header files
  # Note that this preserves the directory structure of the files
  # That's why ${name}_public_headers is not reused
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/{name}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/casacpp
      FILES_MATCHING
      REGEX "/.*(h|tcc)$")

endmacro()

# Create a list with all the components
set(casacpp_all_components
  protobuf_generated
  casatools
  stdcasa
  casadbus
  atnf
  nrao
  asdmstman
  msvis
  flagging
  alma
  atmosphere
  components
  imageanalysis
  graphics
  spectrallines
  parallel
  mstransform
  synthesis
  calanalysis
  singledish
  casa_sakura
  singledishfiller
  miriad
)

# Add a subdirectory per casacpp component
foreach(_component IN LISTS casacpp_all_components)
  add_subdirectory(${CMAKE_SOURCE_DIR}/${_component})
endforeach()

# use MPI (not equivalent to CASA's MPI usage)
option(casacpp_USE_MPI "Use MPI" OFF)
