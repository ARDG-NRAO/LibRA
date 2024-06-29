set (CMAKE_CXX_STANDARD 17)
set(OPT "-g -O2 ")

find_package(Kokkos REQUIRED)
find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)
#find_package(GTest REQUIRED)

#add_library(libra SHARED ${APP_SOURCES})
#target_include_directories(libra PUBLIC ${APP_INCLUDE_DIRS})
target_link_libraries(libra PRIVATE ${APP_LINK_LIBRARIES} "-Wl,--disable-new-dtags")
install(TARGETS libra LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

set(EXAMPLE_LIST dataiter)
set(BUILD_EXAMPLE ON)

foreach(EXAMPLE IN LISTS EXAMPLE_LIST)
   include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_DIR}/)
   string(TOLOWER ${EXAMPLE} EXAMPLE_NAME)


   if(BUILD_EXAMPLE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}.cc")
     add_executable(${EXAMPLE_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_NAME}.cc")
     target_link_libraries(${EXAMPLE_NAME} ${APP_LINK_LIBRARIES} libra)
     install(TARGETS ${EXAMPLE_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
   endif()
endforeach()
