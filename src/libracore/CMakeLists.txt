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

set(MDSPAN_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include") # Adjust this path if your mdspan headers are elsewhere
find_package(WCSLIB REQUIRED)


message(STATUS "Using mdspan include directory: ${MDSPAN_INCLUDE_DIR}")
message(STATUS "Kokkos_FOUND: ${Kokkos_FOUND}")
message(STATUS "Kokkos_INCLUDE_DIRS: ${Kokkos_INCLUDE_DIRS}")
message(STATUS "Kokkos_LIBRARIES: ${Kokkos_LIBRARIES}")
get_target_property(Kokkos_IMPORTED_LOCATION Kokkos::kokkos IMPORTED_LOCATION)
message(STATUS "Kokkos::kokkos IMPORTED_LOCATION: ${Kokkos_IMPORTED_LOCATION}")
get_target_property(Kokkos_INTERFACE_COMPILE_OPTIONS Kokkos::kokkos INTERFACE_COMPILE_OPTIONS)
message(STATUS "Kokkos::kokkos INTERFACE_COMPILE_OPTIONS: ${Kokkos_INTERFACE_COMPILE_OPTIONS}")


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
  ${WCSLIB_INCLUDE_DIRS}
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
  ${WCSLIB_INCLUDE_DIRS}
)
target_link_libraries(testmdspan PRIVATE
  libracore
  casacpp_synthesis
  casacpp_msvis
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
  ${WCSLIB_INCLUDE_DIRS}
)
target_link_libraries(testLibracoreTypes PRIVATE
  libracore
  casacpp_synthesis
  casacpp_msvis
  parafeed::parafeed
  # Add other imported targets for CASA libraries if available, e.g. casa::msfits, casa::ms, etc.
  pthread
  m
  c
  "-Wl,--disable-new-dtags"
)

