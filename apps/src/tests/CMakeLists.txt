cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(LibRATests   VERSION 1.0
                    DESCRIPTION "LibRA tests"
                    LANGUAGES CXX)


include_directories(${CMAKE_CURRENT_LIST_DIR}/..)
link_directories("${INSTALLDIR}/lib")

# Find the Google Test library
FetchContent_Declare(
  gtest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)

# Configure Google Test
FetchContent_MakeAvailable(gtest)

#add_executable(LibRATests test_hummbee.cc test_roadrunner.cc test_coyote.cc test_tableinfo.cc test_subms.cc test_mssplit.cc test_asp.cc)
add_executable(LibRATests test_hummbee.cc test_roadrunner.cc test_coyote.cc test_tableinfo.cc test_subms.cc test_mssplit.cc)

#message("PROJECT_BINARY_DIR is ${PROJECT_BINARY_DIR}")
#message("PROJECT_SOURCE_DIR is ${PROJECT_SOURCE_DIR}")

# Link with Google Test
target_link_libraries(LibRATests GTest::gtest_main stdc++fs ${APP_LINK_LIBRARIES} libra roadrunner_cl_interface coyote_cl_interface hummbee_cl_interface mssplit_cl_interface subms_cl_interface tableinfo_cl_interface)

# Add the test executable as a test target
add_test(NAME LibRATests COMMAND LibRATests)
