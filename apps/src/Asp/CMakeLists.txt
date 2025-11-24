set(CMAKE_CXX_STANDARD 17)

set(APP_NAME asp)
string(TOUPPER ${APP_NAME} APP_NAME_UPPER)

set(APP_SOURCES
  asp.cc
)

find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)

file(GLOB APP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES ${APP_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/")

add_library(${APP_NAME}_lib SHARED ${APP_SOURCES})
target_link_libraries(${APP_NAME}_lib PUBLIC ${APP_LINK_LIBRARIES} casacpp_synthesis librautils libracore)
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

# Optional: Build CLI interface library for tests (commented by default)
# Uncomment if tests need to call CLI interface functions directly
# if(Apps_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_cl_interface.cc")
#   add_library(${APP_NAME}_cl_interface SHARED "${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}_cl_interface.cc")
#   target_compile_definitions(${APP_NAME}_cl_interface PRIVATE ${APP_NAME_UPPER}_LIBRARY_BUILD)
#   target_link_libraries(${APP_NAME}_cl_interface PRIVATE ${APP_NAME}_lib "-Wl,--disable-new-dtags")
#   install(TARGETS ${APP_NAME}_cl_interface LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
# endif()
