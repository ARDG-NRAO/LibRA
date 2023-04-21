# LibRA

## Description
This is the repository for the LibRA project to develop a library of algorithms used for Radio Astronomy (RA) interferometric calibration and imaging.  The intent is that such a library can be treated as a third-party library and usable in other packages.  The API will be C++ STL based with the intention of it being easily usable in the RA community in general and for indirect imaging in other fields (like some medical imaging techniques).

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
make -f makefile.libra allclone
make Kokkos_CUDA_ARCH=<ARCH_NAME from Kokkos web page https://kokkos.github.io/kokkos-core-wiki/keywords.html> -f makefile.libra allbuild
cd apps/src
make -f makefile.casacore roadrunner tableinfo mssplit
make -f makefile.casacore hummbee
```
If successful, this will produce the following tree:
```
.
├── apps
│   └── src
│       ├── hummbee
│       ├── Hummbee
│       ├── hummbee.cc
│       ├── makefile.casacore
│       ├── makefile.casacore.inc
│       ├── mssplit
│       ├── mssplit.cc
│       ├── roadrunner
│       ├── RoadRunner
│       ├── roadrunner.cc
│       ├── subms.cc
│       ├── tableinfo
│       ├── tableinfo.cc
├── dependencies
│   ├── build
│   │   ├── casacore
│   │   ├── casacpp
│   │   ├── hpg
│   │   ├── kokkos
│   │   ├── libsakura
│   │   └── parafeed
│   ├── linux_64b
│   │   ├── bin
│   │   ├── data
│   │   ├── include
│   │   ├── lib
│   │   ├── sbin
│   │   └── share
│   └── src
│       ├── casa6
│       ├── fftw-3.3.10
│       ├── hpg
│       ├── kokkos
│       ├── parafeed
│       └── sakura-libsakura-5.1.3
├── makefile.libra
├── README.md
└── test

```

The binary applications will be in ```apps/src``` directory.

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
- [ ] The [LibRA Singularity Container](https://gitlab.nrao.edu/pjaganna/libra-singularity)
***
