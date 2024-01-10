
set (CMAKE_CXX_STANDARD 11)
set(OPT "-g -O2 ")
#set(WRAPPER_LIST roadrunner hummbee mssplit subms tableinfo coyote)
#set(WRAPPER_LIST roadrunner hummbee mssplit subms tableinfo)

# build app binary and lib
set(APP_SOURCES "${APP_NAME}.cc")
add_library("${APP_NAME}lib" ${APP_NAME}.cc)
#add_executable(${APP_NAME} ${APP_SOURCES})
target_link_libraries("${APP_NAME}lib" PRIVATE ${APP_LINK_LIBRARIES} "-Wl,--disable-new-dtags")
#target_link_libraries(${APP_NAME} ${APP_LINK_LIBRARIES})
install(TARGETS "${APP_NAME}lib" DESTINATION "${CMAKE_SOURCE_DIR}/install") 
#install(TARGETS ${APP_NAME} DESTINATION "${CMAKE_SOURCE_DIR}/install") 


# build app CI interface binary
add_executable(${APP_NAME} "${APP_NAME}_cl_interface.cc")
target_link_libraries(${APP_NAME} ${APP_LINK_LIBRARIES} "${APP_NAME}lib")
install(TARGETS ${APP_NAME} DESTINATION "${CMAKE_SOURCE_DIR}/install") 


# build app python wrapper
FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
        FetchContent_Populate(
        pybind11
        URL https://github.com/pybind/pybind11/archive/refs/tags/v2.10.4.tar.gz
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pybind11
        )          
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pybind11)        
endif()

pybind11_add_module(${APP_NAME}_py_interface "${APP_NAME}_py_interface.cc")
target_link_libraries(${APP_NAME}_py_interface PRIVATE ${APP_LINK_LIBRARIES})
install(TARGETS ${APP_NAME}_py_interface DESTINATION "${CMAKE_SOURCE_DIR}/install")

#foreach(WRAPPER IN LISTS WRAPPER_LIST)
#	add_library("${WRAPPER}lib" ${WRAPPER}.cc)
#	add_executable(${WRAPPER} ${WRAPPER}.cc)
#	target_link_libraries("${WRAPPER}lib" PRIVATE ${APP_LINK_LIBRARIES} "-Wl,--disable-new-dtags")
#	target_link_libraries(${WRAPPER} PRIVATE ${APP_LINK_LIBRARIES} "${WRAPPER}lib" "-Wl,--disable-new-dtags")
#	install(TARGETS "${WRAPPER}lib" DESTINATION "${CMAKE_SOURCE_DIR}/install") 
#    install(TARGETS ${WRAPPER} DESTINATION "${CMAKE_SOURCE_DIR}/install")
#endforeach()
