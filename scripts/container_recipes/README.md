
[![Podman Image CI](https://github.com/ARDG-NRAO/libra-containers/actions/workflows/docker-img.yml/badge.svg)](https://github.com/ARDG-NRAO/libra-containers/actions/workflows/docker-img.yml)

# LibRA Singularity Container

The goal of this repo is to enable the production of a singularity based minimum dependency  build of the `LibRA` applications for development and testing across different NVIDIA GPUs and for easier shared development. At this juncture it is assumed that you have `singularity` or alternatively `apptainer` installed on your machine of choice. If not please contact your sysadmin or refer to the ample documentation available online [here](https://docs.sylabs.io/guides/3.11/user-guide/index.html) or for `apptainer` [here](https://apptainer.org/docs/admin/main/installation.html). Running `singularity` or `apptainer` in a privileged mode for the general user also requires that you be added to the corresponding user group, make sure your user id belongs that group before you proceed any further. To be able to use `--fakeroot` you need to setup `/etc/subuid` or  `/etc/subgid` as 
```
foo:100000:65536
```
where say `foo` is your username.
[[_TOC_]]

## Building the container

### Building from prebuilt containers
The container files shown here are built and pushed to [dockerhub](https://hub.docker.com/repository/docker/pjaganna/libra/tags?page=1&ordering=last_updated) for a variety of NVIDIA GPU architectures. If you would like to pull and use and try out `LibRA` you can do it as follows. 
```
singularity build libra.sif docker://pjaganna/libra:volta70 
```

if using `apptainer` please replace `singularity` in the command to apptainer. The containers can also be built and used in `docker` or `podman` (My personal preference for unprivileged user building) as 
```
docker pull pjaganna/libra:volta70
```
This will result in a container called `libra` with the `volta70` as a tagname. The same is possible with `podman` as a drop in replacement for `docker`.

### Building from the container files
If you are going to use the container or build repeatedly please set the SINGULARITY_CACHEDIR variable in your bashrc to a path that is not in your user area. This will ensure that the build does not fail due to the user quota limits. The [libra](https://gitlab.nrao.edu/sbhatnag/libra) project relies on [kokkos](https://github.com/kokkos/kokkos/) which has device specific compile directives. So the hardware on which you are going to utilize the container will define the flag in the makefile. The default flag is for the new generation of ampere GPUs like the A4000 and so is set to kokkos_ARCH_AMPERE86. Alter it according to the compute archiecture of your GPU. Here is a table for easy reference.

|min CUDA Version|kokkos FLAG|GPU Model|
|--- |--- |--- |
|11.1|kokkos_ARCH_AMPERE86|A4000, A5000, A6000, RTX30X0|
|11.1|kokkos_ARCH_AMPERE80|A100|
|10.0|kokkos_ARCH_TURING75|RTX20X0, QUADRO 4000 - QUADRO 8000|
|9.0|kokkos_ARCH_VOLTA70|V100|
|8.0|kokkos_ARCH_PASCAL61|GTX10X0|
|8.0|kokkos_ARCH_PASCAL60|P100|

In order to build the LibRA container for development so you can make all your edits before shipping out for testing we prefer the `--sandbox` method. In order to build a `sandbox` which is essentially a linux container as a local folder, you can run the following.

```
singularity build --sandbox --fakeroot --fix-perms my_container_folder libra-cuda-12.2.0-devel-rockylinux8_readonly.def
```
The `--fakeroot` flag allows you root access within the container which we need to install the dependencies for development. The `--fix-perms` will allow you for you to remove the directory structure without needing higher privileges (sudo/su).  T

## Running the code from within the container
The application can be used as an app or via the commandline. In order to access the application via commandline you can launch a `shell` inside the container as follows. This allows you to edit code and build the application once again should you need it
```
singularity shell --writable --fakeroot my_container_folder.
```

The `--fakeroot` is only needed if you want to add addtional packages you might need (such as an editor of choice). This will alter your prompt and should look like 
```
Singularity>
```

Within this shell you should be able to launch the applications such as `roadrunner` with the command, however to bind the system nvidia runtime libraries you need to start a singularity shell as follows
```
singularity shell --nv my_container_folder
Singularity> /libra/apps/src/roadrunner
```
This should result in a commandline interface to the `roadrunner` application. By default the path from which you launched the container would be bound inside the container. If you want to bind additional paths to run code you can do it as follows
```
singularity shell --nv  --bind host_path:container_mount_point my_container_folder
```
A point of note is that multiple --bind commands can be utilized to bing multiple host paths inside the container.


## CUDA Versions and Compatibility

The version of cuda and the corresponding versions of drivers needed to run the conainers is available [here](https://docs.nvidia.com/cuda/cuda-toolkit-release-notes/index.html).
We have tested and support cuda toolkit verions 11.1 and beyond where the nvidia graphics driver version is >=450.80.02. There toolkit version is specified on the name of each .def file in the repo. Please choose the one appropriate for your graphics card.


