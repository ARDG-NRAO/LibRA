project("LibRA Scripts"
  DESCRIPTION "LibRA CL, C/C++, Py Interfaces"
  HOMEPAGE_URL https://github.com/ARDG-NRAO/LibRA
  LANGUAGES CXX Fortran C)

cmake_minimum_required(VERSION 3.12.0)

# Check if the TOP_LEVEL_DIR and INSTALL_DIR are defined
if(DEFINED TOP_LEVEL_DIR AND DEFINED INSTALL_DIR)
    message(STATUS "TOP_LEVEL_DIR: ${TOP_LEVEL_DIR}")
    message(STATUS "INSTALL_DIR: ${INSTALL_DIR}")
else()
    message(WARNING "TOP_LEVEL_DIR or INSTALL_DIR is not defined, skipping script installation")
endif()


if(DEFINED TOP_LEVEL_DIR AND DEFINED INSTALL_DIR)
  install(FILES 
    ${TOP_LEVEL_DIR}/scripts/runapp.sh 
    ${TOP_LEVEL_DIR}/scripts/emajor.sh 
    DESTINATION ${INSTALL_DIR}/bin)
  message(STATUS "Scripts installed to ${INSTALL_DIR}/bin")
else()
  message(WARNING "TOP_LEVEL_DIR or INSTALL_DIR is not defined, skipping script installation")
endif()
