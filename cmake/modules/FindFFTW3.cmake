# FindFFTW3.cmake
#
# Find the FFTW3 library with component support
#
# This module defines:
#  FFTW3_FOUND - True if all requested FFTW3 components are found
#  FFTW3_INCLUDE_DIRS - Include directories for FFTW3
#  FFTW3_LIBRARIES - All found FFTW3 libraries
#  FFTW3_VERSION - Version string
#  FFTW3::<component> - Imported targets for each component
#
# Valid components:
#  single, double, longdouble, quad, threads, openmp, mpi
#
# Usage:
#  find_package(FFTW3 REQUIRED COMPONENTS double threads)

# Use FFTW3_ROOT if provided, plus CMAKE_PREFIX_PATH
set(_FFTW3_SEARCH_DIRS "")
if(FFTW3_ROOT)
    list(APPEND _FFTW3_SEARCH_DIRS ${FFTW3_ROOT})
endif()

# Add CMAKE_PREFIX_PATH explicitly for Spack compatibility
if(CMAKE_PREFIX_PATH)
    list(APPEND _FFTW3_SEARCH_DIRS ${CMAKE_PREFIX_PATH})
endif()

# Find the main include directory
find_path(FFTW3_INCLUDE_DIR
    NAMES fftw3.h
    HINTS ${_FFTW3_SEARCH_DIRS}
    PATH_SUFFIXES include
    DOC "FFTW3 include directory"
)

# Extract version if header found
if(FFTW3_INCLUDE_DIR AND EXISTS "${FFTW3_INCLUDE_DIR}/fftw3.h")
    file(READ "${FFTW3_INCLUDE_DIR}/fftw3.h" FFTW3_H)
    string(REGEX MATCH "#define FFTW_VERSION \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" FFTW3_VERSION_MATCH "${FFTW3_H}")
    if(FFTW3_VERSION_MATCH)
        set(FFTW3_VERSION "${CMAKE_MATCH_1}")
    endif()
endif()

# Define component to library name mapping
set(_FFTW3_COMPONENT_LIBS
    single:fftw3f
    double:fftw3
    longdouble:fftw3l
    quad:fftw3q
    threads:fftw3_threads
    openmp:fftw3_omp
    mpi:fftw3_mpi
)

# If no components specified, default to double precision
if(NOT FFTW3_FIND_COMPONENTS)
    set(FFTW3_FIND_COMPONENTS double)
endif()

# Find requested components
set(FFTW3_LIBRARIES "")
set(_FFTW3_MISSING_COMPONENTS "")

foreach(_component ${FFTW3_FIND_COMPONENTS})
    string(TOUPPER ${_component} _COMPONENT_UPPER)
    
    # Get library name for this component
    set(_lib_name "")
    foreach(_mapping ${_FFTW3_COMPONENT_LIBS})
        string(REPLACE ":" ";" _mapping_parts ${_mapping})
        list(GET _mapping_parts 0 _comp_name)
        list(GET _mapping_parts 1 _lib_name_candidate)
        if(_comp_name STREQUAL _component)
            set(_lib_name ${_lib_name_candidate})
            break()
        endif()
    endforeach()
    
    if(NOT _lib_name)
        message(WARNING "Unknown FFTW3 component: ${_component}")
        list(APPEND _FFTW3_MISSING_COMPONENTS ${_component})
        continue()
    endif()
    
    # Find the library for this component
    find_library(FFTW3_${_COMPONENT_UPPER}_LIBRARY
        NAMES ${_lib_name}
        HINTS ${_FFTW3_SEARCH_DIRS}
        PATH_SUFFIXES lib lib64
        DOC "FFTW3 ${_component} library"
    )
    
    if(FFTW3_${_COMPONENT_UPPER}_LIBRARY)
        list(APPEND FFTW3_LIBRARIES ${FFTW3_${_COMPONENT_UPPER}_LIBRARY})
        
        # Create imported target for this component
        if(NOT TARGET FFTW3::${_component})
            add_library(FFTW3::${_component} UNKNOWN IMPORTED)
            set_target_properties(FFTW3::${_component} PROPERTIES
                IMPORTED_LOCATION "${FFTW3_${_COMPONENT_UPPER}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${FFTW3_INCLUDE_DIR}"
            )
        endif()
    else()
        list(APPEND _FFTW3_MISSING_COMPONENTS ${_component})
    endif()
    
    mark_as_advanced(FFTW3_${_COMPONENT_UPPER}_LIBRARY)
endforeach()

# Handle standard arguments
include(FindPackageHandleStandardArgs)
if(_FFTW3_MISSING_COMPONENTS)
    set(_FFTW3_ERROR_MESSAGE "Could not find FFTW3 components: ${_FFTW3_MISSING_COMPONENTS}")
endif()

find_package_handle_standard_args(FFTW3
    REQUIRED_VARS FFTW3_INCLUDE_DIR
    VERSION_VAR FFTW3_VERSION
    FAIL_MESSAGE "${_FFTW3_ERROR_MESSAGE}"
)

if(FFTW3_FOUND)
    set(FFTW3_INCLUDE_DIRS ${FFTW3_INCLUDE_DIR})
    
    # Only succeed if all requested components were found
    if(_FFTW3_MISSING_COMPONENTS)
        set(FFTW3_FOUND FALSE)
    endif()
endif()

# Mark variables as advanced
mark_as_advanced(FFTW3_INCLUDE_DIR)