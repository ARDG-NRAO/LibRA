cmake_minimum_required(VERSION 3.18)

#if cmake is greater than 3.24.0, set policy to old
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 OLD)
endif()

project("LibRA Apps"
  DESCRIPTION "LibRA CL, C/C++, Py Interfaces"
  HOMEPAGE_URL https://github.com/ARDG-NRAO/LibRA
  LANGUAGES CXX Fortran C)

set(LIBRA_ID "LibRA INFO: ")
set(CMAKE_CXX_STANDARD 11)
set(BUILD_TYPE "RelWithDebInfo")
set(CASA_BUILD_TYPE "RelWithDebInfo")

# Include the ExternalProject module
include(ExternalProject)
include(FindPkgConfig)

set(ENV{PATH} $ENV{PATH}:/usr/local/cuda/bin)
message("${LIBRA_ID} PATH=$ENV{PATH}")
LIST(APPEND CMAKE_PROGRAM_PATH  "/usr/local/cuda/bin" ...)
#
#------------------------------------------------------------------------------
# Various repositories and the tags we dependend on
#
set(LIBRA_KOKKOS_GIT_REPOSITORY   "https://github.com/kokkos/kokkos.git")
set(LIBRA_KOKKOS_GIT_TAG          "4.4.01")

set(LIBRA_HPG_GIT_REPOSITORY      "https://gitlab.nrao.edu/mpokorny/hpg.git")
#set(LIBRA_HPG_GIT_REPOSITORY      "https://gitlab.com/dsa-2000/rcp/hpg")
set(LIBRA_HPG_GIT_TAG             "v3.0.3")

set(LIBRA_CASACORE_GIT_REPOSITORY "https://github.com/preshanth/casacore.git")#"https://github.com/casacore/casacore.git")#
set(LIBRA_CASACORE_GIT_TAG        "fix_regex_v3.5.0")#"2e121e67bf881ac7f309604447a4b0d481913902")#

set(LIBRA_PARAFEED_GIT_REPOSITORY "https://github.com/sanbee/parafeed.git")
set(LIBRA_PARAFEED_GIT_TAG        "v1.1.3")

set(LIBRA_SAKURA_GIT_REPOSITORY   "https://github.com/tnakazato/sakura.git")
set(LIBRA_SAKURA_GIT_TAG          "libsakura-5.1.3")

set(LIBRA_PYBIND11_GIT_REPOSITORY "https://github.com/pybind/pybind11.git")
set(LIBRA_PYBIND11_GIT_TAG        "v2.10.2")

set(LIBRA_FFTW_URL                "http://www.fftw.org/fftw-3.3.10.tar.gz")

set(LIBRA_GTEST_GIT_REPOSITORY    "https://github.com/google/googletest.git")
set(LIBRA_GTEST_GIT_TAG           "v1.14.0")

set(LIBRA_MEASURES_FTP            "ftp://ftp.astron.nl/outgoing/Measures/WSRT_Measures.ztar")

set(LIBRA_EXODUS_GIT_REPOSITORY   "https://github.com/sanbee/exodus_sbfork.git")
set(LIBRA_EXODUS_GIT_TAG          "master")

set(LIBRA_EXODUS_BUNDLE_NAME      "libra_apps.sh")
set(LIBRA_EXODUS_BUNDLE_PATH       "")
#------------------------------------------------------------------------------
#


if(NOT DEFINED LIBRA_USE_LIBSAKURA)
  set(LIBRA_USE_LIBSAKURA OFF)
endif()

if(NOT DEFINED LIBRA_USE_EXODUS)
  set(LIBRA_USE_EXODUS OFF)
endif()

# Find system information
if (NOT DEFINED NCORES)
	cmake_host_system_information(RESULT NCORES QUERY NUMBER_OF_PHYSICAL_CORES)
	cmake_host_system_information(RESULT NTHREADS QUERY NUMBER_OF_LOGICAL_CORES)
	message("${LIBRA_ID} Number of physical cores: ${NCORES}")
	message("${LIBRA_ID} Number of logical cores: ${NTHREADS}")
  	math(EXPR NCORES "${NCORES} - 2")
endif()

