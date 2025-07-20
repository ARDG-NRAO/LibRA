#export PATH=$PATH:/usr/local/cuda/bin
export PKG_CONFIG_PATH=../install/lib/pkgconfig
cmake -DKokkos_CUDA_ARCH_NAME=Kokkos_ARCH_$1 -DApps_BUILD_TESTS=ON $2 ..
gmake
