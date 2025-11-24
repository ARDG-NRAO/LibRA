cmake_minimum_required(VERSION 3.12.0)
set (CMAKE_CXX_STANDARD 17)

project("LibRA Apps"
  DESCRIPTION "LibRA CL, C/C++, Py Interfaces"
  HOMEPAGE_URL https://github.com/ARDG-NRAO/LibRA
  LANGUAGES CXX Fortran C)

include(FetchContent)
include(FindPkgConfig)

# Find GSL dependency (required by casacpp libraries)
find_package(GSL REQUIRED)

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

find_package(WCSLIB REQUIRED)
find_package(CFITSIO REQUIRED)
find_package(Kokkos REQUIRED)

# Lazy binding flags to allow apps to run without GPU on non-CUDA machines
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed -Wl,-z,lazy")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--as-needed -Wl,-z,lazy")

# Tier 1: Core CASA libraries (no GPU)
set(APP_LINK_LIBRARIES_CORE
  casa_derivedmscal
  casa_msfits
  casa_ms
  casa_images
  casa_coordinates
  casa_lattices
  casa_fits
  casa_measures
  casa_meas
  casa_mirlib
  casa_tables
  casa_scimath
  casa_scimath_f
  casa_casa
  pthread
  parafeed
  wcslib::wcs
  CFITSIO::CFITSIO
  GSL::gsl
  GSL::gslcblas
  m
  c)

# Tier 2: Add MS processing (no GPU)
set(APP_LINK_LIBRARIES_MS
  ${APP_LINK_LIBRARIES_CORE}
  casacpp_msvis)

# Tier 3: Add LibRA utilities (check if these need GPU)
set(APP_LINK_LIBRARIES_LIBRA
  ${APP_LINK_LIBRARIES_MS}
  librautils
  libracore)

# Tier 4: GPU-enabled imaging
set(APP_LINK_LIBRARIES_GPU
  ${APP_LINK_LIBRARIES_LIBRA}
  casacpp_synthesis
  hpg
  Kokkos::kokkos)

# Default to core (apps opt-in to higher tiers)
set(APP_LINK_LIBRARIES ${APP_LINK_LIBRARIES_CORE})

include_directories(
    ${CMAKE_INSTALL_PREFIX}/include/casacore
    ${CMAKE_INSTALL_PREFIX}/include
    ${CMAKE_INSTALL_PREFIX}/include/casacpp
    /usr/include/cfitsio
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/RoadRunner
    ${CMAKE_SOURCE_DIR}/src/Hummbee
    ${CMAKE_SOURCE_DIR}/src/Coyote
    ${CMAKE_SOURCE_DIR}/src/Chip
    ${CMAKE_SOURCE_DIR}/src/Dale
    ${CMAKE_SOURCE_DIR}/src/Acme
    ${CMAKE_SOURCE_DIR}/src/MSSplit
    ${CMAKE_SOURCE_DIR}/src/SubMS
    ${CMAKE_SOURCE_DIR}/src/TableInfo
    ${CMAKE_SOURCE_DIR}/src/Asp
    ${CMAKE_SOURCE_DIR}/src/Asp_mdspan
    ${CMAKE_SOURCE_DIR}/src/Restore
    ${CMAKE_SOURCE_DIR}/src/Restore_mdspan
    # for libracore
    ${CMAKE_SOURCE_DIR}/../src
    # for mdspan
    ${CMAKE_SOURCE_DIR}/../dependencies/Kokkos/tpls/mdspan/include/mdspan/
    )

link_directories(${CMAKE_INSTALL_PREFIX}/lib
  )


# for runtime linking. Needs to be before add_executable
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${DEPLIB}")
set(CMAKE_BUILD_TYPE "${BUILD_TYPE}")


# add and build each APP
add_subdirectory(src)


