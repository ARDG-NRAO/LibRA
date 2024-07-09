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


include_directories(${CMAKE_SOURCE_DIR}/../dependencies/Kokkos/tpls/mdspan/include/mdspan/
  ${CMAKE_CURRENT_SOURCE_DIR}  
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include/casacore
  ${CMAKE_INSTALL_PREFIX}/include
)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

add_library(${PROJECT_NAME} SHARED "${CMAKE_CURRENT_SOURCE_DIR}/imageInterface.cc")
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER 
	"imageInterface.h")
target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}/../dependencies/Kokkos/tpls/mdspan/include/mdspan/
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
#link_directories("${CMAKE_INSTALL_PREFIX}/lib")
#add_executable(testmdspan "${CMAKE_CURRENT_SOURCE_DIR}/testmdspan.cc")
#target_link_libraries(testmdspan PRIVATE libracore
	#  casacpp_synthesis
	#casacpp_msvis
	#hpg
	#kokkoscore
	#casa_msfits
	#casa_ms
	#casa_images
	#casa_coordinates
	#casa_lattices
	#casa_fits
	#casa_measures
	#casa_tables
	#casa_scimath
	#casa_scimath_f
	#casa_casa
	#pthread
	#parafeed
	#readline
	#history
	#termcap
	#m
	#c
	#"-Wl,--disable-new-dtags")
