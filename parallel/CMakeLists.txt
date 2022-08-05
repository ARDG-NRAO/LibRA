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

# List the files that are going to be compiled. It is a recursive search
file(GLOB_RECURSE parallel_library_modules "*.cc")
# List the header files and template header files, also recursive
file(GLOB_RECURSE parallel_public_headers "*.h" "*.tcc")
# Add a library target to cmake
add_library(casacpp_parallel SHARED)
# Specify which files belong to the target
target_sources(casacpp_parallel
  PRIVATE
    ${parallel_library_modules}
  PUBLIC
    ${parallel_public_headers})
# External dependencies
target_link_libraries(casacpp_parallel PUBLIC PkgConfig::CASACORE)


# Internal dependencies
# the top level source directory, so that we can find all the includes
target_include_directories(casacpp_parallel PUBLIC ${CMAKE_SOURCE_DIR})
