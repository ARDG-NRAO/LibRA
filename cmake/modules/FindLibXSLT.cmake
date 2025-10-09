# - Try to find LibXSLT: XSLT C library for GNOME
# Variables used by this module:
#  LibXSLT_ROOT_DIR     - LibXSLT root directory
# Variables defined by this module:
#  LibXSLT_FOUND        - system has LibXSLT
#  LibXSLT_INCLUDE_DIR  - the LibXSLT include directory (cached)
#  LibXSLT_INCLUDE_DIRS - the LibXSLT include directories
#                         (identical to LibXSLT_INCLUDE_DIR)
#  LibXSLT_LIBRARY      - the LibXSLT library (cached)
#  LibXSLT_LIBRARIES    - the LibXSLT libraries
#                         (identical to LibXSLT_LIBRARY plus dependencies)
#  LibXSLT_VERSION_STRING the found version of LibXSLT

# Copyright (C) 2025 NRAO (National Radio Astronomy Observatory)
# SPDX-License-Identifier: GPL-2.0-or-later

if(NOT LibXSLT_FOUND)

  find_path(LibXSLT_INCLUDE_DIR libxslt/xslt.h
    HINTS ${LibXSLT_ROOT_DIR} 
    PATH_SUFFIXES include libxslt)

  if(LibXSLT_INCLUDE_DIR)
    # Try to extract version from header file if available
    if(EXISTS "${LibXSLT_INCLUDE_DIR}/libxslt/xsltconfig.h")
      FILE(READ "${LibXSLT_INCLUDE_DIR}/libxslt/xsltconfig.h" LIBXSLT_H)
      set(LIBXSLT_VERSION_REGEX ".*#define LIBXSLT_VERSION[^0-9]*([0-9]+).*")
      if ("${LIBXSLT_H}" MATCHES ${LIBXSLT_VERSION_REGEX})
        STRING(REGEX REPLACE ${LIBXSLT_VERSION_REGEX}
                             "\\1" LibXSLT_VERSION_STRING "${LIBXSLT_H}")
      else ()
        set(LibXSLT_VERSION_STRING "Unknown")
      endif ()
    else()
      set(LibXSLT_VERSION_STRING "Unknown")
    endif()
  endif(LibXSLT_INCLUDE_DIR)

  find_library(LibXSLT_LIBRARY xslt
    HINTS ${LibXSLT_ROOT_DIR} 
    PATH_SUFFIXES lib lib64)

  find_library(EXSLT_LIBRARY exslt
    HINTS ${LibXSLT_ROOT_DIR} 
    PATH_SUFFIXES lib lib64)

  # LibXSLT requires libxml2
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_LIBXML2 QUIET libxml-2.0)
  endif()

  find_path(LIBXML2_INCLUDE_DIR libxml/parser.h
    HINTS ${PC_LIBXML2_INCLUDEDIR} ${PC_LIBXML2_INCLUDE_DIRS}
    PATH_SUFFIXES libxml2)

  find_library(LIBXML2_LIBRARY xml2
    HINTS ${PC_LIBXML2_LIBDIR} ${PC_LIBXML2_LIBRARY_DIRS})

  mark_as_advanced(LibXSLT_INCLUDE_DIR LibXSLT_LIBRARY EXSLT_LIBRARY
                   LIBXML2_INCLUDE_DIR LIBXML2_LIBRARY)

  if(CMAKE_VERSION VERSION_LESS "2.8.3")
    find_package_handle_standard_args(LibXSLT DEFAULT_MSG
      LibXSLT_LIBRARY LIBXML2_LIBRARY LibXSLT_INCLUDE_DIR)
  else ()
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(LibXSLT
      REQUIRED_VARS LibXSLT_LIBRARY LIBXML2_LIBRARY LibXSLT_INCLUDE_DIR
      VERSION_VAR LibXSLT_VERSION_STRING)
  endif ()

  if(LibXSLT_FOUND)
    set(LibXSLT_INCLUDE_DIRS ${LibXSLT_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
    set(LibXSLT_LIBRARIES ${LibXSLT_LIBRARY} ${LIBXML2_LIBRARY})
    
    # Add exslt if found
    if(EXSLT_LIBRARY)
      list(APPEND LibXSLT_LIBRARIES ${EXSLT_LIBRARY})
    endif()
  endif()

endif(NOT LibXSLT_FOUND)