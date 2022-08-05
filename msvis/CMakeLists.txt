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

# Declare this component using the function in the top level CMakeList.txt
declare_casacpp_component("msvis")

# External dependencies
target_link_libraries(casacpp_msvis PUBLIC PkgConfig::CASACORE)

# Internal dependencies
add_dependencies(casacpp_msvis casacpp_stdcasa casacpp_asdmstman)
target_link_libraries(casacpp_msvis PUBLIC casacpp_stdcasa)
target_link_libraries(casacpp_msvis PRIVATE casacpp_asdmstman)
# the top level source directory, so that we can find all the includes
target_include_directories(casacpp_msvis PUBLIC ${CMAKE_SOURCE_DIR})
