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
from jobmode import jobmode, execModes

ia2param = [
    ('mslist','vis'),
    ('cfclist','cfcache'),
    ('imagelist','imagename')
    ]


def splitParamList(key, newdict, pardict, inparam, jobname):
    iterableParameters = ['vis','spw','imagename','cfcache']
    iterableParameters.remove(inparam)
    basename = jobname.replace(',','_')
    ext = os.path.splitext(key)[1]

    for i,this_param in enumerate(pardict[inparam]):
        if type(this_param) == list:
            for j,this_paramj  in enumerate(this_param):
                newkey = basename + '_' + str(i) + '_' + str(j) + ext
                newdict[newkey] = pardict.copy()
                newdict[newkey][inparam] = this_paramj
                updateIterablePars(iterableParameters, newdict, newkey, pardict, inparam, i)
        else:
            newkey = basename + '_' + str(i) + ext
            newdict[newkey] = pardict.copy()
            newdict[newkey][inparam] = this_param
            updateIterablePars(iterableParameters, newdict, newkey, pardict, inparam, i)



def updateIterablePars(iterableParameters, newdict, newkey, pardict, inparam, index):
    for param in iterableParameters:
        if param in list(pardict) and type(pardict[param]) == list:
            if len(pardict[param]) == len(pardict[inparam]):
                newdict[newkey][param] = pardict[param][index]
            elif len(pardict[param]) == 1:
                newdict[newkey][param] = pardict[param][0]


def spwRange2List(pardict):
    spwlist = []
    if 'spw' in list(pardict):
        for spw in pardict['spw']:
            #spw = spw.split(',')
            if '~' in spw:
                s0,sf = (int(x) for x in spw.split('~'))
                spwlist.append([str(i) for i in range(s0,sf + 1)])
            else:
                spwlist.append(spw)

    if len(spwlist) == 1:
        spwlist = spwlist[0]
    
    pardict['spw'] = spwlist


