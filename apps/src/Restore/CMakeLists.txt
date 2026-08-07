# Copyright (C) 2021
# Associated Universities, Inc. Washington DC, USA.
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.is
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning this should be addressed as follows:
#        Postal address: National Radio Astronomy Observatory
#                        1003 Lopezville Road,
#                        Socorro, NM - 87801, USA
#
# $Id$
set(CMAKE_CXX_STANDARD 17)

set(APP_NAME restore)
string(TOUPPER ${APP_NAME} APP_NAME_UPPER)

set(APP_SOURCES
  restore.cc
)

find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)

file(GLOB APP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES ${APP_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/")

add_library(${APP_NAME}_lib SHARED ${APP_SOURCES})
target_link_libraries(${APP_NAME}_lib PUBLIC ${APP_LINK_LIBRARIES} casacpp_synthesis librautils libracore)
set_target_properties(${APP_NAME}_lib PROPERTIES OUTPUT_NAME ${APP_NAME})
install(TARGETS ${APP_NAME}_lib LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

set(BUILD_CLWRAPPER ON)
if(BUILD_CLWRAPPER AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_cl_interface.cc")
  add_executable(${APP_NAME}_bin "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_cl_interface.cc")
  target_link_libraries(${APP_NAME}_bin PRIVATE ${APP_NAME}_lib ${READLINE_LDFLAGS} ${CURSES_LIBRARIES})
  set_target_properties(${APP_NAME}_bin PROPERTIES OUTPUT_NAME ${APP_NAME})
  install(TARGETS ${APP_NAME}_bin DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
endif()

set(BUILD_PYWRAPPER ON)
if(BUILD_PYWRAPPER AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_py_interface.cc")
  pybind11_add_module(${APP_NAME}2py "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_py_interface.cc")
  target_link_libraries(${APP_NAME}2py PRIVATE ${APP_NAME}_lib)
  install(TARGETS ${APP_NAME}2py LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
endif()

if(BUILD_TESTING AND Apps_BUILD_TESTS_PYTHON)
  find_program(PYTHON_EXECUTABLE python)
  add_test(NAME test_${APP_NAME}_python
    COMMAND ${PYTHON_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_${APP_NAME}.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/tests)
endif()
