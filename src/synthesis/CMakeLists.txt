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

BISON_TARGET(CalLibraryGramBison CalLibrary/CalLibraryGram.yy ${CMAKE_CURRENT_BINARY_DIR}/CalLibraryGram.ycc COMPILE_FLAGS "-y -p CalLibraryGram")
FLEX_TARGET(CalLibraryGramFlex CalLibrary/CalLibraryGram.ll ${CMAKE_CURRENT_BINARY_DIR}/CalLibraryGram.lcc COMPILE_FLAGS -PCalLibraryGram)
add_flex_bison_dependency(CalLibraryGramFlex CalLibraryGramBison)

set(MODULE_NAME "synthesis")
# The declare_casacpp_component builds cmake variables with names constructed from MODULE_NAME.
# The library name, e,g., becomes "casacpp_"+${MODULE_NAME}.  One just have to know! This. Is. CMAKE.
set(LIB_NAME "casacpp_synthesis") 

# Declare this component using the function in the top level CMakeList.txt
declare_casacpp_component(${MODULE_NAME})

# We need to add the BISON and FLEX outputs to the sources.
# This overwrites the target_sources command from declare_casacpp_component
target_sources(${LIB_NAME}
  PRIVATE
   ${${MODULE_NAME}_library_sources}
   ${BISON_CalLibraryGramBison_OUTPUTS}
   ${FLEX_CalLibraryGramFlex_OUTPUTS}
  PUBLIC
    ${${MODULE_NAME}_public_headers})

# External dependencies
# directory where casacore is installed
target_link_libraries(${LIB_NAME} PUBLIC PkgConfig::CASACORE)
# OpenMP dependency
#target_link_libraries(${LIB_NAME} PUBLIC OpenMP::OpenMP_CXX)
# Libsakura dependency
target_link_libraries(${LIB_NAME} PUBLIC PkgConfig::SAKURA)
# Libhpg dependency
target_link_libraries(${LIB_NAME} PUBLIC hpg::hpg)
# Libparafeed dependency
target_link_libraries(${LIB_NAME} PUBLIC parafeed::parafeed)

# Internal dependencies.  Include protobuf only if using GRPC
if (CMAKE_USE_GRPC)
  target_link_libraries(${LIB_NAME} PUBLIC casacpp_casatools casacpp_msvis casacpp_stdcasa casacpp_atmosphere casacpp_protobuf casacpp_imageanalysis casacpp_mstransform)
  add_dependencies(${LIB_NAME} casacpp_casatools casacpp_msvis casacpp_stdcasa casacpp_atmosphere casacpp_protobuf casacpp_imageanalysis casacpp_mstransform)
  # the generated protobuf headers
  target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_BINARY_DIR}/protobuf_generated)
else()
  target_link_libraries(${LIB_NAME} PUBLIC casacpp_casatools casacpp_msvis casacpp_stdcasa casacpp_atmosphere casacpp_imageanalysis casacpp_mstransform)
  add_dependencies(${LIB_NAME} casacpp_casatools casacpp_msvis casacpp_stdcasa casacpp_atmosphere casacpp_imageanalysis casacpp_mstransform)
endif()

# # the generated protobuf headers
# target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_BINARY_DIR}/protobuf_generated)

# Include directory for the generated flex/bison
target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# the top level source directory, so that we can find all the includes
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR})
