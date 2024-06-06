# syntax=docker/dockerfile:1
FROM nvcr.io/nvidia/cuda:12.2.0-devel-rockylinux8 as base
WORKDIR "/data"
ADD readline.pc /data/
LABEL Author pjaganna@nrao.edu
LABEL Version v0.0.1

# Build stage
FROM base as build
ARG Kokkos_CUDA_ARCH=Kokkos_ARCH_AMPERE80
ARG nproc=8
ENV PATH=/data/libra/apps/src/:/data/libra/dependencies/linux_64b/bin/:/data/libra/dependencies/linux_64b/sbin/:$PATH
ENV LD_LIBRARY_PATH=/data/libra/apps/install:/data/libra/dependencies/linux_64b/lib/:/usr/local/cuda-12.2/compat:$LD_LIBRARY_PATH
ENV LIBRA_PATH=/data/libra
RUN dnf -y clean all \
    && dnf -y install epel-release \
    && dnf install -y dnf-plugins-core \
    && dnf config-manager --set-enabled powertools \
    && dnf -y install git cmake gcc-c++ gcc-gfortran ccache flex bison tar curl bzip2 make \
    && dnf -y install {gtest,readline,ncurses,blas,lapack,cfitsio,fftw,wcslib,gsl,eigen3,openmpi,python38}-devel \
    && mv /data/readline.pc /usr/lib64/pkgconfig/ \
    && cd /data/ \
    && git clone --recursive https://github.com/ARDG-NRAO/LibRA.git libra
RUN cd libra \
    && mkdir -p build && cd build \
    && cmake -DApps_BUILD_TESTS=ON -DKokkos_CUDA_ARCH_NAME=${Kokkos_CUDA_ARCH} ..\
    && make 

COPY start.sh /data/
RUN echo "nvcc --version > /tmp/nvcc_version.txt" >> /tests.sh \
    && echo "echo "nvcc version is $(cat /tmp/nvcc_version.txt)"" >> /tests.sh \
    && echo "nvidia-smi > /tmp/nvidia-smi.txt" >> /tests.sh \
    && echo "echo "nvidia-smi is $(cat /tmp/nvidia-smi.txt)"" >> /tests.sh \
    && echo "exit 0" >> /tests.sh \
    && chmod u+x /tests.sh
ENTRYPOINT ["/bin/bash","/data/start.sh"]