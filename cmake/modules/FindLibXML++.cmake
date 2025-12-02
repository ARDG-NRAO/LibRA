# - Try to find LibXML++: C++ wrapper for libxml2
# Variables used by this module:
#  LibXML++_ROOT_DIR     - LibXML++ root directory
# Variables defined by this module:
#  LibXML++_FOUND        - system has LibXML++
#  LibXML++_INCLUDE_DIR  - the LibXML++ include directory (cached)
#  LibXML++_INCLUDE_DIRS - the LibXML++ include directories
#                          (identical to LibXML++_INCLUDE_DIR)
#  LibXML++_LIBRARY      - the LibXML++ library (cached)
#  LibXML++_LIBRARIES    - the LibXML++ libraries
#                          (identical to LibXML++_LIBRARY)
#  LibXML++_VERSION_STRING the found version of LibXML++

# Copyright (C) 2025 NRAO (National Radio Astronomy Observatory)
# SPDX-License-Identifier: GPL-2.0-or-later

if(NOT LibXML++_FOUND)

  # Try to find LibXML++-2.6 first (most common)
  find_path(LibXML++_INCLUDE_DIR libxml++/libxml++.h
    HINTS ${LibXML++_ROOT_DIR} 
    PATH_SUFFIXES 
      include/libxml++-2.6 
      include/libxml++-3.0
      include
    )

  if(LibXML++_INCLUDE_DIR)
    # Try to extract version from header file if available
    if(EXISTS "${LibXML++_INCLUDE_DIR}/libxml++/libxml++.h")
      FILE(READ "${LibXML++_INCLUDE_DIR}/libxml++/libxml++.h" LIBXMLPP_H)
      set(LIBXMLPP_VERSION_REGEX ".*#define LIBXMLPP_VERSION[^0-9]*([0-9]+)\\.([0-9]+).*")
      if ("${LIBXMLPP_H}" MATCHES ${LIBXMLPP_VERSION_REGEX})
        STRING(REGEX REPLACE ${LIBXMLPP_VERSION_REGEX}
                             "\\1.\\2" LibXML++_VERSION_STRING "${LIBXMLPP_H}")
      else ()
        set(LibXML++_VERSION_STRING "Unknown")
      endif ()
    else()
      set(LibXML++_VERSION_STRING "Unknown")
    endif()
  endif(LibXML++_INCLUDE_DIR)

  # Try to find the library - could be libxml++-2.6 or libxml++-3.0
  find_library(LibXML++_LIBRARY 
    NAMES xml++-2.6 xml++-3.0 xml++
    HINTS ${LibXML++_ROOT_DIR} 
    PATH_SUFFIXES lib lib64)

  # LibXML++ requires libxml2 and glibmm
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_LIBXML2 QUIET libxml-2.0)
    pkg_check_modules(PC_GLIBMM QUIET glibmm-2.4)
  endif()

  find_path(LIBXML2_INCLUDE_DIR libxml/parser.h
    HINTS ${PC_LIBXML2_INCLUDEDIR} ${PC_LIBXML2_INCLUDE_DIRS}
    PATH_SUFFIXES libxml2)

  find_library(LIBXML2_LIBRARY xml2
    HINTS ${PC_LIBXML2_LIBDIR} ${PC_LIBXML2_LIBRARY_DIRS})

  find_path(GLIBMM_INCLUDE_DIR glibmm.h
    HINTS ${PC_GLIBMM_INCLUDEDIR} ${PC_GLIBMM_INCLUDE_DIRS}
    PATH_SUFFIXES glibmm-2.4)

  find_library(GLIBMM_LIBRARY glibmm-2.4
    HINTS ${PC_GLIBMM_LIBDIR} ${PC_GLIBMM_LIBRARY_DIRS})

  mark_as_advanced(LibXML++_INCLUDE_DIR LibXML++_LIBRARY 
                   LIBXML2_INCLUDE_DIR LIBXML2_LIBRARY
                   GLIBMM_INCLUDE_DIR GLIBMM_LIBRARY)

  if(CMAKE_VERSION VERSION_LESS "2.8.3")
    find_package_handle_standard_args(LibXML++ DEFAULT_MSG
      LibXML++_LIBRARY LIBXML2_LIBRARY GLIBMM_LIBRARY LibXML++_INCLUDE_DIR)
  else ()
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(LibXML++
      REQUIRED_VARS LibXML++_LIBRARY LIBXML2_LIBRARY GLIBMM_LIBRARY LibXML++_INCLUDE_DIR
      VERSION_VAR LibXML++_VERSION_STRING)
  endif ()

  if(LibXML++_FOUND)
    set(LibXML++_INCLUDE_DIRS ${LibXML++_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${GLIBMM_INCLUDE_DIR})
    set(LibXML++_LIBRARIES ${LibXML++_LIBRARY} ${LIBXML2_LIBRARY} ${GLIBMM_LIBRARY})
  endif()

endif(NOT LibXML++_FOUND)