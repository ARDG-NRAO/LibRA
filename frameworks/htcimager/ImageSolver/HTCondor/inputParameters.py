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

from imageSolverHelpers import *


class inputParameters(object):

    def __init__(self, parfile):
        if os.path.exists(parfile):
            try:
                with open(parfile, 'r') as infile:
                    inpars = json.load(infile)
                    jsonInput = True
            except json.decoder.JSONDecodeError:
                inpars = {}
                jsonInput = False
                with open(parfile, 'r') as infile:
                    print(f'{parfile}: not a JSON file, trying to read as text.')
                    for line in infile.readlines():
                        if line != '\n' and '#' not in line[0]:
                            line = line.split('#')[0]
                            if len(line) > 0:
                                key, value = line.split('=')
                                key = key.strip()
                                value = value.strip()
                                if ',' in value and '"' not in value: value = value.split(',')
                                inpars[key] = value
        else:
            raise FileNotFoundError(f'{parfile}: file not found.')

        if jsonInput:
            self.htcpars    = inpars['HTCondorConfig']
            self.imgpars    = inpars['GeneralImagingParameters']
            self.gridpars   = inpars['GriddingParameters']
            self.deconvpars = inpars['DeconvolutionParameters']
            self.normpars   = inpars['NormalizationParameters']
        else:
            self.htcpars     = dict.fromkeys(['memResidual', 'memModel', 'min_disk_residual', 'min_disk_model',
                                              'projectdir', '+ProjectName', 'initialdir', 'dataorigin', 'softwaredir', 'cuda_cap',
                                              'libra_bundle', 'imagesdir', 'cfcachedir', 'msdir', 'und_sites_grid',
                                              'notify_user', 'htcimagerdir', 'wantOSPool', '+SingularityImage', 'NRPexclusive', 
                                              'disconnected_timeout', 'min_pelican_version', 'min_gpu_capability',
                                              'min_gpu_memory', 'min_gpu_driver', 'casa'], '')
            self.imgpars     = dict.fromkeys(['useStartModel', 'useStartMask',
                                              'msnamelist', 'cfcachelist', 
                                              'baseimagename', 'pblimit', 'allspwvis', 'nmajor'], '')
            self.gridpars   = dict.fromkeys(['datacolumn', 'sowimageext', 'complexgrid', 'imsize', 
                                              'cell', 'stokes', 'reffreq', 'phasecenter', 'weighting', 
                                              'rmode', 'robust', 'wprojplanes', 'gridder', 'wbawp', 
                                              'field', 'spw', 'uvrange', 'pbcor', 'conjbeams', 
                                              'usepointing', 'pointingoffsetsigdev'], '')
            self.deconvpars = dict.fromkeys(['deconvolver', 'scales', 'largestscale', 'fusedthreshold', 
                                              'nterms', 'nchan' 'cycleniter', 'nsigma', 'gain', 'threshold', 
                                              'cyclefactor', 'mask', 'specmode'], '')
            self.normpars   = dict.fromkeys(['normtype', 'imtype', 'resetimages', 'computepb'], '')

            for this_dict in [self.htcpars, self.imgpars, self.gridpars, self.deconvpars, self.normpars]:
                for key in list(this_dict):
                    if key in list(inpars):
                        this_dict[key] = inpars[key]

            self.imgpars['msnamelist'] = self.imgpars['msnamelist'].split(',')
            self.imgpars['cfcachelist'] = self.imgpars['cfcachelist'].split(',')

        self.multiterm = (self.deconvpars['deconvolver'] == 'mtmfs' and self.deconvpars['nterms'] >= 1)

        self.cfcachelist   = self.imgpars['cfcachelist']
        self.msnamelist    = self.imgpars['msnamelist']
        self.baseimagename = self.imgpars['baseimagename']
        self.nmajor        = self.imgpars['nmajor']

        typeError = ''
        if type(self.msnamelist) not in [dict, list]:
            typeError = 'msnamelist'
        elif type(self.cfcachelist) not in [dict, list]:
            typeError = 'cfcachelist'
        
        if typeError:
            raise(f'{typeError}: unknown format, must be a list or a dictionary.')
        
        self.imgnamelist = makePartialImageNameList(msnamelist = self.msnamelist, imagename = self.baseimagename)
        
        if len(self.cfcachelist) > 1 and len(self.cfcachelist) != len(self.msnamelist):
            raise ValueError('Review inputs: cfcachelist has multiple entries, but the number of entries does not match msnamelist')
        elif len(self.cfcachelist) == 1:
            self.cfcachelist = [self.cfcachelist[0] for msname in self.msnamelist]
        elif type(self.cfcachelist) == list and type(self.msnamelist) == dict:
            self.cfcachelist = convertCFListToDict(cflist = self.cfcachelist, msdict = self.msnamelist)

        projectdir      = self.htcpars['projectdir']
        dataorigin      = self.htcpars['dataorigin']
        self.initialdir = self.htcpars['initialdir']
        self.imagesdir  = self.htcpars['imagesdir']
        self.msdir      = self.htcpars['msdir']
        self.cfcachedir = self.htcpars['cfcachedir']

        for this_dir in ['initialdir', 'imagesdir', 'msdir', 'cfcachedir']:
            for condorvar in ['$(projectdir)', '$(dataorigin)']:
                value = getattr(self, this_dir)
                if condorvar in value:
                    this_var = locals()[condorvar[2:-1]]
                    setattr(self, this_dir, value.replace(condorvar, this_var))