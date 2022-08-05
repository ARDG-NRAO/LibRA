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

set(CASA_VERSION_MAJOR "${CMAKE_PROJECT_VERSION_MAJOR}")
set(CASA_VERSION_MINOR "${CMAKE_PROJECT_VERSION_MINOR}")
set(CASA_VERSION_PATCH "${CMAKE_PROJECT_VERSION_PATCH}")
set(CASA_VERSION_FEATURE "${CMAKE_PROJECT_VERSION_TWEAK}")
set(CASA_VARIANT "")
set(CASA_VERSION_DESC "CASAtools:v1.0.0")
configure_file("version.cc.in" "version.cc" @ONLY)

# Declare this component using the function in the top level CMakeList.txt
declare_casacpp_component("stdcasa")

# We need to add the version.cc to the sources.
# This overwrites the target_sourcescommand from declare_casacpp_component
file(GLOB_RECURSE stdcasa_library_modules "*.cc" "*.f")
list(APPEND stdcasa_library_modules "${CMAKE_CURRENT_BINARY_DIR}/version.cc")
file(GLOB_RECURSE stdcasa_public_headers "*.h" "*.tcc")
target_sources(casacpp_stdcasa
  PRIVATE
   ${stdcasa_library_modules}
  PUBLIC
    ${stdcasa_public_headers})

# External dependencies
target_link_libraries(casacpp_stdcasa PUBLIC PkgConfig::CASACORE)

# Internal dependencies
# the top level source directory, so that we can find all the includes
target_include_directories(casacpp_stdcasa PUBLIC ${CMAKE_SOURCE_DIR})
