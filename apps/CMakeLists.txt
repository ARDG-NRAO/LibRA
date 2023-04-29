#HOST := $(shell $(HOSTNAME))
#include makefile.casacore.inc

project(libra)

cmake_minimum_required(VERSION 3.12.0)

set(DEFS64 "-DAIPS_64B -DCASA_STANDALONE")
set(OPT "-g -O2")


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
           -D_LARGEFILE_SOURCE   \
           -DDBUS_CPP=1  -Dlibcasadbus_EXPORTS"
)                                    



set(ARCH "linux_64b")
#set(SCIHOME "/home/dhruva/disk1/sanjay/bin/")
#set(SCISRC "/home/dhruva/disk1/sanjay/Projects/capps/SCI")
#set(SCILIBDIR "$(SCISRC)/src/lib")
#set(SCIINCLDIR "$(SCISRC)/src/include")
set(CASAHOME "${CMAKE_CURRENT_LIST_DIR}/../dependencies")
set(CLHOME "${CASAHOME}/linux_64b")
set(COREROOT "${CASAHOME}/${ARCH}")
set(CASAROOT "${CASAHOME}/${ARCH}")
set(USRLIB "/usr/lib64")
set(LIBROOT "${CASAHOME}/linux_64b/lib")
set(CMAKE_INSTALL_PREFIX )

#[[INCL = -I.                                   \
       -I$(CLHOME)/include \
       -I$(COREROOT)/include/casacore \
       -I$(CASAROOT)/include/casacpp \
       -I/usr/include/cfitsio                                ]]
       #-I$(CASA_COMPILER_PATH)/include 


#SYNTHESISLIBS = -lcasacpp_synthesis -lcasacpp_msvis \
#                -lhpg -lkokkoscore


#[[CORELIBS = -lcasa_msfits -lcasa_ms -lcasa_images \
            -lcasa_coordinates -lcasa_lattices \
            -lcasa_fits -lcasa_measures \
	    -lcasa_tables -lcasa_scimath -lcasa_scimath_f \
            -lcasa_casa -lpthread]]


#CASALIBS = $(CORELIBS) $(SYNTHESISLIBS) 



#UILIBS = -L$(CLHOME)/lib -lparafeed \
#	 -lreadline -lhistory -ltermcap -lm -lc


#LINKEROPTS = -Xlinker -rpath -Xlinker                       \
	     # /usr/local/lib: \
	     # -Wl,-rpath,$(LIBROOT)
	 

#LIBOPTS = -L$(COREROOT)/lib     \
#	  -L$(USRLIB)                            
                 

#-------------------------------------------------------------------------------------------------
# Variables that control the generic targets below
#
#
# The list of source file names 
#
#SOURCE  = timager.cc tEPJones.cc imasking.cc mssplit.cc tableinfo.cc 
#SOURCE  = imasking.cc mssplit.cc tableinfo.cc subms.cc roadrunner.cc



# collect up the source files
#file(GLOB SOURCE "*.cc")
set(ROADRUNNER_SOURCES "src/roadrunner.cc")


#
# The list of applications to be built derived from the list of source files in $(SOURCE)
#
# APPS    = $(patsubst %.cc,%,$(SOURCE))

include_directories(${CLHOME}/include ${COREROOT}/include/casacore ${CASAROOT}/include/casacpp /usr/include/cfitsio ${CMAKE_CURRENT_LIST_DIR}/src)
message(STATUS)
message("CLHOME/include=${CLHOME}/include")

link_directories(${CLHOME}/lib ${CASAHOME}/linux_64b/lib /usr/local/lib ${LIBROOT} ${COREROOT}/lib ${USRLIB})
#set(PROJECT_LINK_LIBS )

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CASAHOME}/linux_64b/lib")

# create the executables
add_executable(roadrunner ${ROADRUNNER_SOURCES})

#
# The list object files derived from the list of application in $(APPS)
#
#OBJECTS = $(patsubst %,%.o,$(APPS))

#target_include_directories(roadrunner PRIVATE "src/")

target_link_libraries(roadrunner casacpp_synthesis casacpp_msvis hpg kokkoscore casa_msfits casa_ms casa_images casa_coordinates casa_lattices casa_fits casa_measures casa_tables casa_scimath casa_scimath_f casa_casa pthread parafeed readline history termcap m c)
#
#-------------------------------------------------------------------------------------------------
#
# The actual tagets
#
#all: $(APPS)	

#install: all
#	strip $(APPS); 
#find_library(DEPNS_LIBS_PATH 
#  NAMES casacpp_synthesis
#  PATHS ${CASAHOME}/linux_64b/lib)
#message("DEPNS_LIBS_PATH = ${DEPNS_LIBS_PATH}")

#set(CMAKE_INSTALL_RPATH "/home/tesla/mhsieh/work/libra/dependencies/linux_64b/lib")
install(TARGETS roadrunner DESTINATION "${CMAKE_CURRENT_LIST_DIR}/install") 
#install(TARGETS roadrunner RUNTIME DESTINATION "${CMAKE_CURRENT_LIST_DIR}/install")


#$(OBJECTS): %.o: %.cc 
#	 $(C++) -c $(CPPFLAGS) $(INCL) $< -o $@

#$(APPS): % : %.o 
#	 $(C++) -o $@ $< $(CPPFLAGS) $(LINKEROPTS) $(LIBOPTS) \
#	 $(CASALIBS) $(UILIBS) #$(CAAPSLIB)
#
#-------------------------------------------------------------------------------------------------
#
#.PHONY: clean allclean

#depend:
#	makedepend $(INCL) $(SOURCE)
