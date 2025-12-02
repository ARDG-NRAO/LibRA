# FindGSL.cmake
#
# Find the GNU Scientific Library (GSL)
#
# This module defines:
#  GSL_FOUND - True if GSL is found
#  GSL_INCLUDE_DIRS - Include directories for GSL
#  GSL_LIBRARIES - Libraries to link against GSL
#  GSL_VERSION - Version string
#  GSL::gsl - Imported target for GSL main library
#  GSL::gslcblas - Imported target for GSL CBLAS library

# Don't search if already found by CMake's native FindGSL
if(TARGET GSL::gsl)
    return()
endif()

# Use GSL_ROOT_DIR if provided, plus CMAKE_PREFIX_PATH and Spack detection
set(_GSL_SEARCH_DIRS "")
if(GSL_ROOT_DIR)
    list(APPEND _GSL_SEARCH_DIRS ${GSL_ROOT_DIR})
endif()

# Add CMAKE_PREFIX_PATH explicitly
if(CMAKE_PREFIX_PATH)
    list(APPEND _GSL_SEARCH_DIRS ${CMAKE_PREFIX_PATH})
endif()

# Spack-specific detection and handling (only if LIBRA_USE_SPACK is enabled)
if(LIBRA_USE_SPACK AND DEFINED ENV{SPACK_ROOT})
    message(STATUS "Spack environment detected")
    
    # Look for Spack-installed GSL in common locations
    file(GLOB _spack_gsl_dirs "$ENV{SPACK_ROOT}/opt/spack/*/gsl*")
    if(_spack_gsl_dirs)
        list(APPEND _GSL_SEARCH_DIRS ${_spack_gsl_dirs})
        message(STATUS "Found potential Spack GSL installations: ${_spack_gsl_dirs}")
    endif()
    
    # Also check if GSL is in current Spack environment
    if(DEFINED ENV{SPACK_ENV})
        execute_process(COMMAND spack find --paths gsl
            OUTPUT_VARIABLE _spack_gsl_path
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(_spack_gsl_path)
            string(REGEX MATCH "([^ \t\n]+)[ \t]+([^\n]+)" _spack_match "${_spack_gsl_path}")
            if(CMAKE_MATCH_2)
                list(APPEND _GSL_SEARCH_DIRS ${CMAKE_MATCH_2})
                message(STATUS "Spack GSL found at: ${CMAKE_MATCH_2}")
            endif()
        endif()
    endif()
endif()

message(STATUS "GSL search directories: ${_GSL_SEARCH_DIRS}")

# Find the header file
find_path(GSL_INCLUDE_DIR 
    NAMES gsl/gsl_math.h
    HINTS ${_GSL_SEARCH_DIRS}
    PATH_SUFFIXES include
    DOC "GSL include directory"
)

# Extract version if header found
if(GSL_INCLUDE_DIR AND EXISTS "${GSL_INCLUDE_DIR}/gsl/gsl_version.h")
    file(READ "${GSL_INCLUDE_DIR}/gsl/gsl_version.h" GSL_H)
    string(REGEX MATCH "#define GSL_VERSION \"([0-9]+\\.[0-9]+(\\.[0-9]+)?)\"" GSL_VERSION_MATCH "${GSL_H}")
    if(GSL_VERSION_MATCH)
        set(GSL_VERSION "${CMAKE_MATCH_1}")
    else()
        set(GSL_VERSION "Unknown")
    endif()
endif()

# Find the main GSL library
find_library(GSL_LIBRARY 
    NAMES gsl
    HINTS ${_GSL_SEARCH_DIRS}
    PATH_SUFFIXES lib lib64
    DOC "GSL main library"
)

# Find the GSL CBLAS library
find_library(GSL_CBLAS_LIBRARY 
    NAMES gslcblas cblas
    HINTS ${_GSL_SEARCH_DIRS}
    PATH_SUFFIXES lib lib64
    DOC "GSL CBLAS library"
)

# Handle standard arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GSL
    REQUIRED_VARS GSL_LIBRARY GSL_CBLAS_LIBRARY GSL_INCLUDE_DIR
    VERSION_VAR GSL_VERSION
)

if(GSL_FOUND)
    set(GSL_INCLUDE_DIRS ${GSL_INCLUDE_DIR})
    set(GSL_LIBRARIES ${GSL_LIBRARY} ${GSL_CBLAS_LIBRARY})
    
    # Create imported targets
    if(NOT TARGET GSL::gsl)
        add_library(GSL::gsl UNKNOWN IMPORTED)
        set_target_properties(GSL::gsl PROPERTIES
            IMPORTED_LOCATION "${GSL_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${GSL_INCLUDE_DIR}"
        )
    endif()
    
    if(NOT TARGET GSL::gslcblas)
        add_library(GSL::gslcblas UNKNOWN IMPORTED)
        set_target_properties(GSL::gslcblas PROPERTIES
            IMPORTED_LOCATION "${GSL_CBLAS_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${GSL_INCLUDE_DIR}"
        )
    endif()
endif()

# Mark variables as advanced
mark_as_advanced(GSL_INCLUDE_DIR GSL_LIBRARY GSL_CBLAS_LIBRARY)