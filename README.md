# LibRA

## Description
The primary goal of the LibRA project is to directly expose algorithms
used in Radio Astronomy (RA) for image reconstruction from
interferometric telescopes.  The primary target users are research groups (R&D groups at observatories, at university departments) and individual researchers (faculty, students, post-docs) who may benefit from a software system with professional-quality implementation of the core algorithms which are also easy to use, deploy locally and modify as necessary.  Therefore, a strong requirement deriving this work is to keep the interface simple, the software stack shallow and software dependency graph simple.

This repository contains _only_ the algorithmically necessary
scientific code, and a build system to compile it into a
library of algorithms.  Such a library can be used as a
third-party library by other in the RA community.  Different interfaces to the library are provided to access the algorithms vis C++ or Python, or as an end-user via standalone applications built to conveniently configure and execute the algorithms from Linux shell.

Interferometric radio telescopes are indirect imaging devices which
collects data in the Fourier domain. Transforming the raw data from
such devices to images requires application of sophisticated
algorithms to reconstruct the image.  The fundamental scientific
principles behind such telescopes are the same as those in other
domains that rely on indirect imaging such as Magnetic Resonance
Imaging (MRI) and Ultrasound imaging.  To make RA algorithms available
for application in such fields and enable cross-discipline R&D, the
API to the library is based on C++ STL for portability and wider use
that does not required RA-specific software stack and dependencies.

### The LibRA software stack

The `src` directory contains the implementation of the basic
calibration and imaging algorithms.  The code has been derived from
the CASA project but contains _only_ the algorithmically-significant
part of the larger CASA code base which can be compiled into a
reusable software library.  This significantly simplifies the software
stack and the resulting software dependency graph. A suite of
standalone applications are also available which can be built as relocatable
Linux executable.  The resulting software stack is shown below.  Figure on the left shows our current software stack where the RA Algorithms layer is built on RA-specificdata access and CASACore layers.  _Work is in progress to decouple the RA Algorithms layer from RA-specific layers with API based on the C++ Standard Template Library (STL)_.  With a translation layer RA-specific libraries (CASACore, RA Data Access/Iterators) may be replaced for use of RA Algorithms in other domains.

[`libparafeed`](https://github.com/sanbee/parafeed.git) in the figures below is a standalone library for embedded user interface used for command-line configuration of LibRA apps.  

[`libhpg`](https://gitlab.nrao.edu/mpokorny/hpg.git) is a standalone library that deploys the compute-intensive calculations for imaging on a GPU or a CPU core (like the resampling of irregular data to and from a regular grid -- a.k.a. "gridding" and "degridding" in RA jargon).  This library is built on the [`Kokkos`](https://github.com/kokkos/kokkos.git) framework for performance portable implemention.

                      Current Stack                                                        Target Stack

![Stack of software layers](doc/figures/RRStack-Libra.png "LibRA software stack")  &nbsp; &nbsp;&nbsp; &nbsp; &nbsp;&nbsp;  &nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
![Stack of software layers](doc/figures/RRStack-Intended-2.png "Target LibRA software stack")


### The repository contains

- [ ] scientific code to build a software library of algorithms for image reconstruction
- [ ] a suite of standalone applications (apps) to configure and trigger the algorithms from commandline, and
- [ ] a build system to build the library of algorithms, the apps and all dependencies, other than the System Requirements below.


A containerized means of building the LibRA project is available
[here](https://gitlab.nrao.edu/ardg/libra-containers).

The `main` branch of this project is also mirrored [here](https://github.com/ARDG-NRAO/LibRA).

### Available of apps
- [ ] `roadrunner` : An application to transform the data in a Measurement Set (MS) to an image.  This can be deployed on a single CPU core, or on a GPU.  This is a.k.a. as the `major cycle` in RA.
- [ ] `hummbee` : An application to derive a model of the signal in the raw image (e.g., made using `roadrunner`).  This is a.k.a. the `minor cycle` in RA.
- [ ] `coyote` : [NOT YET AVAILABLE] An application to build the CF Cache used as input to the `roadrunner` application.
- [ ] `tableinfo` : An application to print summary of the data (MS) and images (information from image headers).
- [ ] `mssplit` : An application to split a data (in the MS format) along various axies of the data domain.  The resulting data can be written as a deep-copy, or as a reference to the input data base.
- [ ] `subms` : Functionally the same as `mssplit` but additionally re-normalizes the sub-tables in the resulting data base.
- [ ] `htclean` : A framework that implements the Algorithm Architecture that uses the apps as algorithmic components.  This, for example, implements the iterative image reconstruction technique widely used in RA that alternates between the `major cycle` and the `minor cycle`.  The execution graph can be deployed as a DAG on a CPU, a GPU, or on a cluster of CPUs/GPUs.  This is currently used to deploy imaging on a local cluster, and on the PATh and OSG clusters.  A varient of it has been used for prototype deployment on AWS.

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
git clone https://github.com/ARDG-NRAO/LibRA.git
cd libra
make -f makefile.libra init
make -f makefile.libra allclone
make Kokkos_CUDA_ARCH=<ARCH_NAME from Kokkos web page https://kokkos.github.io/kokkos-core-wiki/keywords.html#keywords-arch> -f makefile.libra allbuild
# E.g., the following command to build with support for AMPERE family of NVIDIA GPUs at CUDA Capability 80
# make Kokkos_CUDA_ARCH=Kokkos_ARCH_AMPERE80 -f makefile.libra allbuild
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
                          See list of supported values at the [Kokkos web page](https://kokkos.github.io/kokkos-core-wiki/keywords.html#keywords-arch).
- [ ] `CASA_BUILD_TYPE`, `CASACORE_BUILD_TYPE`: The `cmake` build-type setting for CASA and CASACore code.
- [ ] `{CASA,FFTW,KOKKOS,HPG,PARAFEED}_VERSION`: Names of the specific version or branch for the various packages.
- [ ] `{CASA,CASACORE_DATA,FFTW,KOKKOS,HPG,PARAFEED,SAKURA}_REPOS`: URL for the repository of the various packages.
- [ ] `PATH`: Set this to include the location of the local CUDA installation.  To build the `LibRA` system a GPU is not necessary, but a CUDA install is necessary.
- [ ] `NCORES`: The number of CPU cores used for compiling.  It is used as `make -j ${NCORES}` in the various `build` targets.

### Resources
- [ ] The [LibRA Singularity Container](https://gitlab.nrao.edu/ardg/libra-containers)
***
