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

cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
project(libracore)

find_package(Kokkos REQUIRED)

# Get Kokkos include directories and find mdspan
get_target_property(KOKKOS_INCLUDE_DIRS Kokkos::kokkos INTERFACE_INCLUDE_DIRECTORIES)
set(MDSPAN_INCLUDE_DIR "")
foreach(kokkos_dir ${KOKKOS_INCLUDE_DIRS})
    if(EXISTS "${kokkos_dir}/mdspan/mdspan.hpp")
        set(MDSPAN_INCLUDE_DIR "${kokkos_dir}")
        break()
    elseif(EXISTS "${kokkos_dir}/../tpls/mdspan/include/mdspan/mdspan.hpp")
        set(MDSPAN_INCLUDE_DIR "${kokkos_dir}/../tpls/mdspan/include")
        break()
    elseif(EXISTS "${kokkos_dir}/../tpls/mdspan/include")
        set(MDSPAN_INCLUDE_DIR "${kokkos_dir}/../tpls/mdspan/include")
        break()
    endif()
endforeach()
include(FindPkgConfig)
pkg_check_modules(WCSLIB REQUIRED wcslib)

if(NOT MDSPAN_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find mdspan headers in Kokkos installation")
endif()

message(STATUS "Found mdspan include directory: ${MDSPAN_INCLUDE_DIR}")




# List of public headers
set(LIBRACORE_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/imageInterface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DataBase.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DataIterations.h
  ${CMAKE_CURRENT_SOURCE_DIR}/rWeightor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ThreadCoordinator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/LibracoreTypes.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ArrayBase.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Cube.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Matrix.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Vector.h
)

add_library(${PROJECT_NAME} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/imageInterface.cc
  ${LIBRACORE_PUBLIC_HEADERS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  PUBLIC_HEADER "${LIBRACORE_PUBLIC_HEADERS}"
)


target_include_directories(${PROJECT_NAME} PUBLIC
  ${MDSPAN_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include/casacore
  ${CMAKE_INSTALL_PREFIX}/include
  ${WCSLIB_INCLUDEDIR}
)

# Install library and public headers to include/libracore
install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libracore"
)




# Test executables
add_executable(testmdspan "${CMAKE_CURRENT_SOURCE_DIR}/testmdspan.cc")
target_include_directories(testmdspan PRIVATE
  ${MDSPAN_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include/casacore
  ${CMAKE_INSTALL_PREFIX}/include
  ${WCSLIB_INCLUDEDIR}
)
target_link_libraries(testmdspan PRIVATE
  libracore
  casacpp_synthesis
  casacpp_msvis
  Hpg::hpg
  Kokkos::kokkos
  parafeed::parafeed
  # Add other imported targets for CASA libraries if available, e.g. casa::msfits, casa::ms, etc.
  pthread
  m
  c
  "-Wl,--disable-new-dtags"
)

add_executable(testLibracoreTypes "${CMAKE_CURRENT_SOURCE_DIR}/testLibracoreTypes.cc")
target_include_directories(testLibracoreTypes PRIVATE
  ${MDSPAN_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include/casacore
  ${CMAKE_INSTALL_PREFIX}/include
  ${WCSLIB_INCLUDEDIR}
)
target_link_libraries(testLibracoreTypes PRIVATE
  libracore
  casacpp_synthesis
  casacpp_msvis
  Hpg::hpg
  Kokkos::kokkos
  parafeed::parafeed
  # Add other imported targets for CASA libraries if available, e.g. casa::msfits, casa::ms, etc.
  pthread
  m
  c
  "-Wl,--disable-new-dtags"
)

