# -*- mode: cmake -*-
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2022 CASA project
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# Correspondence concerning CASA should be addressed as follows:
#        Internet email: casa-feedback@nrao.edu.

set(CASA_VERSION_MAJOR "${CMAKE_PROJECT_VERSION_MAJOR}")
set(CASA_VERSION_MINOR "${CMAKE_PROJECT_VERSION_MINOR}")
set(CASA_VERSION_PATCH "${CMAKE_PROJECT_VERSION_PATCH}")
set(CASA_VERSION_FEATURE "${CMAKE_PROJECT_VERSION_TWEAK}")
set(CASA_VARIANT "")
set(CASA_VERSION_DESC "CASAtools:v1.0.0")
configure_file("version.cc.in" "version.cc" @ONLY)

# Declare this component using the function in the top level CMakeList.txt
declare_casacpp_component("stdcasa")

# We need to add the version.cc to the sources.
# This overwrites the target_sources command from declare_casacpp_component
# It adds the generated version.cc to the sources of this module
target_sources(casacpp_stdcasa
  PRIVATE
   ${stdcasa_library_sources}
   "${CMAKE_CURRENT_BINARY_DIR}/version.cc"
  PUBLIC
    ${stdcasa_public_headers})

# External dependencies
target_link_libraries(casacpp_stdcasa PUBLIC ${CASACORE_LIBRARIES})

# Internal dependencies
# the top level source directory, so that we can find all the includes
target_include_directories(casacpp_stdcasa PUBLIC ${CMAKE_SOURCE_DIR})
