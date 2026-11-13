# Image Solver implementation for HTCondor

## Description

The image solver for HTCondor is implemented in Python 3 and uses the [HTCondor Python Bindings](https://htcondor.readthedocs.io/en/latest/apis/python-bindings/index.html) to create and submit imaging workflows consisting of DAGs representing the data flow, and jobs that execute each imaging component. The application provides functionality to perform continuum imaging, either single- or multi-term, checking for convergence or divergence at the end of every iterative imaging cycle, signaling the DAGMan workflow manager to submit jobs for the next iteration, or run the restore step and finalize workflow exection.


## Usage

Imaging workflows in HTCondor are run by executing `imageSolver.py` on the HTCondor Access Point (AP), with an input parameter file that can either be in JSON format (preferred, full functionality), or a simple text file (compatible only with single-term imaging). Refer to section [Structure and format of parameter files](#structure-and-format-of-the-parameter-files) for more information about the parameter file formats and structure.

There are a few command line arguments that can be used with `imageSolver.py`, to enable specific functionality. A usage summary can be obtained by running:

```
$ imageSolver.py -h
```

The usage summary obtained by running this command is:

```
usage: imageSolver.py [-h] [-d] [-r] [-f] [-c LASTCYCLE] parfile

Writes and submits an HTCondor imaging workflow.

positional arguments:
  parfile               name of the input parameter file. Can be a JSON or text file with parafeed syntax.

optional arguments:
  -h, --help            show this help message and exit
  -d, --debug           add print statements to help with debugging.
  -r, --dryrun          do not submit, only create workflow submit description files.
  -f, --force           force new submission.
  -c LASTCYCLE, --lastcycle LASTCYCLE
                        if restarting a run, the index of the last completed imaging cycle before restart
```

To run an imaging workflow, create a working directory (usually in your home directory) where HTCondor will store logs, runtime outputs (stdout and stderr) and submit description files. For a given project name, run:

```
$ mkdir -p ~/project/name/bin
```

It is important to also create the `bin` directory, because `imageSolver.py` must run from that directory, where the parameter file must also be located. Make a copy of the template parameter file, `imageSolver.json`, and edit according to the imaging parameters and the instructions for workflow configuration in section [Structure and format of parameter files](#structure-and-format-of-the-parameter-files).

After editing the parameter file, submit the workflow by running:

```
$ imageSolver.py <parameter file>
```

The output of this command indicates the DAG structure of the workflow and the condor job id.

Refer to [HTCondor's documentation on managing jobs](https://htcondor.readthedocs.io/en/latest/users-manual/managing-a-job.html) for detailed reference on how to monitor and manage HTCondor jobs. The recommended command to check workflow progress is:

```
$ condor_q -dag -nobatch
```


## Structure and format of the parameter files

The JSON input parameter file is the preferred input file type and the only one compatible with both single- and multi-term imaging. The JSON parameter files are expected to have the parameters split into categories according to the following structure:

```
{
"HTCondorConfig" : { <parameters that control the HTCondor configuration> },
"GeneralImagingParameters" : { <parameters that apply to all imaging stages> },
"GriddingParameters" : { <parameters that apply exclusively to gridding jobs> },
"DeconvolutionParameters" : { <parameters that apply exclusively to deconvolution jobs> },
"NormalizationParameters" : { <parameters that apply exclusively to normalization jobs>}
}
```

A template parameter file, `imageSolver.json` is available for reference. All the parameters in `HTCondorConfig` must be declared as strings to be correctly converted to HTCondor variables, with a few (indicated in the setup instructions below) having escaped double-quotes (\") in order to be interpreted and stored as strings by HTCondor.

The simple text parameter file input is only compatible with single-term imaging workflows. The file should contain one parameter declaration per line, in the format `<parameter> = <value>`, a format that is directly compatible with HTCondor and Parafeed variable declarations.

Optional parameters that can be removed from the parameter file are indicated in the list below. Parameters that are required in the JSON file are also required in the text format.

**HTCondor configuration parameters:**

> Note: Some of the parameters show default values. Instead of being embedded in the code, these defaults are simply pre-filled in the template parameter file.

* `memResidual` \
    memory request (RAM, not GPU memory) for jobs that compute residual images (also weight and psf). \
    Example: `26 G` for a job that is expected to use 26 GB of RAM.
* `memModel` \
    memory request (RAM) for jobs that process exclusively in the image domain (update model, restore, gather, normalization, psfc2tt, resc2tt, tt2cube). \
    Example: `20 G` for a job that is expected to use 20 GB of RAM.
* `min_disk_residual` \
    minimum disk request for gridding (residual) jobs. If the MS(es) and CFCache(s) are stored in a directory that is directly accessible from the AP (see data origin, below), this value will be added to the sum of MS and CFCache disk sizes to form the disk space requirement. Otherwise, this value will be used as disk space requirement. \
    Example: `10 G` for a job that is expected to use 10 GB of local storage.
* `min_disk_model` \
    local disk request for jobs that process exclusively in the image domain. \
    Example: `50 G` for a job that is expected to use 50 GB of local storage.
* `projectdir` \
    name of the project directory. String used to form workflow related data paths, such as initial directory, image storage, and data source(s). Slashes can be used to represent relative paths. \
    Example: `my/project/name`
* `initialdir` \
    full path of the directory where HTCondor should store logs and products that are not redirected (using `transfer_output_remaps`). This directory is the parent of the `bin` directory mentioned in section (Usage)[#usage], where the parameter file is located and from where the workflow must be submitted. \
    Suggested: `<user's home directory>/$(projectdir)` (Notice the use of the previous parameter to form this path: `projectdir` referenced as an HTCondor variable!)
* `dataorigin` \
    address of the data origin where files are located and/or saved to. \
    Examples:
    * `osdf:///path-facility/data/<username>/` (PATh Facility data origin)
    * `osdf:///nrao-ardg/users/<username>/` (NRAO-ARDG experimental data origin)
* `softwaredir` \
    Location of the LibRA exodus bundle and `casa-data.tgz` (a compressed tarball with the contents of `<casa root>/data` from a local CASA installation), typically on a data origin. The LibRA bundle and CASA-data must be pre-staged in this location before submitting the workflow. \
    Default: `$(dataorigin)/apps`
* `cuda_cap` \
    (Optional) The default value of `libra_bundle` uses this parameter to dynamically select the LibRA bundle (see below). Provide a fixed value to ensure a bundle is transferred to jobs that do not require a GPU.
* `cuda_drv` \
    (Optional) The default value of `libra_bundle` uses this parameter to dynamically select the LibRA bundle (see below). Provide a fixed value to ensure a bundle is transferred to jobs that do not require a GPU.
* `libra_bundle` \
    Name of the relocatable LibRA bundle. If accessing GPUs with different CUDA capabilities and driver versions, the HTCondor variables `$(cuda_cap)` and `$(cuda_drv)` are dynamically set respectively to the (integer) CUDA capability and CUDA Driver Version of the GPU device on the Execution Point (EP). Keep in mind that separate LibRA bundles (builds) have to be provided for different combinations of CUDA capabilities and driver versions. The HTCondor variables can be removed from the bundle name (setting a fixed bundle name) if running the workflow on a homogeneous system where all the GPUs have the same capability and driver version. \
    Default: `libra.$(cuda_cap)x_cuda$(cuda_drv).sh`
* `imagesdir` \
    Location where the workflow jobs write the image products. Usually an OSDF data origin. \
    Default: `$(dataorigin)/imagingdata/$(projectdir)/working`
* `cfcachedir` \
    Location of Convolution Function Cache(s), that must be pre-staged in this location before submitting the workflow. \
    Default: `$(dataorigin)/cfcache/$(projectdir)`
* `msdir` \
    Location of input MS(es), that must be pre-staged in this location before submitting the workflow. \
    Default: `$(dataorgin)/data/$(projectdir)`
* `und_sites_grid` \
    (Optional) Comma-separated list of sites to avoid for gridding jobs. This parameter must be surrounded by escaped double-quotes (\") in the JSON parameter file to be interpreted and stored as a string by HTCondor. \
    Example (JSON): "\"FIU-PATH,UTC-Epyc\""
* `notify_user` \
    (Optional) E-mail address to get notifications about job completion status. Model cycle jobs always send notifications; residual cycle jobs only notify if there are errors.
* `htcimagerdir` \
    Location of `htcimager` files. \
    Default: `<libra root>/frameworks/htcimager`
* `wantOSPool` \
    (Optional) Set to `True` if submitting jobs from the PATh facility AP and expecting to also use opportunistic resources from the Open Science Pool. \
    Default: `False`
* `NRPexclusive` \
    (Optional) Set to `True` if gridding jobs are required to run exclusively on NRP resources. \
    Default: `False`
* `useVLASSGPUs` \
    (Optional) Set to `True` if running jobs on the AOC cluster and jobs are allowed to use the GPUs of the VLASS HTCondor cluster. \
    Default: `False`
* `+SingularityImage` \
    (Optional) Container image to run jobs on. This parameter must be surrounded by escaped double-quotes (\") in the JSON parameter file to be interpreted and stored as a string by HTCondor. \
    Suggested for PATh/OSG/NRP: `"\"/cvmfs/singularity.opensciencegrid.org/opensciencegrid/osgvo-el8:latest\""`
* `disconnected_timeout` \
    (Optional) Maximum time (in seconds) to wait for reconnection of disconnected jobs. Tipically set to 1.5x to 2x the average job duration.
* `min_pelican_version` \
    (Optional) Minimum version of Pelican required on the EP. This parameter is likely to be deprecated soon as Pelican stability increases. The value currently used for testing this software is `"\"7.22\""`, while the latest version is 7.26, and the majority of OSPool is generaly up-to-date with the latest, or no more than one version behind. This parameter must be surrounded by escaped double-quotes (\") in the JSON parameter file to be interpreted and stored as a string by HTCondor.
* `min_gpu_capability` \
    Minimum CUDA Capability of the GPU required on the EP.
* `min_gpu_memory` \
    Minimum GPU Memory required on the EP.
* `min_gpu_driver` \
    Minimum CUDA Driver Version required on the EP.
* `casa` \
    Version of CASA to use for multi-term imaging with the CASA interface that uses MTMFS-via-cube (MVC). The file must be a `.tgz`, which is automatically unpacked by Pelican when transferred to the EP. When developing and testing multiterm functionality, it was necessary to unpack the downloaded CASA tarballs (`.tar.xz`) and repack as `.tgz` in order to transfer and unpack with Pelican. \
    Suggested: `casa-6.7.5-18-py3.12.el8` (version used for validation)



**General Imaging parameters:**
* `useStartModel` \
    Currently only supported for single-term. Set to `True` if using an initial model for the imaging project.
* `useStartMask` \
    Currently only supported for single-term. Set to `True` if using an initial mask for the imaging project.
* `msnamelist` \
    Name of input MS(es) to use for imaging. This parameter can be declared in the JSON parameter file as a list or a **dictionary, the latter being required for multi-term imaging**. Alternatively, if using a simple text parameter file, provide a comma-separated list of msnames (no spaces). The application expects the MS(es) to be pre-staged as compressed tarballs (`.tgz`) in the OSDF data origin location specified via `msdir`, but only MS names must be provided in this list - the application will add the extension (`.tgz`) and Pelican flags for automatic unpacking. \
    Accepted formats:
    * list (JSON):
    ```
    "msnamelist" : ["msname1.ms", "msname2.ms", "msname3.ms", "msname4.ms",...]
    ```
    * dictionary (JSON):
    ```
    "msnamelist" : {
        "spw0" : ["msname1.ms", "msname2.ms", ...],
        "spw1" : ["msname3.ms", "msname4.ms", ...],
        ...
    }
    ```
    * comma-separated list (simple text):
    ```
    msnamelist = msname1.ms,msname2.ms,msname3.ms,msname4.ms,...
    ```
* `cfcachelist` \
    Name of CFCache(s) to use for imaging. This parameter can be declared in the JSON parameter file as a list or a dictionary. Alternatively, if using a simple text parameter file, provide a comma-separated list of msnames (no spaces). The application expects the CFs to be pre-staged as compressed tarballs (`.tgz`) in the OSDF data origin location specified via `cfcachedir`, but only CF names must be provided in this list - the application will add the extension (`.tgz`) and Pelican flags for automatic unpacking. \
    Accepted formats:
    * list (JSON):
    ```
    "cfcachelist" : ["cfcache1.cf", "cfcache2.cf", "cfcache3.cf", "cfcache4.cf",...]
    ```
    * dictionary (JSON):
    ```
    "msnamelist" : {
        "spw0" : ["cfcache1.cf", "cfcache2.cf", ...],
        "spw1" : ["cfcache3.cf", "cfcache4.cf", ...],
        ...
    }
    ```
    * comma-separated list (simple text, must map directly to entries in `msnamelist`):
    ```
    cfcachelist = cfcache1.cf,cfcache2.cf,cfcache3.cf,cfcache4.cf,...
    ```
    The following declaration patterns are accepted:
    * One CF for all the imaging jobs: \
    JSON:
    ```
    "cfcachelist" : ["cfname.cf"]
    ```
    Text:
    ```
    cfcachelist = cfname.cf
    ```
    * One CF per Spectral Window (SPW, must match the number of SPWs if `msnamelist` is a dictionary, or the number of entries if it is a list):\
    JSON:
    ```
    "cfcachelist" : ["cfnamespw0.cf", "cfnamespw1.cf", ...]
    ```
    * CFs map directly to input MSes: \
    JSON:
    ```
    "cfcachelist" : {
        "spw0" : ["cfname1.cf", "cfname2.cf", ...],
        "spw1" : ["cfname3.cf", "cfname4.cf", ...],
        ...
    }
    Text (must map directly to entries in `msnamelist`):
    ```
    cfcachelist = cfcache1.cf,cfcache2.cf,cfcache3.cf,cfcache4.cf,...
    ```
* `baseimagename` \
    String used to define image names during the execution of the workflow. Main product names are defined as `$(baseimagename).image`, `$(baseimagename).image.pbcor`, etc. \
    Type: string
* `pblimit` \
    PB gain level at which to cut off normalizations. \
    Type: double
* `nmajor` \
    Maximum number of major cycles to evaluate. \
    Type: integer
* `allspwvis` \
    Only required for multi-term imaging using the CASA interface, because the MVC algorithm uses the MS obtain the frequencies for each spectral window. This can be a small MS, containing only a few timestamps, but crafted to contain all the SPWs selected for imaging. Similarly to `msnamelist`, this MS must be staged in `msdir` as a compressed tarball, but only the base name must be provided - the application will add the extension (`.tgz`) and Pelican flags for automatic unpacking.

> For gridding, deconvolution, and normalization parameters, refer to [tclean's documentation](https://casadocs.readthedocs.io/en/stable/api/tt/casatasks.imaging.tclean.html).


## To do:

- [ ] Refactor imageSolverHelpers.py into modules
- [ ] Develop a class that generalizes DAG generation (currently imageSolver.py and imagingCycle.py)
- [ ] Integrate LibRA-based MVC functionality: create functions to generate jobs that run `Taylor` by calling `htcimager.py` with `--interface libra_cl_imager` and `--jobmode psfc2tt | resc2tt | tt2cube`.
- [ ] Support distributed CFCache computation via HTCondor jobs running `Coyote`.
- [ ] Support distributed processing for simulation
- [ ] Support partitioning and re-arranging of input MSes to optimize IO patterns.