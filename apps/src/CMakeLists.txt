set (CMAKE_CXX_STANDARD 17)

set(OPT "-g -O2 ")

find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)

if(BUILD_TESTING)
  find_package(GTest REQUIRED)
endif()

add_subdirectory(RoadRunner)
add_subdirectory(Hummbee)
add_subdirectory(Coyote)
add_subdirectory(Chip)
add_subdirectory(Dale)
add_subdirectory(Acme)
add_subdirectory(MSSplit)
add_subdirectory(SubMS)
add_subdirectory(TableInfo)
add_subdirectory(Asp)
add_subdirectory(Asp_mdspan)
add_subdirectory(Restore)
add_subdirectory(Restore_mdspan)
add_subdirectory(Utilities)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/translation)

if(BUILD_TESTING)
  find_package(GSL REQUIRED)
  find_package(CFITSIO REQUIRED)
  set(CASACORE_LIBS
    casa_derivedmscal casa_msfits casa_ms casa_images casa_coordinates
    casa_lattices casa_fits casa_measures casa_meas casa_mirlib
    casa_tables casa_scimath casa_scimath_f casa_casa
    pthread parafeed ${WCSLIB_LIBRARIES} CFITSIO::CFITSIO GSL::gsl GSL::gslcblas m c)

  set(TESTS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

  add_executable(test_asp ${CMAKE_CURRENT_SOURCE_DIR}/Asp/test_asp.cc)
  target_include_directories(test_asp PRIVATE ${TESTS_INCLUDE_DIR})
  target_link_libraries(test_asp GTest::gtest_main stdc++fs casacore_asp casacpp_synthesis ${CASACORE_LIBS})
  add_test(NAME test_asp COMMAND test_asp WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
  install(TARGETS test_asp RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/tests")

  add_executable(test_restore ${CMAKE_CURRENT_SOURCE_DIR}/Restore/test_restore.cc)
  target_include_directories(test_restore PRIVATE ${TESTS_INCLUDE_DIR})
  target_link_libraries(test_restore GTest::gtest_main stdc++fs restore_lib casacore_restore ${CASACORE_LIBS})
  add_test(NAME test_restore COMMAND test_restore WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
  install(TARGETS test_restore RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/tests")
endif()
