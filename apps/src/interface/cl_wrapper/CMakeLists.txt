
set (CMAKE_CXX_STANDARD 11)
set(OPT "-g -O2 ")
set(WRAPPER_LIST roadrunner hummbee mssplit subms tableinfo wabbler)
foreach(WRAPPER IN LISTS WRAPPER_LIST)
    add_executable(${WRAPPER} ${WRAPPER}.cc)
    target_link_libraries(${WRAPPER} PRIVATE ${APP_LINK_LIBRARIES})
    install(TARGETS ${WRAPPER} DESTINATION "${CMAKE_SOURCE_DIR}/install") 
endforeach()
