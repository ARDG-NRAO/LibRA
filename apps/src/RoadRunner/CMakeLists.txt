#set(APP_NAME "roadrunner")
include_directories(${CLHOME}/include ${COREROOT}/include/casacore ${CASAROOT}/include/casacpp /usr/include/cfitsio ${CMAKE_SOURCE_DIR}/src)
link_directories(${CLHOME}/lib ${CASAHOME}/linux_64b/lib /usr/local/lib ${LIBROOT} ${COREROOT}/lib ${USRLIB})

set(APP_SOURCES "${APP_NAME}.cc")

add_executable(${APP_NAME} ${APP_SOURCES})
target_link_libraries(${APP_NAME} PRIVATE ${APP_LINK_LIBRARIES})
install(TARGETS ${APP_NAME} DESTINATION "${CMAKE_SOURCE_DIR}/install") 
