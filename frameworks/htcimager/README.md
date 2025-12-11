# HTCImager

## Description

HTCImager is a command line utility that abstracts the operations of setup, prestaging and post-processing and executes imaging components of the LibRA algorithm architecture on a variety of computing environments, such as local workstations, High Performance Computing (HPC) clusters and distributed High Throughput Computing (HTC) systems. The tool currently supports the execution of LibRA applications through the command line interface, also provides a simple interface to integrate custom imaging scripts, and is planned to support execution of LibRA applications through the python interface as well as the capability to execute imaging steps using CASA.


## Usage

The utility is executed by running the main program `htcimager.py`, with a series of command line arguments that enable selection of the imaging mode, definition of environment and imaging parameters, and how to access LibRA applications. HTCImager can be used to run a single imaging component, to iterate over multiple imaging components, or to iterate over multiple imaging inputs. This allows implementing imaging workflows in which all individual jobs call the same executable (`htcimager.py`), independent of the imaging mode and processing backend.

A usage summary of `htcimager.py` can be obtained by running:

```
$ htcimager.py -h
```

The above command produces the following output:

```
Name:
    htcimager.py - wrapper to configure environment and execute imaging components
                   of the LibRA algorithm architecture.

Usage:
    htcimager.py -f <parfile> -j <jobmode> -n <jobname> [--nogpu] [-h]
                [-m <ms list>] [-c <cfcache list>] [-i <image list>] [-t <image type>]
                [-w <working directory>] [-l <log directory>]
                [-b </path/to/libra/bundle>] [-p </path/to/libra/install>]
                [--vbbucketsize <vbbcuketsize>] [--interface <interface>]
                [--prescript <prescript>] [--postscript <postscript>] [--command <command>]

Parameters:
    -f, --parfile      : Name of parameter file to use for imaging. Single value or comma-separated list.
    -j, --jobmode      : Job mode to execute. Single value or comma-separated list.
                         Supported values: weight, psf, residual, model, gather, normalize, restore, makepsf, gatherpsf, runresidualcycle, makedirtyimage, runmodelcycle
    -n, --jobname      : A string defining the name of the job. Used for naming parameter files and logs.
    -m, --mslist       : Name of input MS(es). Single value or comma-separated list.
    -c, --cfclist      : Name of CF cache(s). Single value or comma-separated list (length must match mslist)
    -i, --imagename    : Name of output image(s). For gather modes, this becomes the list of input images to gather. Single value or comma-separated list.
    -t, --imtype       : Image type (extension) to normalize (applies only to normalization jobs).
    -w, --workdir      : Name of working directory. Default is <condor scratch directory>/working for HTCondor jobs and <current directory> for local or SLURM jobs.
    -l, --logdir       : Name of directory to save log files. Default is <working directory>/logs.
    -b, --libra_bundle : Name of relocatable bundle containing LibRA applications, more commonly used on HTCondor jobs.
    -p, --libra_path   : Path to local LibRA installation, more commonly used with SLURM or local jobs.
    -h, --help         : Print this usage summary and exit.
    --nogpu            : Do not use GPUs for processing. Default is to use GPUs for gridding.
    --vbbucketsize     : Size of visibility buffer for gridding jobs.
    --interface        : Name of imaging interface to use. Supported values: libra_cl, custom.
    --prescript        : Command line to execute script before processing (optional, only for interface = custom). 
    --postscript       : Command line to execute script after processing (optional, only for interface = custom).
    --command          : Command line to execute processing application (only for interface = custom).
```


## Image Solver implementations

Implementations of image solver for single-term continuum using `htcimager.py` are provided in this repository for use with SLURM and HTCondor, respectively in directories `ImageSolver/Slurm` and `ImageSolver/HTCondor`. Refer to the `README.md` files in those locations for detailed usage information.