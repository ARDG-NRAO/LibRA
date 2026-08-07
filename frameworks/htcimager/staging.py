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
from glob import glob
import shutil
import tarfile
from jobmode import jobmode

class staging(object):
    def __init__(self, scheduler, inputArgs):
        self.scheduler = scheduler
        self.scratchdir = ''
        self.addStartResidual = []
        
        if scheduler == 'HTCondor':
            self.scratchdir = os.environ['_CONDOR_SCRATCH_DIR']


        if not inputArgs.workdir:
            inputArgs.workdir = 'working' if self.scheduler == 'HTCondor' else '.'
        if not inputArgs.logdir:
            inputArgs.logdir = f'{inputArgs.workdir}/logs'

        self.workdir = inputArgs.workdir
        self.logdir = inputArgs.logdir

        print(f'Creating directory: {self.workdir}')
        os.makedirs(self.workdir, exist_ok = True)
        print(f'Creating directory: {self.logdir}')
        os.makedirs(self.logdir, exist_ok = True)
        print()

        self.abslogdir = os.path.realpath(self.logdir)

        if jobmode.GATHERPSF in inputArgs.jobmode:
            impars = inputArgs.getImagingParameters()
            startImage = ['useStartModel', 'useStartMask']
            for parfile in list(impars):
                if any(x in list(impars[parfile]) for x in startImage):
                    imagename = inputArgs.imagelist[0].split('.')[0]
                    if eval(impars[parfile]['useStartModel']):
                        self.copyStartImage(imext = 'model', imagename = imagename)
                        print("Found 'useStartModel = True'.")
                    elif eval(impars[parfile]['useStartMask']):
                        self.copyStartImage(imext ='mask', imagename = imagename)
                        print("Found 'useStartMask = True'.")
                    else:
                        print("Parameters 'useStartModel' or 'useStartMask' set to False. Continuing without initial mask and model.")
                else:
                    print("Parameters 'useStartModel' or 'useStartMask' not present. Continuing without initial mask and model.")

        if scheduler == 'HTCondor':
            self.move_to_workdir()

    def move_to_workdir(self):
        extlist = ['ms', 'cfc', 'psf', 'pb', 'mask', 'residual', 'model', 'divmodel', 'weight', 'sumwt']
        movelist = []
        for ext in extlist:
            movelist += glob(f'*.{ext}')
        for file in movelist:
            print(f'Moving {file} to {self.workdir}')
            shutil.move(file, self.workdir)

    def move_to_scratchdir(self, filelist):
        for file in filelist:
            print(f'Moving {file} to {self.scratchdir}')
            shutil.move(file, self.scratchdir)

    def makeTarballs(self, filelist):
        outputList = self.addStartResidual
        for file in filelist:
            print(f'Creating tarball: {file}.tgz')
            with tarfile.open(f'{file}.tgz', 'x:gz') as tarball:
                tarball.add(file)
            outputList.append(f'{file}.tgz')

        return outputList
    
    def copyStartImage(self, imext, imagename):
        for ext in ['weight', 'psf', 'residual']:
            shutil.copytree(f'{imagename}.{imext}', f'{imagename}.{ext}')

        with tarfile.open(f'{imagename}.residual.start.tgz', 'x:gz') as tarball:
            tarball.add(f'{imagename}.residual')

        #self.addStartResidual.append(f'{imagename}.residual.start.tgz')
