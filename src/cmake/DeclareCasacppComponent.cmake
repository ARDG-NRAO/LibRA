# -*- mode: cmake -*-
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2022 CASA project
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# Correspondence concerning CASA should be addressed as follows:
#        Internet email: casa-feedback@nrao.edu.

# Function to help define a casacpp component
macro(declare_casacpp_component name)

  # List the files that are going to be compiled. It is a recursive search
  file(GLOB_RECURSE ${name}_library_sources "*.cc" "*.c" "*.f")

  # Ignore files under test directories
  list(FILTER ${name}_library_sources EXCLUDE REGEX "/test/")
  
  # List the header files and template header files, also recursive
  file(GLOB_RECURSE ${name}_public_headers CONFIGURE_DEPENDS "*.h" "*.tcc" "*.INC")

  # Add the library target to cmake
  add_library(casacpp_${name} SHARED)

  # Specify which files belong to the target
  target_sources(casacpp_${name}
    PRIVATE 
      ${${name}_library_sources}
    PUBLIC 
      ${${name}_public_headers})

  # Set the version property of the shared library
  set_target_properties(casacpp_${name} PROPERTIES VERSION ${PROJECT_VERSION})

  # Set the shared object version property to the project's major version.
  set_target_properties(casacpp_${name} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

  # Set -Wall compiler option for supported compilers
  target_compile_options(casacpp_${name} PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall>)

  # Add C++ tests
  # Take all files under /test/ directory
  file(GLOB_RECURSE ${name}_cpp_tests_sources "*.cc")
  list(FILTER ${name}_cpp_tests_sources INCLUDE REGEX "/test/")
  # Exclude Google tests
  list(FILTER ${name}_cpp_tests_sources EXCLUDE REGEX "_GT.cc")

  # Loop on found tests
  foreach(test_source ${${name}_cpp_tests_sources})
    get_filename_component(test_name ${test_source} NAME_WLE)

    # Add executable with the name of the file without extension
    add_executable(${test_name} ${test_source})

    # Internal dependencies
    # Also here declare at least this module as implicit dependency.
    # If the cpp module has declared additional include or link 
    # options with PUBLIC then those are automatically propagated
    # to the test
    target_link_libraries(${test_name} PRIVATE casacpp_${name})
    add_dependencies(${test_name} casacpp_${name})
   #target_link_libraries( ${filename}  celma ${Boost_Test_Link_Libs} )


    # Declare this is a test
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()

  # Install the library
  install(TARGETS casacpp_${name}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

  # Install the header and template header files
  # Note that this preserves the directory structure of the files
  # That's why ${name}_public_headers is not reused
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/${name}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/casacpp
      FILES_MATCHING
      REGEX "/.*(h|tcc|INC)$")

endmacro()
