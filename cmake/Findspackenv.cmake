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

# Use spack environment, if set.  This enables use of dependencies
# built in a SPACK environment to be used for building the rest of the
# code.  The SPACK_FLAGS adds paths to include, lib and lib64 of the
# SPACK env. to the CXX_FLAGS complier and linker flags.
#
# Detect if we are in a Spack environment
if(DEFINED ENV{SPACK_ENV} OR DEFINED ENV{SPACK_ENV_PATH} OR DEFINED ENV{SPACK_ROOT})
  set(SPACK_ENV_ACTIVE TRUE)
  # Try to get the view root (adjust as needed for your Spack setup)
  if(DEFINED ENV{SPACK_ENV})
    set(SPACK_VIEW_ROOT $ENV{SPACK_ENV})
  elseif(DEFINED ENV{SPACK_ENV_PATH})
    set(SPACK_VIEW_ROOT $ENV{SPACK_ENV_PATH})
  elseif(DEFINED ENV{SPACK_ROOT})
    set(SPACK_VIEW_ROOT $ENV{SPACK_ROOT})
  endif()
  set(SPACK_FLAGS "-DCMAKE_CXX_FLAGS=-I${SPACK_VIEW_ROOT}/include -L${SPACK_VIEW_ROOT}/lib64 -L${SPACK_VIEW_ROOT}/lib")
  message(STATUS "Spack environment detected. Adding SPACK flags: ${SPACK_FLAGS}")
  # Optionally update CMAKE_PREFIX_PATH, CMAKE_LIBRARY_PATH, etc.
  list(APPEND CMAKE_PREFIX_PATH "${SPACK_VIEW_ROOT}")
  link_directories("${SPACK_VIEW_ROOT}/lib" "${SPACK_VIEW_ROOT}/lib64")
  include_directories("${SPACK_VIEW_ROOT}/include")
else()
  set(SPACK_ENV_ACTIVE FALSE)
endif()