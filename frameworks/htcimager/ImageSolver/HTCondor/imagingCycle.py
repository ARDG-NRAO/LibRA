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

import json
import argparse
from pathlib import Path

import htcondor2 as htcondor
from htcondor2 import dags

from inputParameters import inputParameters
from imageSolverHelpers import *


class imagingCycle(dags.DAG):
    def __init__(self, parfile : str = 'imageSolver.json',
                 noderetry     : int = 0,
                 dag_file_name : str = 'imagingCycle.dag',
                 lastcycle     : int = 0
                ):

        super().__init__()

        noderetry += lastcycle
        imcycle = noderetry + 1

        allpars = inputParameters(parfile)

        cfcachedir    = allpars.cfcachedir.replace('osdf://','')
        msdir         = allpars.msdir.replace('osdf://','/')

        htcpars   = allpars.htcpars
        imgpars   = allpars.imgpars
        multiterm = allpars.multiterm

        cfcachelist = allpars.cfcachelist
        msnamelist  = allpars.msnamelist
        imgnamelist = allpars.imgnamelist

        baseimagename = allpars.baseimagename

        imageSolverDict = makeJobBaseDict(HTCpars = htcpars, IMGpars = imgpars)

        if multiterm:
            nterms      = allpars.deconvpars['nterms']
            jobmode     = 'updateModel'
            updateModel = self.layer(
                name = f'{jobmode}_imcycle{imcycle}',
                submit_description = createUpdateModelJob(
                    jobbasedict    = imageSolverDict,
                    jobmode        = jobmode,
                    parfile        = parfile,
                    nterms         = nterms,
                    noderetry      = noderetry,
                    imcycle        = imcycle
                ),
                abort = dags.DAGAbortCondition(node_exit_value = 1, dag_return_value = 10)
            )
        
            jobmode  = 'tt2cube'
            tt2cube  = self.layer(
                name = f'{jobmode}_imcycle{imcycle}',
                submit_description = createCASAMultitermJob(
                    jobbasedict    = imageSolverDict,
                    jobmode        = jobmode,
                    parfile        = parfile,
                    imallkeys      = imgnamelist['allkeys'],
                    nterms         = nterms,
                    imcycle        = imcycle
                ),
            )
        else:
            jobmode = 'runModelCycle'
            runModelCycle = self.layer(
                name = f'{jobmode}_imcycle{imcycle}',
                submit_description = createUpdateModelJob(
                    jobbasedict    = imageSolverDict,
                    jobmode        = jobmode,
                    parfile        = parfile,
                    noderetry      = noderetry,
                    imcycle        = imcycle
                ),
                abort = dags.DAGAbortCondition(node_exit_value = 1, dag_return_value = 10)
            )
        
        jobmode = 'runResidualCycle'
        runResidualCycle = self.layer(
            name = f'{jobmode}_imcycle{imcycle}',
            submit_description = createGriddingJob(
                jobbasedict   = imageSolverDict,
                jobmode       = jobmode,
                parfile       = parfile,
                multiterm     = multiterm,
                imcycle       = imcycle
            ),
            vars = writeGriddingVARS(
                msnamelist  = msnamelist,
                cfcachelist = cfcachelist,
                imgnamelist = imgnamelist,
                msdir       = msdir,
                cfcachedir  = cfcachedir,
                jobmode     = jobmode,
                mindiskreq  = htcpars['min_disk_residual']
            ),
            retries = 2
        )
        
        jobmode = 'gather,normalize'
        gathernorm = self.layer(
            name = f'{jobmode}_imcycle{imcycle}',
            submit_description = createGatherJob(
                jobbasedict   = imageSolverDict,
                jobmode       = jobmode,
                parfile       = parfile,
                multiterm     = multiterm,
                imcycle       = imcycle
            ),
            vars = writeGatherVARS(
                imagename   = baseimagename,
                imgnamelist = imgnamelist,
                jobmode     = jobmode,
                multiterm   = multiterm
            )
        )
        
        if multiterm:
            jobmode = 'resc2tt'
            resc2ttimc = self.layer(
                name = f'{jobmode}_imcycle{imcycle}',
                submit_description = createCASAMultitermJob(
                    jobbasedict = imageSolverDict,
                    jobmode     = jobmode,
                    parfile     = parfile,
                    nterms      = nterms,
                    imallkeys   = imgnamelist['allkeys'],
                    imcycle     = imcycle
                ),
            )
        
        # Job dependencies in the inner DAG
        gathernorm.add_parents(runResidualCycle)
        if multiterm:
            tt2cube.add_parents(updateModel)
            runResidualCycle.add_parents(tt2cube)
            resc2ttimc.add_parents(gathernorm)
        else:
            runResidualCycle.add_parents(runModelCycle)

        self._writeDAG(dag = self, dag_file_name = dag_file_name)


    def _writeDAG(self, dag, dag_file_name) -> None:
        dags.write_dag(
            dag = dag,
            dag_dir = '.',
            dag_file_name = dag_file_name
        )


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('parfile')
    parser.add_argument('noderetry')
    parser.add_argument('dag_file_name')
    parser.add_argument('lastcycle')
    inputargs = parser.parse_args()

    parfile       = inputargs.parfile
    noderetry     = int(inputargs.noderetry)
    dag_file_name = inputargs.dag_file_name
    lastcycle     = int(inputargs.lastcycle)

    imagingCycleDAG = imagingCycle(
        parfile = parfile, 
        noderetry = noderetry, 
        dag_file_name = dag_file_name, 
        lastcycle = lastcycle
    )
    print(imagingCycleDAG.describe())
