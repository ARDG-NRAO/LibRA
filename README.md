# LibRA

## Description
This is the repository for the LibRA project to develop a library of algorithms used for Radio Astronomy (RA) interferometric calibration and imaging.  The intent is that such a library can be treated as a third-party library and usable in other packages.  The API will be C++ STL based with the intention of it being easily usable in the RA community in general and for indirect imaging in other fields (like some medical imaging techniques). A containerized means of building the LibRA project is available [here](https://gitlab.nrao.edu/ardg/libra-containers).

The `main` branch of this project is also mirrored [here](https://github.com/ARDG-NRAO/LibRA).

The repository includes

- [ ] a suite of standalone applications (apps) that link to the current incarnation of the said library, and
- [ ] a build system to build the apps and all the required dependencies other than the System Requirements below.

## System requirements
The following come default with RHEL8 or similar systems:

- [ ] GCC 8.x or later
- [ ] cmake 3.x or later
- [ ] git 2.0 or later, gcc-gfortran gtest-devel ccache

The following need to be installed as below:

- [ ] ```dnf -y install {readline,ncurses,blas,lapack,cfitsio,fftw,wcslib,gsl,eigen3}-devel ```

- [ ] An installation of the appropriate version of CUDA  is also required for GPU support in the ```roadrunner``` app.  This dependence is limited to the ```Kokkos``` and ```HPG``` libraries below.

## Getting started

A clone of this repository will get the ```apps/src``` directory with the source code for the standalone application, and the ```makefile.libra``` to compile this code and all other dependencies.  For now, follow the following sequence of commands to clone and build the system:

```
git clone gitlab@gitlab.nrao.edu:sbhatnag/libra.git
cd libra
make -f makefile.libra init
make -f makefile.libra allclone
make Kokkos_CUDA_ARCH=<ARCH_NAME from Kokkos web page https://kokkos.github.io/kokkos-core-wiki/keywords.html#keywords-arch> -f makefile.libra allbuild
# E.g., the following command to build with support for AMPERE family of NVIDIA GPUs at CUDA Capability 80
# make Kokkos_CUDA_ARCH=Kokkos_ARCH_AMPERE80 -f makefile.libra allbuild
cd apps/build
cmake ..
make
make install
```
If successful, this will produce the following tree:
```
.
├── apps
│   ├── build
│   │   ├── CMakeFiles
│   │   ├── src
│   │   ├── CMakeCache.txt
│   │   ├── cmake_install.cmake
│   │   ├── install_manifest.txt
│   │   └── Makefile
│   ├── install
│   │   ├── hummbee
│   │   ├── hummbee2py.cpython-36m-x86_64-linux-gnu.so
│   │   ├── mssplit
│   │   ├── roadrunner
│   │   ├── roadrunner2py.cpython-36m-x86_64-linux-gnu.so
│   │   ├── subms
│   │   └── tableinfo
│   ├── src
│   │   ├── Hummbee
│   │   ├── interface
│   │   ├── MSSplit
│   │   ├── RoadRunner
│   │   ├── SubMS
│   │   └── TableInfo
│   └── CMakeLists.txt
├── dependencies
│   ├── build
│   │   ├── casacore
│   │   ├── casacpp
│   │   ├── hpg
│   │   ├── kokkos
│   │   ├── libsakura
│   │   └── parafeed
│   ├── install
│   │   └── linux_64b
│   └── src
│       ├── casa6
│       ├── fftw-3.3.10
│       ├── hpg
│       ├── kokkos
│       ├── parafeed
│       ├── py_local_env
│       └── sakura-libsakura-5.1.3
├── test
├── makefile.libra
└── README.md
```

The binary applications will be install in ```apps/install``` directory.

### Setting up the various variables in `makefile.libra`

- [ ] `Kokkos_CUDA_ARCH`: This is set via the commandline as `Kokkos_CUDA_ARCH=<CUDA ARCH>`.  
                          Set it to the value appropriate for the CUDA architecture of the GPU used with the `ftm=awphpg` setting of the `roadrunner` app.  
                          See list of supported values at the [Kokkos web page](https://kokkos.github.io/kokkos-core-wiki/keywords.html).
- [ ] `CASA_BUILD_TYPE`, `CASACORE_BUILD_TYPE`: The `cmake` build-type setting for CASA and CASACore code.
- [ ] `{CASA,FFTW,KOKKOS,HPG,PARAFEED}_VERSION`: Names of the specific version or branch for the various packages.
- [ ] `{CASA,CASACORE_DATA,FFTW,KOKKOS,HPG,PARAFEED,SAKURA}_REPOS`: URL for the repository of the various packages.
- [ ] `PATH`: Set this to include the location of the local CUDA installation.  To build the `LibRA` system a GPU is not necessary, but a CUDA install is necessary.
- [ ] `NCORES`: The number of CPU cores used for compiling.  It is used as `make -j ${NCORES}` in the various `build` targets.

### Resources
- [ ] The [LibRA Singularity Container](https://gitlab.nrao.edu/ardg/libra-containers)
***
