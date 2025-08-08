# Casacore external project
include(ExternalProject)

# Create the path ${CMAKE_SOURCE_DIR}/install/data and download the data into it
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/install/data)
if(CASACORE_DATA_DOWNLOAD)
  ExternalProject_Add(
    DataDownload
    DOWNLOAD_COMMAND bash -c "curl ${LIBRA_MEASURES_FTP} | tar -C ${CMAKE_SOURCE_DIR}/install/data -xzf - || curl ${LIBRA_MEASURES_HTTPS} | tar -C ${CMAKE_SOURCE_DIR}/install/data -xzf -"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
else()
  ExternalProject_Add(
    DataDownload
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
endif()

ExternalProject_Add(
  Casacore
  GIT_REPOSITORY ${LIBRA_CASACORE_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_CASACORE_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/casacore
  BINARY_DIR     ${BUILD_DIR}/casacore
  CONFIGURE_COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/dependencies/casacore
                    -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                    -DCMAKE_PREFIX_PATH=${INSTALL_DIR}
                    -DCMAKE_INSTALL_LIBDIR=lib
                    -DCMAKE_INSTALL_BINDIR=bin/casacore
                    -DDATA_DIR=${CMAKE_SOURCE_DIR}/install/data
                    -DUSE_OPENMP=ON
                    -DUSE_THREADS=ON
                    -DCMAKE_CXX_FLAGS="-fno-tree-vrp"
                    -DBUILD_FFTPACK_DEPRECATED=ON
                    -DBUILD_TESTING=OFF
                    -DBUILD_PYTHON=OFF
                    -DBUILD_PYTHON3=OFF
                    -DBUILD_DYSCO=ON
                    -DUseCcache=0 
                    -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
                    -DFFTW_LIBRARIES=${INSTALL_DIR}/lib/ 
                    -DFFTW_ROOT=${INSTALL_DIR}
                    -DFFTW3_INCLUDE_DIR=${INSTALL_DIR}/include
                    ${CCACHE_LAUNCHER}
                    ${SPACK_FLAGS}
  BUILD_COMMAND  make -j${NCORES}
  INSTALL_COMMAND make install
  DEPENDS FFTW_float DataDownload
)
