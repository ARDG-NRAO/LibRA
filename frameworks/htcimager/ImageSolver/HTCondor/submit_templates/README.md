# Image Solver implementation for HTCondor

## Description

The implementation of the image solver for HTCondor consists of a collection of HTCondor job definitions (.htc), DAGMan workflow definitions (.dag) and scripts (.py, .sh), and a parameter file template (imageSolver.def) that enable execution of the image solver algorithm (current implementation supports single term continuum imaging) as a DAGMan workflow.

## Usage

Each imaging workflow needs its own copy of ImageSolver/HTCondor, so after cloning LibRA on the HTCondor Access Point (AP), create an initial directory for the imaging project and copy the contents of `<libra root>/frameworks/htcimager/ImageSolver/HTCondor` to `/path/to/initial/directory/bin`.

Change into the above directory and edit `imageSolver.def` according to imaging specifications. Follows a list of HTCondor specific parameters, and simple setup instructions:

Workflow configuration parameters:
* `memResidual` \
    memory request (RAM, not GPU memory) for jobs that compute residual images (also weight and psf). \
    Example: `26 G` for a job that is expected to use 26 GB.
* `memModel` \
    memory request (RAM) for jobs that process exclusively in the imaging domain (update model, restore, gather, normalization).
* `diskModel` \
    local disk request for jobs that process exclusively in the imaging domain. \
    Example: `50 G` for a job that is expected to use 50 GB of local storage.
* `projectdir` \
    name of the project directory. String used to form workflow related data paths, such as image storage and data source(s). \
    Example: `/path-facility/data/<username>/imagingdata/$(projectdir)`
* `initialdir` \
    full path of the directory where HTCondor should store logs and products that are not redirected (using `transfer_output_remaps`). \
    Default: `/home/<username>/$(projectdir)` (Notice the use of `projectdir` to form this path)
* `dataorigin` \
    address of the data origin where files are located and/or saved to. \
    Example: `osdf:///path-facility/data/<username>/` (PATh Facility data origin)
* `softwaredir` \
    location of the LibRA exodus bundle and `casa-data.tgz` (a tar.gz package with the contents of `<casa root>/data` from a local CASA installation), typically on a data origin. \
    Default: `$(dataorigin)/apps`
* `cuda_cap` \
    (Optional) The default value of `libra_bundle` uses this parameter to dynamically select the LibRA bundle (see below). Provide a fixed value to ensure a bundle is transferred to jobs that do not require a GPU.
* `libra_bundle` \
    Name of the relocatable LibRA bundle. If accessing GPUs with different CUDA capabilities, the variable `$(cuda_cap)` is dynamically set to the (integer) CUDA capability of the GPU device on the Execution Point (EP). Keep in mind that separate LibRA bundles (builds) have to be provided for different CUDA capabilities. \
    Default: `libra.$(cuda_cap)x.sh`
* `imagesdir` \
    Location where images are stored. \
    Default: `$(dataorigin)/imagingdata/$(projectdir)/working`
* `cfcachedir` \
    Location of Convolution Function Cache(s). \
    Default: `$(dataorigin)/cfcache`
* `msdir` \
    Location of input MS(es). \
    Default: `$(dataorgin)/data`
* `und_sites_grid` \
    (Optional) Comma-separated list of sites to avoid for gridding jobs. \
    Example: "FIU-PATH,UTC-Epyc"
* `notify_user` \
    (Optional) E-mail address to get notifications about job completion status. Model cycle jobs always send notifications; residual cycle jobs only notify if there are errors.
* `htcimagerdir` \
    Location of `htcimager` files. \
    Default: `<libra root>/frameworks/htcimager`
* `wantOSPool` \
    Set to `False` if not using resources from the Open Science Pool. \
    Default: `True`
* `NRPexclusive` \
    Set to `True` if gridding jobs are required to run exclusively on NRP resources. \
    Default: `False`

Imaging parameters:
* `useStartModel` \
    not yet supported - use `<libra root>/frameworks/htclean` if the imaging project requires a start model.
* `useStartMask` \
    not yet supported - use `<libra root>/frameworks/htclean` if the imaging project requires a start mask.
* `msnamelist` \
    Comma separated list of all the MSes to use for imaging. Only MS names are supported (not absolute paths), all MSes are required to be in `$(msdir)`.
* `cfcachelist` \
    Comma separated list of all the CFcaches to use for imaging. The list must either map exactly to `$(msnamelist)` or contain only one CFCache (if all the MSes use the same CFCache). All CFCaches are required to be in `$(cfcachedir)`.
* `baseimagename` \
    String used to define image names during the execution of the workflow. Main product names are defined as `$(baseimagename).image`, `$(baseimagename).image.pbcor`, etc.

Once configuration is complete in `imageSolver.def`, the imaging workflow can be submitted with the following command:

```
$ condor_submit_dag imageSolver.dag
```

And the progress of the workflow can be monitored with:

```
$ condor_q -dag -nobatch
```


## To do:

- [ ] Automated setup utility
- [ ] Port implementation to HTCondor Python Bindings
