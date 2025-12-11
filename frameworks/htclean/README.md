# Description

Framework code for iterative image reconstruction using LibRA [apps](/README.md#currently-available-apps).

The current implementation enables continuum, single-term imaging. There are currently two different options to run the `htclean` framework:

- single process imaging workflow: mostly intended for usage on a local workstation or cluster node. The application `libra_htclean.sh` controls the steps and iterations in the imaging process (gridding, normalization, deconvolution, restore) by calling LibRA applications.
- distributed process imaging on HTCondor systems (currently deployed and tested on PATh and OSG): enables large scale gridding parallelization by running `htclean.dag` and underlying components.

The following sections will cover the specific details of each of these options.

## Single Processs Imaging Workflow

The following resources must be available in order to start a single process imaging workflow on a local workstation or cluster node:

- local build of LibRA;
- local copy of CASA data (this is available from a local installation of CASA, at `<casa root dir>/data`);
    - the line `measures.directory:/path/to/casa/data` must be present in file `~/.casarc`;
- input Measurement Set (MS);
- input Convolution Function Cache (CFCache);
- a clean working directory containing a modified copy of `<libra root dir>/frameworks/htclean/libra_htclean.std.def` with the desired imaging parameters.

The script `libra_htclean.sh` is installed in `<libra root dir>/install/bin` during the build process.

- To get a list of available options, run:

```
$ libra_htclean.sh -h
```

- To start an imaging workflow with default parameters, run:

```
$ libra_htclean.sh -i imagename
```

notice that the imagename must always be specified in the command line. Other parameters are optional.

## Distributed Process Imaging Workflow on HTCondor Systems

The following resources must be available in order to start a distributed process imaging workflow on HTCondor systems:

- access to an HTCondor pool (NRAO-DSOC cluster, NRAO-CV cluster, [PATh](https://path-cc.io/facility/), [OSG](https://osg-htc.org/), [CHTC](https://chtc.cs.wisc.edu/), etc);
- The following must be present on the HTCondor Access Point (AP):
    - local copy of the `htclean` framework, which contains the HTCondor submit files and the application `libra_htclean_condor.sh`, that enables distributed imaging operations. This can be obtained by cloning the LibRA repository;
    - the above directory must also contain a modified copy of `libra_htclean.def` with the desired imaging parameters;
- The following must be present on an Open Science Data Fedaration (OSDF) data origin:
    - `casa-data.tgz`: a compressed tarball with CASA data (can be obtained from an installation of CASA, by packing `<casa root dir>/data`);
    - a relocatable LibRA bundle;
    - compressed tarball(s) of the input MS(es);
    - compressed tarball(s) of the input CFC(s);

> About OSDF data origins:
> The `htclean` workflow uses [Pelican](https://pelicanplatform.org/) to transfer data to and from jobs, which enables accessing data origins at the [Open Science Data Federation (OSDF)](https://osg-htc.org/services/osdf.html). This eliminates the need for pre-staging data in a storage system that is directly accessible by the HTCondor AP. 

The automated setup utility `htclean_setup` is part of the `htclean` framework and can be used to easily configure an `htclean` workflow on the [PATh Facility](https://path-cc.io/facility/). Alternatively, the parameters of the workflow can be manually configured by editing `libra_htclean.def` and adequately pre-staging all the necessary files in the desired locations. 

The `htclean` workflow is pre-configured in `libra_htclean.def` to expect inputs at the following locations and with the following naming conventions and formats:

- input MS(es): `/<data origin>/data/<project name>/<ms name>.tgz`; 
- input CF(s): `/<data origin>/cfcache/<project name>/<cfcache name>.tgz`
- applications: 
    - `/<data origin>/apps/casa-data.tgz`;
    - `/<data origin>/apps/libra-<cuda capability>x.sh` - relocatable LibRA bundle, identified by the (integer) CUDA capability that LibRA was built for;

> Before you begin: Make a clone of the LibRA repository on the HTCondor AP:
>
> `git clone https://github.com/ARDG-NRAO/LibRA.git`
>
> The above command will create directory `libra`, and the `htclean` directory is located at `libra/frameworks/htclean`

### Automated workflow configuration with `htclean_setup`

The application `htclean_setup` simplifies the steps of configuration and pre-staging of data that are necessary for running an `htclean` imaging workflow. The current implementation supports running workflows on the PATh facility, with input data coming from either the PATh 
facility data origin (`osdf:///path-facility/data/<username>`) or the NRAO data origin (`osdf:///nrao-ardg/`), or both. The application runs from the `htclean` directory, which will be ommited in the commands shown below.

The list of all available options can be obtained by running:

```
$ htclean_setup -h
```

This guide will now focus on the options related with the most common use cases.


#### Inputs pre-staged in the PATh AP and in the expected directories and formats

This is the simplest use case as all the inputs (MSes, CFs, applications) are already available at the expected locations in the PATh AP, from where the workflow jobs are submitted.

From a terminal session at the PATh AP, run:

```
$ htclean_setup -P <project name> -i <image name>
```

The output will warn that the directories already exist and will complete by pre-configuring `/home/<username>/<project name>/bin/libra_htclean.def` with the appropriate workflow parameters. Review this file to ensure the correct imaging parameters before submitting the workflow. Although not strictly required, it is recommended to use `-i <image name>` to set the desired image name in `/home/<username>/<project name>/bin/libra_htclean.def`.


#### Inputs located in the NRAO OSDF data origin in the expected directories and formats

The NRAO data origin is hosted on `monte.aoc.nrao.edu`. The local path on `monte` that corresponds to `osdf://nrao-ardg` is `/export/home/monte2/origin/ardg`. Currently, this data origin is read-only, so all the job outputs are written in the PATh data origin, at `/path-facility/data/<username>/imagingdata/<project name>/working`.

To configure a workflow that reads inputs from the NRAO data origin, `htclean_setup` needs to run twice: first at the NRAO data origin and subsequently at the PATh AP. If the inputs are pre-staged in the above expected locations, log in to the NRAO data origin and run the command

```
$ htclean_setup -P <project name> -o <file name>
```

to write a small text file with the given `file name` that contains the data origin configurations and list of input MS(es) and CFCache(s). To create the workflow environment and conclude the configuration, copy this file and log in to the PATh AP to run:

```
$ htclean_setup -f <file name> -i <image name>
```


#### Inputs pre-staged in arbitrary locations

When the inputs are available in one of the supported data origins, but not in the expected locations and/or formats, run `htclean_setup` with the following options to move files to the expected locations and make tarballs if needed.

- `-M /path/to/ms/directory` to indicate the path to the directory containing the original MS(es) (or pre-packaged .tgz);
- `-C /path/to/cfc/directory` to indicate the path to the directory containing the original CFCache(s) (or pre-packaged .tgz);
- `-a /path/to/casa-data.tgz` to indicate the path to CASA data package (pre-packaged .tgz);
- `-b /path/to/libra-bundle.sh` to indicate the location of the relocatable LibRA bundle.

The options `-M` and `-C` assume the default extensions `.ms` and `.cfc`, respectively for MS(es) and CFCache(s). If using different extensions, these can be indicated with options `-m` and `-c`. In addition, default ordering by name is used when generating input MS and CFCache lists, and the order controls the mapping of MS to CFC. If the naming is not sufficient to ensure correct mapping, the following options should be used:

- `-s msname1.ms,msname2.ms,...,msnameN.ms`
- `-t cfname1.cfc,cfname2.cfc,...,cfnameN.cfc`

These options cannot be used independently of each other, but they do not need to be combined with `-M`, `-C`, `-m` or `-c`.

As an example, if the inputs are available in arbitrary locations in the PATh AP, the following command will move MSes and CFCs from arbitrary locations and configure the given MS to CFC mapping in `/home/<username>/<project name>/bin/libra_htclean.def`:

```
$ htclean_setup -P <project name> -i <image name> -M </path/to/MSdir> -C </path/to/CFCdir> -s <ms1.ms,ms2.ms,ms3.ms> -t <cf1.cfc,cf2.cfc,cf3.cfc>
```

Alternatively, **if the inputs are available in arbitrary locations in the NRAO data origin**, the following command sequence will result in the equivalent environment configuration:

**On the NRAO data origin:**
```
$ htclean_setup -P <project name> -o <file name> -M </path/to/MSdir> -C </path/to/CFCdir> -s <ms1.ms,ms2.ms,ms3.ms> -t <cf1.cfc,cf2.cfc,cf3.cfc>
```
and copy the resulting output file `file name` to the PATh AP.

**On the PATh AP:**
```
$ htclean_setup -f <file name> -i <image name>
```

#### Job notifications

The `htclean` workflow can send e-mail about the completion and/or error status of jobs. This can be configured when running `htclean_setup` at the HTCondor AP, by adding the option `-n <e-mail address>` to the command line.

### Submitting jobs

The jobs in the `htclean` workflow are submitted from the HTCondor AP.

After successful configuration of the workflow environment, review imaging parameters in `/home/<username>/<project name>/bin/libra_htclean.def`.

Next, change into directory `/home/<username>/<project name>/bin/` and launch the workflow:

```
$ condor_submit_dag htclean.dag
```
