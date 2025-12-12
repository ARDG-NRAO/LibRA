#!/usr/bin/python3

# Copyright (C) 2024
# Associated Universities, Inc. Washington DC, USA.
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.is
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning this should be addressed as follows:
#        Postal address: National Radio Astronomy Observatory
#                        1003 Lopezville Road,
#                        Socorro, NM - 87801, USA
#
# $Id$

import sys
import shutil
from imageSolverHelper import *
from datetime import datetime

def usage():
    print(f'''
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
    --libra_install : Path to LibRA installation (binaries) directory (if different from {libra_home_install_path}).
'''
#    Following options are not printed in usage summary but are available in the interface
#    for testing and specific use cases:
#    -n, --nparts    : Number of partitions of the input MS and CFCache. Default: 1 (not partitioned)
#    -i, --imagename : Name of output image (without extension) - imagename must be blank in parameter file
#    -m, --msname    : Name of input MS (basename if nparts > 1) - vis must be blank in parameter file
#    -c, --cfcache   : Name of CFCache (basename if nparts > 1) - cfcache must be blank in parameter file
    )

date = datetime.now().strftime('%Y%m%d-%H%M%S')
timing = { 'start' : datetime.now(), 'end' : None}
print(f"{getTimeStr(timing['start'])}Begin workflow execution")

partition     = False
nparts        = 1
usegpu        = False
gridMem       = '10G'
imdmMem       = '10G'
max_iter      = 20
msname        = ''
imagename     = ''
cfcache       = ''
logdir        = f'logs_{date}'
libra_install = ''
parfile       = 'imageSolver.def'
queuename     = ''
usespackenv   = ''
taccqueues    = ['gg', 'gh']
verbose       = False


args = sys.argv
if len(args) > 1:
    args = args[1:]
    readArg = ''
    for arg in args:
        if readArg:
            exec(f"{readArg} = '{arg}'")
            readArg = ''
        elif arg in ['-m','--msname']:
            readArg = 'msname'
        elif arg in ['-i','--imagename']:
            readArg = 'imagename'
        elif arg in ['-c','--cfcache']:
            readArg = 'cfcache'
        elif arg in ['-g','--gpu']:
            usegpu = True
        elif arg in ['-l','--logdir']:
            readArg = 'logdir'
        elif arg in ['-f','--parfile']:
            readArg = 'parfile'
        elif arg in ['-n','--nparts']:
            readArg = 'nparts'
        elif arg in ['-p','--partition']:
            partition = True
        elif arg in ['-q','--queue']:
            readArg = 'queuename'
        elif arg in ['-v','--verbose']:
            verbose = True
        elif arg in ['--gridMem']:
            readArg = 'gridMem'
        elif arg in ['--imdmMem']:
            readArg = 'imdmMem'
        elif arg in ['--max_iter']:
            readArg = 'max_iter'
        elif arg in ['--libra_install']:
            readArg = 'libra_install'
        elif arg in ['-h','--help']:
            usage()
            sys.exit(0)
        else:
            usage()
            raise ValueError(f'{arg}: unknown argument.')
else:
    usage()
    raise ValueError('No arguments.')

if not imagename or not msname or not cfcache:
    imagename, msname, cfcache = readParfile(parfile)

if partition:
    msname, cfcache, nparts = makePartitions(msname, cfcache, libra_install, verbose)

nparts = int(nparts)

griddingResources = f'-N1 -G1 -c2' if usegpu else f'-N1 --exclusive'
imdomainResources = '-N1 -c2'

if queuename:
    griddingResources += f' -p {queuename}'
    if queuename in taccqueues:
        if queuename == 'gg':
            griddingResources += f' -t 150'
        elif queuename == 'gh':
            griddingResources += f' -t 10'
            griddingResources = griddingResources.replace('-G1 ','')
        imdomainResources += f' -p gg -t 20'
    else:
        imdomainResources += f' -p {queuename}'


