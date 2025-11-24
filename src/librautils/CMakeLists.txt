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
project(librautils)

find_package(WCSLIB REQUIRED)




# List of public headers
set(LIBRAUTILS_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.h
)

add_library(${PROJECT_NAME} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.cc
  ${LIBRAUTILS_PUBLIC_HEADERS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  PUBLIC_HEADER "${LIBRAUTILS_PUBLIC_HEADERS}"
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include/casacore
  ${CMAKE_INSTALL_PREFIX}/include
  ${WCSLIB_INCLUDE_DIRS}
)

# Install library and public headers to include/librautils
install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/librautils"
)
