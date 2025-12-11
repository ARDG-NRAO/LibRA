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
import time
import shutil
from subprocess import Popen,PIPE
from datetime import datetime


this_path = os.path.dirname(os.path.realpath(__file__))
libra_home = os.path.realpath(f'{this_path}/../../../..')
libra_home_install_path = os.path.realpath(f'{libra_home}/install')
htcimagerPath = os.path.realpath(f'{libra_home}/frameworks/htcimager')
htcimagerExe  = os.path.realpath(f'{htcimagerPath}/htcimager.py')


def sbatchComm(stageName : str,
               resources : str,
               imagename : str,
               parfile : str,
               logdir : str,
               libra_install_path : str,
               verbose :bool,
               msname : str = '',
               cfcache : str = '',
               dependencies : str = '',
               waitForJob : bool = False,
               removeImage : str = ''):
    jobid = ''
    addcmdflag = ''

    if dependencies:
        resources += f' -d {dependencies}'

    if waitForJob:
        resources += f' -W'
    
    outputName = stageName
    slurmStageName = stageName
    if '-a ' in resources:
        arrayindex = '${SLURM_ARRAY_TASK_ID}'
        slurmStageName = f'{stageName}.n%a'
        stageName += f'.n{arrayindex}'
        msname += f'_SPW{arrayindex}.ms'
        imagename += f'.n{arrayindex}'
        cfcache += f'_SPW{arrayindex}.cfc'
        outputName += '.n%a'
    
    jobmode = stageName.split('.')[0]
    if stageName == 'normalizePSF':
        jobmode = 'normalize'
        addcmdflag = ' -t psf'
    elif 'normalize' in jobmode:
        addcmdflag = ' -t residual'
    
    if not libra_install_path:
        libra_install_path = libra_home_install_path

    htcimagercmd = f'{htcimagerExe} -j {jobmode} -n {stageName} -f {parfile} -l {logdir} -p {libra_install_path} -i {imagename}'
    if msname:
        htcimagercmd += f' -m {msname}'
    if cfcache:
        htcimagercmd += f' -c {cfcache}'
    if not any(gpustr in resources for gpustr in ['-G', '-p gh']):
        htcimagercmd += ' --nogpu --vbbucketsize 200000'
    if addcmdflag:
        htcimagercmd += addcmdflag
    
    if removeImage:
        htcimagercmd = f'rm -rf {removeImage}; {htcimagercmd}'

    stage_cmd = f'sbatch -J {slurmStageName} {resources} -o {outputName}.stdout -e {outputName}.stderr --wrap'
    cmd = stage_cmd.split(' ')
    cmd.append(htcimagercmd)
    print(f"{getTimeStr(datetime.now())}Submitting job {stageName}")
    if verbose:
        print(f"SLURM command line: {' '.join(cmd)}")
    
    with Popen(cmd, stdout = PIPE) as jobSubmit:
        cmdOutput = jobSubmit.stdout.read().decode('ascii').strip()
        exitcode = jobSubmit.wait()

    if exitcode != 0:
        raise RuntimeError(f"Job submission for stage {slurmStageName} failed. Return value: {exitcode}.\n{cmdOutput}")
    elif 'Submitted batch job' in cmdOutput:
        index = int(cmdOutput.find('batch job')) + 9
        jobid = cmdOutput[index:].strip()
        if not jobid:
            raise RuntimeError(f"Job submission failed for stage {slurmStageName}.")

    return jobid


def checkConvergence(logdir : str):
    cmd = f'{htcimagerPath}/checkConvergence.py -n -o checkConvergence.out -l {logdir}/*.model.*.log'
    cmd = cmd.split(' ')

    with Popen(cmd) as chkconv:
        exitcode = chkconv.wait()
    
    return exitcode


