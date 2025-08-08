# FFTW external project
include(ExternalProject)


# Download and extract FFTW only once
ExternalProject_Add(
  FFTW_source
  URL             ${LIBRA_FFTW_URL}
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
  CONFIGURE_COMMAND ""
  BUILD_COMMAND    ""
  INSTALL_COMMAND  ""
)

# Build double precision (default)
ExternalProject_Add(
  FFTW_double
  DOWNLOAD_COMMAND ""
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
  BINARY_DIR      ${BUILD_DIR}/FFTW_double
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E sleep 2 && <SOURCE_DIR>/configure --enable-shared --enable-threads --enable-openmp --prefix=${INSTALL_DIR} --bindir=${INSTALL_DIR}/bin/fftw --libdir=${INSTALL_DIR}/lib
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
  DEPENDS         FFTW_source
)

# Build single precision (float)
ExternalProject_Add(
  FFTW_float
  DOWNLOAD_COMMAND ""
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
  BINARY_DIR      ${BUILD_DIR}/FFTW_float
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E sleep 2 && <SOURCE_DIR>/configure --enable-shared --enable-threads --enable-openmp --enable-float --prefix=${INSTALL_DIR} --bindir=${INSTALL_DIR}/bin/fftw --libdir=${INSTALL_DIR}/lib
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
  DEPENDS         FFTW_double
)