cmake_host_system_information(RESULT OS_NAME QUERY OS_NAME)
cmake_host_system_information(RESULT OS_RELEASE QUERY OS_RELEASE)
cmake_host_system_information(RESULT OS_VERSION QUERY OS_VERSION)
cmake_host_system_information(RESULT OS_PLATFORM QUERY OS_PLATFORM)
cmake_host_system_information(RESULT AVAILABLE_PHYSICAL_MEMORY QUERY AVAILABLE_PHYSICAL_MEMORY)
cmake_host_system_information(RESULT TOTAL_PHYSICAL_MEMORY QUERY TOTAL_PHYSICAL_MEMORY)
cmake_host_system_information(RESULT TOTAL_VIRTUAL_MEMORY QUERY TOTAL_VIRTUAL_MEMORY)


message("${LIBRA_ID} OS Name: ${OS_NAME}")
message("${LIBRA_ID} OS Release: ${OS_RELEASE}")
message("${LIBRA_ID} OS Version: ${OS_VERSION}")
message("${LIBRA_ID} OS Platform: ${OS_PLATFORM}")
message("${LIBRA_ID} Available Physical Memory: ${AVAILABLE_PHYSICAL_MEMORY} BYTES")
message("${LIBRA_ID} Total Physical Memory: ${TOTAL_PHYSICAL_MEMORY} BYTES")
if ($NCORES LESS 1)
  set(NCORES 1)
endif()
message(${LIBRA_ID} "Setting number of cores to ${NCORES}")

# Set the top-level build and install directories
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install)
set(TOP_LEVEL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)

# set(CMAKE_C_FLAGS "${OPT} -std=c++11 -DUseCasacoreNamespace -Wno-deprecated \
#            ${DEFS64}                                        \
#            -DHAVE_QT4                                       \
#            -DAIPS_NEEDS_RETHROW                             \
#            -DAIPS_LINUX                                     \
#            -DAIPS_LITTLE_ENDIAN                             \
#            -DAIPS_STDLIB                                    \
#            -DAIPS_NO_LEA_MALLOC                             \
#            -D_GLIBCPP_DEPRECATED                            \
#            -DAIPS_AUTO_STL                                  \
#            -DAIPS_ARRAY_INDEX_CHECK                         \
#            -DSIGNBIT                                        \
#            -DAIPS_NO_TEMPLATE_SRC                           \
#            -Wall -pipe -Wno-non-template-friend             \
#            -Woverloaded-virtual -Wcast-align                \
#            -Wno-comment -D_FILE_OFFSET_BITS=64              \
#            -D_LARGEFILE_SOURCE                              \
#            -DDBUS_CPP=1  -Dlibcasadbus_EXPORTS"                             
# )  

message("${LIBRA_ID} BUILD_DIR: ${BUILD_DIR}")
message("${LIBRA_ID} INSTALL_DIR: ${INSTALL_DIR}")
message("${LIBRA_ID} Installing all libraries to ${INSTALL_DIR}/lib")
message("${LIBRA_ID} Installing all headers to ${INSTALL_DIR}/include")
message("${LIBRA_ID} Installing all binaries to ${INSTALL_DIR}/bin")

message("${LIBRA_ID} LIBRA_USE_EXODUS: ${LIBRA_USE_EXODUS}")  

