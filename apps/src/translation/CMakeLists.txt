cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)


#include_directories(${CMAKE_CURRENT_LIST_DIR}/..
#	${CMAKE_SOURCE_DIR}/../../../dependencies/Kokkos/tpls/mdspan/include/mdspan/)
link_directories("${CMAKE_INSTALL_PREFIX}/lib")

## casacore_asp
add_library(casacore_asp SHARED casacore_asp.cc casacore_asp_cube.cc
	casacore_asp_mdspan.cc)
target_link_libraries(casacore_asp PRIVATE ${APP_LINK_LIBRARIES} libra libracore "-Wl,--disable-new-dtags")
install(TARGETS casacore_asp
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")


## casacore_restore
add_library(casacore_restore SHARED casacore_restore.cc)
target_link_libraries(casacore_restore PRIVATE ${APP_LINK_LIBRARIES} libra libracore "-Wl,--disable-new-dtags")
install(TARGETS casacore_restore
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
