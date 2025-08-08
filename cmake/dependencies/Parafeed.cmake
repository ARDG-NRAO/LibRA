# Parafeed external project
include(ExternalProject)

ExternalProject_Add(
  Parafeed
  GIT_REPOSITORY ${LIBRA_PARAFEED_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_PARAFEED_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/parafeed
  BINARY_DIR     ${BUILD_DIR}/parafeed
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} 
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/parafeed
                 ${CCACHE_LAUNCHER}
                 -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
                 -DBUILD_SHARED_LIBS=ON
                 -DGNUREADLINE=ON
                 ${SPACK_FLAGS}
  BUILD_COMMAND make -j ${NCORES}
  INSTALL_COMMAND make install
)
