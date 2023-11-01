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

# Declare this component using the function in the top level CMakeList.txt
declare_casacpp_component("atnf")

# Fortran compiler option
target_compile_options(casacpp_atnf PRIVATE $<$<COMPILE_LANGUAGE:Fortran>:-std=legacy>)

# External dependencies
target_link_libraries(casacpp_atnf PUBLIC PkgConfig::CASACORE)

# Internal dependecies
# the top level source directory, so that we can find all the includes
target_include_directories(casacpp_atnf PUBLIC ${CMAKE_SOURCE_DIR})
