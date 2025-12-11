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
import sys
import tarfile
import json
import shutil
from glob import glob


def usage():
    print(f'''
Name:
    checkConvergence.py - process model cycle logs to determine convergence/divergence trends
          
Usage:
    checkConvergence.py [-n] [-h] [-z <package name>] [-l <log name>] [-o <output file>]
                        [-t <divergence threshold>]

Parameters:
    -n, --nopackage    : Do not expand tarball with log files. Default is to look for package with default name.
    -h, --help         : Print this usage summary and exit.
    -l, --logname      : Pattern of model cycle log file names. Default is logs/*.model.*.log.
    -o, --outputfile   : Name of output file. Default is checkConvergence.out.
    -t, --threshold    : Divergence threshold. Default is 0.
    -z, --packagename  : Name pattern of tarball containing log files. Default is ../logs.runModelCycle.imcycle*.tgz.
'''
    )

args = sys.argv
dag_excode = 0

divergence_threshold = 0
packagename = '../logs.runModelCycle.imcycle*.tgz'
logname = 'logs/*.model.*.log'
outputfile = '../checkConvergence.out'

if len(args) > 1:
    args = args[1:]
    readArg = ''
    for arg in args:
        if readArg:
            exec(f"{readArg} = '{arg}'")
            readArg = ''
        elif arg in ['-z', '--packagename']:
            readArg = 'packagename'
        elif arg in ['-l', '--logname']:
            readArg = 'logname'
        elif arg in ['-o', '--outputfile']:
            readArg = 'outputfile'
        elif arg in ['-t','--threshold']:
            readArg = 'divergence_threshold'
        elif arg in ['-n','--nopackage']:
            packagename = ''
        elif arg in ['-h','--help']:
            usage()
            sys.exit(0)
        else:
            usage()
            raise ValueError(f'{arg}: unknown argument.')

try:
    with open('imagingCycle.dag.metrics', 'r') as infile:
        dag_metrics = json.load(infile)
    if dag_metrics['exitcode'] == 10:
        sys.exit(2)
except:
    print('Did not find file imagingCycle.dag.metrics.')


divergence_threshold = float(divergence_threshold)
    
if packagename:
    packagelist = glob(packagename)
    if len(packagelist) > 1:
        for package in packagelist:
            with tarfile.open(package) as tar:
                tar.extractall(filter = 'tar')
    
loglist = glob(logname)
loglist.sort()

outfile = open(outputfile, 'w')

peakreslist = []
converged = False
for i, logfile in enumerate(loglist):
    with open(logfile, 'r') as log:
        for line in log.readlines():
            if 'peakres=' in line:
                logmsg = line.split('\t')[3].split(',')
                outfile.writelines(logmsg)
                peakres = logmsg[2].replace('peakres=','').strip().split('->')
                message = logmsg[3]
                if any(msg in message for msg in ['Reached n-sigma threshold', 'Reached global stopping criteria']):
                    print('Converged!')
                    converged = True
                    break
                peakres = [float(num) for num in peakres]
                peakreslist += peakres
                 
d_peakreslist = [y-x for x,y in zip(peakreslist[:-1],peakreslist[1:])]
peakres_trend = sum(d_peakreslist)/len(d_peakreslist)

outfile.writelines(f'\n')
outfile.writelines(f'd_peakres = {d_peakreslist}\n')
outfile.writelines(f'peakres_trend = {peakres_trend}\n')

if packagename:
    for file in loglist:
        os.remove(file)

if converged:
    excode = 0
elif peakres_trend > 0:
    divmsg = f'Possibly diverging: the derivative of peakres is greater than the threshold of {divergence_threshold}.'
    print(divmsg)
    outfile.writelines(divmsg)
    excode = 2
else:
    excode = 1

outfile.close()
sys.exit(excode)