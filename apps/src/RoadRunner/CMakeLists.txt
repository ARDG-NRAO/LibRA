set(APP_SOURCES "${APP_NAME}.cc")
set (CMAKE_CXX_STANDARD 11)
set(OPT "-g -O2 $(python3.8-config --includes) \
$(python3.8 -m pybind11 --includes) ")
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

add_executable(${APP_NAME} ${APP_SOURCES})
find_package(pybind11 REQUIRED)
pybind11_add_module(roadrunner2py roadrunner2py.cc)
target_link_libraries(${APP_NAME} ${APP_LINK_LIBRARIES})
install(TARGETS ${APP_NAME} DESTINATION "${CMAKE_SOURCE_DIR}/install") 
