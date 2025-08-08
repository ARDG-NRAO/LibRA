# Pybind11 external project
include(ExternalProject)

ExternalProject_Add(
  Pybind11
  GIT_REPOSITORY ${LIBRA_PYBIND11_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_PYBIND11_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/pybind11
  BINARY_DIR     ${BUILD_DIR}/pybind11
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/pybind11
                 ${CCACHE_LAUNCHER}
                 -DPYBIND11_TEST=OFF
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
)
