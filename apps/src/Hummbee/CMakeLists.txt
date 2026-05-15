set(CMAKE_CXX_STANDARD 17)

set(APP_NAME hummbee)
string(TOUPPER ${APP_NAME} APP_NAME_UPPER)

set(APP_SOURCES
  hummbee.cc
)

find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)

file(GLOB APP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES ${APP_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/")

add_library(${APP_NAME}_lib SHARED ${APP_SOURCES})
target_link_libraries(${APP_NAME}_lib PUBLIC ${APP_LINK_LIBRARIES_LIBRA} casacpp_synthesis)
set_target_properties(${APP_NAME}_lib PROPERTIES OUTPUT_NAME ${APP_NAME})
install(TARGETS ${APP_NAME}_lib LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

set(BUILD_CLWRAPPER ON)
if(BUILD_CLWRAPPER AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_cl_interface.cc")
  add_executable(${APP_NAME}_bin "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_cl_interface.cc")
  target_link_libraries(${APP_NAME}_bin PRIVATE ${APP_NAME}_lib ${READLINE_LDFLAGS} ${CURSES_LIBRARIES})
  set_target_properties(${APP_NAME}_bin PROPERTIES OUTPUT_NAME ${APP_NAME})
  install(TARGETS ${APP_NAME}_bin DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
endif()

set(BUILD_PYWRAPPER ON)
if(BUILD_PYWRAPPER AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_py_interface.cc")
  pybind11_add_module(${APP_NAME}2py "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_py_interface.cc")
  target_link_libraries(${APP_NAME}2py PRIVATE ${APP_NAME}_lib)
  install(TARGETS ${APP_NAME}2py LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
endif()


if(BUILD_TESTING)
  find_package(GTest REQUIRED)
  add_executable(test_${APP_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/test_${APP_NAME}.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_cl_interface.cc)
  target_include_directories(test_${APP_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/src/tests)
  target_compile_definitions(test_${APP_NAME} PRIVATE HUMMBEE_LIBRARY_BUILD)
  target_link_libraries(test_${APP_NAME}
    GTest::gtest_main stdc++fs ${APP_NAME}_lib)
  add_test(NAME test_${APP_NAME} COMMAND test_${APP_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/tests)
  install(TARGETS test_${APP_NAME}
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/tests")

  add_executable(test_htclean
    ${CMAKE_CURRENT_SOURCE_DIR}/test_htclean.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_cl_interface.cc)
  target_include_directories(test_htclean PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/src/tests)
  target_compile_definitions(test_htclean PRIVATE
    HUMMBEE_LIBRARY_BUILD
    LIBRA_INSTALL_BIN="${CMAKE_INSTALL_PREFIX}/bin")
  target_link_libraries(test_htclean
    GTest::gtest_main stdc++fs ${APP_NAME}_lib)
  add_test(NAME test_htclean COMMAND test_htclean
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/tests)
  install(TARGETS test_htclean
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/tests")
  if(Apps_BUILD_TESTS_PYTHON)
    find_program(PYTHON_EXECUTABLE python)
    add_test(NAME test_${APP_NAME}_python
      COMMAND ${PYTHON_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_${APP_NAME}.py
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/tests)
  endif()
endif()
