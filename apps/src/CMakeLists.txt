# Set the C++ standard to C++17
set (CMAKE_CXX_STANDARD 17)

# Set optimization flags
set(OPT "-g -O2 ")

# Find required packages
# find_package(Kokkos REQUIRED)
find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)

# Add shared library target 'libra' and link libraries
add_library(libra SHARED ${LIBRA_SOURCES})
target_link_libraries(libra PRIVATE ${APP_LINK_LIBRARIES} "-Wl,--disable-new-dtags")
install(TARGETS libra LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

# Loop through each application directory in APP_LIST
foreach (APP_DIR IN LISTS APP_LIST)
  # Include directories for each application
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/)
  string(TOLOWER ${APP_DIR} APP_NAME)

  # Install application headers
  file(GLOB APP_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/*.h")
  install(FILES ${APP_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/")

  # Build command line interface (CLI) binary if the source file exists
  set(BUILD_CLWRAPPER ON)
  if(BUILD_CLWRAPPER AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_cl_interface.cc")
    add_executable(${APP_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_cl_interface.cc")
    target_link_libraries(${APP_NAME} ${APP_LINK_LIBRARIES} libra ${READLINE_LDFLAGS} ${CURSES_LIBRARIES})
    install(TARGETS ${APP_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
  endif()

  # Build individual CLI interface library for unit tests if the source file exists
  if(Apps_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_cl_interface.cc")
   add_library(${APP_NAME}_cl_interface SHARED "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_cl_interface.cc")
   string(TOUPPER ${APP_NAME} APP_NAME_UPPER)
   target_compile_definitions(${APP_NAME}_cl_interface PRIVATE ${APP_NAME_UPPER}_LIBRARY_BUILD)
   target_link_libraries(${APP_NAME}_cl_interface PRIVATE ${APP_LINK_LIBRARIES} libra "-Wl,--disable-new-dtags")
   install(TARGETS ${APP_NAME}_cl_interface
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
  endif()

  # Build Python wrapper if the source file exists
  set(BUILD_PYWRAPPER ON)
  if(BUILD_PYWRAPPER AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_py_interface.cc")
    pybind11_add_module(${APP_NAME}2py "${CMAKE_CURRENT_SOURCE_DIR}/${APP_DIR}/${APP_NAME}_py_interface.cc")
    target_link_libraries(${APP_NAME}2py PRIVATE ${APP_LINK_LIBRARIES} libra)
    install(TARGETS ${APP_NAME}2py
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
  endif()
endforeach()

# Add subdirectory for examples
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)




