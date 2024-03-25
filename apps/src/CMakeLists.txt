set (CMAKE_CXX_STANDARD 11)
set(OPT "-g -O2 ")



find_package(Kokkos REQUIRED)
find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)
find_package(GTest REQUIRED)


#add_dependencies(libra ${APP_DEPENDENCIES})
add_library(libra SHARED ${APP_SOURCES})
target_include_directories(libra PUBLIC ${APP_INCLUDE_DIRS})
target_link_libraries(libra PRIVATE ${APP_LINK_LIBRARIES} "-Wl,--disable-new-dtags")
install(TARGETS libra LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")


foreach(APP_DIR IN LISTS APP_LIST)
   include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/)
   string(TOLOWER ${APP_DIR} APP_NAME)


   # build app CL interface binary
   set(BUILD_CLWRAPPER ON)
   if(BUILD_CLWRAPPER)
     message("building CL binary ${APP_NAME}")
     add_executable(${APP_NAME} "${APP_DIR}/${APP_NAME}_cl_interface.cc")
     target_link_libraries(${APP_NAME} ${APP_LINK_LIBRARIES} libra)
     install(TARGETS ${APP_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
   endif()

   # build individual app CL interface lib for unit tests
   if(Apps_BUILD_TESTS)
    add_library(${APP_NAME}_cl_interface SHARED "${APP_DIR}/${APP_NAME}_cl_interface.cc")
    target_link_libraries(${APP_NAME}_cl_interface PRIVATE ${APP_LINK_LIBRARIES} libra GTest::GTest GTest::Main "-Wl,--disable-new-dtags")
    install(TARGETS ${APP_NAME}_cl_interface LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
   endif()

   # build app python wrapper
   set(BUILD_PYWRAPPER ON)
   if(BUILD_PYWRAPPER)
     message("building python wrapper ${APP_NAME}2py")
     pybind11_add_module(${APP_NAME}2py "${APP_DIR}/${APP_NAME}_py_interface.cc")
     target_link_libraries(${APP_NAME}2py PRIVATE ${APP_LINK_LIBRARIES} libra)
     install(TARGETS ${APP_NAME}2py LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
   endif()
endforeach()
