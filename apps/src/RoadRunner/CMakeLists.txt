#set(APP_NAME "roadrunner")
include_directories(${CLHOME}/include ${COREROOT}/include/casacore ${CASAROOT}/include/casacpp /usr/include/cfitsio ${CMAKE_SOURCE_DIR}/src)
link_directories(${CLHOME}/lib ${CASAHOME}/linux_64b/lib /usr/local/lib ${LIBROOT} ${COREROOT}/lib ${USRLIB})

set(APP_SOURCES "${APP_NAME}.cc")
#set (CMAKE_CXX_STANDARD 11)
#set(OPT "-g -O2 $(python3.8-config --includes) \
#$(python3.8 -m pybind11 --includes) ")

add_executable(${APP_NAME} ${APP_SOURCES})
#find_package(pybind11 REQUIRED)
#pybind11_add_module(roadrunner2py roadrunner2py.cc)
target_link_libraries(${APP_NAME} PRIVATE ${APP_LINK_LIBRARIES})
#target_link_libraries(roadrunner2py PRIVATE ${APP_LINK_LIBRARIES})
install(TARGETS ${APP_NAME} DESTINATION "${CMAKE_SOURCE_DIR}/install") 
