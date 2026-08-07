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
import shutil
from glob import glob
from jobmode import jobmode, execModes

class setupLibRA(object):

    def __init__(self, bundle = None, path = None, casadata = None):
        if (path and bundle) or (not path and not bundle):
            raise TypeError('Not allowed: provide either bundle OR path.')
        
        if not casadata:
            pwd = os.getcwd()
            casadata = pwd + '/casa-data'

        homedir = os.environ['HOME']
        if os.path.isdir(casadata):
            pwd = os.getcwd()
            with open(homedir + '/.casarc', 'w') as casarc:
                casarc.write('measures.directory: ' + casadata)
        
        if bundle:
            try:
                os.system(f'sh {bundle} libra')
            except Exception as e:
                raise e
            self.path = os.path.realpath('libra')
        elif path:
            self.path = path
        
        self.install = self.path + '/bin'
        self.griddingApp = self.path + '/bin/roadrunner'
        self.gatherApp = self.path + '/bin/chip'
        self.normalizationApp = self.path + '/bin/dale'
        self.deconvolutionApp = self.path + '/bin/hummbee'
        self.rrenviron = False


    def setupRoadrunner(self, bundle = False, usegpu = True):
        if bundle and usegpu:
            bundles_dir = glob(self.path + '/bundles/*')[0]
            dest = bundles_dir + '/lib64'
            if os.path.isfile('/.singularity.d/libs/libcuda.so.1'):
                print(f'Copying /.singularity.d/libs/libcuda.so.1 to {dest}')
                shutil.copy('/.singularity.d/libs/libcuda.so.1',dest)
            elif os.path.isfile('/lib64/libcuda.so.1'):
                print(f'Copying /lib64/libcuda.so.1 to {dest}')
                shutil.copy('/lib64/libcuda.so.1',dest)
            else:
                print('libcuda.so.1: File not found on known paths. Trying with packaged version')
        elif not usegpu:
            print('Setting HPGDEVICE = opium')
            os.environ['HPGDEVICE'] = 'opium'
            self.griddingApp = f'{self.path}/bin/kokkos/hpcbind -v -- {self.path}/bin/roadrunner'
        
        try:
            LDlibraryPath = os.environ['LD_LIBRARY_PATH']
        except:
            LDlibraryPath = 'environment variable not set.'
        
        print(f'LD_LIBRARY_PATH: {LDlibraryPath}')
        print('Setting OMP_PROC_BIND = false')
        os.environ['OMP_PROC_BIND'] = 'false'
        print('Setting NPROCS = 0')
        os.environ['NPROCS'] = '0'
        self.rrenviron = True


    def getApp(self, mode):
        for item in list(execModes):
            if mode in execModes[item.name].value:
                app = getattr(self, item.name + 'App')
        return app
