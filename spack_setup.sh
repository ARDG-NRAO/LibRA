
# Some user-defined variables
LIBRA_HOME=$WORK/Packages/LibRA/libra
SPACK_INSTALL_HOME=$WORK/Packages
CUDA_ARCH="HOPPER90"
SPACK_ENV_NAME="LibRA_Build_1"
#
# SPACK install command, if ncessary
#
# git clone -c feature.manyFiles=true --depth=2 https://github.com/spack/spack.git $SPACK_INSTALL_HOME/spack
# git clone -c feature.manyFiles=true --depth=2 --branch=releases/v0.23 https://github.com/spack/spack.git $SPACK_INSTALL_HOME/spack

. $SPACK_INSTALL_HOME/spack/share/spack/setup-env.sh
spack env create ${SPACK_ENV_NAME}

#
# SPACK_ROOT gets defined in setup-env.sh above.
# The only additional env var that SPACK env activation defines is SPACK_ENV.
# If we build this by-hand as below, actiation isn't necessary (but is also harmless).
#
spacktivate -p ${SPACK_ENV_NAME}
SPACK_ENV=$SPACK_ROOT/var/spack/environments/$SPACK_ENV_NAME
SPACK_ENV_ROOT=$SPACK_ENV/.spack-env/view/

spack install --add --reuse cuda ncurses@6.5 gsl@2.8 cmake@3.29.6 termcap@1.3.1 readline@8.2 wcslib cfitsio fftw

#NVHPC=/home1/apps/nvidia/Linux_aarch64/24.7/compilers/lib
#for p in gsl cuda readline wcslib cfitsio fftw; do
#    export LD_LIBRARY_PATH=`spack location -i $p`/lib:$LD_LIBRARY_PATH
#done

export LD_LIBRARY_PATH=/lib64:/lib:$SPACK_ENV_ROOT/lib64:$SPACK_ENV_ROOT/lib:$LD_LIBRARY_PATH

CUDALIB=$SPACK_ENV_ROOT/lib64
CUDASTUBS=$SPACK_ENV_ROOT/lib64/stubs
ln -s $CUDASTUBS/libcuda.so $CUDALIB/libcuda.so.1

export CC=/bin/gcc
export CXX=/bin/g++

cd $LIBRA_HOME/build

#
#Copy package .pc files to <LIBRAHOME>/install/lib/pkgconfig
#
LIBRA_PKGCONFIG=$LIBRA_HOME/install/lib/pkgconfig
mkdir -p $LIBRA_PKGCONFIG
for p in gsl wcslib cfitsio fftw readline ncurses; do
    cp `spack location -i $p`/lib/pkgconfig/*.pc $LIBRA_PKGCONFIG
done

sh ../scripts/fullbuild.sh $CUDA_ARCH "-DNCORES=50 -DCMAKE_CXX_COMPILER=/bin/g++ -DCMAKE_C_COMPILER=/bin/gcc"

#
# To run a node with the GPU and libcuda.so.1 in /lib64 or /lib, following should be sufficient to run the LibRA apps
#
export LD_LIBRARY_PATH=/lib64:/lib:$SPACK_ENV_ROOT/lib64:$SPACK_ENV_ROOT/lib:$LD_LIBRARY_PATH
