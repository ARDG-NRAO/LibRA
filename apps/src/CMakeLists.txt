set (CMAKE_CXX_STANDARD 11)
set(OPT "-g -O2 ")

# build app binary and lib
add_library(libra SHARED ${APP_SOURCES})
target_link_libraries(libra PRIVATE ${APP_LINK_LIBRARIES} "-Wl,--disable-new-dtags")
install(TARGETS libra DESTINATION "${CMAKE_SOURCE_DIR}/install") 


FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
        FetchContent_Populate(
        pybind11
        URL https://github.com/pybind/pybind11/archive/refs/tags/v2.10.4.tar.gz
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pybind11
        )          
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pybind11)        
endif()


foreach(APP_DIR IN LISTS APP_LIST)
   string(TOLOWER ${APP_DIR} APP_NAME)
   # build app CI interface binary
   set(BUILD_CLWRAPPER ON)
   if(BUILD_CLWRAPPER)
     message("building CL binary ${APP_NAME}")
     add_executable(${APP_NAME} "${APP_DIR}/${APP_NAME}_cl_interface.cc")
     target_link_libraries(${APP_NAME} ${APP_LINK_LIBRARIES} libra)
     install(TARGETS ${APP_NAME} DESTINATION "${CMAKE_SOURCE_DIR}/install") 
   endif()
   
   # build app python wrapper
   set(BUILD_PYWRAPPER ON)
   if(BUILD_PYWRAPPER)
     message("building python wrapper ${APP_NAME}2py")
     pybind11_add_module(${APP_NAME}2py "${APP_DIR}/${APP_NAME}_py_interface.cc")
     target_link_libraries(${APP_NAME}2py PRIVATE ${APP_LINK_LIBRARIES} libra)
     install(TARGETS ${APP_NAME}2py DESTINATION "${CMAKE_SOURCE_DIR}/install")
   endif()
endforeach()

