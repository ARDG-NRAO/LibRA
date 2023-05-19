
set (CMAKE_CXX_STANDARD 11)
set(OPT "-g -O2 ")
FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
        FetchContent_Populate(
        pybind11
        URL https://github.com/pybind/pybind11/archive/refs/tags/v2.10.4.tar.gz
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pybind11
        )          
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pybind11)        
endif()
#FetchContent_MakeAvailable(pybind11)
#find_package(pybind11 REQUIRED)
set(WRAPPER_LIST roadrunner2py hummbee2py)

foreach(WRAPPER IN LISTS WRAPPER_LIST)
    pybind11_add_module(${WRAPPER} ${WRAPPER}.cc)
    target_link_libraries(${WRAPPER} PRIVATE ${APP_LINK_LIBRARIES})
    install(TARGETS ${WRAPPER} DESTINATION "${CMAKE_SOURCE_DIR}/install") 
endforeach()
