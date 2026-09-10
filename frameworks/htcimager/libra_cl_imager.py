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
from GPUMonitor import GPUMonitor
from setupLibRA import setupLibRA
from subprocess import Popen,PIPE
from datetime import datetime
from jobmode import jobmode, execModes


def writeParfile(impars, parfile, logdir = '.'):
    with open(f'{logdir}/{parfile}', 'w') as outfile:
        for key,value in impars[parfile].items():
            outfile.write(f'{key:22}= {value}\n')


# -------- taylor / coyote CLI parameter builders --------
# Ported directly from the validated argument construction in
# scripts/run_taylor_fixed.py (TaylorPipeline._taylor_* / _build_cfcaches):
# these do not re-derive the Taylor math or cfcache build sequence, they
# reproduce the exact CLI arguments already proven correct there, adapted to
# the per-stage parfile dict built by _makeProcessingList/_changeImageParameters.
# 'gatherimagelist' (the per-SPW image basenames) is populated the same way
# GATHER modes already require it (see inputArgs.makeUnitParfiles).

def _spwImages(basenames, suffix):
    return [f'{base}.{suffix}' for base in basenames]


def _buildComputeAvgPBParams(impars, cubetype=None):
    basenames = impars['gatherimagelist']
    outputimagename = impars['imagename']
    impars['cubeImage'] = ','.join(_spwImages(basenames, 'residual'))
    impars['pbimage'] = ','.join(_spwImages(basenames, 'pb'))
    impars['taylorImages'] = ''
    impars['sumwtImage'] = ','.join(_spwImages(basenames, 'taylorwt'))
    impars['overwrite'] = '1'
    impars['mode'] = 'computeavgpb'
    impars.setdefault('avgpbname', f'{outputimagename}.avgpb')
    impars.setdefault('minfreqpbname', f'{outputimagename}.minfreq')
    impars['avgpbmode'] = 'mean'
    return impars


def _buildRemoveFreqDepPBParams(impars, cubetype=None):
    imtype = cubetype or 'residual'
    basenames = impars['gatherimagelist']
    impars['cubeImage'] = ','.join(_spwImages(basenames, imtype))
    impars['pbimage'] = ','.join(_spwImages(basenames, 'pb'))
    impars['taylorImages'] = ''
    impars['sumwtImage'] = ''
    impars['overwrite'] = '1'
    impars['mode'] = 'removefreqdepPB'
    # avgpbname must already have been set by a prior computeavgpb stage
    return impars


def _buildCube2TaylorParams(impars, cubetype=None):
    # cubetype: 'residual', 'psf', 'pb' or 'sumwt' - selects which per-SPW
    # cube is Taylor-summed (run_taylor_fixed.py's
    # TaylorPipeline._taylor_cube2taylor(imtype)). Passed via the imext
    # suffix, e.g. '-t cube2taylor_psf', the same way NORMALIZE's dale calls
    # already vary by imext (e.g. '-t psf' vs '-t residual').
    imtype = cubetype or 'residual'
    basenames = impars['gatherimagelist']
    outputimagename = impars['imagename']

    taylorwts = _spwImages(basenames, 'taylorwt')
    cube_images = taylorwts if imtype == 'sumwt' else _spwImages(basenames, imtype)
    pb_list = _spwImages(basenames, 'pb') + [impars['avgpbname']]

    impars['cubeImage'] = ','.join(cube_images)
    impars['taylorImages'] = f'{outputimagename}.{imtype}'
    impars['pbimage'] = ','.join(pb_list)
    impars['sumwtImage'] = ','.join(taylorwts)
    impars['overwrite'] = '1'
    impars['mode'] = 'cube2taylor'
    impars['imtype'] = imtype
    # reffreq, nTerms, pblimit are expected to already be static parfile keys
    return impars


def _buildTaylor2CubeParams(impars, cubetype=None):
    basenames = impars['gatherimagelist']
    outputimagename = impars['imagename']
    nterms = int(impars['nTerms'])
    taylor_images = [f'{outputimagename}.model.tt{i}' for i in range(nterms)]

    impars['cubeImage'] = ','.join(_spwImages(basenames, 'model'))
    impars['taylorImages'] = ','.join(taylor_images)
    # pbimage='' skips the internal applyPB(divide, pbnames[0]); avgPB
    # removal / per-SPW PB apply would be explicit applyPB stages, but the
    # validated pipeline (run_taylor_fixed.py) feeds roadrunner's awp
    # degridder the true-sky model directly and never calls applyPB here -
    # see stage_prepare_model's docstring there.
    impars['pbimage'] = ''
    impars['sumwtImage'] = ','.join(_spwImages(basenames, 'sumwt'))
    impars['overwrite'] = '1'
    impars['mode'] = 'taylor2cube'
    impars['pblimit'] = '0'
    impars['imtype'] = 'model'
    return impars


