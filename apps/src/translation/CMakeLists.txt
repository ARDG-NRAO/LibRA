cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
#project(LibRATests  VERSION 1.0
#                    DESCRIPTION "LibRA tests"
#                    LANGUAGES CXX)


include_directories(${CMAKE_CURRENT_LIST_DIR}/..)
link_directories("${INSTALLDIR}/lib")


add_library(casacore_asp SHARED casacore_asp.cc)


# Link with Google Test
#target_link_libraries(casacore_asp stdc++fs ${APP_LINK_LIBRARIES} libra roadrunner_cl_interface coyote_cl_interface hummbee_cl_interface mssplit_cl_interface subms_cl_interface tableinfo_cl_interface)
target_link_libraries(casacore_asp PRIVATE ${APP_LINK_LIBRARIES} libra "-Wl,--disable-new-dtags")
install(TARGETS casacore_asp
        LIBRARY DESTINATION "${INSTALLDIR}/lib")
