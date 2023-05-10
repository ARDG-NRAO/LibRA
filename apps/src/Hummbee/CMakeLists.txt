include_directories(${CLHOME}/include ${COREROOT}/include/casacore ${CASAROOT}/include/casacpp /usr/include/cfitsio ${CMAKE_SOURCE_DIR}/src)
link_directories(${CLHOME}/lib ${CASAHOME}/linux_64b/lib /usr/local/lib ${LIBROOT} ${COREROOT}/lib ${USRLIB})

set(HUMMBEE_SOURCES "hummbee.cc")

add_executable(hummbee ${HUMMBEE_SOURCES})
target_link_libraries(hummbee casacpp_synthesis casacpp_msvis hpg kokkoscore casa_msfits casa_ms casa_images casa_coordinates casa_lattices casa_fits casa_measures casa_tables casa_scimath casa_scimath_f casa_casa pthread parafeed readline history termcap m c)
install(TARGETS hummbee DESTINATION "${CMAKE_SOURCE_DIR}/install") 