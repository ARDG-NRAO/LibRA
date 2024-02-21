set (CMAKE_CXX_STANDARD 11)
set(OPT "-g -O2 ")

# build app binary and lib
add_library(libra SHARED ${APP_SOURCES})
target_link_libraries(libra PRIVATE ${APP_LINK_LIBRARIES} "-Wl,--disable-new-dtags")
install(TARGETS libra 
	LIBRARY DESTINATION "${INSTALLDIR}/lib") 


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
 

   # build app CL interface binary
   set(BUILD_CLWRAPPER ON)
   if(BUILD_CLWRAPPER AND EXISTS "${APP_DIR}/${APP_NAME}_cl_interface.cc")
     message("building CL binary ${APP_NAME}")
     add_executable(${APP_NAME} "${APP_DIR}/${APP_NAME}_cl_interface.cc")
     target_link_libraries(${APP_NAME} ${APP_LINK_LIBRARIES} libra)
     install(TARGETS ${APP_NAME} DESTINATION "${INSTALLDIR}/bin") 
   endif()

   # build individual app CL interface lib for unit tests
   if(Apps_BUILD_TESTS AND EXISTS "${APP_DIR}/${APP_NAME}_cl_interface.cc")
    add_library(${APP_NAME}_cl_interface SHARED "${APP_DIR}/${APP_NAME}_cl_interface.cc")
    target_link_libraries(${APP_NAME}_cl_interface PRIVATE ${APP_LINK_LIBRARIES} libra "-Wl,--disable-new-dtags")
    install(TARGETS ${APP_NAME}_cl_interface 
    	LIBRARY DESTINATION "${INSTALLDIR}/lib") 
   endif()
   
   # build app python wrapper
   set(BUILD_PYWRAPPER ON)
   if(BUILD_PYWRAPPER AND EXISTS "${APP_DIR}/${APP_NAME}_py_interface.cc")
     message("building python wrapper ${APP_NAME}2py")
     pybind11_add_module(${APP_NAME}2py "${APP_DIR}/${APP_NAME}_py_interface.cc")
     target_link_libraries(${APP_NAME}2py PRIVATE ${APP_LINK_LIBRARIES} libra)
     install(TARGETS ${APP_NAME}2py 
     	LIBRARY DESTINATION "${INSTALLDIR}/lib")
   endif()
endforeach()
