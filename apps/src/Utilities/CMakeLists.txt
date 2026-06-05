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

set(APP_NAME utilities)
string(TOUPPER ${APP_NAME} APP_NAME_UPPER)

find_package(pybind11 REQUIRED)

set(BUILD_PYWRAPPER ON)
if(BUILD_PYWRAPPER AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_py_interface.cc")
  pybind11_add_module(${APP_NAME}2py "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_py_interface.cc")
  target_link_libraries(${APP_NAME}2py PRIVATE ${APP_LINK_LIBRARIES_LIBRA} casacpp_synthesis)
  install(TARGETS ${APP_NAME}2py LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
endif()
