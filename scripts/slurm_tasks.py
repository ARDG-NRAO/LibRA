#!/bin/python 
# slurm_tasks.py: Script to construct executable commands to run coyote mode=fillcf in parallel.
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
# License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning AIPS++ should be addressed as follows:
#        Internet email: aips2-request@nrao.edu.
#        Postal address: AIPS++ Project Office
#                        National Radio Astronomy Observatory
#                        520 Edgemont Road
#                        Charlottesville, VA 22903-2475 USA
#

import os,sys;

argc=len(sys.argv);
if (argc < 7):
    print("Usage: "+sys.argv[0]+" cfcachdir cfslistfile coyotebin casapath taskid nprocs");
    sys.exit(0);

#------------------------------------------------------------------------------
# User-level options
cfcacheDir=sys.argv[1];#"../temp.cf";
cfsFile=sys.argv[2];#"../cf.list";
coyoteBin=sys.argv[3];#"./libra/bin/coyote";
casapath=sys.argv[4];#"~";
taskID=int(sys.argv[5])-1;
procs=int(sys.argv[6]);

# Makes rather unituitive code due to 0-based counting (in this
# script) and 1-based indexs supplied (is this a Slurm requirement?).
if (taskID >= procs):
    raise ValueError("taskID should =< procs");
    

def makeExecCommand(tasks,procs,taskID,cfList,execCmd):
    # For each taskID, determine the number of tasks it gets (the
    # parameter 'n').  Take care to distribute the tasks which would
    # otherwise be missed due to rounding errors, while making sure
    # all the nprocs processes get used (i.e. they have, reasonably
    # load-balanced work to do).  Then make list of CFS for each
    # taskID (the parameter 'thisList') and from it construct the
    # executable commandline (the parameter 'coyoteExec').
    m=0;
    l0=0; l1=0;
    thisList='';

    for i in range(0,taskID):
        m = m + int((tasks / procs) + int(i < tasks % procs))
    n = int((tasks / procs) + int(taskID < tasks % procs))
    l0=m;
    l1=l0+n-1;

    for j in range(l0,l1):
        thisList=thisList+(cfsList[j]+',');
    thisList=thisList+cfsList[l1];
    #print(taskID,": ",l0,l1,n,thisList);
    cmdLine=execCmd+" cflist=\""+thisList+"\"";

    return cmdLine,n,l0,l1;
#------------------------------------------------------------------------------

# Initialize internal variables. Python seems like a real computer
# langauge at least for uninitialized variables.
coyoteOpts="help=noprompt";
coyoteOpts=coyoteOpts+" mode=fillcf";
coyoteOpts=coyoteOpts+" cfcache=\""+cfcacheDir+"\"";
coyoteExec="CASAPATH="+casapath+" "+coyoteBin+" "+coyoteOpts;

cfsList=[];

# Read the cfsFile, and make an array of lines read after stripping newline
with open(cfsFile, 'r') as f:
    for line in f:
        cfsName=os.path.basename(line.strip())
        if (cfsName != ''): cfsList.append(cfsName);

# No. of tasks is equal to the number of lines in cfsFile
tasks=len(cfsList);        
#print("# No. of CFs: ",len(cfsList),cfsList[0],"~",cfsList[tasks-1]);

cmdLine,n,l0,l1=makeExecCommand(tasks,procs,taskID,cfsList,coyoteExec);
print("ncfs: ",n,"startndx: ",l0,"endndx: ",l1,"total # of cfs: ",tasks);
print(cmdLine);
