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

set(MDSPAN_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")

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

add_library(libracore SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/imageInterface.cc
  ${LIBRACORE_PUBLIC_HEADERS}
)

set_target_properties(libracore PROPERTIES
  PUBLIC_HEADER "${LIBRACORE_PUBLIC_HEADERS}"
)

target_include_directories(libracore PUBLIC
  ${MDSPAN_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include/casacore
  ${CMAKE_INSTALL_PREFIX}/include
  ${WCSLIB_INCLUDE_DIRS}
)

install(TARGETS libracore
  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libracore"
)

if(BUILD_TESTING)
  add_executable(test_libracoretypes ${CMAKE_CURRENT_SOURCE_DIR}/test_libracoretypes.cc)
  target_include_directories(test_libracoretypes PRIVATE
    ${MDSPAN_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_INSTALL_PREFIX}/include/casacore
    ${CMAKE_INSTALL_PREFIX}/include
    ${WCSLIB_INCLUDE_DIRS}
  )
  target_link_libraries(test_libracoretypes PRIVATE
    GTest::gtest_main libracore stdc++fs ${CASACORE_LIBRARIES})
  add_test(NAME test_libracoretypes COMMAND test_libracoretypes
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  install(TARGETS test_libracoretypes
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/tests")
endif()
