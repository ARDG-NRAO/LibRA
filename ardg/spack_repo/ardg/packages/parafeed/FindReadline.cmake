# - Try to find readline, a library for easy editing of command lines.
# Variables used by this module:
#  READLINE_ROOT_DIR     - Readline root directory
# Variables defined by this module:
#  READLINE_FOUND        - system has Readline
#  READLINE_INCLUDE_DIR  - the Readline include directory (cached)
#  READLINE_INCLUDE_DIRS - the Readline include directories
#                          (identical to READLINE_INCLUDE_DIR)
#  READLINE_LIBRARY      - the Readline library (cached)
#  READLINE_LIBRARIES    - the Readline library plus the libraries it 
#                          depends on
# Imported targets:
#  Readline::Readline    - the readline library target

# Copyright (C) 2009
# ASTRON (Netherlands Institute for Radio Astronomy)
# Updated 2025 for modern CMake practices with pkg-config fallback

if(NOT READLINE_FOUND)
  # First attempt: Standard CMake find with good hints
  find_path(READLINE_INCLUDE_DIR 
    NAMES readline/readline.h
    HINTS 
      ${READLINE_ROOT_DIR}
      $ENV{READLINE_ROOT}
      ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES include
    DOC "Readline include directory")

  find_library(READLINE_LIBRARY 
    NAMES readline
    HINTS 
      ${READLINE_ROOT_DIR}
      $ENV{READLINE_ROOT}
      ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES lib lib64
    DOC "Readline library")

  find_library(NCURSES_LIBRARY 
    NAMES ncurses cursesw curses termcap
    HINTS
      ${READLINE_ROOT_DIR}
      $ENV{READLINE_ROOT}
      ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES lib lib64
    DOC "NCurses/termcap library required by readline")

  # If standard find failed, try pkg-config fallback
  if(NOT READLINE_LIBRARY OR NOT READLINE_INCLUDE_DIR)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
      pkg_check_modules(PC_READLINE QUIET readline)
      if(PC_READLINE_FOUND)
        message(STATUS "Using pkg-config fallback for readline")
        
        # Use pkg-config results to find actual library files
        find_library(READLINE_LIBRARY 
          NAMES ${PC_READLINE_LIBRARIES}
          HINTS ${PC_READLINE_LIBRARY_DIRS}
          DOC "Readline library (via pkg-config)")
          
        find_path(READLINE_INCLUDE_DIR 
          NAMES readline/readline.h
          HINTS ${PC_READLINE_INCLUDE_DIRS}
          DOC "Readline include directory (via pkg-config)")
          
        # Also try to find ncurses in the same location
        find_library(NCURSES_LIBRARY 
          NAMES ncurses cursesw curses termcap
          HINTS ${PC_READLINE_LIBRARY_DIRS}
          DOC "NCurses library (via pkg-config)")
      endif()
    endif()
  endif()

  mark_as_advanced(READLINE_INCLUDE_DIR READLINE_LIBRARY NCURSES_LIBRARY)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Readline DEFAULT_MSG
    READLINE_LIBRARY READLINE_INCLUDE_DIR)

  if(READLINE_FOUND)
    set(READLINE_INCLUDE_DIRS ${READLINE_INCLUDE_DIR})
    # Always include ncurses if found, otherwise just readline
    if(NCURSES_LIBRARY)
      set(READLINE_LIBRARIES ${READLINE_LIBRARY} ${NCURSES_LIBRARY})
    else()
      set(READLINE_LIBRARIES ${READLINE_LIBRARY})
    endif()

    # Create imported target for modern CMake usage
    if(NOT TARGET Readline::Readline)
      add_library(Readline::Readline UNKNOWN IMPORTED)
      set_target_properties(Readline::Readline PROPERTIES
        IMPORTED_LOCATION "${READLINE_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${READLINE_INCLUDE_DIR}")
      
      if(NCURSES_LIBRARY)
        set_property(TARGET Readline::Readline APPEND PROPERTY
          INTERFACE_LINK_LIBRARIES "${NCURSES_LIBRARY}")
      endif()
    endif()
  endif()

endif(NOT READLINE_FOUND)