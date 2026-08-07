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
set (CMAKE_CXX_STANDARD 17)
set(OPT "-g -O2 ")

find_package(Kokkos REQUIRED)
find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX}/include)

set(EXAMPLE_LIST dataiter)
#set(HEADER_LIST dataiter.h)
set(BUILD_EXAMPLE ON)

message("${CMAKE_INSTALL_PREFIX}/bin")
foreach(EXAMPLE IN LISTS EXAMPLE_LIST)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
   if(BUILD_EXAMPLE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE}.cc")
     add_executable(${EXAMPLE} "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE}.cc")
     target_link_libraries(${EXAMPLE} ${APP_LINK_LIBRARIES} casacpp_msvis libracore)
     install(TARGETS ${EXAMPLE} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
   endif()
endforeach()

# foreach(HEADER IN LISTS HEADER_LIST)
#    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}")
#      install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/")
#    endif()
