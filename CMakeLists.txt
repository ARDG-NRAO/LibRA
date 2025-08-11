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

cmake_minimum_required(VERSION 3.18)
project("LibRA Apps"
  DESCRIPTION "LibRA CL, C/C++, Py Interfaces"
  HOMEPAGE_URL https://github.com/ARDG-NRAO/LibRA
  LANGUAGES CXX Fortran C)



# Path and directory setup
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paths.cmake)

# Load project configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libraconfig.cmake)

# Print summary of machine and LIBRA configuration
print_libra_summary()

# Include dependencies
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/Kokkos.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/FFTW.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/HPG.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/Casacore.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/Parafeed.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/Pybind11.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies/GTest.cmake)

# Include components
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components/Casacpp.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components/Apps.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components/Frameworks.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/components/Scripts.cmake)




