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

# NVTX detection and configuration for profiling support

if(NOT LIBRA_ENABLE_NVTX)
  return()
endif()

# Try to find NVTX through CUDA Toolkit first
find_package(CUDAToolkit QUIET)
if(CUDAToolkit_FOUND)
  set(NVTX_ROOT_DIR "${CUDAToolkit_TARGET_DIR}")
else()
  # Fallback: look in common system locations
  set(NVTX_SEARCH_PATHS
    "/usr/local/cuda"
    "/opt/cuda"
    "/usr/cuda"
    "$ENV{CUDA_ROOT}"
    "$ENV{CUDA_HOME}"
    "$ENV{CUDA_PATH}"
  )
  
  foreach(SEARCH_PATH ${NVTX_SEARCH_PATHS})
    if(EXISTS "${SEARCH_PATH}")
      set(NVTX_ROOT_DIR "${SEARCH_PATH}")
      break()
    endif()
  endforeach()
endif()

# Look for NVTX3 headers
if(NVTX_ROOT_DIR)
  find_path(NVTX_INCLUDE_DIR
    NAMES nvtx3/nvToolsExt.h
    HINTS ${NVTX_ROOT_DIR}
    PATH_SUFFIXES include targets/x86_64-linux/include
    DOC "NVTX include directory"
  )
  
  find_library(NVTX_LIBRARY
    NAMES nvToolsExt
    HINTS ${NVTX_ROOT_DIR}
    PATH_SUFFIXES lib lib64 targets/x86_64-linux/lib targets/x86_64-linux/lib/stubs
    DOC "NVTX library"
  )
endif()

# Check if we found everything we need
if(NVTX_INCLUDE_DIR AND NVTX_LIBRARY)
  set(NVTX_FOUND TRUE)
  
  # Create imported target for NVTX
  if(NOT TARGET NVTX::nvtx)
    add_library(NVTX::nvtx SHARED IMPORTED)
    set_target_properties(NVTX::nvtx PROPERTIES
      IMPORTED_LOCATION "${NVTX_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${NVTX_INCLUDE_DIR}"
    )
  endif()
  
  # Set variables for use in other CMake files
  set(LIBRA_NVTX_INCLUDE_DIR "${NVTX_INCLUDE_DIR}" CACHE PATH "NVTX include directory")
  set(LIBRA_NVTX_LIBRARY "${NVTX_LIBRARY}" CACHE FILEPATH "NVTX library")
  
else()
  set(NVTX_FOUND FALSE)
  message(WARNING "${LIBRA_ID}NVTX not found! Profiling will be disabled.")
  message(WARNING "${LIBRA_ID}Install CUDA Toolkit or ensure NVTX headers/libraries are available.")
  message(WARNING "${LIBRA_ID}Automatically disabling LIBRA_ENABLE_NVTX for build.")
  
  # Automatically disable NVTX if not found
  set(LIBRA_ENABLE_NVTX OFF PARENT_SCOPE)
  set(LIBRA_ENABLE_NVTX OFF)
endif()

# Mark variables as advanced
mark_as_advanced(NVTX_INCLUDE_DIR NVTX_LIBRARY NVTX_ROOT_DIR)