# Keyed by the taylor-mode prefix of the imext string (e.g. 'cube2taylor' out
# of '-t cube2taylor_psf'). applyPB is intentionally not wired in as an
# active stage: run_taylor_fixed.py's validated pipeline never calls it (the
# old applyPB+dale-divmodel chain over-subtracted by 1/PB - see that file's
# Bug 4 note and stage_prepare_model docstring).
TAYLOR_PARAM_BUILDERS = {
    'computeavgpb': _buildComputeAvgPBParams,
    'removefreqdeppb': _buildRemoveFreqDepPBParams,
    'cube2taylor': _buildCube2TaylorParams,
    'taylor2cube': _buildTaylor2CubeParams,
}


def _buildCoyoteCommonParams(impars):
    impars.setdefault('telescope', 'EVLA')
    impars.setdefault('wbawp', '1')
    impars.setdefault('aterm', '1')
    impars.setdefault('psterm', '0')
    impars.setdefault('muellertype', 'diagonal')
    impars.setdefault('dpa', '360')
    impars.setdefault('buffersize', '0')
    impars.setdefault('oversampling', '20')
    return impars


def _buildCoyoteDryrunParams(impars):
    impars = _buildCoyoteCommonParams(impars)
    impars['mode'] = 'dryrun'
    impars['cflist'] = ''
    return impars


def _buildCoyoteFillcfParams(impars):
    impars = _buildCoyoteCommonParams(impars)
    impars['mode'] = 'fillcf'
    impars['cflist'] = 'CFS*'
    return impars


COYOTE_PARAM_BUILDERS = {
    'dryrun': _buildCoyoteDryrunParams,
    'fillcf': _buildCoyoteFillcfParams,
}

