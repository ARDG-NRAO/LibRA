project("LibRA Apps"
  DESCRIPTION "LibRA CL, C/C++, Py Interfaces"
  HOMEPAGE_URL https://github.com/ARDG-NRAO/LibRA
  LANGUAGES CXX Fortran C)

cmake_minimum_required(VERSION 3.12.0)
set (CMAKE_CXX_STANDARD 17)

include(FetchContent)
include(FindPkgConfig)

set(DEFS64 "-DAIPS_64B -DCASA_STANDALONE")

if (ARM_CPU)
  set(OPT "-g -O2 -mcpu=neoverse-v2 ")
else()
  set(OPT "-g -O2 -march=native ")
endif()

set(BUILD_TYPE "RelWithDebInfo")


set(DEPLIB "${CMAKE_INSTALL_PREFIX}/lib")
set(DEPINC "${CMAKE_INSTALL_PREFIX}/include")
set(USRLIB "/usr/lib64")

set(CMAKE_PREFIX_PATH  ${CMAKE_PREFIX_PATH}
  :${CMAKE_INSTALL_PREFIX}/lib/cmake/hpg
  :${CMAKE_INSTALL_PREFIX}/lib/cmake/parafeed
  :${CMAKE_INSTALL_PREFIX}/lib/cmake/Kokkos
  :${CMAKE_INSTALL_PREFIX}/lib/cmake/casacore
  :${CMAKE_INSTALL_PREFIX}/lib/cmake/casacpp
  :${CMAKE_INSTALL_PREFIX}/lib/cmake/sakura
  :${CMAKE_INSTALL_PREFIX}/lib/cmake/pybind11)

set(hpg_DIR        ${CMAKE_INSTALL_PREFIX}/lib/cmake/hpg)
set(parafeed_DIR   ${CMAKE_INSTALL_PREFIX}/lib/cmake/parafeed)
set(Kokkos_DIR     ${CMAKE_INSTALL_PREFIX}/lib/cmake/Kokkos)
set(pybind11_DIR   ${CMAKE_INSTALL_PREFIX}/share/cmake/pybind11)
set(casacore_DIR   ${CMAKE_INSTALL_PREFIX}/lib/cmake/casacore)
set(casacpp_DIR    ${CMAKE_INSTALL_PREFIX}/lib/cmake/casacpp)
set(GTest_DIR      ${CMAKE_INSTALL_PREFIX}/lib/cmake/GTest)

message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message("hpg_DIR: ${hpg_DIR}")
message("parafeed_DIR: ${parafeed_DIR}")
message("Kokkos_DIR: ${Kokkos_DIR}")
message("pybind11_DIR: ${pybind11_DIR}")

set(CMAKE_C_FLAGS "${OPT} -std=c++11 -DUseCasacoreNamespace -Wno-deprecated \
           ${DEFS64}                                        \
           -DHAVE_QT4                                       \
           -DAIPS_NEEDS_RETHROW                             \
           -DAIPS_LINUX                                     \
           -DAIPS_LITTLE_ENDIAN                             \
           -DAIPS_STDLIB                                    \
           -DAIPS_NO_LEA_MALLOC                             \
           -D_GLIBCPP_DEPRECATED                            \
           -DAIPS_AUTO_STL                                  \
           -DAIPS_ARRAY_INDEX_CHECK                         \
           -DSIGNBIT                                        \
           -DAIPS_NO_TEMPLATE_SRC                           \
           -Wall -pipe -Wno-non-template-friend             \
           -Woverloaded-virtual -Wcast-align                \
           -Wno-comment -D_FILE_OFFSET_BITS=64              \
           -D_LARGEFILE_SOURCE                              \
           -DDBUS_CPP=1  -Dlibcasadbus_EXPORTS"                             
)                                    

set(APP_LINK_LIBRARIES
  casacpp_synthesis
  casacpp_msvis
  librautils
  libracore
  hpg
  kokkoscore
  casa_msfits
  casa_ms
  casa_images
  casa_coordinates
  casa_lattices
  casa_fits
  casa_measures
  casa_tables
  casa_scimath
  casa_scimath_f
  casa_casa
  pthread
  parafeed
  readline
  history
  termcap
  m
  c)

include_directories(
  ${CMAKE_INSTALL_PREFIX}/include/casacore
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/include/casacpp
  /usr/include/cfitsio 
  ${CMAKE_SOURCE_DIR}/src
  # for libracore
  ${CMAKE_SOURCE_DIR}/../src
  # for mdspan
  ${CMAKE_SOURCE_DIR}/../dependencies/Kokkos/tpls/mdspan/include/mdspan/)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)


# for runtime linking. Needs to be before add_executable
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${DEPLIB}")
set(CMAKE_BUILD_TYPE "${BUILD_TYPE}")


# add and build each APP
set(APP_LIST Coyote Hummbee RoadRunner Chip Dale Acme MSSplit SubMS TableInfo Asp Asp_mdspan Restore Restore_mdspan Utilities)

set (ACME_SOURCES
  Acme/acme.cc
  Acme/acme.h
)

set (ASP_SOURCES
  Asp/asp.cc
  Asp/asp.h
)

set (ASP_MDSPAN_SOURCES
  Asp_mdspan/asp_mdspan.cc
  Asp_mdspan/asp_mdspan.h
)

set (CHIP_SOURCES
  Chip/chip.cc
  Chip/chip.h
)

set (COYOTE_SOURCES
  Coyote/coyote.cc
  Coyote/coyote.h
)

set (DALE_SOURCES
  Dale/dale.cc
  Dale/dale.h
)

set (HUMMBEE_SOURCES
  Hummbee/hummbee.cc
  Hummbee/hummbee.h
)

set (MSSPLIT_SOURCES
  MSSplit/mssplit.cc
  MSSplit/mssplit.h
)

set (RESTORE_SOURCES
  Restore/restore.cc
  Restore/restore.h
)

set (RESTORE_MDSPAN_SOURCES
  Restore_mdspan/restore_mdspan.cc
  Restore_mdspan/restore_mdspan.h
)

set (ROADRUNNER_SOURCES
  RoadRunner/roadrunner.cc
  RoadRunner/roadrunner.h
  # RoadRunner/DataBase.h
  # RoadRunner/DataIterations.h
  # RoadRunner/MakeComponents.h
  # RoadRunner/rWeightor.h
  # RoadRunner/ThreadCoordinator.h
)

set (SUBMS_SOURCES
  SubMS/subms.cc
  SubMS/subms.h
)

set (TABLEINFO_SOURCES
  TableInfo/tableinfo.cc
  TableInfo/tableinfo.h
)

# set(UTILITIES_SOURCES 
  # Utilities/backup.cc
  # Utilities/backup.h
# )

set (LIBRA_SOURCES
  ${ACME_SOURCES}
  ${ASP_SOURCES}
  ${ASP_MDSPAN_SOURCES}
  ${CHIP_SOURCES}
  ${COYOTE_SOURCES}
  ${HUMMBEE_SOURCES}
  ${MSSPLIT_SOURCES}
  ${RESTORE_SOURCES}
  ${RESTORE_MDSPAN_SOURCES}
  ${ROADRUNNER_SOURCES}
  ${SUBMS_SOURCES}
  ${TABLEINFO_SOURCES}
  # ${UTILITIES_SOURCES}
  ${DALE_SOURCES}
)

# add and build googletest
if(Apps_BUILD_TESTS)
  add_subdirectory(src/tests)
endif()

# add and build translation layer
add_subdirectory(src/translation)

add_subdirectory(src)


