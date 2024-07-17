set (CMAKE_CXX_STANDARD 17)
set(OPT "-g -O2 ")

find_package(Kokkos REQUIRED)
find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)

find_library(LIBRA_LIBRARY NAMES libra PATHS ${CMAKE_INSTALL_PREFIX}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX}/include)

set(EXAMPLE_LIST dataiter)
#set(HEADER_LIST dataiter.h)
set(BUILD_EXAMPLE ON)

message("${CMAKE_INSTALL_PREFIX}/bin")
foreach(EXAMPLE IN LISTS EXAMPLE_LIST)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
   if(BUILD_EXAMPLE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE}.cc")
     add_executable(${EXAMPLE} "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE}.cc")
     target_link_libraries(${EXAMPLE} ${APP_LINK_LIBRARIES} libra)
     install(TARGETS ${EXAMPLE} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
   endif()
endforeach()

# foreach(HEADER IN LISTS HEADER_LIST)
#    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}")
#      install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/")
#    endif()
