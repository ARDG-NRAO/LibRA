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
# License for more details.
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
# Container building module - Docker and Singularity support

include(ExternalProject)

if(LIBRA_BUILD_DOCKER_CONTAINER OR LIBRA_BUILD_SINGULARITY_CONTAINER)
    message("${LIBRA_ID} ========================================")
    message("${LIBRA_ID} Container Building: ENABLED")
    message(WARNING "${LIBRA_ID} Container builds are enabled - build will take significantly longer!")
    message("${LIBRA_ID} This is normal and expected behavior.")
    message("${LIBRA_ID} ========================================")

    # Validate OS choice
    if(NOT LIBRA_CONTAINER_OS MATCHES "^(rockylinux8|ubuntu24\\.04)$")
        message(WARNING "${LIBRA_ID} Invalid LIBRA_CONTAINER_OS: '${LIBRA_CONTAINER_OS}'")
        message(WARNING "${LIBRA_ID} Must be 'rockylinux8' or 'ubuntu24.04'")
        message(WARNING "${LIBRA_ID} Container build will be SKIPPED")
        return()
    endif()

    # Detect container runtime
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindContainerRuntime.cmake)

    # Check Docker availability
    set(BUILD_DOCKER FALSE)
    if(LIBRA_BUILD_DOCKER_CONTAINER)
        if(NOT DOCKER_FOUND)
            message(WARNING "${LIBRA_ID} LIBRA_BUILD_DOCKER_CONTAINER is ON but Docker not found or not working")
            message(WARNING "${LIBRA_ID} Docker container build will be SKIPPED")
        else()
            set(BUILD_DOCKER TRUE)
        endif()
    endif()

    # Check Singularity availability
    set(BUILD_SINGULARITY FALSE)
    if(LIBRA_BUILD_SINGULARITY_CONTAINER)
        if(NOT SINGULARITY_FOUND)
            message(WARNING "${LIBRA_ID} LIBRA_BUILD_SINGULARITY_CONTAINER is ON but Singularity/Apptainer not found or not working")
            message(WARNING "${LIBRA_ID} Singularity container build will be SKIPPED")
        else()
            set(BUILD_SINGULARITY TRUE)
        endif()
    endif()

    # If nothing to build, return early
    if(NOT BUILD_DOCKER AND NOT BUILD_SINGULARITY)
        message("${LIBRA_ID} No container runtimes available - skipping container builds")
        return()
    endif()

    # Determine CUDA architecture
    set(CONTAINER_CUDA_ARCH ${Kokkos_CUDA_ARCH_NAME})
    if(NOT LIBRA_CONTAINER_CUDA_ARCH STREQUAL "")
        message(WARNING "${LIBRA_ID} Using override CUDA arch for container: ${LIBRA_CONTAINER_CUDA_ARCH}")
        message(WARNING "${LIBRA_ID} (Main build uses: ${Kokkos_CUDA_ARCH_NAME})")
        set(CONTAINER_CUDA_ARCH ${LIBRA_CONTAINER_CUDA_ARCH})
    endif()

    # Fallback if no CUDA arch specified
    if(CONTAINER_CUDA_ARCH STREQUAL "")
        set(CONTAINER_CUDA_ARCH "VOLTA70")
        message("${LIBRA_ID} No CUDA arch specified, using default: ${CONTAINER_CUDA_ARCH}")
    endif()

    # Normalize: Strip 'Kokkos_ARCH_' prefix if present
    # This ensures consistency whether user passes VOLTA70 or Kokkos_ARCH_VOLTA70
    string(REGEX REPLACE "^Kokkos_ARCH_" "" CONTAINER_CUDA_ARCH "${CONTAINER_CUDA_ARCH}")
    message("${LIBRA_ID} Normalized CUDA arch for container: ${CONTAINER_CUDA_ARCH}")

    # Map OS names to Docker base image tags and descriptions
    if(LIBRA_CONTAINER_OS STREQUAL "rockylinux8")
        set(CONTAINER_OS_BASE "rockylinux8")
        set(CONTAINER_OS_VARIANT "rockylinux")
        set(CONTAINER_OS_DESCRIPTION "RockyLinux 8 with CUDA 12.9, GCC Toolset 13")
    elseif(LIBRA_CONTAINER_OS STREQUAL "ubuntu24.04")
        set(CONTAINER_OS_BASE "ubuntu24.04")
        set(CONTAINER_OS_VARIANT "ubuntu")
        set(CONTAINER_OS_DESCRIPTION "Ubuntu 24.04 with CUDA 12.9, GCC 13")
    endif()

    # Get git commit SHA and build date
    execute_process(
        COMMAND git rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_SHA
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(GIT_COMMIT_SHA STREQUAL "")
        set(GIT_COMMIT_SHA "unknown")
    endif()

    string(TIMESTAMP BUILD_DATE UTC)

    message("${LIBRA_ID} Container Configuration:")
    message("${LIBRA_ID}   OS: ${LIBRA_CONTAINER_OS} (${CONTAINER_OS_DESCRIPTION})")
    message("${LIBRA_ID}   CUDA Arch: ${CONTAINER_CUDA_ARCH}")
    message("${LIBRA_ID}   Git SHA: ${GIT_COMMIT_SHA}")
    set(NPROC ${NCORES})
    message("${LIBRA_ID}   Build Cores: ${NPROC}")

    # Prepare OS-specific content for templates
    if(CONTAINER_OS_VARIANT STREQUAL "rockylinux")
        # RockyLinux-specific Dockerfile content
        set(ROCKYLINUX_SETUP "# Install EPEL and enable PowerTools
RUN dnf -y clean all \\
    && dnf -y install epel-release \\
    && dnf install -y dnf-plugins-core \\
    && dnf config-manager --set-enabled powertools \\
    && dnf -y clean all

# Install GCC Toolset 13 (modern compiler)
RUN dnf -y install gcc-toolset-13-gcc-c++ gcc-toolset-13-gcc-gfortran \\
    && dnf -y clean all

# Enable GCC Toolset 13 by default
RUN echo 'source /opt/rh/gcc-toolset-13/enable' >> /etc/bashrc \\
    && echo 'source /opt/rh/gcc-toolset-13/enable' >> ~/.bashrc

# Install Python 3.12 development libraries
RUN dnf -y install python3.12 python3.12-devel python3.12-pip \\
    && dnf -y clean all

# Set Python 3.12 as default
RUN alternatives --set python3 /usr/bin/python3.12 || update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.12 1

# Install build tools
RUN dnf -y install git cmake ccache flex bison tar curl bzip2 make wget which \\
    && dnf -y clean all

# Install system development libraries
RUN dnf -y install \\
    gtest-devel \\
    readline-devel \\
    ncurses-devel \\
    blas-devel \\
    lapack-devel \\
    cfitsio-devel \\
    fftw-devel \\
    wcslib-devel \\
    gsl-devel \\
    eigen3-devel \\
    openmpi-devel \\
    && dnf -y clean all")

        set(UBUNTU_SETUP "")

        set(ROCKYLINUX_ENV "# Enable GCC Toolset 13 for all shells
ENV BASH_ENV=/opt/rh/gcc-toolset-13/enable
ENV ENV=/opt/rh/gcc-toolset-13/enable")

        set(SOURCE_GCC_TOOLSET "source /opt/rh/gcc-toolset-13/enable &&")

        # Singularity-specific RockyLinux content
        set(ROCKYLINUX_POST "    # Install EPEL and enable PowerTools
    dnf -y clean all
    dnf -y install epel-release
    dnf install -y dnf-plugins-core
    dnf config-manager --set-enabled powertools
    dnf -y clean all

    # Install GCC Toolset 13
    dnf -y install gcc-toolset-13-gcc-c++ gcc-toolset-13-gcc-gfortran
    dnf -y clean all

    # Install Python 3.12
    dnf -y install python3.12 python3.12-devel python3.12-pip
    dnf -y clean all
    alternatives --set python3 /usr/bin/python3.12 || update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.12 1

    # Install build tools
    dnf -y install git cmake ccache flex bison tar curl bzip2 make wget which
    dnf -y clean all

    # Install system libraries
    dnf -y install \\
        gtest-devel \\
        readline-devel \\
        ncurses-devel \\
        blas-devel \\
        lapack-devel \\
        cfitsio-devel \\
        fftw-devel \\
        wcslib-devel \\
        gsl-devel \\
        eigen3-devel \\
        openmpi-devel
    dnf -y clean all")

        set(UBUNTU_POST "")
        set(ROCKYLINUX_BUILD_PREFIX "    source /opt/rh/gcc-toolset-13/enable\n")
        set(ROCKYLINUX_ENVIRONMENT "    export BASH_ENV=/opt/rh/gcc-toolset-13/enable
    export ENV=/opt/rh/gcc-toolset-13/enable")

    else()  # Ubuntu
        set(ROCKYLINUX_SETUP "")

        set(UBUNTU_SETUP "# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# Update and install GCC 13
RUN apt-get update && apt-get upgrade -y \\
    && apt-get install -y gcc-13 g++-13 gfortran-13 \\
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 \\
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100 \\
    && update-alternatives --install /usr/bin/gfortran gfortran /usr/bin/gfortran-13 100 \\
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Install Python 3.12
RUN apt-get update && apt-get install -y \\
    python3.12 python3.12-dev python3.12-venv python3-pip \\
    && update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.12 1 \\
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Install build tools
RUN apt-get update && apt-get install -y \\
    git cmake ccache flex bison tar curl bzip2 make wget pkg-config \\
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Install system libraries
RUN apt-get update && apt-get install -y \\
    libgtest-dev libreadline-dev libncurses-dev \\
    libblas-dev liblapack-dev libcfitsio-dev \\
    libfftw3-dev libwcslib-dev libgsl-dev \\
    libeigen3-dev libopenmpi-dev openmpi-bin \\
    && apt-get clean && rm -rf /var/lib/apt/lists/*")

        set(ROCKYLINUX_ENV "")
        set(SOURCE_GCC_TOOLSET "")

        # Singularity Ubuntu content
        set(ROCKYLINUX_POST "")
        set(UBUNTU_POST "    export DEBIAN_FRONTEND=noninteractive
    export TZ=UTC

    # Update and install GCC 13
    apt-get update && apt-get upgrade -y
    apt-get install -y gcc-13 g++-13 gfortran-13
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100
    update-alternatives --install /usr/bin/gfortran gfortran /usr/bin/gfortran-13 100
    apt-get clean && rm -rf /var/lib/apt/lists/*

    # Install Python 3.12
    apt-get update && apt-get install -y python3.12 python3.12-dev python3.12-venv python3-pip
    update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.12 1
    apt-get clean && rm -rf /var/lib/apt/lists/*

    # Install build tools
    apt-get update && apt-get install -y git cmake ccache flex bison tar curl bzip2 make wget pkg-config
    apt-get clean && rm -rf /var/lib/apt/lists/*

    # Install system libraries
    apt-get update && apt-get install -y \\
        libgtest-dev libreadline-dev libncurses-dev \\
        libblas-dev liblapack-dev libcfitsio-dev \\
        libfftw3-dev libwcslib-dev libgsl-dev \\
        libeigen3-dev libopenmpi-dev openmpi-bin
    apt-get clean && rm -rf /var/lib/apt/lists/*")

        set(ROCKYLINUX_BUILD_PREFIX "")
        set(ROCKYLINUX_ENVIRONMENT "")
    endif()

    # Configure Dockerfile from template
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/Dockerfile.libra.in
        ${CMAKE_CURRENT_BINARY_DIR}/Dockerfile.libra
        @ONLY
    )

    # Configure Singularity .def from template
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/libra.def.in
        ${CMAKE_CURRENT_BINARY_DIR}/libra-${LIBRA_CONTAINER_OS}.def
        @ONLY
    )

    message("${LIBRA_ID} Container build files configured in: ${CMAKE_CURRENT_BINARY_DIR}")

    # Docker container build using ExternalProject
    if(BUILD_DOCKER)
        ExternalProject_Add(
            DockerContainer
            DOWNLOAD_COMMAND ""
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
            BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ${DOCKER_EXECUTABLE} build
                          -f ${CMAKE_CURRENT_BINARY_DIR}/Dockerfile.libra
                          -t libra:${LIBRA_CONTAINER_OS}-local
                          ${CMAKE_CURRENT_SOURCE_DIR}
            INSTALL_COMMAND ${DOCKER_EXECUTABLE} run --rm libra:${LIBRA_CONTAINER_OS}-local libra-info
            DEPENDS Apps
        )
        message("${LIBRA_ID} Docker container target 'DockerContainer' added")
        message("${LIBRA_ID}   Image tag: libra:${LIBRA_CONTAINER_OS}-local")
    endif()

    # Singularity container build using ExternalProject
    if(BUILD_SINGULARITY)
        ExternalProject_Add(
            SingularityContainer
            DOWNLOAD_COMMAND ""
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
            BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ${SINGULARITY_EXECUTABLE} build
                          --fakeroot
                          --force
                          libra-${LIBRA_CONTAINER_OS}.sif
                          ${CMAKE_CURRENT_BINARY_DIR}/libra-${LIBRA_CONTAINER_OS}.def
            INSTALL_COMMAND ${SINGULARITY_EXECUTABLE} exec
                            ${CMAKE_CURRENT_BINARY_DIR}/libra-${LIBRA_CONTAINER_OS}.sif
                            echo "LibRA Singularity Container - Build Verified!"
            DEPENDS Apps
        )
        message("${LIBRA_ID} Singularity container target 'SingularityContainer' added")
        message("${LIBRA_ID}   SIF file: ${CMAKE_CURRENT_BINARY_DIR}/libra-${LIBRA_CONTAINER_OS}.sif")
    endif()

else()
    message("${LIBRA_ID} Container building: DISABLED")
endif()