# Set the PKG_CONFIG_PATH environment variable
set(ENV{PKG_CONFIG_PATH} "${CMAKE_SOURCE_DIR}/install/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

if(NOT "${Kokkos_CUDA_ARCH_NAME}" STREQUAL "")
  # Add Kokkos as an external project
  message("${LIBRA_ID} Kokkos_CUDA_ARCH_NAME: ${Kokkos_CUDA_ARCH_NAME}")
  ExternalProject_Add(
    Kokkos
    GIT_REPOSITORY ${LIBRA_KOKKOS_GIT_REPOSITORY}
    GIT_TAG        ${LIBRA_KOKKOS_GIT_TAG}
    SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/Kokkos
    BINARY_DIR     ${BUILD_DIR}/Kokkos
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                   -DCMAKE_INSTALL_LIBDIR=lib
                   -DCMAKE_INSTALL_BINDIR=bin/kokkos
                   -DKokkos_ENABLE_CUDA_LAMBDA=ON
  	           -DKokkos_ENABLE_CUDA=ON
  	           -D${Kokkos_CUDA_ARCH_NAME}=ON
  	           -DKokkos_ENABLE_OPENMP=ON
  	           -DKokkos_ENABLE_SERIAL=ON
  	           -DBUILD_SHARED_LIBS=ON
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
  )
else()
  message("${LIBRA_ID} Kokkos_CUDA_ARCH_NAME is empty. Will automatically detect the CUDA architecture")
  ExternalProject_Add(
    Kokkos
    GIT_REPOSITORY ${LIBRA_KOKKOS_GIT_REPOSITORY}
    GIT_TAG        ${LIBRA_KOKKOS_GIT_TAG}
    SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/Kokkos
    BINARY_DIR     ${BUILD_DIR}/Kokkos
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                   -DCMAKE_INSTALL_LIBDIR=lib
                   -DCMAKE_INSTALL_BINDIR=bin/kokkos
                   -DKokkos_ENABLE_CUDA_LAMBDA=ON
                   -DKokkos_ENABLE_CUDA=ON
                   -DKokkos_ENABLE_OPENMP=ON
                   -DKokkos_ENABLE_SERIAL=ON
                   -DKokkos_ENABLE_PROFILING=ON
                   -DBUILD_SHARED_LIBS=ON
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
  )
endif()
# Add FFTW as an external project
ExternalProject_Add(
  FFTW
  URL             ${LIBRA_FFTW_URL}
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/dependencies/FFTW
  BINARY_DIR      ${BUILD_DIR}/FFTW
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --enable-shared --enable-threads --enable-openmp --prefix=${INSTALL_DIR} --bindir=${INSTALL_DIR}/bin/fftw --libdir=${INSTALL_DIR}/lib
  BUILD_COMMAND   make -j ${NCORES}
  INSTALL_COMMAND make install
)

# Get the install directory of Kokkos
ExternalProject_Get_Property(Kokkos install_dir)


# Add HPG as an external project
ExternalProject_Add(
  Hpg
  GIT_REPOSITORY ${LIBRA_HPG_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_HPG_GIT_TAG}
  DEPENDS        Kokkos FFTW
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/HPG
  BINARY_DIR     ${BUILD_DIR}/HPG
  CMAKE_ARGS     PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig
                 -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/Hpg
                 -DCMAKE_BUILD_TYPE=Release
                 -DFFTW_LIBRARIES=${INSTALL_DIR}/lib/
	         -DHpg_ENABLE_MIXED_PRECISION=OFF
		 -DHpg_ENABLE_EXPERIMENTAL_IMPLEMENTATIONS=OFF
	         -DHpg_API=11
	         -DHpg_BUILD_TESTS=OFF
	         -DBUILD_SHARED_LIBS=ON
	         -DCMAKE_CXX_COMPILER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper
                 -DKokkos_DIR=${INSTALL_DIR}/lib/cmake/Kokkos
  CONFIGURE_COMMAND PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:${INSTALL_DIR}/lib/cmake/Kokkos 
                    ${CMAKE_COMMAND} <SOURCE_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_BINDIR=bin/Hpg -DCMAKE_BUILD_TYPE=Release -DFFTW_LIBRARIES=${INSTALL_DIR}/lib/ -DHpg_ENABLE_MIXED_PRECISION=OFF -DHpg_ENABLE_EXPERIMENTAL_IMPLEMENTATIONS=OFF -DHpg_API=11 -DHpg_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_CXX_COMPILER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper -DKokkos_DIR=${INSTALL_DIR}/lib/cmake/Kokkos
  BUILD_COMMAND   make -j ${NCORES};
  INSTALL_COMMAND make install
)

# Add parafeed as an external project
ExternalProject_Add(
  Parafeed
  GIT_REPOSITORY ${LIBRA_PARAFEED_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_PARAFEED_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/parafeed
  BINARY_DIR     ${BUILD_DIR}/parafeed
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} 
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/parafeed
                 -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
                 -DBUILD_SHARED_LIBS=ON
  BUILD_COMMAND make -j ${NCORES}
  INSTALL_COMMAND make install
)

# Create the path ${CMAKE_SOURCE_DIR}/install/data and download the data into it
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/install/data)
# Create an external project for the data download
ExternalProject_Add(
  DataDownload
  DOWNLOAD_COMMAND curl ${LIBRA_MEASURES_FTP} | tar -C ${CMAKE_SOURCE_DIR}/install/data -xzf -
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

# Create the Casacore external project
ExternalProject_Add(
  Casacore
  GIT_REPOSITORY ${LIBRA_CASACORE_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_CASACORE_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/casacore
  BINARY_DIR     ${BUILD_DIR}/casacore
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/install
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/casacore
                 -DDATA_DIR=${CMAKE_SOURCE_DIR}/install/data
                 -DBUILD_PYTHON=OFF
                 -DUSE_OPENMP=ON
                 -DUSE_THREADS=ON
                 -DCMAKE_CXX_FLAGS="-fno-tree-vrp"
                 -DBUILD_FFTPACK_DEPRECATED=ON
                 -DBUILD_TESTING=OFF
                 -DBUILD_PYTHON=OFF
                 -DBUILD_PYTHON3=OFF
                 -DBUILD_DYSCO=ON
                 -DUseCcache=1 
                 -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
  CONFIGURE_COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/dependencies/casacore
                    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/install
                    -DCMAKE_INSTALL_LIBDIR=lib
                    -DCMAKE_INSTALL_BINDIR=bin/casacore
                    -DDATA_DIR=${CMAKE_SOURCE_DIR}/install/data
                    -DBUILD_PYTHON=OFF
                    -DUSE_OPENMP=ON
                    -DUSE_THREADS=ON
                    -DCMAKE_CXX_FLAGS="-fno-tree-vrp"
                    -DBUILD_FFTPACK_DEPRECATED=ON
                    -DBUILD_TESTING=OFF
                    -DBUILD_PYTHON=OFF
                    -DBUILD_PYTHON3=OFF
                    -DBUILD_DYSCO=ON
                    -DUseCcache=1 
                    -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
  BUILD_COMMAND  make -j${NCORES}
  INSTALL_COMMAND make install
  DEPENDS DataDownload
)


if (LIBRA_USE_LIBSAKURA)
  #Add Sakura as an external project
  ExternalProject_Add(
    Sakura
    GIT_REPOSITORY ${LIBRA_SAKURA_GIT_REPOSITORY}
    GIT_TAG        4{LIBRA_SAKURA_GIT_TAG}
    SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/sakura
    BINARY_DIR     ${BUILD_DIR}/sakura
    SOURCE_SUBDIR  libsakura
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_INSTALL_BINDIR=bin/sakura
    -DBUILD_DOC:BOOL=OFF
    -DPYTHON_BINDING:BOOL=OFF
    -DSIMD_ARCH=GENERIC
    -DENABLE_TEST:BOOL=OFF
    BUILD_COMMAND   make -j ${NCORES};
    INSTALL_COMMAND make install
    )
else()
  message("${LIBRA_ID} LIBRA_USE_LIBSAKURA is ${LIBRA_USE_LIBSAKURA}.")
endif()
# Add pybind11 as an external project
ExternalProject_Add(
  Pybind11
  GIT_REPOSITORY ${LIBRA_PYBIND11_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_PYBIND11_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/pybind11
  BINARY_DIR     ${BUILD_DIR}/pybind11
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/pybind11
                 -DPYBIND11_TEST=OFF
  BUILD_COMMAND   make -j ${NCORES}; 
  INSTALL_COMMAND make install
)

#if(Apps_BUILD_TESTS)
# Add GTest as an external project
ExternalProject_Add(
  GTest
  GIT_REPOSITORY ${LIBRA_GTEST_GIT_REPOSITORY}
  GIT_TAG        ${LIBRA_GTEST_GIT_TAG}
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/gtest
  BINARY_DIR     ${BUILD_DIR}/gtest
  CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                 -DCMAKE_INSTALL_LIBDIR=lib
                 -DCMAKE_INSTALL_BINDIR=bin/gtest
                 -DBUILD_GMOCK=OFF
                 -DBUILD_GTEST=ON
                 -DBUILD_SHARED_LIBS=ON
  BUILD_COMMAND   make -j ${ncores};
  INSTALL_COMMAND make install
)
#endif()
# Add Libra/src as an external project
if (LIBRA_USE_LIBSAKURA)
  set(SAKURA_DEPENDENCE "Sakura")
else()
  set(SAKURA_DEPENDENCE "")
endif()

ExternalProject_Add(
  Casacpp
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/src/
  DOWNLOAD_COMMAND ""
  BINARY_DIR      ${BUILD_DIR}/Libra/src
  DEPENDS         Casacore ${SAKURA_DEPENDENCE} Hpg Parafeed Pybind11
  #DEPENDS         Casacore Hpg Parafeed Pybind11
  CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                  -DCMAKE_INSTALL_LIBDIR=lib
                  -DCMAKE_INSTALL_BINDIR=bin/casacpp
                  -DCMAKE_CXX_FLAGS=-I${INSTALL_DIR}/include
                  -DHpg_DIR=${INSTALL_DIR}/lib/cmake/Hpg/
		              -Dparafeed_DIR=${INSTALL_DIR}/lib/cmake/parafeed/
                  -DKokkos_COMPILE_LAUNCHER=${INSTALL_DIR}/bin/kokkos/kokkos_launch_compiler
                  -DKokkos_NVCC_WRAPPER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper
                  -DCMAKE_PREFIX_PATH=${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos
  CONFIGURE_COMMAND PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos 
                                    ${CMAKE_COMMAND} <SOURCE_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_BINDIR=bin/casacpp -DCMAKE_CXX_FLAGS=-I${INSTALL_DIR}/include 
                                    -DHpg_DIR=${INSTALL_DIR}/lib/cmake/Hpg/ -Dparafeed_DIR=${INSTALL_DIR}/lib/cmake/parafeed/ -DKokkos_COMPILE_LAUNCHER=${INSTALL_DIR}/bin/kokkos/kokkos_launch_compiler
                                    -DKokkos_NVCC_WRAPPER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper
                                    -DCMAKE_PREFIX_PATH=${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE}
	BUILD_COMMAND   ${CMAKE_COMMAND} -E env PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig make -j${NCORES}
  INSTALL_COMMAND make install
)

# If not defined on the command line, switch off apps testing
if(NOT DEFINED Apps_BUILD_TESTS)
  set(Apps_BUILD_TESTS OFF)
else()
  # Update git submodules
  execute_process(
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif()
message ("${LIBRA_ID} Apps_BUILD_TESTS=${Apps_BUILD_TESTS}")
message ("Pulling in the tests data from libra-tests as a submodule")
# Add Libra/apps as an external project
ExternalProject_Add(
    Apps
    SOURCE_DIR      ${CMAKE_SOURCE_DIR}/apps
    DOWNLOAD_COMMAND ""
    BINARY_DIR      ${BUILD_DIR}/Libra/apps
    DEPENDS         Casacpp Hpg Parafeed Casacore Pybind11 GTest
    CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                    -DCMAKE_INSTALL_LIBDIR=lib
                    -DCMAKE_CXX_FLAGS=-I${INSTALL_DIR}/include
                    -Dcasacore_DIR=${INSTALL_DIR}/lib/cmake/casacore
                    -DKokkos_COMPILE_LAUNCHER=${INSTALL_DIR}/bin/kokkos/kokkos_launch_compiler
                    -DKokkos_NVCC_WRAPPER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper
		                -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
                    -DTOP_LEVEL_DIR=${TOP_LEVEL_DIR}
    CONFIGURE_COMMAND PKG_CONFIG_PATH=${INSTALL_DIR}/lib/pkgconfig CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos 
                                      ${CMAKE_COMMAND} <SOURCE_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_CXX_FLAGS=-I${INSTALL_DIR}/include -Dcasacore_DIR=${INSTALL_DIR}/lib/cmake/casacore 
		                                  -DCMAKE_PREFIX_PATH=${INSTALL_DIR}/lib/cmake/Hpg:${INSTALL_DIR}/lib/cmake/parafeed:${INSTALL_DIR}/lib/cmake/Kokkos:${INSTALL_DIR}/lib/cmake/GTest -DApps_BUILD_TESTS=${Apps_BUILD_TESTS} 
                                      -DCMAKE_BUILD_TYPE=${CASA_BUILD_TYPE} -DKokkos_COMPILE_LAUNCHER=${INSTALL_DIR}/bin/kokkos/kokkos_launch_compiler -DKokkos_NVCC_WRAPPER=${INSTALL_DIR}/bin/kokkos/nvcc_wrapper
                                      #-DINSTALL_DIR=${INSTALL_DIR} -DTOP_LEVEL_DIR=${TOP_LEVEL_DIR}
    BUILD_COMMAND   make -j${NCORES}
    INSTALL_COMMAND make install
)

# Add Libra/frameworks as an external project
ExternalProject_Add(
  Frameworks
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/frameworks
  DOWNLOAD_COMMAND ""
  BINARY_DIR      ${BUILD_DIR}/Libra/frameworks
  DEPENDS         Apps
  CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                  -DCMAKE_INSTALL_LIBDIR=lib
                  -DTOP_LEVEL_DIR=${TOP_LEVEL_DIR}
                  -DINSTALL_DIR=${INSTALL_DIR}
  CONFIGURE_COMMAND ${CMAKE_COMMAND} <SOURCE_DIR> -B <BINARY_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DTOP_LEVEL_DIR=${TOP_LEVEL_DIR} -DINSTALL_DIR=${INSTALL_DIR}
  BUILD_COMMAND   make -j${NCORES} -C <BINARY_DIR>
  INSTALL_COMMAND make install -C <BINARY_DIR>
)

# Add Libra/scripts as an external project
ExternalProject_Add(
  Scripts
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/scripts
  DOWNLOAD_COMMAND ""
  BINARY_DIR      ${BUILD_DIR}/Libra/scripts
  DEPENDS         Frameworks
  CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR}
                  -DCMAKE_INSTALL_LIBDIR=lib
                  -DTOP_LEVEL_DIR=${TOP_LEVEL_DIR}
                  -DINSTALL_DIR=${INSTALL_DIR}
  CONFIGURE_COMMAND ${CMAKE_COMMAND} <SOURCE_DIR> -B <BINARY_DIR> -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DTOP_LEVEL_DIR=${TOP_LEVEL_DIR} -DINSTALL_DIR=${INSTALL_DIR}
  BUILD_COMMAND   make -j${NCORES} -C <BINARY_DIR>
  INSTALL_COMMAND make install -C <BINARY_DIR>
)

if (LIBRA_USE_EXODUS)
  # setup a python vitual environment inside the install directory
  # after checking if python3 is installed
  find_package(Python3 COMPONENTS Interpreter Development)
  #if (NOT Python3_FOUND)
  #  message(FATAL_ERROR "Python3 not found. Please install python3 to use exodus")
  # else()
  #  message("${LIBRA_ID} Python3 found at ${Python3_EXECUTABLE}")
  #  message("${LIBRA_ID} Python3 version: ${Python3_VERSION}")
  message("Setting up librapython venv")
    execute_process(
      COMMAND python3 -m venv ${CMAKE_SOURCE_DIR}/install/librapython
      COMMAND source ${CMAKE_SOURCE_DIR}/install/librapython/bin/activate
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/install
     
    ) 
  # Add exodus as an external project install the python environment
  if (LIBRA_EXODUS_BUNDLE_PATH STREQUAL "")
    set(LIBRA_EXODUS_BUNDLE_PATH ${CMAKE_SOURCE_DIR}/install/bin)
  endif()
  set(EXODUS_APPS "roadrunner,hummbee,coyote,chip,dale,dataiter,mssplit,subms,tableinfo,acme")
    ExternalProject_Add(
      Exodus
      GIT_REPOSITORY ${LIBRA_EXODUS_GIT_REPOSITORY}
      GIT_TAG        ${LIBRA_EXODUS_GIT_TAG}
      SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dependencies/exodus
      CONFIGURE_COMMAND python3 -m venv ${CMAKE_SOURCE_DIR}/install/librapython && source ${CMAKE_SOURCE_DIR}/install/librapython/bin/activate
      BUILD_COMMAND PYTHONUSERBASE=${CMAKE_SOURCE_DIR}/install/librapython ${CMAKE_SOURCE_DIR}/install/librapython/bin/pip3 install --user ${CMAKE_SOURCE_DIR}/dependencies/exodus/
      INSTALL_COMMAND ${CMAKE_SOURCE_DIR}/install/librapython/bin/exodus ${CMAKE_SOURCE_DIR}/install/bin/{${EXODUS_APPS}} -o ${LIBRA_EXODUS_BUNDLE_PATH}/${LIBRA_EXODUS_BUNDLE_NAME}
    )
  # pip install exodus into the virtual environment
    #set a list of exodus apps in a bash variable to install
    execute_process(
      COMMAND ${CMAKE_SOURCE_DIR}/install/librapython/bin/exodus 
    )
#  endif()
endif()

include_directories(
  ${INSTALL_DIR}/include
  ${INSTALL_DIR}/include/casacore
  ${INSTALL_DIR}/include/casacpp
  /usr/include/cfitsio
  ${CMAKE_SOURCE_DIR}/apps/src
  ${CMAKE_SOURCE_DIR}/src
  ${INSTALL_DIR}/include/pybind11)

link_directories(/usr/local/lib
  /usr/lib64
  /usr/lib
  /usr/lib/x86_64-linux-gnu
  ${INSTALL_DIR}/lib)
