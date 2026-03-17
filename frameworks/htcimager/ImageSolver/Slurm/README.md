# Image Solver implementation for SLURM

## Description

The image solver for SLURM is implemented in Python 3 and uses `subprocess.Popen()` to call `sbatch` for each imaging component. The application provides functionality to partition MS and CFCache by Spectral Window (SPW), and is also a simple workflow manager as it controls the iterative imaging cycle loop and checks for convergence or divergence before submitting jobs for the next iteration or running the restore step to finalize the workflow execution.

## Usage

Imaging workflows in SLURM are run by executing `imageSolver.py` with the combination of arguments that will produce the desired imaging results. Because `imageSolver.py` runs during all the time that the imaging workflow is executing, it is recommended to use `runImageSolver`, which is a wrapper for `imageSolver.py` that uses `nohup` to detach the process from the terminal.

A usage summary for `imageSolver.py` and `runImageSolver` can be obtained by running:

```
$ imageSolver.py -h
```

or

```
$ runImageSolver
```

And both commands above will produce the same output (actually the ouput of `imageSolver.py`), as follows (notice that `runImageSolver` will always save the output to `imageSolver.out`, including the help output):

```
Name:
    imageSolver.py - configure and run image solver on a SLURM cluster

Usage:
    imageSolver.py [-p] [-g] [-h] [-f <parameter file>] [-q <queue name>]
                   [--gridMem <gridding Memory request>] [--imdmMem <image Memory request>]
                   [--max_iter <maximum number of imaging cycles>]
                   [--libra_install </path/to/libra/install>]
                   [-l <logdir>]

Parameters:
    -f, --parfile   : Name of parameter file to use for imaging. Default: imageSolver.def
    -g, --gpu       : Use GPUs for gridding jobs. Default: do not use GPUs
    -h, --help      : Print this usage summary and exit.
    -l, --logdir    : Name of directory to save log files. Default: <current directory>/logs_<submit date>
    -p, --partition : Partition input MS and CFCache by SPW. Default: do not partition
    -q, --queue     : Name of the queue (SLURM partition) to submit jobs to. Default: not specified.
    --max_iter      : Maximum number of imaging cycle iterations. Default: 20
    --gridMem       : Memory footprint of gridding operations. Default: 10G (corresponds to 10 GB)
    --imdmMem       : Memory footprint of image domain operations. Default: 10G (same as above)
    --libra_install : Path to LibRA installation (binaries) directory (if different from /Users/fmadsen/Documents/NRAO/ARDG/libra/install).
```

