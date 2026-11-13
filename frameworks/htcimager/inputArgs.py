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
import ast
import json
import argparse
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
        self._parseargs(args)
        self.printInputArgs()

        # Following are converted to lists at this point
        for attr in ['jobmode', 'parfile', 'mslist', 'cfclist', 'imagelist', 'concatimagelist', 'gatherimagelist','spwlist']:
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
        for parfile in self.parfile:
            if os.path.exists(parfile):
                this_pars = {}
                try:
                    with open(parfile, 'r') as infile:
                        inpars = json.load(infile)
                        jsonInput = True
                        for pargroup in list(inpars):
                            if pargroup != 'HTCondorConfig':
                                this_pars.update(inpars[pargroup])

                        parfile = os.path.splitext(parfile)[0] + '.def'

                        if par != None:
                            for key in list(this_pars):
                                if key != par:
                                    del this_pars[key]

                except json.decoder.JSONDecodeError:
                    this_pars = {}
                    jsonInput = False

                    with open(parfile, 'r') as infile:
                        for line in infile.readlines():
                            if line != '\n' and '#' not in line[0]:
                                line = line.split('#')[0].split('=')
                                if "'" in line[1]:
                                    value = ast.literal_eval(line[1].strip())
                                else:
                                    value = line[1].strip()
                                
                                if not par or line[0].strip() == par:
                                    this_pars[line[0].strip()] = value

                finally:
                    pars[parfile] = this_pars

            else:
                raise FileNotFoundError(f'{parfile}: file not found.')

        return pars
                

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
            if any(mode in execModes.gather.value or mode in execModes.taylor.value
                   for mode in self.jobmode):
                if not self.gatherimagelist:
                    this_pars['gatherimagelist'] = this_pars['imagename']
                    # this is a simple fix for the issue in jobmode = gather,normalize with odd number of images
                    # review and test carefully to verify the validity for a wider range of use cases
                    this_pars['imagename'] = this_pars['baseimagename']
                else:
                    this_pars['gatherimagelist'] = self.gatherimagelist
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
    

    def _parseargs(self, args) -> argparse.ArgumentParser:
        parser = argparse.ArgumentParser(
            description = 'Wrapper to configure environment and execute imaging components of the LibRA algorithm architecture.')
        parser.add_argument('-f', '--parfile', required = True,
            help = 'Name of parameter file to use for imaging. Single value or comma-separated list.')
        parser.add_argument('-j', '--jobmode', required = True,
            help = f"Job mode to execute. Single value or comma-separated list. Supported values: {', '.join([x.lower() for x in jobmode.__members__.keys()])}")
        parser.add_argument('-n', '--jobname', required = True,
            help = 'A string defining the name of the job. Used for naming parameter files and logs.')
        parser.add_argument('-m', '--mslist', default = '',
            help = 'Name of input MS(es). Single value or comma-separated list.')
        parser.add_argument('-c', '--cfclist', default = '',
            help = 'Name of CF cache(s). Single value or comma-separated list (length must match mslist)')
        parser.add_argument('-i', '--imagelist', default = '',
            help = 'Name of output image(s). Single value or comma-separated list.')
        parser.add_argument('-t', '--imtype', default = '',
            help = 'Image type (extension) to normalize (applies only to normalization jobs).')
        parser.add_argument('-w', '--workdir', default = '',
            help = 'Name of working directory. Default is <condor scratch directory>/working for HTCondor jobs and <current directory> for local or SLURM jobs.')
        parser.add_argument('-l', '--logdir', default = '',
            help = 'Name of directory to save log files. Default is <working directory>/logs.')
        parser.add_argument('-b', '--libra_bundle', default = '',
            help = 'Name of relocatable bundle containing LibRA applications, more commonly used on HTCondor jobs.')
        parser.add_argument('-p', '--libra_path', default = '',
            help = 'Path to local LibRA installation, more commonly used with SLURM or local jobs.')
        parser.add_argument('--nogpu', action = 'store_true',
            help = 'Do not use GPUs for processing. Default is to use GPUs for gridding.')
        parser.add_argument('--vbbucketsize', default = '',
            help = 'Size of visibility buffer for gridding jobs.')
        parser.add_argument('--concatimagelist', default = '',
            help = 'Name of input images to concatenate in a cube to make multiterm images in interface casa_multiterm. This is also used in mode TT2CUBE to extract per SPW model images for subtraction in the residual cycle.')
        parser.add_argument('--gatherimagelist', default = '',
            help = 'Name of input images to gather. Comma-separated list, no extension.')
        # following are commented because they are fixed in current use cases
        #parser.add_argument('--spwlist',
        #    help = 'Do not use.')
        #parser.add_argument('--nstokes',
        #    help = '')
        parser.add_argument('--fitsconcatpath', default = '',
            help = "Path to the directory containing fitsconcat.py. If this parameter is not set, use CASA's imageconcat instead to concatenate cubes.")
        parser.add_argument('--interface', default = 'libra_cl',
            help = 'Name of imaging interface to use. Supported values: libra_cl, casa_multiterm, casalith_script, custom.')
        parser.add_argument('--prescript', default = '',
            help = 'Command line to execute script before processing (optional, only for interface = custom).')
        parser.add_argument('--postscript', default = '',
            help = 'Command line to execute script after processing (optional, only for interface = custom).')
        parser.add_argument('--command', default = '',
            help = 'Command line to execute processing application (only for interface = custom).')
        iargs = parser.parse_args(args)

        for arg, value in vars(iargs).items():
            setattr(self, arg, value)

        self.spwlist = ''
        self.nstokes = 1
        self.usegpu = not iargs.nogpu
