cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(LibRATests   VERSION 1.0
                    DESCRIPTION "LibRA tests"
                    LANGUAGES CXX)


link_directories("${CMAKE_INSTALL_PREFIX}/lib")

find_package(GTest REQUIRED)

add_executable(LibRATests test_hummbee.cc test_roadrunner.cc 
  test_coyote.cc test_tableinfo.cc test_subms.cc 
  test_mssplit.cc test_asp.cc test_acme.cc test_restore.cc)


# Link with Google Test
target_link_libraries(LibRATests GTest::gtest_main stdc++fs ${APP_LINK_LIBRARIES} libra libracore roadrunner_cl_interface coyote_cl_interface hummbee_cl_interface mssplit_cl_interface subms_cl_interface tableinfo_cl_interface acme_cl_interface casacore_asp)

# Add the test executable as a test target
add_test(NAME LibRATests COMMAND LibRATests)
