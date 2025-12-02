cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(LibRATests VERSION 1.0
          DESCRIPTION "LibRA tests"
          LANGUAGES CXX)

link_directories("${CMAKE_INSTALL_PREFIX}/lib")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../RoadRunner
  ${CMAKE_CURRENT_SOURCE_DIR}/../Coyote
  ${CMAKE_CURRENT_SOURCE_DIR}/../Hummbee
  ${CMAKE_CURRENT_SOURCE_DIR}/../MSSplit
  ${CMAKE_CURRENT_SOURCE_DIR}/../SubMS
  ${CMAKE_CURRENT_SOURCE_DIR}/../TableInfo
  ${CMAKE_CURRENT_SOURCE_DIR}/../Chip
  ${CMAKE_CURRENT_SOURCE_DIR}/../Dale
  ${CMAKE_CURRENT_SOURCE_DIR}/../Acme
  ${CMAKE_CURRENT_SOURCE_DIR}/../Asp
)

enable_testing()

find_package(GTest REQUIRED)

set(TEST_SOURCES
  test_hummbee.cc
  test_roadrunner.cc
  test_coyote.cc
  test_tableinfo.cc
  test_subms.cc
  test_mssplit.cc
  test_asp.cc
  test_chip.cc
  test_dale.cc
  test_restore.cc
  test_htclean.cc
  test_libracoretypes.cc
)

add_executable(LibRATests ${TEST_SOURCES})

set(APP_LIBRARIES
  roadrunner_lib
  coyote_lib
  hummbee_lib
  mssplit_lib
  subms_lib
  tableinfo_lib
  chip_lib
  dale_lib
  casacore_asp
)

set(APP_CLI_INTERFACE_LIBRARIES
  roadrunner_cl_interface
  coyote_cl_interface
  hummbee_cl_interface
  mssplit_cl_interface
  subms_cl_interface
  tableinfo_cl_interface
  chip_cl_interface
  dale_cl_interface
)

target_link_libraries(LibRATests GTest::gtest_main stdc++fs ${APP_LIBRARIES} ${APP_CLI_INTERFACE_LIBRARIES})

add_test(NAME LibRATests COMMAND LibRATests)

if(Apps_BUILD_TESTS_PYTHON)
  find_program(PYTHON_EXECUTABLE python)
  add_test(
      NAME LibRAPythonTests
      COMMAND ${PYTHON_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
  )
endif()

install(TARGETS LibRATests
  RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/tests"
)
