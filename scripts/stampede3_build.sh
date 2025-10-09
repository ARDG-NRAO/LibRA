# This script sets up variables for an out-of-tree build.                                                                                                                                    # It also assumes that the CMakeLists.txt is in ".."                                                                                                                                         #                                                                                                                                                                                            # Following two lines are needed only if the spack env isn't activated                                                                                                                       #SPACK_ENV_NAME=LibRA_Build_1                                                                                                                                                                #SPACK_ENV=$SPACK_ROOT/var/spack/environments/$SPACK_ENV_NAME                                                                                                                                # $SPACK_ENV gets defined in an activated env.                                                                                                                                                
SPACK_ENV_ROOT=$SPACK_ENV/.spack-env/view/

export GSL_ROOT=$(spack location -i gsl)
export WCSLIB_ROOT=$(spack location -i wcslib)

export CXX=/usr/bin/g++
export CC=/usr/bin/gcc
export FC=/usr/bin/gfortran
export LD_LIBRARY_PATH=/lib64:/lib:$SPACK_ENV_ROOT/lib64:$SPACK_ENV_ROOT/lib:$LD_LIBRARY_PATH
export CMAKE=$(spack location -i cmake)/bin/cmake
#cmake -S . -B build -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_Fortran_COMPILER=/usr/bin/gfortran -DKokkos_CUDA_ARCH_NAME=Kokkos_ARCH_HOPPER90                
#cmake --build build                                                                                                                                                                          
$CMAKE -S .. -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=${CC} -DCMAKE_Fortran_COMPILER=${FC} -DLIBRA_ENABLE_CUDA_BACKEND=OFF -DNCORES=24 -S ../build
$CMAKE --build ../build
