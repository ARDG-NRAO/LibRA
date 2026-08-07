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

import os
import platform
import sys
from inputArgs import inputArgs
from makeImages import makeImages
from staging import staging
from datetime import datetime


def getTimeStr(intime : datetime = datetime.now()):
    log_time_fmt = '%Y-%m-%d %H:%M:%S'
    return f'{intime.strftime(log_time_fmt):24}'


machineAdFile       = ''
k8sphysicalhostname = ''
outputImageList     = []

os.environ['OMP_NUM_THREADS'] = '1'

print(f'{getTimeStr()}Executing htcimager.py')
timing = { 'start' : datetime.now(), 'end' : None}

try:
    scheduler = os.environ['BATCH_SYSTEM']
except:
    print(f"{getTimeStr()}Did not find env variable 'BATCH_SYSTEM'.\n")
    scheduler = ''

try:
    # Read arguments
    inputArgs = inputArgs(args = sys.argv[1:])

    if scheduler == 'HTCondor':
        machineAdFile = os.environ['_CONDOR_MACHINE_AD']

    
    # print OS and host info
    osversion = platform.platform()
    hostname = platform.node()
    if machineAdFile:
        with open(machineAdFile,'r') as classAds:
            for line in classAds.readlines():
                if 'k8sphysicalhostname' in line.lower():
                    k8sphysicalhostname = line.strip().split('=')[1]
    
    print(f"Host information:")
    print(f"{'OS Version':24}: {osversion}")
    print(f"{'Hostname':24}: {hostname}")
    if k8sphysicalhostname:
        print(f"{'k8s Physical Host Name':24}: {k8sphysicalhostname}")
    print()

    stage = staging(scheduler, inputArgs)
 
    imaging_task = makeImages(inputArgs = inputArgs)
    outputImageList = imaging_task.outputImageList

except Exception as e:
    raise e

finally:
    print(f'{getTimeStr()}Output images: {outputImageList}')
    if scheduler == 'HTCondor':
        rlogdir = os.path.relpath(stage.abslogdir)
        createTarList = outputImageList + [rlogdir]
        outputList = stage.makeTarballs(createTarList)
        stage.move_to_scratchdir(outputList)
    timing['end'] = datetime.now()
    walltime = (timing['end'] - timing['start']).total_seconds()
    print(f"{getTimeStr(timing['end'])}htcimager.py wallclock time: {walltime} seconds")
    print(f"{getTimeStr(timing['end'])}Completed htcimager.py execution")

