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
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)


#include_directories(${CMAKE_CURRENT_LIST_DIR}/..
#	${CMAKE_SOURCE_DIR}/../../../dependencies/Kokkos/tpls/mdspan/include/mdspan/)
link_directories("${CMAKE_INSTALL_PREFIX}/lib")

## casacore_asp
add_library(casacore_asp SHARED casacore_asp.cc casacore_asp_cube.cc
	casacore_asp_mdspan.cc)
target_link_libraries(casacore_asp PRIVATE ${APP_LINK_LIBRARIES} libracore "-Wl,--disable-new-dtags")
install(TARGETS casacore_asp
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")


## casacore_restore
add_library(casacore_restore SHARED casacore_restore.cc)
target_link_libraries(casacore_restore PRIVATE ${APP_LINK_LIBRARIES} libracore "-Wl,--disable-new-dtags")
install(TARGETS casacore_restore
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