def runtimeWorkarounds(stage, undo = False):
    this_mode = stage['jobmode']
    imagename,imext = os.path.splitext(stage['outputImages'][0])
    if not undo:
        if this_mode == jobmode.NORMALIZE:
            if imext == '.divmodel':
                with open(f'{imagename}.model/table.info','r') as tableinfo:
                    lines = tableinfo.readlines()
                for line in lines:
                    if 'SubType' in line:
                        lines.remove(line)
                with open(f'{imagename}.model/table.info','w') as tableinfo:
                    tableinfo.writelines(lines)
        elif this_mode == jobmode.MODEL or this_mode == jobmode.RESTORE:
            shutil.move(f'{imagename}.sumwt',f'{imagename}.sumwt.noop')
    else:
        if this_mode == jobmode.MODEL or this_mode == jobmode.RESTORE:
            shutil.move(f'{imagename}.sumwt.noop',f'{imagename}.sumwt')
        elif this_mode == jobmode.TAYLOR and stage.get('taylorMode') == 'taylor2cube':
            # taylor2cube's per-SPW .model cubes inherit the 'residual
            # normalized' SubType from their source residual images. dale's
            # isNormalized() guard (dale.cc:305) then skips normalization on
            # every cycle after the first when roadrunner/dale next reads
            # these as the model. Equivalent of
            # run_taylor_fixed.py::_strip_normalized_subtype.
            for cubeimage in stage['outputImages']:
                info_path = f'{cubeimage}/table.info'
                if os.path.exists(info_path):
                    with open(info_path, 'r') as tableinfo:
                        lines = tableinfo.readlines()
                    newlines = []
                    for line in lines:
                        if line.startswith('SubType'):
                            stripped = (line.replace(' normalized', '')
                                            .replace('normalized', '')
                                            .rstrip('\n').rstrip())
                            if stripped.endswith('='):
                                stripped = 'SubType = model'
                            line = stripped + '\n'
                        newlines.append(line)
                    with open(info_path, 'w') as tableinfo:
                        tableinfo.writelines(newlines)



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
            runtimeWorkarounds(stage, undo = True)
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
        # the code below fails whem jobmode = gather,normalize and len(parfilelist) (= number of partitions) is an odd number
        # the current fix in inputArgs.makeUnitParfiles() works for the above mode with a single parameter file,
        # but was not tested against a wider range of use cases
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
                        if imext in ['gather', 'normalize', 'taylor', 'coyote']:
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
                        elif mode == jobmode.TAYLOR:
                            taylormode = impars[this_parfile]['mode']
                            if taylormode == 'computeavgpb':
                                outputImages = [impars[this_parfile]['avgpbname'],
                                                 impars[this_parfile]['minfreqpbname']]
                            elif taylormode in ('removefreqdepPB', 'applyPB', 'taylor2cube'):
                                # taylor2cube's actual output is the per-SPW
                                # .model cubes (cubeImage); taylorImages
                                # (tt0/tt1) are its inputs.
                                outputImages = impars[this_parfile]['cubeImage'].split(',')
                            else:
                                # cube2taylor
                                outputImages = impars[this_parfile]['taylorImages'].split(',')
                        elif mode == jobmode.COYOTE:
                            outputImages = [impars[this_parfile]['cfcache']]
                        stageDict = {
                            'jobmode' : mode,
                            'parfile' : this_parfile,
                            'outputImages' : outputImages
                        }
                        if mode == jobmode.TAYLOR:
                            stageDict['taylorMode'] = taylormode
                        processingList.append(stageDict)

        # Remove raw entries (superseded by their expanded per-imtype entries)
        # and redundant duplicate entries (same jobmode + same outputImages),
        # keyed on (jobmode, tuple(outputImages)) rather than an O(n^2) scan.
        removeProcessingEntries = []
        seenStageKeys = set()
        for stage in processingList:
            if 'outputImages' not in stage.keys():
                removeProcessingEntries.append(stage)
                continue
            stageKey = (stage['jobmode'], tuple(stage['outputImages']))
            if stageKey in seenStageKeys:
                removeProcessingEntries.append(stage)
            else:
                seenStageKeys.add(stageKey)

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
                    basename = imagename.split('.')[0]
                    # single-term MFS predicts from dale's normalized
                    # .divmodel; the Taylor path (taylor2cube) writes the
                    # true-sky model directly to .model instead, since
                    # roadrunner's awp degridder applies PB(nu) itself
                    # during predict (see run_taylor_fixed.py stage_prepare_model)
                    for modelext in ('divmodel', 'model'):
                        modelimagename = f'{basename}.{modelext}'
                        if os.path.exists(f'{self.workdir}/{modelimagename}'):
                            this_impars['modelimagename'] = modelimagename
                            break
            imext = f'.{mode.name.lower()}'
            imagename += imext
        elif mode in execModes.deconvolution.value:
            imext = ''
            if mode == jobmode.MODEL:
                appmode = ''
        elif mode in execModes.gather.value:
            if imext != 'sumwt' and any(x in list(this_impars) for x in ['useStartModel', 'useStartMask']):
                if any(eval(this_impars[x]) for x in ['useStartModel', 'useStartMask']):
                    this_impars['overwrite'] = '1'
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
            elif imext == 'taylorpsf':
                # dale imtype=taylorpsf operates on .psf but the SoW image
                # it reads/writes is the raw sumwt, not taylorwt (see
                # scripts/run_taylor_fixed.py::_dale)
                this_impars['sowimage'] = f'{imagename}.sumwt'
        elif mode in execModes.taylor.value:
            # imext carries 'taylormode[_cubetype]', e.g. 'cube2taylor_psf'
            # or 'taylor2cube', passed through via '-t' the same way
            # NORMALIZE's per-call imtype (e.g. '-t psf') already is.
            appmode = ''
            taylormode, _, cubetype = imext.partition('_')
            this_impars = TAYLOR_PARAM_BUILDERS[taylormode](this_impars, cubetype or None)
            appmode = this_impars['mode']
        elif mode in execModes.coyote.value:
            # imext carries the coyote CLI submode (dryrun, fillcf)
            appmode = ''
            this_impars = COYOTE_PARAM_BUILDERS[imext](this_impars)
            appmode = this_impars['mode']
        this_impars['imagename'] = imagename
        this_impars['mode'] = appmode
        if 'gatherimagelist' in this_impars.keys():
            this_impars.pop('gatherimagelist')

        return this_impars
    