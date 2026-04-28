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

set(LIBRAUTILS_PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/ProfilingTools.h
)

add_library(librautils SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.cc
  ${LIBRAUTILS_PUBLIC_HEADERS}
)

set_target_properties(librautils PROPERTIES
  PUBLIC_HEADER "${LIBRAUTILS_PUBLIC_HEADERS}"
)

target_include_directories(librautils PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_INSTALL_PREFIX}/include/casacore
  ${CMAKE_INSTALL_PREFIX}/include
  ${WCSLIB_INCLUDE_DIRS}
)

# Link NVTX if profiling is enabled
if(LIBRA_ENABLE_NVTX)
  target_link_libraries(${PROJECT_NAME} PUBLIC NVTX::nvtx)
  message(STATUS "librautils: NVTX profiling enabled")
endif()

# Install library and public headers to include/librautils
install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/librautils"
)

if(BUILD_TESTING)
  add_executable(test_librautils ${CMAKE_CURRENT_SOURCE_DIR}/test_librautils.cc)
  target_include_directories(test_librautils PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_INSTALL_PREFIX}/include
  )
  target_link_libraries(test_librautils PRIVATE
    GTest::gtest_main librautils stdc++fs ${CASACORE_LIBRARIES})
  add_test(NAME test_librautils COMMAND test_librautils
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  install(TARGETS test_librautils
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/tests")
endif()
