project("LibRA Apps"
  DESCRIPTION "LibRA CL, C/C++, Py Interfaces"
  HOMEPAGE_URL https://github.com/ARDG-NRAO/LibRA
  LANGUAGES CXX Fortran C)

cmake_minimum_required(VERSION 3.12.0)
set (CMAKE_CXX_STANDARD 11)

include(FetchContent)
set(DEFS64 "-DAIPS_64B -DCASA_STANDALONE")
set(OPT "-g -O2 ")

set(BUILD_TYPE "RelWithDebInfo")

set(ARCH "linux_64b")
set(INSTALLDIR "install/${ARCH}")
set(DEPDIR "${CMAKE_CURRENT_LIST_DIR}/../dependencies")
set(DEPINC "${DEPDIR}/${INSTALLDIR}/include")
set(DEPLIB "${DEPDIR}/${INSTALLDIR}/lib")
set(USRLIB "/usr/lib64")

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




set(APP_LINK_LIBRARIES casacpp_synthesis casacpp_msvis hpg kokkoscore casa_msfits casa_ms casa_images casa_coordinates casa_lattices casa_fits casa_measures casa_tables casa_scimath casa_scimath_f casa_casa pthread parafeed readline history termcap m c)
include_directories(${DEPINC} ${DEPINC}/casacore ${DEPINC}/casacpp /usr/include/cfitsio ${CMAKE_CURRENT_LIST_DIR}/src)
link_directories(${DEPLIB} /usr/local/lib  ${USRLIB})

# for runtime linking. Needs to be before add_executable
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${DEPLIB};${CMAKE_SOURCE_DIR}/install")
set(CMAKE_BUILD_TYPE "${BUILD_TYPE}")


# add and build each APP
set(APP_LIST Coyote Hummbee RoadRunner MSSplit SubMS TableInfo acme)
foreach(APP IN LISTS APP_LIST)
   set(APP_DIR ${APP})
   string(TOLOWER ${APP_DIR} APP_NAME)
   list(APPEND APP_SOURCES "${APP_DIR}/${APP_NAME}.cc")
endforeach()

add_subdirectory(src)


# add and build the wrapper
# OLD CODE
#set(BUILD_CLWRAPPER ON)
#if(BUILD_CLWRAPPER)
#  add_subdirectory(src/interface/cl_wrapper)
#endif()

# add and build the wrapper
# OLD CODE
#set(BUILD_PYWRAPPER ON)
#if(BUILD_PYWRAPPER)
#  add_subdirectory(src/interface/python_wrapper)
#endif()

# add and build googletest
if(Apps_BUILD_TESTS)
  add_subdirectory(src/tests)
endif()

