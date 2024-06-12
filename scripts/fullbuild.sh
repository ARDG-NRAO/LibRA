export PATH=$PATH:/usr/local/cuda/bin
cmake -DKokkos_CUDA_ARCH_NAME=Kokkos_ARCH_$1 -DApps_BUILD_TESTS=ON ..
gmake
