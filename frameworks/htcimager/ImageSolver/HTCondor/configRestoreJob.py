#!/usr/bin/env python3

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

import argparse

import htcondor2 as htcondor
import classad2 as classad

from imageSolverHelpers import createRestoreJob,makeJobBaseDict
from inputParameters import inputParameters


def getLastImcycle(nodeStatus) -> int:
    for ad in nodeStatus:
        node = ad.get('Node') if 'Node' in ad else ''
        retries = ad.get('RetryCount')
        if 'imagingCycle' in node:
            lastCycle = retries + 1
            break

    return lastCycle
        

def loadNodeStatus(nodeStatusFile) -> list:
    with open(nodeStatusFile, 'r') as infile:
        nodestatus = [ad for ad in classad.parseAds(infile)]

    return nodestatus


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('nodeStatusFile')
    parser.add_argument('parfile')
    parser.add_argument('lastcycle')
    inputargs = parser.parse_args()

    nodeStatusFile = inputargs.nodeStatusFile
    lastcycleinput = int(inputargs.lastcycle)
    nodeStatus = loadNodeStatus(nodeStatusFile)
    lastCycle  = lastcycleinput + getLastImcycle(nodeStatus)

    parfile = inputargs.parfile
    allpars = inputParameters(parfile)
    htcpars   = allpars.htcpars
    imgpars   = allpars.imgpars
    multiterm = allpars.multiterm

    imageSolverDict = makeJobBaseDict(HTCpars = htcpars, IMGpars = imgpars)

    if multiterm:
        nterms = allpars.deconvpars['nterms']
    else:
        nterms = 1

    jobmode = 'restore'
    restorejob = createRestoreJob(
        jobbasedict = imageSolverDict,
        jobmode     = jobmode,
        parfile     = parfile,
        multiterm   = multiterm,
        imcycle     = lastCycle,
        nterms      = nterms
    )

    with open(f'{jobmode}.sub', 'w') as jobfile:
        jobfile.writelines(str(restorejob))