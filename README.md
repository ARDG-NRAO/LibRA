# LibRA

## Description
This is the repository for the LibRA project to develop a library of algorithms used for Radio Astronomy (RA) interferometric calibration and imaging.  The intent is that such a library can be treated as a third-party library and usable in other packages.  The API will be C++ STL based with the intention of it being easily usable in the RA community in general and for indirect imaging in other fields (like some medical imaging techniques).

The repository includes a suite of standalone applications and a build system to build all the required dependencies other than the System Requirements below.


## System requirements
The following come default with RHEL8 or similar systems:

- [ ] GCC 8.x or later
- [ ] cmake 3.x or later
- [ ] git gcc-gfortran gtest-devel ccache

The following need to be installed as below:

- [ ] ```dnf -y install {readline,ncurses,blas,lapack,cfitsio,fftw,wcslib,gsl,eigen3}-devel ```


## Getting started

A clone of this repository will get the ```apps/src``` directory with the source code for the standalone application, and the ```makefile.libra``` to compile this code and all other dependencies.  For now, follow the following sequence of commands to clone and build the system:

```
git clone gitlab@gitlab.nrao.edu:sbhatnag/libra.git
cd libra
make -f makefile.libra allclone
make -f makefile.libra allbuild
cd apps/src
make -f makefile.casacore roadrunner tableinfo mssplit
```
If successful, this will produce the following tree:
```
.
├── apps
│   └── src
│       ├── bosque
│       ├── RoadRunner
│       ├── makefile.casacore
│       ├── makefile.casacore.inc.dhruva
│       ├── mssplit.cc
│       ├── roadrunner.cc
│       └── tableinfo.cc
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
├── test
├── makefile.libra
└── README.md
```

The binary applicaions will be in ```apps/src``` directory.

### Setting up the various variables in the `makefile`

Watch this space...

## Collaborate with your team

- [ ] [Invite team members and collaborators](https://docs.gitlab.com/ee/user/project/members/)
- [ ] [Create a new merge request](https://docs.gitlab.com/ee/user/project/merge_requests/creating_merge_requests.html)
- [ ] [Automatically close issues from merge requests](https://docs.gitlab.com/ee/user/project/issues/managing_issues.html#closing-issues-automatically)
- [ ] [Enable merge request approvals](https://docs.gitlab.com/ee/user/project/merge_requests/approvals/)
- [ ] [Automatically merge when pipeline succeeds](https://docs.gitlab.com/ee/user/project/merge_requests/merge_when_pipeline_succeeds.html)

## Test and Deploy

Use the built-in continuous integration in GitLab.

- [ ] [Get started with GitLab CI/CD](https://docs.gitlab.com/ee/ci/quick_start/index.html)
- [ ] [Analyze your code for known vulnerabilities with Static Application Security Testing(SAST)](https://docs.gitlab.com/ee/user/application_security/sast/)
- [ ] [Deploy to Kubernetes, Amazon EC2, or Amazon ECS using Auto Deploy](https://docs.gitlab.com/ee/topics/autodevops/requirements.html)
- [ ] [Use pull-based deployments for improved Kubernetes management](https://docs.gitlab.com/ee/user/clusters/agent/)
- [ ] [Set up protected environments](https://docs.gitlab.com/ee/ci/environments/protected_environments.html)

***
