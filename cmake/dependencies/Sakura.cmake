#-*- cmake -*-
# Copyright (C) 2025, 2026
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
# Sakura.cmake - Sakura dependency check and build (optional)

function(check_and_add_sakura)
    if(NOT LIBRA_USE_LIBSAKURA)
        message("${LIBRA_ID} Sakura disabled - skipping")
        set(LIBRA_SAKURA_BUILT FALSE PARENT_SCOPE)
        return()
    endif()
    
    message("${LIBRA_ID} Checking for Sakura...")
    
    # Try to find system Sakura
    find_package(libsakura 5.1.0 QUIET CONFIG)
    
    if(libsakura_FOUND)
        message("${LIBRA_ID} Using system Sakura ${libsakura_VERSION}")
        set(LIBRA_SAKURA_BUILT FALSE PARENT_SCOPE)
    else()
        message("${LIBRA_ID} System Sakura not found - will build from source")
        
        ExternalProject_Add(
            Sakura
            GIT_REPOSITORY ${LIBRA_SAKURA_GIT_REPOSITORY}
            GIT_TAG        ${LIBRA_SAKURA_GIT_TAG}
            UPDATE_DISCONNECTED TRUE
            SOURCE_DIR     ${LIBRA_DEPENDENCIES_DIR}/sakura
            BINARY_DIR     ${BUILD_DIR}/sakura
            SOURCE_SUBDIR  libsakura
            CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                           -DCMAKE_INSTALL_LIBDIR=lib
                           -DCMAKE_INSTALL_BINDIR=bin/sakura
                           -DBUILD_DOC:BOOL=OFF
                           -DPYTHON_BINDING:BOOL=OFF
                           -DSIMD_ARCH=GENERIC
                           -DENABLE_TEST:BOOL=OFF
            BUILD_COMMAND   make -j ${NCORES}
            INSTALL_COMMAND make install
        )
        
        set(LIBRA_SAKURA_BUILT TRUE PARENT_SCOPE)
    endif()
    
    message("${LIBRA_ID} Sakura configuration complete")
endfunction()
