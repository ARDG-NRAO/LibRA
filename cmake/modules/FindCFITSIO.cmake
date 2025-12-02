# FindCFITSIO.cmake
#
# Find the CFITSIO library
#
# This module defines:
#  CFITSIO_FOUND - True if CFITSIO is found
#  CFITSIO_INCLUDE_DIRS - Include directories for CFITSIO
#  CFITSIO_LIBRARIES - Libraries to link against CFITSIO
#  CFITSIO_VERSION_STRING - Version string
#  CFITSIO::CFITSIO - Imported target for CFITSIO

# Don't search if already found
if(TARGET CFITSIO::CFITSIO)
    return()
endif()

# Use CFITSIO_ROOT if provided, plus CMAKE_PREFIX_PATH
set(_CFITSIO_SEARCH_DIRS "")
if(CFITSIO_ROOT)
    list(APPEND _CFITSIO_SEARCH_DIRS ${CFITSIO_ROOT})
endif()

# Add CMAKE_PREFIX_PATH explicitly for Spack compatibility
if(CMAKE_PREFIX_PATH)
    list(APPEND _CFITSIO_SEARCH_DIRS ${CMAKE_PREFIX_PATH})
endif()

# Spack-specific detection and handling
if(DEFINED ENV{SPACK_ROOT})
    message(STATUS "Spack environment detected")

    # Look for Spack-installed CFITSIO in common locations
    file(GLOB _spack_cfitsio_dirs "$ENV{SPACK_ROOT}/opt/spack/*/cfitsio*")
    if(_spack_cfitsio_dirs)
        list(APPEND _CFITSIO_SEARCH_DIRS ${_spack_cfitsio_dirs})
        message(STATUS "Found potential Spack CFITSIO installations: ${_spack_cfitsio_dirs}")
    endif()

    # Also check if CFITSIO is in current Spack environment
    if(DEFINED ENV{SPACK_ENV})
        execute_process(COMMAND spack find --paths cfitsio
            OUTPUT_VARIABLE _spack_cfitsio_path
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(_spack_cfitsio_path)
            string(REGEX MATCH "([^ \t\n]+)[ \t]+([^\n]+)" _spack_match "${_spack_cfitsio_path}")
            if(CMAKE_MATCH_2)
                list(APPEND _CFITSIO_SEARCH_DIRS ${CMAKE_MATCH_2})
                message(STATUS "Spack CFITSIO found at: ${CMAKE_MATCH_2}")
            endif()
        endif()
    endif()
endif()

# Find the header file
find_path(CFITSIO_INCLUDE_DIR
    NAMES fitsio.h
    HINTS ${_CFITSIO_SEARCH_DIRS}
    PATH_SUFFIXES include include/cfitsio include/libcfitsio
    DOC "CFITSIO include directory"
)

# Extract version if header found
if(CFITSIO_INCLUDE_DIR AND EXISTS "${CFITSIO_INCLUDE_DIR}/fitsio.h")
    file(READ "${CFITSIO_INCLUDE_DIR}/fitsio.h" CFITSIO_H)
    string(REGEX MATCH "#define CFITSIO_VERSION ([0-9]+\\.[0-9]+)" CFITSIO_VERSION_MATCH "${CFITSIO_H}")
    if(CFITSIO_VERSION_MATCH)
        set(CFITSIO_VERSION_STRING "${CMAKE_MATCH_1}")
    endif()
endif()

# Find the CFITSIO library
find_library(CFITSIO_LIBRARY
    NAMES cfitsio
    HINTS ${_CFITSIO_SEARCH_DIRS}
    PATH_SUFFIXES lib lib64
    DOC "CFITSIO library"
)

# Handle standard arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CFITSIO
    REQUIRED_VARS CFITSIO_LIBRARY CFITSIO_INCLUDE_DIR
    VERSION_VAR CFITSIO_VERSION_STRING
)

if(CFITSIO_FOUND)
    set(CFITSIO_INCLUDE_DIRS ${CFITSIO_INCLUDE_DIR})
    set(CFITSIO_LIBRARIES ${CFITSIO_LIBRARY})
    
    # Create imported target
    if(NOT TARGET CFITSIO::CFITSIO)
        add_library(CFITSIO::CFITSIO UNKNOWN IMPORTED)
        set_target_properties(CFITSIO::CFITSIO PROPERTIES
            IMPORTED_LOCATION "${CFITSIO_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${CFITSIO_INCLUDE_DIR}"
        )
    endif()
endif()

# Mark variables as advanced
mark_as_advanced(CFITSIO_INCLUDE_DIR CFITSIO_LIBRARY)