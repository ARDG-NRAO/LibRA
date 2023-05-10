include_directories(${CLHOME}/include ${COREROOT}/include/casacore ${CASAROOT}/include/casacpp /usr/include/cfitsio ${CMAKE_SOURCE_DIR}/src)
link_directories(${CLHOME}/lib ${CASAHOME}/linux_64b/lib /usr/local/lib ${LIBROOT} ${COREROOT}/lib ${USRLIB})

set(TABLEINFO_SOURCES "tableinfo.cc")

add_executable(tableinfo ${TABLEINFO_SOURCES})
target_link_libraries(tableinfo casacpp_synthesis casacpp_msvis hpg kokkoscore casa_msfits casa_ms casa_images casa_coordinates casa_lattices casa_fits casa_measures casa_tables casa_scimath casa_scimath_f casa_casa pthread parafeed readline history termcap m c)
install(TARGETS tableinfo DESTINATION "${CMAKE_SOURCE_DIR}/install") 