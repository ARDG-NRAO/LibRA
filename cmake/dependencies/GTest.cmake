# GTest external project
include(ExternalProject)

ExternalProject_Add(
  GTest
  GIT_REPOSITORY ${LIBRA_GTEST_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_GTEST_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/gtest
  BINARY_DIR     ${BUILD_DIR}/gtest
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/gtest
                 ${CCACHE_LAUNCHER}
                 -DBUILD_GMOCK=OFF
                 -DBUILD_GTEST=ON
                 -DBUILD_SHARED_LIBS=ON
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
)
