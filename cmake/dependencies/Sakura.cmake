# Sakura.cmake - Sakura dependency check and build (optional)
# Copyright 2025 Associated Universities, Inc. Washington DC, USA.

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
            SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/sakura
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