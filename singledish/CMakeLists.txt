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
declare_casacpp_component("singledish")

# External dependencies
target_include_directories(casacpp_singledish PUBLIC ${CASACORE_INCLUDE_DIRS})
target_link_libraries(casacpp_singledish PUBLIC PkgConfig::CASACORE)
target_include_directories(casacpp_singledish PUBLIC ${SAKURA_INCLUDE_DIRS})
target_link_libraries(casacpp_singledish PUBLIC PkgConfig::SAKURA)

# Internal dependencies
target_link_libraries(casacpp_singledish PRIVATE casacpp_singledishfiller casacpp_casa_sakura casacpp_msvis casacpp_mstransform casacpp_stdcasa)
add_dependencies(casacpp_singledish casacpp_singledishfiller casacpp_casa_sakura casacpp_msvis casacpp_mstransform casacpp_stdcasa)
# the top level source directory, so that we can find all the includes
target_include_directories(casacpp_singledish PRIVATE ${CMAKE_SOURCE_DIR})
# the generated protobuf headers
target_include_directories(casacpp_singledish PRIVATE ${CMAKE_BINARY_DIR}/protobuf_generated)
