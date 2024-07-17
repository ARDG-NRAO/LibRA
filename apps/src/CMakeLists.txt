set (CMAKE_CXX_STANDARD 17)
set(OPT "-g -O2 ")



find_package(Kokkos REQUIRED)
find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)
#find_package(GTest REQUIRED)

#include_directories(
#   ${CMAKE_SOURCE_DIR}
#	${CMAKE_SOURCE_DIR}/../dependencies/Kokkos/tpls/mdspan/include/mdspan/)

#add_dependencies(libra ${APP_DEPENDENCIES})
add_library(libra SHARED ${APP_SOURCES})
#target_include_directories(libra PUBLIC ${APP_INCLUDE_DIRS})
target_link_libraries(libra PRIVATE ${APP_LINK_LIBRARIES} "-Wl,--disable-new-dtags")
install(TARGETS libra LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")


foreach(APP_DIR IN LISTS APP_LIST)
   include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/)
   string(TOLOWER ${APP_DIR} APP_NAME)

   file(GLOB APP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/*.h")
   install(FILES ${APP_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/")


   # build app CL interface binary
   set(BUILD_CLWRAPPER ON)
   if(BUILD_CLWRAPPER AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_cl_interface.cc")
     add_executable(${APP_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_cl_interface.cc")
     target_link_libraries(${APP_NAME} ${APP_LINK_LIBRARIES} libra)
     install(TARGETS ${APP_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
   endif()

   # build individual app CL interface lib for unit tests
   if(Apps_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_cl_interface.cc")
    add_library(${APP_NAME}_cl_interface SHARED "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_cl_interface.cc")
    target_link_libraries(${APP_NAME}_cl_interface PRIVATE ${APP_LINK_LIBRARIES} libra "-Wl,--disable-new-dtags")
    install(TARGETS ${APP_NAME}_cl_interface
    	LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
   endif()

   # build app python wrapper
   set(BUILD_PYWRAPPER ON)
   if(BUILD_PYWRAPPER AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_py_interface.cc")
     pybind11_add_module(${APP_NAME}2py "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_py_interface.cc")
     target_link_libraries(${APP_NAME}2py PRIVATE ${APP_LINK_LIBRARIES} libra)
     install(TARGETS ${APP_NAME}2py
     	LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
   endif()
endforeach()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
