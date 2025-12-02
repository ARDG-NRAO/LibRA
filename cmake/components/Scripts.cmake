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
# Scripts component
include(ExternalProject)

ExternalProject_Add(
  Scripts
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/scripts
  DOWNLOAD_COMMAND ""
  BINARY_DIR      ${BUILD_DIR}/Libra/scripts
  DEPENDS         Frameworks
  CONFIGURE_COMMAND ${CMAKE_COMMAND} <SOURCE_DIR> -B <BINARY_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DTOP_LEVEL_DIR=${TOP_LEVEL_DIR} -DINSTALL_DIR=${INSTALL_DIR}
  BUILD_COMMAND   make -j${NCORES} -C <BINARY_DIR>
  INSTALL_COMMAND make install -C <BINARY_DIR>
)