class inputArgs(object):

    def __init__(self, args):
        self.interface = ''
        self.jobmode = ''
        self.parfile = ''
        self.jobname = ''
        self.mslist = ''
        self.cfclist = ''
        self.imagelist = ''
        self.imtype = ''
        self.workdir = ''
        self.logdir = ''
        self.vbbucketsize = ''
        self.libra_path = ''
        self.libra_bundle = ''
        self.usegpu = True
        self.prescript = ''
        self.postscript = ''
        self.command = ''

        if len(args) == 0:
           self._usage()
           raise TypeError('No arguments given')

        readArg = False
        for arg in args:
            if (readArg):
                setattr(self, readArg, arg)
                readArg = ''
            elif arg in ['-j', '--jobmode']:
                readArg = 'jobmode'
            elif arg in ['-f', '--parfile']:
                readArg = 'parfile'
            elif arg in ['-m', '--mslist']:
                readArg = 'mslist'
            elif arg in ['-c', '--cfclist']:
                readArg = 'cfclist'
            elif arg in ['-i', '--imagelist']:
                readArg = 'imagelist'
            elif arg in ['-n', '--jobname']:
                readArg = 'jobname'
            elif arg in ['-t', '--imtype']:
                readArg = 'imtype'
            elif arg in ['-w', '--workdir']:
                readArg = 'workdir'
            elif arg in ['-l', '--logdir']:
                readArg = 'logdir'
            elif arg in ['-b', '--libra_bundle']:
                readArg = 'libra_bundle'
            elif arg in ['-p', '--libra_path']:
                readArg = 'libra_path'
            elif arg == '--vbbucketsize':
                readArg = 'vbbucketsize'
            elif arg == '--interface':
                readArg = 'interface'
            elif arg == '--prescript':
                readArg = 'prescript'
            elif arg == '--postscript':
                readArg = 'postscript'
            elif arg == '--command':
                readArg = 'command'
            elif arg == '--nogpu':
                self.usegpu = False
            elif arg in ['-h','--help']:
                self._usage()
                sys.exit(0)
            else:
                self._usage()
                raise TypeError(arg + ': unknown argument.')

        if not self.interface:
            self.interface = 'libra_cl'
               
        if not self.jobmode:
            raise TypeError('jobmode cannot be empty')
        elif not self.parfile:
            raise TypeError('parfile cannot be empty')
        elif not self.jobname:
            raise TypeError('jobname cannot be empty')
        else:
            self.printInputArgs()
                
        # Following are converted to lists at this point
        for attr in ['jobmode', 'parfile', 'mslist', 'cfclist', 'imagelist']:
            value = getattr(self, attr)
            setattr(self, attr, value.split(','))

        for i, mode in enumerate(self.jobmode):
            if not jobmode.ismode(mode):
                raise TypeError(f'{jobmode}: Unknown mode.')
            else:
                self.jobmode[i] = jobmode[mode.upper()]
                

        for ia,param in ia2param:
            if getattr(self, ia) != [''] and self.isParameter(param):
                raise TypeError('input parameters cannot be passed via command line and parameter file at the same time.')


    def printInputArgs(self):
        print(f"Input arguments:")
        print(f"{'jobmode':24}: {self.jobmode}")
        print(f"{'parfile':24}: {self.parfile}")
        print(f"{'jobname':24}: {self.jobname}")
        if self.mslist:    print(f"{'mslist':24}: {self.mslist}")
        if self.cfclist:   print(f"{'cfclist':24}: {self.cfclist}")
        if self.imagelist: print(f"{'imagelist':24}: {self.imagelist}")
        print()


    def getImagingParameters(self, par = None):
        pars = {}
        try:
            for parfile in self.parfile:
                this_pars = {}
                with open(parfile, 'r') as infile:
                    for line in infile.readlines():
                        if line != '\n' and '#' not in line[0]:
                            line = line.split('#')[0].split('=')
                            if "'" in line[1]:
                                value = eval(line[1].strip())
                            else:
                                value = line[1].strip()
                            
                            if not par or line[0].strip() == par:
                                this_pars[line[0].strip()] = value

                pars[parfile] = this_pars

            return pars
            
        except Exception as e:
            raise e
    

    def makeUnitParfiles(self):
        pars = self.getImagingParameters()

        for ia,param in ia2param:
            if getattr(self, ia) != ['']:
                if len(self.parfile) == 1:
                    parfile = list(pars)[0]
                    pars[parfile][param] = getattr(self, ia)
        
        newpars = {}
        for key in iter(pars):
            this_pars = pars[key]
            if any(mode in execModes.gather.value for mode in self.jobmode):
                this_pars['gatherimagelist'] = this_pars['imagename']
            spwRange2List(this_pars)
            if 'spw' in list(this_pars) and type(this_pars['spw']) == list and len(this_pars['spw']) > 0:
                splitParamList(key, newpars, this_pars, 'spw', self.jobname)
            elif 'vis' in list(this_pars) and type(this_pars['vis']) == list:
                splitParamList(key, newpars, this_pars, 'vis', self.jobname)
            elif 'imagename' in list(this_pars) and type(this_pars['imagename']) == list:
                splitParamList(key, newpars, this_pars, 'imagename', self.jobname)
            else:
                newpars[key] = this_pars

        return newpars


    def isParameter(self, param):
        result = False
        pars = self.getImagingParameters(par = param)

        for key in iter(pars):
            if param in list(pars[key]) and len(pars[key][param]) > 0:
                result = True
                break
    
        return result
    

    def _usage(self):
        print(f'''
Name:
    htcimager.py - wrapper to configure environment and execute imaging components
                   of the LibRA algorithm architecture.

Usage:
    htcimager.py -f <parfile> -j <jobmode> -n <jobname> [--nogpu] [-h]
                [-m <ms list>] [-c <cfcache list>] [-i <image list>] [-t <image type>]
                [-w <working directory>] [-l <log directory>]
                [-b </path/to/libra/bundle>] [-p </path/to/libra/install>]
                [--vbbucketsize <vbbcuketsize>] [--interface <interface>]
                [--prescript <prescript>] [--postscript <postscript>] [--command <command>]

Parameters:
    -f, --parfile      : Name of parameter file to use for imaging. Single value or comma-separated list.
    -j, --jobmode      : Job mode to execute. Single value or comma-separated list.
                         Supported values: {', '.join([x.lower() for x in jobmode.__members__.keys()])}
    -n, --jobname      : A string defining the name of the job. Used for naming parameter files and logs.
    -m, --mslist       : Name of input MS(es). Single value or comma-separated list.
    -c, --cfclist      : Name of CF cache(s). Single value or comma-separated list (length must match mslist)
    -i, --imagename    : Name of output image(s). For gather modes, this becomes the list of input images to gather. Single value or comma-separated list.
    -t, --imtype       : Image type (extension) to normalize (applies only to normalization jobs).
    -w, --workdir      : Name of working directory. Default is <condor scratch directory>/working for HTCondor jobs and <current directory> for local or SLURM jobs.
    -l, --logdir       : Name of directory to save log files. Default is <working directory>/logs.
    -b, --libra_bundle : Name of relocatable bundle containing LibRA applications, more commonly used on HTCondor jobs.
    -p, --libra_path   : Path to local LibRA installation, more commonly used with SLURM or local jobs.
    -h, --help         : Print this usage summary and exit.
    --nogpu            : Do not use GPUs for processing. Default is to use GPUs for gridding.
    --vbbucketsize     : Size of visibility buffer for gridding jobs.
    --interface        : Name of imaging interface to use. Supported values: libra_cl, custom.
    --prescript        : Command line to execute script before processing (optional, only for interface = custom). 
    --postscript       : Command line to execute script after processing (optional, only for interface = custom).
    --command          : Command line to execute processing application (only for interface = custom).
'''
        )