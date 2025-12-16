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
from GPUMonitor import GPUMonitor
from setupLibRA import setupLibRA
from subprocess import Popen,PIPE
from datetime import datetime
from jobmode import jobmode, execModes


def writeParfile(impars, parfile, logdir = '.'):
    with open(f'{logdir}/{parfile}', 'w') as outfile:
        for key,value in impars[parfile].items():
            outfile.write(f'{key:22}= {value}\n')

def runtimeWorkarounds(stage):
    jobmode = stage['jobmode']
    if jobmode == jobmode.NORMALIZE:
        imagename,imext = os.path.splitext(stage['outputImages'][0])
        if imext == '.divmodel':
            with open(f'{imagename}.model/table.info','r') as tableinfo:
                lines = tableinfo.readlines()
            for line in lines:
                if 'SubType' in line:
                    lines.remove(line)
            with open(f'{imagename}.model/table.info','w') as tableinfo:
                tableinfo.writelines(lines)


class libra_cl_imager(object):
    def __init__(self, inputArgs):
        self.jobmodeList = inputArgs.jobmode
        self.outputImageList = []
        self.workdir = inputArgs.workdir
        jobname = inputArgs.jobname
        vbbucketsize = inputArgs.vbbucketsize
        usegpu = inputArgs.usegpu
        logdir = os.path.realpath(inputArgs.logdir)
        rlogdir = os.path.relpath(logdir)
        self.datestr = datetime.now().strftime('%Y%m%d-%H%M%S')
        processingList = self._makeProcessingList(inputArgs)

        # Read libra environment from the first parameter file
        parfile = list(inputArgs.getImagingParameters())[0]
        try:
            casadata = list(inputArgs.getImagingParameters(par = 'casa-data')[parfile].values())[0]
        except:
            casadata = None

        bundle = False
        if inputArgs.libra_bundle: #isParameter('libra_bundle'):
            #libra_bundle = list(inputArgs.getImagingParameters(par = 'libra_bundle')[parfile].values())[0]
            libra_bundle = inputArgs.libra_bundle
            self.libra_install = setupLibRA(bundle = libra_bundle, casadata = casadata)
            bundle = True
        elif inputArgs.libra_path: #isParameter('libra_path'):
            #libra_path = list(inputArgs.getImagingParameters(par = 'libra_path')[parfile].values())[0]
            libra_path = inputArgs.libra_path
            self.libra_install = setupLibRA(path = libra_path, casadata = casadata)
        else:
            raise TypeError("Path to LibRA applications not specified. Please provide 'libra_path' or 'libra_bundle' as command line arguments.")
        

        gmon = False
        if any(mode in self.jobmodeList for mode in execModes.gridding.value):
            self.libra_install.setupRoadrunner(bundle, usegpu)
            if usegpu:
                gmon = GPUMonitor()
                gmon.startMonitor(outputfile = f'{rlogdir}/gpumonitor.{jobname}.{self.datestr}.out')
            if vbbucketsize:
                os.environ['VBBUCKETSIZE'] = vbbucketsize
        
        if inputArgs.workdir != '.':
            print(f'Entering directory {inputArgs.workdir}')
            os.chdir(inputArgs.workdir)

        getApp = self.libra_install.getApp
        rlogdir = os.path.relpath(logdir)
        for stage in processingList:
            app = getApp(stage['jobmode'])
            filename = stage['parfile']
            runtimeWorkarounds(stage)
            exitcode = self._clLibRAAppCall(app, filename, logdir = rlogdir)
            if exitcode != 0:
                raise RuntimeError(f'Application {app} failed. Return value: {exitcode}.')
            for image in stage['outputImages']:
                if os.path.exists(image):
                    self.outputImageList.append(image)
                else:
                    raise FileNotFoundError(f'{image}: file not found.')

        if gmon:
            gmon.stopMonitor()
        
        # Remove duplicate image names from the output image list
        self.outputImageList = list(dict.fromkeys(self.outputImageList))


    def _clLibRAAppCall(self, app, filename, logdir = '.'):
        basename, ext = os.path.splitext(filename)
        if ' ' in app:
            cmd = app.split(' ')
        else:
            cmd = [app]
        cmd.append(f'help=def,{logdir}/{filename}')
        cmdtext = ''
        for item in cmd:
            cmdtext += f'{item} '
        print(f'Running LibRA application via command line interface:\n {cmdtext}')
        stagelog = open(f'{logdir}/{basename}.log','w')
        with Popen(cmd, stdout = PIPE, stderr = PIPE) as runner:
            stagelog.write(runner.communicate()[1].decode('ascii'))
            exitcode = runner.wait()
        stagelog.close()
        print()
        return exitcode


    def _makeProcessingList(self, inputArgs):
        jobmodeList = inputArgs.jobmode
        logdir = inputArgs.logdir

        impars = inputArgs.makeUnitParfiles()
        
        parfileList = list(impars)
        processingList = []

        if len(jobmodeList) == 1:
            processingList = [
                {'jobmode' : jobmodeList[0],
                 'parfile' : parfile} for parfile in parfileList]
        elif len(parfileList) == 1:
            processingList = [
                {'jobmode' : jobmode,
                 'parfile' : parfileList[0]} for jobmode in jobmodeList]
        elif len(jobmodeList) == len(parfileList):
            for this_mode,parfile in zip(jobmodeList,parfileList):
                processingList.append(
                    {'jobmode' : this_mode,
                     'parfile' : parfile})
        elif len(parfileList)%len(jobmodeList) == 0:
            k = len(parfileList)/len(jobmodeList)
            for i,parfile in enumerate(parfileList):
                processingList.append(
                    {'jobmode' : jobmodeList[int(i/k)],
                     'parfile' : parfile})
        else:
            raise RuntimeError('Unknown combination of mode(s) and parameter file(s), aborting.')

        for stage in processingList:
            this_mode = stage['jobmode']
            parfile = stage['parfile']
            basename,ext = os.path.splitext(parfile)
            if 'outputImages' not in stage.keys():
                imextlist = this_mode.imtype
                if this_mode == jobmode.GATHERPSF:
                    if 'useStartModel' in list(impars[parfile]) and impars[parfile]['useStartModel'] == 'True':
                        imextlist[1].append('model')
                for j,mode in enumerate(jobmode.list(this_mode)):
                    for imext in imextlist[j]:
                        modename = mode.name.lower()
                        if imext in ['gather', 'normalize']:
                            if not inputArgs.imtype:
                                raise ValueError(f'{mode}: imtype cannot be empty')
                            else:
                                imext = inputArgs.imtype
                        if imext != modename and len(imext) > 0:
                            this_parfile = f'{basename}.{modename}_{imext}.{self.datestr}' + ext
                        else:
                            this_parfile = f'{basename}.{modename}.{self.datestr}' + ext
                        impars[this_parfile] = impars[parfile].copy()
                        this_pars = impars[this_parfile]
                        impars[this_parfile] = self._changeImageParameters(this_pars, mode, imext)
                        writeParfile(impars, this_parfile, logdir)
                        outputImages = [impars[this_parfile]['imagename']]
                        if mode == jobmode.WEIGHT:
                            baseimagename = os.path.splitext(outputImages[0])[0]
                            outputImages.append(f'{baseimagename}.sumwt')
                        elif mode in jobmode.GATHER:
                            outputImages = [impars[this_parfile]['outputimage']]
                        elif mode == jobmode.MODEL:
                            outputImages = [
                                impars[this_parfile]['imagename'] + '.model',
                                impars[this_parfile]['imagename'] + '.mask',
                                impars[this_parfile]['imagename'] + '.residual']
                        elif mode == jobmode.NORMALIZE and imext == 'model':
                            outputImages = [impars[this_parfile]['imagename'] + '.divmodel']
                        elif mode == jobmode.NORMALIZE:
                            outputImages = [impars[this_parfile]['imagename'] + f'.{imext}']
                            if imext == 'psf':
                                outputImages.append(impars[this_parfile]['imagename'] + '.pb') 
                        elif mode == jobmode.RESTORE:
                            outputImages = [impars[this_parfile]['imagename'] + '.image']
                            if impars[this_parfile]['pbcor'] == '1':
                                outputImages.append(impars[this_parfile]['imagename'] + '.image.pbcor')
                        processingList.append({
                            'jobmode' : mode,
                            'parfile' : this_parfile,
                            'outputImages' : outputImages
                        })

        # Remove raw and redundant processing entries
        # (general solution, although only gather was observed to create redundant entries)
        removeProcessingEntries = []    
        for i,stage in enumerate(processingList):
            this_jobmode = stage['jobmode']
            if 'outputImages' not in stage.keys():
                removeProcessingEntries.append(stage)
            else:
                this_outputs = stage['outputImages']
                if i > 0:
                    for pstage in processingList[0:i]:
                        if 'outputImages' in pstage.keys():
                            if this_jobmode == pstage['jobmode'] and this_outputs == pstage['outputImages']:
                                removeProcessingEntries.append(stage)
        
        for stage in removeProcessingEntries:
            if os.path.exists(f"{logdir}/{stage['parfile']}"):
                os.remove(f"{logdir}/{stage['parfile']}")
            if stage in processingList:
                processingList.remove(stage)

        return processingList
            

    def _changeImageParameters(self, this_impars, mode, imext = None):
        appmode = mode.name.lower()
        imagename = this_impars['imagename']
        if mode in execModes.gridding.value:
            if mode == jobmode.RESIDUAL:
                if 'modelimagename' not in this_impars.keys() or len(this_impars['modelimagename']) == 0:
                    modelimagename = f"{imagename.split('.')[0]}.divmodel"
                    if os.path.exists(f'{self.workdir}/{modelimagename}'):
                        this_impars['modelimagename'] = modelimagename
            imext = f'.{mode.name.lower()}'
            imagename += imext
        elif mode in execModes.deconvolution.value:
            imext = ''
            if mode == jobmode.MODEL:
                appmode = ''
        elif mode in execModes.gather.value:
            # add logics in this mode to set overwrite = 1 when using start model and/or mask
            outputimagename = imagename.split('.')[0] if '.' in imagename else imagename
            imagename = f','.join([f'{imname}.{imext}' for imname in this_impars['gatherimagelist']])
            this_impars['outputimage'] = f'{outputimagename}.{imext}'
            this_impars['resetoutput'] = '1'
            this_impars['stats'] = 'all'
        elif mode in execModes.normalization.value:
            appmode = ''
            this_impars['imtype'] = imext
            imagename = imagename.split('.')[0] if '.' in imagename else imagename
            if imext == 'psf':
                this_impars['computepb'] = '1'
        this_impars['imagename'] = imagename
        this_impars['mode'] = appmode
        if 'gatherimagelist' in this_impars.keys():
            this_impars.pop('gatherimagelist')

        return this_impars
    