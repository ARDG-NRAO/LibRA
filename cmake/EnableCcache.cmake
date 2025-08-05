# EnableCcache.cmake - Comprehensive ccache integration for LibRA
# Handles regular C++, nvcc_wrapper, and Kokkos compilation flows

# Find ccache
find_program(CCACHE_PROGRAM ccache)
if(NOT CCACHE_PROGRAM)
    message(STATUS "ccache not found - compilation will proceed without ccaching")
    return()
endif()

message(STATUS "Found ccache: ${CCACHE_PROGRAM}")

# Get ccache version and stats
execute_process(
    COMMAND ${CCACHE_PROGRAM} --version
    OUTPUT_VARIABLE CCACHE_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "ccache version: ${CCACHE_VERSION}")

# Show ccache configuration
execute_process(
    COMMAND ${CCACHE_PROGRAM} --show-stats
    OUTPUT_VARIABLE CCACHE_STATS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "ccache stats: ${CCACHE_STATS}")

# Set global ccache configuration
set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE STRING "Use ccache for C compilation")
set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE STRING "Use ccache for C++ compilation")

# For Kokkos nvcc_wrapper integration
# We need to create a ccache-wrapped nvcc_wrapper script
function(setup_ccache_nvcc_wrapper INSTALL_DIR)
    set(ORIGINAL_NVCC_WRAPPER "${INSTALL_DIR}/bin/kokkos/nvcc_wrapper")
    set(CCACHE_NVCC_WRAPPER "${INSTALL_DIR}/bin/kokkos/ccache_nvcc_wrapper")
    
    # Create ccache wrapper script for nvcc_wrapper
    file(WRITE "${CCACHE_NVCC_WRAPPER}" "#!/bin/bash\n")
    file(APPEND "${CCACHE_NVCC_WRAPPER}" "# ccache wrapper for Kokkos nvcc_wrapper\n")
    file(APPEND "${CCACHE_NVCC_WRAPPER}" "export CCACHE_LOGFILE=\"${INSTALL_DIR}/ccache_nvcc.log\"\n")
    file(APPEND "${CCACHE_NVCC_WRAPPER}" "export CCACHE_DEBUG=1\n")
    file(APPEND "${CCACHE_NVCC_WRAPPER}" "exec ${CCACHE_PROGRAM} \"${ORIGINAL_NVCC_WRAPPER}\" \"$@\"\n")
    
    # Make executable
    file(CHMOD "${CCACHE_NVCC_WRAPPER}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    
    message(STATUS "Created ccache nvcc_wrapper: ${CCACHE_NVCC_WRAPPER}")
endfunction()

# Environment variables for ccache optimization
set(ENV{CCACHE_COMPILERCHECK} "content")
set(ENV{CCACHE_SLOPPINESS} "file_macro,time_macros,include_file_mtime")
set(ENV{CCACHE_MAXSIZE} "2G")

# Verbose logging for debugging
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ENV{CCACHE_LOGFILE} "${CMAKE_BINARY_DIR}/ccache.log")
endif()

message(STATUS "ccache integration enabled")
message(STATUS "  C compiler launcher: ${CMAKE_C_COMPILER_LAUNCHER}")
message(STATUS "  C++ compiler launcher: ${CMAKE_CXX_COMPILER_LAUNCHER}")
message(STATUS "  ccache max size: $ENV{CCACHE_MAXSIZE}")
message(STATUS "  ccache sloppiness: $ENV{CCACHE_SLOPPINESS}")