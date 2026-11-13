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
import sys
import tarfile
import json
import shutil
import argparse
from glob import glob


parser = argparse.ArgumentParser(description = 'process model cycle logs to determine convergence/divergence trends')
parser.add_argument('-z', '--packagename', default = '../logs.runModelCycle*.tgz',
    help = 'Name pattern of tarball containing log files.')
parser.add_argument('-l', '--logname', default = 'logs/*.model.*.log',
    help = 'Pattern of model cycle log file names.')
parser.add_argument('-o', '--outputfile', default = '../checkConvergence.out',
    help = 'Name of output file.')
parser.add_argument('-t', '--threshold', default = 0,
    help = 'Divergence threshold.')
parser.add_argument('-d', '--dagname', default = 'imagingCycle.dag',
    help = 'If given, will look for variable exitcode in file <dagname>.metrics.')
parser.add_argument('-n', '--nopackage', action = 'store_true',
    help = 'Do not expand tarball with log files.')
parser.add_argument('--nodag', action = 'store_true',
    help = 'Do not read dag metrics file.')
inputargs = parser.parse_args()

dag_excode = 0
divergence_threshold = inputargs.threshold
packagename          = inputargs.packagename if not inputargs.nopackage else ''
logname              = inputargs.logname
outputfile           = inputargs.outputfile
dagname              = inputargs.dagname if not inputargs.nodag else ''


if dagname:
    dagname = f'{dagname}.dag' if '.dag' not in dagname else dagname
    try:
        with open(f'{dagname}.metrics', 'r') as infile:
            dag_metrics = json.load(infile)
        if dag_metrics['exitcode'] == 10:
            sys.exit(2)
        # when restarting a run that completed any imaging cycles, remove the rescue file now
        if dag_metrics['dag_nodes_failed'] == 0:
            rescuefilelist = glob(f'{dagname}.rescue*')
            if len(rescuefilelist) > 0:
                for rescuefile in rescuefilelist:
                    os.remove(rescuefile)
    except:
        print(f'Did not find file {dagname}.metrics.')


divergence_threshold = float(divergence_threshold)
    
if packagename:
    packagelist = glob(packagename)
    if len(packagelist) > 0:
        for package in packagelist:
            with tarfile.open(package) as tar:
                tar.extractall(filter = 'tar')
    
loglist = glob(logname)

if len(loglist) == 0:
    raise ValueError(f"{logname}: couldn't find log files matching this name pattern.")

loglist.sort()

outfile = open(outputfile, 'w')

peakreslist = []
converged = False
for logfile in loglist:
    with open(logfile, 'r') as log:
        for line in log.readlines():
            if 'peakres=' in line:
                logmsg = line.split('\t')[3].split(',')
                outfile.writelines(logmsg)
                peakres = logmsg[2].replace('peakres=','').strip().split('->')
                message = logmsg[3]
                if any(msg in message for msg in ['Reached n-sigma threshold', 'Reached global stopping criterion']):
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
elif peakres_trend > divergence_threshold:
    divmsg = f'Possibly diverging: the derivative of peakres is greater than the threshold of {divergence_threshold}.'
    print(divmsg)
    outfile.writelines(f'{divmsg}\n')
    excode = 2
else:
    excode = 1

outfile.close()
sys.exit(excode)
