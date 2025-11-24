set(CMAKE_CXX_STANDARD 17)

set(APP_NAME utilities)
string(TOUPPER ${APP_NAME} APP_NAME_UPPER)

find_package(pybind11 REQUIRED)

set(BUILD_PYWRAPPER ON)
if(BUILD_PYWRAPPER AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_py_interface.cc")
  pybind11_add_module(${APP_NAME}2py "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_py_interface.cc")
  target_link_libraries(${APP_NAME}2py PRIVATE ${APP_LINK_LIBRARIES_LIBRA} casacpp_synthesis)
  install(TARGETS ${APP_NAME}2py LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
endif()
