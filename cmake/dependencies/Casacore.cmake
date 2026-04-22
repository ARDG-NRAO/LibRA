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
# Casacore external project
include(ExternalProject)
find_package(Git REQUIRED)

set(LIBRA_CASACORE_CXX_FLAGS "")
set(LIBRA_CASACORE_C_FLAGS "")
set(LIBRA_CASACORE_EXE_LINKER_FLAGS "")
set(LIBRA_CASACORE_SHARED_LINKER_FLAGS "")
set(LIBRA_CASACORE_MODULE_LINKER_FLAGS "")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(LIBRA_CASACORE_CXX_FLAGS "-fno-tree-vrp")
endif()
if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Homebrew GCC on macOS misdetects quick_exit support and does not accept
  # Apple's _Static_assert usage in Mach headers without this compatibility shim.
  string(APPEND LIBRA_CASACORE_CXX_FLAGS
         " -D_Static_assert=static_assert"
         " -U_GLIBCXX_HAVE_AT_QUICK_EXIT"
         " -U_GLIBCXX_HAVE_QUICK_EXIT")
endif()
if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  string(APPEND LIBRA_CASACORE_C_FLAGS
         " -I/opt/homebrew/opt/libomp/include")
  string(APPEND LIBRA_CASACORE_CXX_FLAGS
         " -I/opt/homebrew/opt/libomp/include")
  set(LIBRA_CASACORE_EXE_LINKER_FLAGS
      "-L/opt/homebrew/opt/libomp/lib -lomp")
  set(LIBRA_CASACORE_SHARED_LINKER_FLAGS
      "-L/opt/homebrew/opt/libomp/lib -lomp")
  set(LIBRA_CASACORE_MODULE_LINKER_FLAGS
      "-L/opt/homebrew/opt/libomp/lib -lomp")
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  # GCC 15 defaults to a newer C dialect that breaks casacore's legacy mirlib
  # declarations with empty parameter lists.
  set(LIBRA_CASACORE_C_FLAGS "-std=gnu17")
endif()

set(LIBRA_CASACORE_REPOSITORY ${LIBRA_CASACORE_GIT_REPOSITORY})
if(EXISTS "$ENV{HOME}/SoftwareProjects/casa/casatools/casacore/.git")
  set(LIBRA_CASACORE_REPOSITORY "$ENV{HOME}/SoftwareProjects/casa/casatools/casacore")
  message(STATUS "Using local casacore checkout at ${LIBRA_CASACORE_REPOSITORY}")
elseif(EXISTS "$ENV{HOME}/SoftwareProjects/casacore/.git")
  set(LIBRA_CASACORE_REPOSITORY "$ENV{HOME}/SoftwareProjects/casacore")
  message(STATUS "Using local casacore checkout at ${LIBRA_CASACORE_REPOSITORY}")
endif()

# Create the path ${CMAKE_SOURCE_DIR}/install/data and download the data into it
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/install/data)
if(CASACORE_DATA_DOWNLOAD)
  ExternalProject_Add(
    DataDownload
    DOWNLOAD_COMMAND bash -c "curl ${LIBRA_MEASURES_FTP} | tar -C ${CMAKE_SOURCE_DIR}/install/data -xzf - || curl ${LIBRA_MEASURES_HTTPS} | tar -C ${CMAKE_SOURCE_DIR}/install/data -xzf -"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
else()
  ExternalProject_Add(
    DataDownload
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
endif()

ExternalProject_Add(
  Casacore
  DOWNLOAD_COMMAND ${CMAKE_COMMAND}
                   -Dgit_executable=${GIT_EXECUTABLE}
                   -Drepo=${LIBRA_CASACORE_REPOSITORY}
                   -Dtag=${LIBRA_CASACORE_GIT_TAG}
                   -Ddestination=${CMAKE_SOURCE_DIR}/dependencies/casacore
                   -P ${CMAKE_SOURCE_DIR}/cmake/dependencies/scripts/CloneFilteredRepo.cmake
  UPDATE_COMMAND  ""
  PATCH_COMMAND   ${CMAKE_COMMAND}
                  -Dsource_dir=<SOURCE_DIR>
                  -Dpatch_dir=${CMAKE_SOURCE_DIR}/cmake/dependencies/patches
                  -P ${CMAKE_SOURCE_DIR}/cmake/dependencies/scripts/PatchCasacore.cmake
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/casacore
  BINARY_DIR     ${BUILD_DIR}/casacore
  CONFIGURE_COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/dependencies/casacore
                    -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                    -DCMAKE_PREFIX_PATH=${INSTALL_DIR}
                    -DCMAKE_INSTALL_LIBDIR=lib
                    -DCMAKE_INSTALL_BINDIR=bin/casacore
                    ${LIBRA_EXTERNAL_CMAKE_COMPILER_ARGS}
                    -DCMAKE_C_FLAGS=${LIBRA_CASACORE_C_FLAGS}
                    -DCMAKE_EXE_LINKER_FLAGS=${LIBRA_CASACORE_EXE_LINKER_FLAGS}
                    -DCMAKE_SHARED_LINKER_FLAGS=${LIBRA_CASACORE_SHARED_LINKER_FLAGS}
                    -DCMAKE_MODULE_LINKER_FLAGS=${LIBRA_CASACORE_MODULE_LINKER_FLAGS}
                    -DDATA_DIR=${CMAKE_SOURCE_DIR}/install/data
                    -DUSE_OPENMP=ON
                    -DUSE_THREADS=ON
                    -DCMAKE_CXX_FLAGS=${LIBRA_CASACORE_CXX_FLAGS}
                    -DBUILD_FFTPACK_DEPRECATED=ON
                    -DBUILD_TESTING=OFF
                    -DBUILD_PYTHON=OFF
                    -DBUILD_PYTHON3=OFF
                    -DBUILD_DYSCO=ON
                    -DUseCcache=0 
                    -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
                    -DFFTW_LIBRARIES=${INSTALL_DIR}/lib/ 
                    -DFFTW_ROOT=${INSTALL_DIR}
                    -DFFTW3_INCLUDE_DIR=${INSTALL_DIR}/include
                    ${CCACHE_LAUNCHER}
                    ${SPACK_FLAGS}
  BUILD_COMMAND  make -j${NCORES}
  INSTALL_COMMAND make install
  DEPENDS FFTW_float DataDownload
)