def readParfile(parfile : str):
    impars = {}
    with open(parfile, 'r') as infile:
        for line in infile.readlines():
            if line != '\n' and '#' not in line[0]:
                line = line.split('#')[0].split('=')
                if "'" in line[1]:
                    value = eval(line[1].strip())
                else:
                    value = line[1].strip()
                
                impars[line[0].strip()] = value
    
    msname    = impars['vis']
    cfcache   = impars['cfcache']
    imagename = impars['imagename']

    if not imagename or not msname or not cfcache:
        raise ValueError('Blank input parameters: none of imagename, msname or cfcache can be blank.')

    # clear parameters to write parameter file to be used by htcimager.py
    impars['vis']       = ''
    impars['cfcache']   = ''
    impars['imagename'] = ''

    shutil.copyfile(parfile, parfile.replace('.def','.orig.def'))
    writeParfile(impars, parfile)

    return imagename, msname, cfcache


# The following is copied (simplified) from libra_cl_imager.py - this must be added to a common library
def writeParfile(impars, parfile, logdir = '.'):
    with open(f'{logdir}/{parfile}', 'w') as outfile:
        for key,value in impars.items():
            outfile.write(f'{key:22}= {value}\n')


def getSPWIds(msname : str, libra_install_path : str):
#   Using string 'TOPO' to detect SPW indexes in the output of tableinfo, not optimal.
    SPWIdList = []
    application = f'{libra_install_path}/bin/tableinfo'
    inputms = f'table={msname}'
    cmd = [application,'help=noprompt',inputms]

    with Popen(cmd, stdout = PIPE, stderr = PIPE) as tableinfo:
        msdata = tableinfo.communicate()[1].decode('ascii').split('\n')
        exitcode = tableinfo.wait()
    
    if exitcode != 0:
        raise RuntimeError(f"Failed to execute command line: {' '.join(cmd)}")
    
    for line in msdata:
        if 'TOPO' in line:
            SPWIdList.append(line.split()[4])
    
    return SPWIdList


def partitionMS(msname : str, libra_install_path : str, verbose : bool):
    SPWIdList = getSPWIds(msname, libra_install_path)
    print(f'{getTimeStr(datetime.now())}Found {len(SPWIdList)} SPWs. Partitioning MS by SPW.')

    for ispw, spw in enumerate(SPWIdList):
        application = f'{libra_install_path}/bin/mssplit'
        outms = msname.split('/')[-1].replace('.ms',f'_SPW{ispw}.ms')
        deepcopy = f'deepcopy=1'
        outmsstr = f'outms={outms}'
        cmd = [application,'help=noprompt',f'ms={msname}',outmsstr,f'spw={spw}',deepcopy]
    
        print(f"{getTimeStr(datetime.now()):24}Splitting SPW: {spw}")
        if verbose:
            print(f"Running command {' '.join(cmd)}")
        with Popen(cmd) as splitMS:
            exitcode = splitMS.wait()
        
        if exitcode != 0:
            raise RuntimeError('Failed to partition MS.')
        
    return len(SPWIdList)


def partitionCF(cfcache : str, libra_install_path : str, verbose : bool):
    # for testing prior to PR, reference branch LibRA path
    # application = f'{libra_install_path}/../scripts/partitionCFC_spw.sh'
    application = f'{libra_home_install_path}/../scripts/partitionCFC_spw.sh'
    cmd = [application,cfcache,'deepcopy']

    print(f"{getTimeStr(datetime.now())}Partitioning CFCache by SPW")
    if verbose:
        print(f"Running command: {' '.join(cmd)}")
    with Popen(cmd) as splitCF:
        exitcode = splitCF.wait()
    
    if exitcode != 0:
        raise RuntimeError('Failed to partition CFCache.')


def makePartitions(msname : str, cfcache : str, libra_install_path : str, verbose : bool):
    if not libra_install_path:
        libra_install_path = libra_home_install_path

    msbasename  = msname.split('/')[-1].replace('.ms','')
    cfcbasename = cfcache.split('/')[-1].replace('.cfc','')

    nparts = partitionMS(msname, libra_install_path, verbose)
    partitionCF(cfcache, libra_install_path, verbose)

    return msbasename, cfcbasename, nparts


def getTimeStr(intime : datetime):
    log_time_fmt = '%Y-%m-%d %H:%M:%S'
    return f'{intime.strftime(log_time_fmt):24}'

