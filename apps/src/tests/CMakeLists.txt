cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(LibRATests VERSION 1.0
          DESCRIPTION "LibRA tests"
          LANGUAGES CXX)

link_directories("${CMAKE_INSTALL_PREFIX}/lib")

enable_testing()

find_package(GTest REQUIRED)

# List of test source files
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
  test_dale.cc
)

add_executable(LibRATests ${TEST_SOURCES})

# Link with Google Test
set(INTERFACE_LIBRARIES libra libracore roadrunner_cl_interface coyote_cl_interface hummbee_cl_interface mssplit_cl_interface subms_cl_interface tableinfo_cl_interface chip_cl_interface dale_cl_interface casacore_asp)

target_link_libraries(LibRATests GTest::gtest_main stdc++fs ${APP_LINK_LIBRARIES} ${INTERFACE_LIBRARIES})

# Add the test executable as a test target
add_test(NAME LibRATests COMMAND LibRATests)

if(Apps_BUILD_TESTS_PYTHON)
  find_program(PYTHON_EXECUTABLE python)
  add_test(
      NAME LibRAPythonTests
      COMMAND ${PYTHON_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
  )
endif()
