#!/bin/bash
# Copyright (C) 2025
# Associated Universities, Inc. Washington DC, USA.
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.is
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning this should be addressed as follows:
#        Postal address: National Radio Astronomy Observatory
#                        1003 Lopezville Road,
#                        Socorro, NM - 87801, USA
#
# $Id$

#=================================================================================
# This script sets up variables for an out-of-tree build.
# It also assumes that the CMakeLists.txt is in ".."
#
# Following two lines are needed only if the spack env isn't activated
#
# Note that ". <SPACK INSTALL LOCATION>/share/spack/setup-env.sh" is
# still required.  This defines the $SPACK_ROOT (plus some more).
#
#SPACK_ENV_NAME=LibRA_Build_1
#SPACK_ENV=$SPACK_ROOT/var/spack/environments/$SPACK_ENV_NAME

#
# The list of packages required in SPACK are checked-for below.
#
# For CPU-only build, use  "-DLIBRA_ENABLE_CUDA_BACKEND=OFF" as a commandline arg.
# For GPU+CPU build, use  "-DKokkos_CUDA_ARCH_NAME=<GPU ARCH NAME>" as a commandline arg.
#

# Additional commands required on stampede3 before running this script:
# . <PATH_TO_SPACK_PACKAGE>/share/spack/setup-env.sh
# spacktivate -p <LIBRA_ENV_NAME>
# module unload python/3.9.18 intel/24.0 

function in_spack_env() {
    if [[ -v SPACK_ROOT ]]; then
 	return 0; # True is bash!
    else
	return 1; # False in bash!
    fi
}
# This function checks if a given package is installed in the spack env.
function is_installed_in_spack() {
if in_spack_env; then
    if spack find --format "{name}" "$1" > /dev/null 2>&1; then
	return 1;
    else
	echo "$1 "
    fi
# else
#     echo "is_installed_in_spack: Not in an SPACK env.";
fi
}

export CMAKE=`which cmake`

#
# Setup and checks for build in an SPACK env
#
if in_spack_env; then
    # $SPACK_ENV gets defined in an activated env.
    SPACK_ENV_ROOT=$SPACK_ENV/.spack-env/view/

    #---------------------------------------------------------
    echo "Checking required packages in SPACK ($SPACK_ROOT)"...
    #
    # Required dependencies
    #
    tt=$(is_installed_in_spack "cfitsio")
    tt+=$(is_installed_in_spack "wcslib")
    tt+=$(is_installed_in_spack "gsl")
    tt+=$(is_installed_in_spack "ncurses")
    tt+=$(is_installed_in_spack "readline")
    tt+=$(is_installed_in_spack "cmake")

    if [[ -z "$tt" ]]; then
	echo -n "";
    else
	echo "###Error: Following required dependencies not found: "$tt;
	exit;
    fi

    #---------------------------------------------------------
    # Maybe required
    #
    tt=$(is_installed_in_spack "openblas")
    tt+=$(is_installed_in_spack "python")

    if [[ -z "$tt" ]]; then
	echo -n "";
    else
	echo;
	echo "WARNING: Following dependencies not found but may be required later in the build process: "$tt;
	echo;
    fi
    #
    #---------------------------------------------------------
    #
    export GSL_ROOT=$(spack location -i gsl)
    export WCSLIB_ROOT=$(spack location -i wcslib)

    export LD_LIBRARY_PATH=/lib64:/lib:$SPACK_ENV_ROOT/lib64:$SPACK_ENV_ROOT/lib:$LD_LIBRARY_PATH

    export CMAKE=$(spack location -i cmake)/bin/cmake
fi

#
# NOTE: HPG does not build with the GCC 11.4 and CUDA 12.x combo
#       GCC 11.4 with CUDA 11.x works.
#       Not clear what other combinations work/not work.
#
export CXX=/usr/bin/g++
export CC=/usr/bin/gcc
export FC=/usr/bin/gfortran
# GNU configure script uses the env variable F77 (for building FFTW package)
export F77=${FC}

# 
# Use -DLIBRA_ENABLE_CUDA_BACKEND=OFF for CPU-only build.
# Use -DKokkos_CUDA_ARCH_NAME=<GPU ARCH NAME> to build for GPU and CPU.
#
$CMAKE -S .. -B ../build -DCMAKE_CXX_COMPILER=${CXX} -DCMAKE_C_COMPILER=${CC} -DCMAKE_Fortran_COMPILER=${FC} "$@"
$CMAKE --build ../build
