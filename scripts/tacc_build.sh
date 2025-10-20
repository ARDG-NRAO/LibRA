#
# This script sets up variables for an out-of-tree build.
# It also assumes that the CMakeLists.txt is in ".."
#
# Following two lines are needed only if the spack env isn't activated
#SPACK_ENV_NAME=LibRA_Build_1
#SPACK_ENV=$SPACK_ROOT/var/spack/environments/$SPACK_ENV_NAME
#
#
# The list of packages required in SPACK are checked-for below.
#
# For CPU-only build, use  "-DLIBRA_ENABLE_CUDA_BACKEND=OFF" as a commandline arg.
# For GPU+CPU build, use  "-DKokkos_CUDA_ARCH_NAME=Kokkos_ARCH_<GPU ARCH NAME"" as a commandline arg.
#

# Additional commands required on stampede3 before running this script:
# . <PATH_TO_SPACK_PACKAGE>/share/spack/setup-env.sh
# spacktivate -p <LIBRA_ENV_NAME>
# module unload python/3.9.18 intel/24.0 


# This function checks if a given package is installed in the spack env.
function is_installed_in_spack() {
if [[ -v SPACK_ROOT ]]; then
    if spack find --format "{name}" "$1" > /dev/null 2>&1; then
	return 1;
    else
	echo "Package $1 needs to be installed in SPACK";
	exit;
	return 0;
    fi
else
    echo "is_installed_in_spack: Not in an SPACK env.";
fi
}

# $SPACK_ENV gets defined in an activated env.
SPACK_ENV_ROOT=$SPACK_ENV/.spack-env/view/

echo "Checking required packages in SPACK ($SPACK_ROOT)"...
is_installed_in_spack "openblas";
is_installed_in_spack "cfitsio";
is_installed_in_spack "wcslib";
is_installed_in_spack "python";
is_installed_in_spack "gsl";
is_installed_in_spack "ncurses";
is_installed_in_spack "readline";
is_installed_in_spack "cmake";


export GSL_ROOT=$(spack location -i gsl)
export WCSLIB_ROOT=$(spack location -i wcslib)

export CXX=/usr/bin/g++
export CC=/usr/bin/gcc
export FC=/usr/bin/gfortran
export LD_LIBRARY_PATH=/lib64:/lib:$SPACK_ENV_ROOT/lib64:$SPACK_ENV_ROOT/lib:$LD_LIBRARY_PATH
export CMAKE=$(spack location -i cmake)/bin/cmake
#cmake -S . -B build -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_Fortran_COMPILER=/usr/bin/gfortran -DKokkos_CUDA_ARCH_NAME=Kokkos_ARCH_HOPPER90                
#cmake --build build                                                                                                                                                                          
$CMAKE -S .. -B ../build -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=${CC} -DCMAKE_Fortran_COMPILER=${FC} "$@" -DNCORES=24 
$CMAKE --build ../build

