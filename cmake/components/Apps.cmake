# Apps component
include(ExternalProject)

ExternalProject_Add(
  Apps
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/apps
  DOWNLOAD_COMMAND ""
  BINARY_DIR      ${BUILD_DIR}/Libra/apps
  DEPENDS         Casacpp Hpg Parafeed Casacore Pybind11 GTest
  CONFIGURE_COMMAND
    PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig
    CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos
    ${CMAKE_COMMAND} <SOURCE_DIR>
      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
      -DCMAKE_INSTALL_LIBDIR=lib
      -DCMAKE_CXX_FLAGS=-I${INSTALL_DIR}/include
      -Dcasacore_DIR=${INSTALL_DIR}/lib/cmake/casacore
      -DCMAKE_PREFIX_PATH=${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos:${INSTALL_DIR}/lib/cmake/GTest
      -DApps_BUILD_TESTS=${Apps_BUILD_TESTS}
      -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
      -DKokkos_COMPILE_LAUNCHER=${INSTALL_DIR}/bin/kokkos/kokkos_launch_compiler
      -DKokkos_NVCC_WRAPPER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper
      ${SPACK_FLAGS}
  BUILD_COMMAND   make -j${NCORES}
  INSTALL_COMMAND make install
)
