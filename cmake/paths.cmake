#-*- cmake -*-
# Path and directory setup for LibRA

include_directories(
  ${INSTALL_DIR}/include
  ${INSTALL_DIR}/include/casacpp
  /usr/include/cfitsio
  ${CMAKE_SOURCE_DIR}/apps/src
  ${CMAKE_SOURCE_DIR}/src
  ${INSTALL_DIR}/include/pybind11)

link_directories(
  ${INSTALL_DIR}/lib
  /usr/local/lib
  /usr/lib64
  /usr/lib
  /usr/lib/x86_64-linux-gnu)
