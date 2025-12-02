set (CMAKE_CXX_STANDARD 17)

set(OPT "-g -O2 ")

find_package(parafeed REQUIRED)
find_package(pybind11 REQUIRED)

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

if(Apps_BUILD_TESTS)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()
