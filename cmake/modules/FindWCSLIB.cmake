# FindWCSLIB.cmake
#
# Find the WCSLIB library
#
# This module defines:
#  WCSLIB_FOUND - True if WCSLIB is found
#  WCSLIB_INCLUDE_DIRS - Include directories for WCSLIB
#  WCSLIB_LIBRARIES - Libraries to link against WCSLIB
#  WCSLIB_VERSION_STRING - Version string
#  wcslib::wcs - Imported target for WCSLIB

# Don't search if already found
if(TARGET wcslib::wcs)
    return()
endif()

# Use WCSLIB_ROOT if provided, plus CMAKE_PREFIX_PATH
set(_WCSLIB_SEARCH_DIRS "")
if(WCSLIB_ROOT)
    list(APPEND _WCSLIB_SEARCH_DIRS ${WCSLIB_ROOT})
endif()

# Add CMAKE_PREFIX_PATH explicitly for Spack compatibility
if(CMAKE_PREFIX_PATH)
    list(APPEND _WCSLIB_SEARCH_DIRS ${CMAKE_PREFIX_PATH})
endif()

# Spack-specific detection and handling (only if LIBRA_USE_SPACK is enabled)
if(LIBRA_USE_SPACK AND DEFINED ENV{SPACK_ROOT})
    message(STATUS "Spack environment detected")

    # Look for Spack-installed WCSLIB in common locations
    file(GLOB _spack_wcslib_dirs "$ENV{SPACK_ROOT}/opt/spack/*/wcslib*")
    if(_spack_wcslib_dirs)
        list(APPEND _WCSLIB_SEARCH_DIRS ${_spack_wcslib_dirs})
        message(STATUS "Found potential Spack WCSLIB installations: ${_spack_wcslib_dirs}")
    endif()

    # Also check if WCSLIB is in current Spack environment
    if(DEFINED ENV{SPACK_ENV})
        execute_process(COMMAND spack find --paths wcslib
            OUTPUT_VARIABLE _spack_wcslib_path
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(_spack_wcslib_path)
            string(REGEX MATCH "([^ \t\n]+)[ \t]+([^\n]+)" _spack_match "${_spack_wcslib_path}")
            if(CMAKE_MATCH_2)
                list(APPEND _WCSLIB_SEARCH_DIRS ${CMAKE_MATCH_2})
                message(STATUS "Spack WCSLIB found at: ${CMAKE_MATCH_2}")
            endif()
        endif()
    endif()
endif()

# Find the header file
find_path(WCSLIB_INCLUDE_DIR
    NAMES wcslib/wcs.h wcs.h
    HINTS ${_WCSLIB_SEARCH_DIRS}
    PATH_SUFFIXES include include/wcslib
    DOC "WCSLIB include directory"
)

# Extract version if header found
if(WCSLIB_INCLUDE_DIR)
    if(EXISTS "${WCSLIB_INCLUDE_DIR}/wcslib/wcs.h")
        set(_wcs_header "${WCSLIB_INCLUDE_DIR}/wcslib/wcs.h")
    elseif(EXISTS "${WCSLIB_INCLUDE_DIR}/wcs.h")
        set(_wcs_header "${WCSLIB_INCLUDE_DIR}/wcs.h")
    endif()
    
    if(_wcs_header)
        file(READ "${_wcs_header}" WCSLIB_H)
        string(REGEX MATCH "#define WCSLIB_VERSION \"([0-9]+\\.[0-9]+(\\.[0-9]+)?)\"" WCSLIB_VERSION_MATCH "${WCSLIB_H}")
        if(WCSLIB_VERSION_MATCH)
            set(WCSLIB_VERSION_STRING "${CMAKE_MATCH_1}")
        endif()
    endif()
endif()

# Find the main WCSLIB library
find_library(WCSLIB_LIBRARY
    NAMES wcs
    HINTS ${_WCSLIB_SEARCH_DIRS}
    PATH_SUFFIXES lib lib64
    DOC "WCSLIB main library"
)

# WCSLIB sometimes has utility libraries (optional)
find_library(WCSLIB_UTILS_LIBRARY
    NAMES wcsutils
    HINTS ${_WCSLIB_SEARCH_DIRS}
    PATH_SUFFIXES lib lib64
    DOC "WCSLIB utilities library"
)

# Handle standard arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WCSLIB
    REQUIRED_VARS WCSLIB_LIBRARY WCSLIB_INCLUDE_DIR
    VERSION_VAR WCSLIB_VERSION_STRING
)

if(WCSLIB_FOUND)
    set(WCSLIB_INCLUDE_DIRS ${WCSLIB_INCLUDE_DIR})
    set(WCSLIB_LIBRARIES ${WCSLIB_LIBRARY})
    
    # Add utils library if found
    if(WCSLIB_UTILS_LIBRARY)
        list(APPEND WCSLIB_LIBRARIES ${WCSLIB_UTILS_LIBRARY})
    endif()
    
    # Create imported target
    if(NOT TARGET wcslib::wcs)
        add_library(wcslib::wcs UNKNOWN IMPORTED)
        set_target_properties(wcslib::wcs PROPERTIES
            IMPORTED_LOCATION "${WCSLIB_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${WCSLIB_INCLUDE_DIR}"
        )
        
        # Link utils library if found
        if(WCSLIB_UTILS_LIBRARY)
            set_property(TARGET wcslib::wcs APPEND PROPERTY
                IMPORTED_LINK_INTERFACE_LIBRARIES "${WCSLIB_UTILS_LIBRARY}"
            )
        endif()
    endif()
endif()

# Mark variables as advanced
mark_as_advanced(WCSLIB_INCLUDE_DIR WCSLIB_LIBRARY WCSLIB_UTILS_LIBRARY)