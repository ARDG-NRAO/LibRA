# LibraUtilities.cmake - Helper functions and macros for LibRA
# Copyright 2025 Associated Universities, Inc. Washington DC, USA.

# Function to detect system information
function(libra_detect_system_info)
    if (NOT DEFINED NCORES)
        cmake_host_system_information(RESULT NCORES QUERY NUMBER_OF_PHYSICAL_CORES)
        cmake_host_system_information(RESULT NTHREADS QUERY NUMBER_OF_LOGICAL_CORES)
        message("${LIBRA_ID} Number of physical cores: ${NCORES}")
        message("${LIBRA_ID} Number of logical cores: ${NTHREADS}")
        math(EXPR NCORES "${NCORES} - 2")
        if (${NCORES} LESS 1)
            set(NCORES 1)
        endif()
        set(NCORES ${NCORES} PARENT_SCOPE)
    endif()

    cmake_host_system_information(RESULT OS_NAME QUERY OS_NAME)
    cmake_host_system_information(RESULT OS_RELEASE QUERY OS_RELEASE)
    cmake_host_system_information(RESULT OS_VERSION QUERY OS_VERSION)
    cmake_host_system_information(RESULT OS_PLATFORM QUERY OS_PLATFORM)
    cmake_host_system_information(RESULT AVAILABLE_PHYSICAL_MEMORY QUERY AVAILABLE_PHYSICAL_MEMORY)
    cmake_host_system_information(RESULT TOTAL_PHYSICAL_MEMORY QUERY TOTAL_PHYSICAL_MEMORY)

    message("${LIBRA_ID} OS Name: ${OS_NAME}")
    message("${LIBRA_ID} OS Release: ${OS_RELEASE}")
    message("${LIBRA_ID} OS Version: ${OS_VERSION}")
    message("${LIBRA_ID} OS Platform: ${OS_PLATFORM}")
    message("${LIBRA_ID} Available Physical Memory: ${AVAILABLE_PHYSICAL_MEMORY} BYTES")
    message("${LIBRA_ID} Total Physical Memory: ${TOTAL_PHYSICAL_MEMORY} BYTES")
    message("${LIBRA_ID} Setting number of cores to ${NCORES}")
endfunction()

# Function to setup SPACK environment detection
function(libra_setup_spack)
    find_package(SPACKENV)
    if (SPACKENV_FOUND)
        set(SPACK_FLAGS "-DCMAKE_CXX_FLAGS=-I${SPACK_VIEW_ROOT}/include  -L${SPACK_VIEW_ROOT}/lib64 -L${SPACK_VIEW_ROOT}/lib " PARENT_SCOPE)
        message("${LIBRA_ID} Adding SPACK flags ${SPACK_FLAGS}")
    else()
        set(SPACK_FLAGS "" PARENT_SCOPE)
    endif()
endfunction()

# Function to setup CCACHE if enabled
function(libra_setup_ccache)
    if (USE_CCACHE)
        include(cmake/EnableCcache.cmake)
        set(CCACHE_LAUNCHER "-DCMAKE_CXX_COMPILER_LAUNCHER=${CCACHE_PROGRAM} -DCMAKE_C_COMPILER_LAUNCHER=${CCACHE_PROGRAM}" PARENT_SCOPE)
        message("${LIBRA_ID} Adding CCACHE_LAUNCHER flag ${CCACHE_LAUNCHER}")
    else()
        set(CCACHE_LAUNCHER "" PARENT_SCOPE)
    endif()
endfunction()

# Function to setup common include and link directories
function(libra_setup_common_paths)
    include_directories(
        ${INSTALL_DIR}/include
        ${INSTALL_DIR}/include/casacpp
        /usr/include/cfitsio
        ${CMAKE_SOURCE_DIR}/apps/src
        ${CMAKE_SOURCE_DIR}/src
        ${INSTALL_DIR}/include/pybind11
    )

    link_directories(
        ${INSTALL_DIR}/lib
        /usr/local/lib
        /usr/lib64
        /usr/lib
        /usr/lib/x86_64-linux-gnu
    )

    # Set RPATH to prefer our installed libraries over system ones
    set(CMAKE_INSTALL_RPATH "${INSTALL_DIR}/lib" PARENT_SCOPE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE PARENT_SCOPE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE PARENT_SCOPE)
endfunction()

# Macro to standardize ExternalProject configuration
macro(libra_add_external_project PROJECT_NAME)
    set(options "")
    set(oneValueArgs GIT_REPOSITORY GIT_TAG URL SOURCE_SUBDIR)
    set(multiValueArgs DEPENDS CMAKE_ARGS CONFIGURE_COMMAND BUILD_COMMAND INSTALL_COMMAND)
    cmake_parse_arguments(LIBRA_EP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(LIBRA_EP_GIT_REPOSITORY)
        set(DOWNLOAD_ARGS
            GIT_REPOSITORY ${LIBRA_EP_GIT_REPOSITORY}
            GIT_TAG        ${LIBRA_EP_GIT_TAG}
        )
    elseif(LIBRA_EP_URL)
        set(DOWNLOAD_ARGS
            URL ${LIBRA_EP_URL}
        )
    endif()

    ExternalProject_Add(
        ${PROJECT_NAME}
        ${DOWNLOAD_ARGS}
        SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/${PROJECT_NAME}
        BINARY_DIR     ${BUILD_DIR}/${PROJECT_NAME}
        ${LIBRA_EP_SOURCE_SUBDIR}
        DEPENDS        ${LIBRA_EP_DEPENDS}
        CMAKE_ARGS     ${LIBRA_EP_CMAKE_ARGS}
        CONFIGURE_COMMAND ${LIBRA_EP_CONFIGURE_COMMAND}
        BUILD_COMMAND     ${LIBRA_EP_BUILD_COMMAND}
        INSTALL_COMMAND   ${LIBRA_EP_INSTALL_COMMAND}
    )
endmacro()

# Function to create data directory
function(libra_create_data_directory)
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/install/data)
endfunction()