# -*- makefile -*-
#
# Most of the user defined variables are here.  More complicated
# compiler and linker variables are in the makefile.  It is hoped that
# the end-user will need to set up only this file - but again, it is
# only a hope. :-)
#
ARCH         = linux_64b
#
# The instance of CASA pacakge that you want to link to
#
PWD          = $(shell pwd)
CASAHOME     = ${PWD}/../../dependencies
LIBROOT      = $(CASAHOME)/linux_64b/lib
LIBROOT64      = $(CASAHOME)/linux_64b/lib64
#
# This is the home of the stand-alone interface library used for
# embedded user interface (Parafeed --
# http://code.google.com/p/parafeed)
#
CLHOME       = $(CASAHOME)/linux_64b
#
# The home of the Simple CASA Interface package (stand-alone programs
# to drive CASA algorithms).
#
SCIHOME      = /home/dhruva/disk1/sanjay/bin/
SCISRC       = /home/dhruva/disk1/sanjay/Projects/capps/SCI
SCILIBDIR    = $(SCISRC)/src/lib
SCIINCLDIR   = $(SCISRC)/src/include
#
# The CASA distribution used to build the SCI applications (this
# appends to CASAHOME).
#

#CASADISTRO   = casa_active_swig/active/
#CASADISTRO   = casa_active_personal/rhel6_trunk
CASADISTRO   = 
SYNTHESISPKG = $(CASAHOME)
COREPKG      = $(CASADISTRO)
DISPLAYPKG   = $(CASADISTRO)

#
# The CASACore package to link to
#
CASACORENAME = casacore
CASACODENAME = casacpp
CASABASE = casa
WCSPKG       = $(CASAROOT)/$(ARCH)/lib
PGPLOTPKG    = $(CASAROOT)/$(ARCH)/lib
WCSPKG       = /usr/lib64
PGPLOTPKG    = /usr/lib64

#CASA_COMPILER_PATH=/opt/casa/02
#COMPILER_PATH=/opt/rh/devtoolset-4/root/usr/bin
C++          = $(COMPILER_PATH)g++ -L$(LIBROOT)
DEFS64       = -DAIPS_64B -DCASA_STANDALONE
#OPT64        = -m64
USRLIB       = /usr/lib64
X11LIB       = /usr/X11R6/lib
OPT          = -g -O2 $(OPT64)

#---------------------------------------------------------- 
#
# Settings below this should not require twiking.  Well...but have a
# look here if the final linking does not find libraries!
#
SYNTHROOT    = $(SYNTHESISPKG)/$(ARCH)/lib64
COREROOT     = $(CASAHOME)/$(COREPKG)/$(ARCH)
DISPLAYROOT  = $(CASAHOME)/$(DISPLAYPKG)/$(ARCH)

CASAROOT     = $(CASAHOME)/$(CASADISTRO)/$(ARCH)
SCIBIN       = $(SCIHOME)/linux_gnu/bin
