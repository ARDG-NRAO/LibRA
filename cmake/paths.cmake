#-*- cmake -*-
# Copyright (C) 2025
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
# Path and directory setup for LibRA

# Path and directory variables moved from libraconfig.cmake
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install)
set(TOP_LEVEL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LIBRA_MODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# Exodus-specific paths
set(LIBRA_PYTHON_VENV_DIR ${INSTALL_DIR}/librapython)
set(LIBRA_EXODUS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/exodus)
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(ENV{PKG_CONFIG_PATH} "${CMAKE_SOURCE_DIR}/install/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

include_directories(
  ${INSTALL_DIR}/include
  ${INSTALL_DIR}/include/casacpp
  /usr/include/cfitsio
  ${CMAKE_SOURCE_DIR}/apps/src
  ${CMAKE_SOURCE_DIR}/src
  ${INSTALL_DIR}/include/pybind11)

link_directories(
  ${INSTALL_DIR}/lib
  /usr/local/lib
  /usr/lib64
  /usr/lib
  /usr/lib/x86_64-linux-gnu)
