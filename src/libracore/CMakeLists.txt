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

include_directories(${MDSPAN_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}  
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include/casacore
  ${CMAKE_INSTALL_PREFIX}/include
  ${WCSLIB_INCLUDEDIR}
)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

add_library(${PROJECT_NAME} SHARED 
	${CMAKE_CURRENT_SOURCE_DIR}/imageInterface.cc)
	
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER 
	"${CMAKE_CURRENT_SOURCE_DIR}/imageInterface.h;
	${CMAKE_CURRENT_SOURCE_DIR}/DataBase.h;
	${CMAKE_CURRENT_SOURCE_DIR}/DataIterations.h;
	${CMAKE_CURRENT_SOURCE_DIR}/rWeightor.h;
	${CMAKE_CURRENT_SOURCE_DIR}/ThreadCoordinator.h;
	${CMAKE_CURRENT_SOURCE_DIR}/LibracoreTypes.h;
	${CMAKE_CURRENT_SOURCE_DIR}/ArrayBase.h;
	${CMAKE_CURRENT_SOURCE_DIR}/Cube.h;
	${CMAKE_CURRENT_SOURCE_DIR}/Matrix.h;
	${CMAKE_CURRENT_SOURCE_DIR}/Vector.h
	")

target_include_directories(${PROJECT_NAME} PUBLIC
  ${MDSPAN_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include/casacore
  ${CMAKE_INSTALL_PREFIX}/include
  )
install(TARGETS ${PROJECT_NAME} 
	LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
	#PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libracore")

# does NOT work since the header includes headers in other experimental dirs
#set(MDSPAN_HEADER "${CMAKE_SOURCE_DIR}/../dependencies/Kokkos/tpls/mdspan/include/mdspan/mdspan.hpp")
#install(FILES ${MDSPAN_HEADER} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mdspan")

# for testing
link_directories(
  "${CMAKE_INSTALL_PREFIX}/lib"
  ${WCSLIB_LIBRARY_DIRS}
  )
add_executable(testmdspan "${CMAKE_CURRENT_SOURCE_DIR}/testmdspan.cc")
#target_link_libraries(testmdspan PRIVATE libracore
#casacpp_synthesis casacpp_msvis hpg kokkoscore casa_msfits casa_ms casa_images
#casa_coordinates casa_lattices casa_fits casa_measures casa_tables casa_scimath
#casa_scimath_f casa_casa pthread parafeed readline history termcap m c
#"-Wl,--disable-new-dtags")
target_link_libraries(testmdspan PRIVATE libracore
casacpp_synthesis casacpp_msvis hpg kokkoscore casa_msfits casa_ms casa_images
casa_coordinates casa_lattices casa_fits casa_measures casa_tables casa_scimath
casa_scimath_f casa_casa pthread parafeed m c
"-Wl,--disable-new-dtags")

# for tesing
link_directories("${CMAKE_INSTALL_PREFIX}/lib")
add_executable(testLibracoreTypes "${CMAKE_CURRENT_SOURCE_DIR}/testLibracoreTypes.cc")
target_link_libraries(testLibracoreTypes PRIVATE libracore
casacpp_synthesis casacpp_msvis hpg kokkoscore casa_msfits casa_ms casa_images
casa_coordinates casa_lattices casa_fits casa_measures casa_tables casa_scimath
casa_scimath_f casa_casa pthread parafeed  m c
"-Wl,--disable-new-dtags")

