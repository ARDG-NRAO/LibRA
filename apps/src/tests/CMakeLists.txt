cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(LibRATests   VERSION 1.0
                    DESCRIPTION "LibRA tests"
                    LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_LIST_DIR}/..)
link_directories(${CMAKE_SOURCE_DIR}/install)
#message("link_dir is ${CMAKE_SOURCE_DIR}/install")

# Find the Google Test library
FetchContent_Declare(
  gtest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)

# Configure Google Test
FetchContent_MakeAvailable(gtest)

add_executable(LibRATests test_hummbee.cc test_roadrunner.cc)

#message("PROJECT_BINARY_DIR is ${PROJECT_BINARY_DIR}")
#message("PROJECT_SOURCE_DIR is ${PROJECT_SOURCE_DIR}")

# Link with Google Test
target_link_libraries(LibRATests GTest::gtest_main stdc++fs hummbeelib roadrunnerlib)
# Add the test executable

# Add the test executable as a test target
add_test(NAME LibRATests COMMAND LibRATests)
