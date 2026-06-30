# syntax=docker/dockerfile:1
#
# LibRA profiling image: NVTX-instrumented build for use with Nsight Systems.
# Enables -DLIBRA_ENABLE_NVTX=ON (NVTX ranges in HPG + librautils ProfilingTools).
# Build context must be scripts/container_recipes/ (uses readline.pc, start.sh).
#
# Example:
#   cd scripts/container_recipes
#   docker build -f libra-profiling.dockerfile \
#     --build-arg Kokkos_CUDA_ARCH=Kokkos_ARCH_HOPPER90 \
#     --build-arg LIBRA_BRANCH=profiling \
#     -t pjaganna/libra:hopper90-profiling .
#   docker push pjaganna/libra:hopper90-profiling

FROM nvcr.io/nvidia/cuda:13.2.0-devel-rockylinux8 as base
WORKDIR "/data"
ADD readline.pc /data/
LABEL Author pjaganna@nrao.edu
LABEL Version v0.0.1-profiling

# Build stage
FROM base as build
ARG Kokkos_CUDA_ARCH=Kokkos_ARCH_HOPPER90
ARG LIBRA_BRANCH=profiling
ARG nproc=8
ENV PATH=/data/libra/apps/src/:/data/libra/dependencies/linux_64b/bin/:/data/libra/dependencies/linux_64b/sbin/:/usr/local/cuda/bin:$PATH
ENV LD_LIBRARY_PATH=/data/libra/apps/install:/data/libra/dependencies/linux_64b/lib/:/usr/local/cuda/compat:$LD_LIBRARY_PATH
ENV LIBRA_PATH=/data/libra
RUN dnf -y clean all \
    && dnf -y install epel-release \
    && dnf install -y dnf-plugins-core \
    && dnf config-manager --set-enabled powertools \
    && dnf -y install git cmake ccache flex bison tar curl bzip2 make \
    && dnf -y install gcc-toolset-13 gcc-toolset-13-gcc-gfortran \
    && dnf -y install {gtest,readline,ncurses,blas,lapack,cfitsio,fftw,wcslib,gsl,eigen3,openmpi,python38}-devel \
    && mv /data/readline.pc /usr/lib64/pkgconfig/ \
    && cd /data/ \
    && git clone -b ${LIBRA_BRANCH} https://github.com/ARDG-NRAO/LibRA.git libra \
    && cd libra \
    # Fetch the gold_standard test-data submodule over public https (the
    # upstream .gitmodules may still point at the SSH URL, which needs a key).
    && git config -f .gitmodules submodule.apps/src/tests/gold_standard.url https://gitlab.nrao.edu/ardg/libra-data.git \
    && git submodule sync apps/src/tests/gold_standard \
    && git submodule update --init apps/src/tests/gold_standard
RUN . /opt/rh/gcc-toolset-13/enable \
    && cd libra \
    && mkdir -p build && cd build \
    && cmake -DLIBRA_ENABLE_NVTX=ON \
             -DKokkos_CUDA_ARCH_NAME=${Kokkos_CUDA_ARCH} \
             -DBUILD_TESTING=ON .. \
    && make

COPY start.sh /data/
RUN echo "nvcc --version > /tmp/nvcc_version.txt" >> /tests.sh \
    && echo "echo "nvcc version is $(cat /tmp/nvcc_version.txt)"" >> /tests.sh \
    && echo "nvidia-smi > /tmp/nvidia-smi.txt" >> /tests.sh \
    && echo "echo "nvidia-smi is $(cat /tmp/nvidia-smi.txt)"" >> /tests.sh \
    && echo "exit 0" >> /tests.sh \
    && chmod u+x /tests.sh
ENTRYPOINT ["/bin/bash","/data/start.sh"]