if not queuename or queuename not in taccqueues:
    griddingResources += f' --mem={gridMem}'
    imdomainResources += f' --mem={imdmMem}'

gnimagename = imagename

if nparts > 1:
    array = f'-a 0-{nparts - 1}'
    griddingResources += f' {array}'
    gnimagename = f','.join([f'{imagename}.n{i}' for i in range(nparts)])

makePSFjid = sbatchComm(stageName = 'makePSF',
                        resources = griddingResources,
                        msname = msname,
                        imagename = imagename,
                        cfcache = cfcache,
                        parfile = parfile,
                        logdir = logdir,
                        libra_install_path = libra_install,
                        verbose = verbose)

stage = 'gatherPSF' if nparts > 1 else 'normalizePSF'
gnpsfjid = sbatchComm(stageName = stage,
                      resources = imdomainResources,
                      dependencies = f'afterok:{makePSFjid}',
                      imagename = gnimagename,
                      parfile = parfile,
                      logdir = logdir,
                      libra_install_path = libra_install,
                      verbose = verbose)

makeDirtyImagejid = sbatchComm(stageName = 'makeDirtyImage',
                               resources = griddingResources,
                               dependencies = f'afterok:{gnpsfjid}',
                               msname = msname,
                               imagename = imagename,
                               cfcache = cfcache,
                               parfile = parfile,
                               logdir = logdir,
                               libra_install_path = libra_install,
                               verbose = verbose)

stage = 'gather,normalize' if nparts > 1 else 'normalize'
gathernormjid = sbatchComm(stageName = stage,
                           resources = imdomainResources,
                           dependencies = f'afterok:{makeDirtyImagejid}',
                           imagename = gnimagename,
                           parfile = parfile,
                           logdir = logdir,
                           waitForJob = True,
                           libra_install_path = libra_install,
                           verbose = verbose)

notConverged = 1
imcycle = 1
while notConverged == 1 and imcycle < max_iter:
    stage = f'runModelCycle.imcycle{imcycle:02}'
    modelCyclejid = sbatchComm(stageName = stage,
                               resources = imdomainResources,
                               dependencies = '',
                               imagename = imagename,
                               parfile = parfile,
                               logdir = logdir,
                               libra_install_path = libra_install,
                               verbose = verbose)

    stage = f'runResidualCycle.imcycle{imcycle:02}'
    residualCyclejid = sbatchComm(stageName = stage,
                                  resources = griddingResources,
                                  dependencies = f'afterok:{modelCyclejid}',
                                  msname = msname,
                                  imagename = imagename,
                                  cfcache = cfcache,
                                  parfile = parfile,
                                  logdir = logdir,
                                  libra_install_path = libra_install,
                                  verbose = verbose)

    # Remove image prior to gather step to avoid NOOP in chip (as part of job to avoid race condition)
    if nparts > 1:
        stage = f'gather,normalize.imcycle{imcycle:02}'
        removeImage = f'{imagename}.residual'
    else:
        stage = f'normalize.imcycle{imcycle:02}'
        removeImage = ''
    gathernormjid = sbatchComm(stageName = stage,
                               resources = imdomainResources,
                               dependencies = f'afterok:{residualCyclejid}',
                               imagename = gnimagename,
                               parfile = parfile,
                               logdir = logdir,
                               waitForJob = True,
                               libra_install_path = libra_install,
                               removeImage = removeImage,
                               verbose = verbose)

    notConverged = checkConvergence(logdir)
    imcycle += 1

stage = 'restore'
sbatchComm(stageName = stage,
           resources = imdomainResources,
           dependencies = '',
           imagename = imagename,
           parfile = parfile,
           logdir = logdir,
           libra_install_path = libra_install,
           verbose = verbose)

timing['end'] = datetime.now()
walltime = (timing['end'] - timing['start']).total_seconds()
print(f"{getTimeStr(timing['end'])}Workflow wallclock time: {walltime} seconds")
print(f"{getTimeStr(timing['end'])}Completed workflow execution")
