#set(APP_SOURCES "${APP_NAME}.cc")
set (CMAKE_CXX_STANDARD 11)
set(OPT "-g -O2 $(python3.8-config --includes) \
$(python3.8 -m pybind11 --includes) ")

find_package(pybind11 REQUIRED)
#add_subdirectory(pybind11)
set(WRAPPER_LIST roadrunner2py hummbee2py)
foreach(WRAPPER IN LISTS WRAPPER_LIST)
    pybind11_add_module(${WRAPPER} ${WRAPPER}.cc)
    target_link_libraries(${WRAPPER} PRIVATE ${APP_LINK_LIBRARIES})
    install(TARGETS ${WRAPPER} DESTINATION "${CMAKE_SOURCE_DIR}/install") 
endforeach()
