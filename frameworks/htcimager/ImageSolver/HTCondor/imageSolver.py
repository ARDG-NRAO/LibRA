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

import os
import json
import argparse
from pathlib import Path

import htcondor2 as htcondor
from htcondor2 import dags

from inputParameters import inputParameters
from imagingCycle import imagingCycle
from imageSolverHelpers import *

this_dir = Path(__file__).parent.resolve()

class imageSolver(dags.DAG):
    def __init__(self, parfile : str, 
                 debug         : bool,
                 dryrun        : bool,
                 force         : bool,
                 lastcycle     : int):

        super().__init__()
        
        allpars = inputParameters(parfile)
            
        cfcachedir     = allpars.cfcachedir.replace('osdf://','')
        msdir          = allpars.msdir.replace('osdf://','/')
        
        htcpars        = allpars.htcpars
        imgpars        = allpars.imgpars
        
        cfcachelist    = allpars.cfcachelist
        msnamelist     = allpars.msnamelist
        baseimagename  = allpars.baseimagename
        
        multiterm      = allpars.multiterm
        nmajor         = allpars.nmajor
        
        imgnamelist    = allpars.imgnamelist

        nodeStatusFile = 'imageSolver.dag.nodestatus'
        
        # Begin writing the workflow
        imageSolverDAG  = dags.DAG(
            node_status_file = dags.NodeStatusFile(
                path = ( Path.cwd() / f'{nodeStatusFile}' )
            ))
        
        imageSolverDict = makeJobBaseDict(HTCpars = htcpars, IMGpars = imgpars)
        if debug: print(imageSolverDict)
        
        jobmode = 'makePSF'
        makePSF = imageSolverDAG.layer(
            name = jobmode,
            submit_description = createGriddingJob(
                jobbasedict = imageSolverDict,
                jobmode     = jobmode,
                parfile     = parfile,
                multiterm   = multiterm
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
            retries = 2,
        )
        
        jobmode = 'gatherPSF'
        gatherPSF = imageSolverDAG.layer(
            name = jobmode,
            submit_description = createGatherJob(
                jobbasedict   = imageSolverDict,
                jobmode       = jobmode,
                parfile       = parfile,
                multiterm     = multiterm
            ),    
            vars = writeGatherVARS(
                imagename   = baseimagename,
                imgnamelist = imgnamelist,
                jobmode     = jobmode,
                multiterm   = multiterm
            )
        )
        
        if multiterm:
            nterms   = allpars.deconvpars['nterms']
            jobmode = 'psfc2tt'
            psfc2tt = imageSolverDAG.layer(
                    name = jobmode,
                    submit_description = createCASAMultitermJob(
                        jobbasedict = imageSolverDict,
                        jobmode     = jobmode,
                        parfile     = parfile,
                        nterms      = nterms,
                        imallkeys   = imgnamelist['allkeys'],
                    ),
            )
        
        
        jobmode = 'makeDirtyImage'
        makeDirtyImage = imageSolverDAG.layer(
            name = jobmode,
            submit_description = createGriddingJob(
                jobbasedict   = imageSolverDict,
                jobmode       = jobmode,
                parfile       = parfile,
                multiterm     = multiterm
            ),
            vars = writeGriddingVARS(
                msnamelist  = msnamelist,
                cfcachelist = cfcachelist,
                imgnamelist = imgnamelist,
                msdir       = msdir,
                cfcachedir  = cfcachedir,
                jobmode     = 'runResidualCycle',
                mindiskreq  = htcpars['min_disk_residual']),
            retries = 2
        )
        
        jobmode = 'gather,normalize'
        gatherdirty = imageSolverDAG.layer(
            name = f'{jobmode}_imcycle0',
            submit_description = createGatherJob(
                jobbasedict   = imageSolverDict,
                jobmode       = jobmode,
                parfile       = parfile,
                multiterm     = multiterm
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
            resc2tt = imageSolverDAG.layer(
                name = f'{jobmode}_imcycle0',
                submit_description = createCASAMultitermJob(
                    jobbasedict    = imageSolverDict,
                    jobmode        = jobmode,
                    parfile        = parfile,
                    imallkeys      = imgnamelist['allkeys'],
                    nterms         = nterms
                ),
            )
            checkConvergenceArgs = ['-z', '../logs.updateModel*.tgz']
        else:
            checkConvergenceArgs = ['']
        
        
        subDAG = 'imagingCycle'
        dagname = f'{subDAG}.dag'
        imagingCycleSubDAG = imageSolverDAG.subdag(
            name = subDAG,
            dag_file = (Path.cwd() / f'{dagname}'),
            retries = nmajor - 1,
            abort = dags.DAGAbortCondition(2),
            pre = dags.Script(
                executable = f'{this_dir}/imagingCycle.py',
                arguments  = [parfile, '$RETRY', dagname, lastcycle]
            ),
            post = dags.Script(
                executable = f'{this_dir}/../../checkConvergence.py',
                arguments  = checkConvergenceArgs
            )
        )
        
        jobmode = 'restore'
        restore = imageSolverDAG.layer(
            name = jobmode,
            submit_description = (Path.cwd() / f'{jobmode}.sub'),
            pre = dags.Script(
                executable = f'{this_dir}/configRestoreJob.py',
                arguments  = [nodeStatusFile, parfile, lastcycle]
            )
        )
        
        gatherPSF.add_parents(makePSF)
        gatherdirty.add_parents(makeDirtyImage)
        restore.add_parents(imagingCycleSubDAG)
        if multiterm:
            psfc2tt.add_parents(gatherPSF)
            makeDirtyImage.add_parents(psfc2tt)
            resc2tt.add_parents(gatherdirty)
            imagingCycleSubDAG.add_parents(resc2tt)
        else:
            makeDirtyImage.add_parents(gatherPSF)
            imagingCycleSubDAG.add_parents(gatherdirty)
        
        print(imageSolverDAG.describe())
        dags.write_dag(
            dag = imageSolverDAG,
            dag_dir = '.',
            dag_file_name = 'imageSolver.dag'
        )
        
        if not dryrun:
            dag_submit = htcondor.Submit.from_dag('imageSolver.dag', {'force' : int(force)})
            
            schedd = htcondor.Schedd()
            cluster_id = schedd.submit(dag_submit).cluster()
            
            print(f"DAGMan job cluster is {cluster_id}")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description = 'Writes and submits an HTCondor imaging workflow.')
    parser.add_argument('parfile',
        help = 'name of the input parameter file. Can be a JSON or text file with parafeed syntax.')
    parser.add_argument('-d', '--debug', action = 'store_true',
        help = 'add print statements to help with debugging.')
    parser.add_argument('-r', '--dryrun', action = 'store_true',
        help = 'do not submit, only create workflow submit description files.')
    parser.add_argument('-f', '--force', action = 'store_true',
        help = 'force new submission.')
    parser.add_argument('-c','--lastcycle', default = 0,
        help = 'if restarting a run, the index of the last completed imaging cycle before restart')
    # considering completed imaging cycles ensures the index will be correct even when the
    # imaging cycle is marked as failed in the dag rescue file
    inputargs = parser.parse_args()
    
    parfile   = inputargs.parfile
    debug     = inputargs.debug
    dryrun    = inputargs.dryrun
    force     = inputargs.force
    lastcycle = inputargs.lastcycle

    imageSolverDAG = imageSolver(
        parfile   = parfile,
        debug     = debug,
        dryrun    = dryrun,
        force     = force,
        lastcycle = lastcycle
    )
