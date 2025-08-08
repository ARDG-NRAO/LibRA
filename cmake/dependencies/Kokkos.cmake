# Kokkos external project
include(ExternalProject)

if(NOT "${Kokkos_CUDA_ARCH_NAME}" STREQUAL "")
  set(Kokkos_CUDA_ARCH_NAME_OPT "-D${Kokkos_CUDA_ARCH_NAME}=ON")
else()
  set(Kokkos_CUDA_ARCH_NAME_OPT "")
endif()
ExternalProject_Add(
  Kokkos
  GIT_REPOSITORY ${LIBRA_KOKKOS_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_KOKKOS_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/Kokkos
  BINARY_DIR     ${BUILD_DIR}/Kokkos
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/kokkos
                 -DCMAKE_CXX_COMPILER=${CMAKE_SOURCE_DIR}/dependencies/Kokkos/bin/nvcc_wrapper
                 -DKokkos_ENABLE_CUDA_LAMBDA=ON
                 -DKokkos_ENABLE_CUDA=ON
                 ${Kokkos_CUDA_ARCH_NAME_OPT}
                 -DKokkos_ENABLE_OPENMP=ON
                 -DKokkos_ENABLE_SERIAL=ON
                 -DBUILD_SHARED_LIBS=ON
                 ${SPACK_FLAGS}
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
)
