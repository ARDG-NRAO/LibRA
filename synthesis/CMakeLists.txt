# -*- mode: cmake -*-
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
BISON_TARGET(CalLibraryGramBison CalLibrary/CalLibraryGram.yy ${CMAKE_CURRENT_BINARY_DIR}/CalLibraryGram.ycc COMPILE_FLAGS "-y -p CalLibraryGram")
FLEX_TARGET(CalLibraryGramFlex CalLibrary/CalLibraryGram.ll ${CMAKE_CURRENT_BINARY_DIR}/CalLibraryGram.lcc COMPILE_FLAGS -PCalLibraryGram)
add_flex_bison_dependency(CalLibraryGramFlex CalLibraryGramBison)

# Declare this component using the function in the top level CMakeList.txt
declare_casacpp_component("synthesis")

# We need to add the BISON and FLEX outputs to the sources.
# This overwrites the target_sourcescommand from declare_casacpp_component
target_sources(casacpp_synthesis
  PRIVATE
   ${synthesis_library_sources}
   ${BISON_CalLibraryGramBison_OUTPUTS}
   ${FLEX_CalLibraryGramFlex_OUTPUTS}
  PUBLIC
    ${synthesis_public_headers})

# External dependencies
# directory where casacore is installed
target_link_libraries(casacpp_synthesis PUBLIC PkgConfig::CASACORE)
# OpenMP dependency
target_link_libraries(casacpp_synthesis PUBLIC OpenMP::OpenMP_CXX)
# Libsakura dependency
target_link_libraries(casacpp_synthesis PUBLIC PkgConfig::SAKURA)

# Internal dependencies
target_link_libraries(casacpp_synthesis PUBLIC casacpp_msvis casacpp_stdcasa casacpp_atmosphere casacpp_protobuf casacpp_imageanalysis casacpp_mstransform)
add_dependencies(casacpp_synthesis casacpp_msvis casacpp_stdcasa casacpp_atmosphere casacpp_protobuf casacpp_imageanalysis casacpp_mstransform)
# the generated protobuf headers
target_include_directories(casacpp_synthesis PRIVATE ${CMAKE_BINARY_DIR}/protobuf_generated)
# Include directory for the generated flex/bison
target_include_directories(casacpp_synthesis PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# the top level source directory, so that we can find all the includes
target_include_directories(casacpp_synthesis PUBLIC ${CMAKE_SOURCE_DIR})
