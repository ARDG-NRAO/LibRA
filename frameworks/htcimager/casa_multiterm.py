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
import sys
import numpy as np
import inspect

from casaconfig import config
if 'CASADATA' in os.environ.keys():
    config.measurespath = os.environ['CASADATA']
from casatasks.private.imagerhelpers.imager_mtmfs_via_cube import PyMtmfsViaCubeSynthesisImager
from casatasks.private.imagerhelpers.input_parameters import ImagerParameters
from casatasks  import imsubimage,exportfits,importfits,imtrans,imhead,imstat
from casatools  import synthesisutils as su
from casatools  import image as ia

from setupLibRA import setupLibRA
from jobmode    import jobmode, execModes

from subprocess import Popen,PIPE
from datetime   import datetime
from glob       import glob


_su = su()
_ia = ia()

class casa_multiterm(object):
    def __init__(self, inputArgs) -> None:
        if len(inputArgs.parfile) == 1:
            parfile = inputArgs.parfile[0]
        else:
            raise TypeError(f'{__name__}: multiple parameter files are not currently supported.')

        if len(inputArgs.jobmode) == 1:
            self.jobmode = inputArgs.jobmode[0]
        else:
            raise TypeError(f'{__name__}: multiple jobmodes are not currently supported.')

        if self.jobmode == jobmode.TT2CUBE:
            casadata = config.measurespath
            if inputArgs.libra_bundle:
                libra_bundle = inputArgs.libra_bundle
                self.libra_install = setupLibRA(bundle = libra_bundle, casadata = casadata)
            elif inputArgs.libra_path:
                libra_path = inputArgs.libra_path
                self.libra_install = setupLibRA(path = libra_path, casadata = casadata)
            else:
                raise TypeError(f"{__name__}: mode '{jobmode.TT2CUBE.name.lower()}' needs path to LibRA applications to run model normalization. Please provide 'libra_path' or 'libra_bundle' as command line arguments.")

        self.workdir = inputArgs.workdir
        jobname = inputArgs.jobname
        parfile = os.path.splitext(parfile)[0] + '.def'
        inpars = inputArgs.getImagingParameters()[parfile]
        imgpars = list(inspect.signature(ImagerParameters).parameters.keys())
        allspwvis = inpars['allspwvis'] if 'allspwvis' in inpars.keys() else inpars['vis']
        imagename = inpars['baseimagename'] if 'baseimagename' in inpars.keys() else ''
        for par in list(inpars):
            if par not in imgpars:
                del inpars[par]
        for par in imgpars:
            if par not in list(inpars):
                inpars[par] = inspect.signature(ImagerParameters).parameters[par].default
        inpars['msname'] = allspwvis
        inpars['cell'] = str(inpars['cell']) + 'arcsec'
        if imagename != '': inpars['imagename'] = imagename
        inpars['specmode'] = 'mvc'
        inpars['spw'] = '*'
        inpars['pblimit'] = float(inpars['pblimit']) # this setting enables deriving parameters from .def

        logdir = os.path.realpath(inputArgs.logdir)
        
        if inputArgs.workdir != '.':
            print(f'Entering directory {inputArgs.workdir}')
            os.chdir(inputArgs.workdir)

        self.rlogdir = os.path.relpath(logdir)
        self.concatImageList = inputArgs.concatimagelist
        self.fitsconcatpath = inputArgs.fitsconcatpath
        self.nstokes = inputArgs.nstokes
        self.logname = parfile
        self.datestr = datetime.now().strftime('%Y%m%d-%H%M%S')

        self.usefitsconcat = False
        if os.path.isdir(self.fitsconcatpath):
            self.fitsconcatpath = os.path.realpath(self.fitsconcatpath)
            print(f'Using local copy of fitsconcat: {self.fitsconcatpath}')
            sys.path.insert(0, f'{self.fitsconcatpath}')
            import fitsconcat

            self.usefitsconcat = True
            self._fitsconcat = fitsconcat
        
        self._createImagerObj(inpars)
        self.baseimagename = self.imager.allimpars['0']['imagename']
        self.nterms = self.imager.allnormpars['0']['nterms']
        self.pblimit = self.imager.allnormpars['0']['pblimit']

        self.outputImageList = []

        if self.jobmode == jobmode.RESC2TT:
            self._concatenateCubes(cycle = 'residual')
            self._makeTTResidualImages()
        elif self.jobmode == jobmode.PSFC2TT:
            self._concatenateCubes(cycle = 'initial')
            self._makeTTSupportImages()
        elif self.jobmode == jobmode.TT2CUBE:
            self._makeCubeModel()
            self._extractPerSPWModel(spwlist = inputArgs.spwlist)
        
        for imagename in self.outputImageList:
            self._printImageInfo(imagename = imagename, printFreq = True)


    def _printImageInfo(self, imagename: str, printFreq : bool = False) -> None:
        infostr = f'imagename: {imagename:40} max: {imstat(f'{imagename}')['max']}'
        if printFreq: infostr = f'{infostr:85} freq: {imhead(f'{imagename}')['refval'][3]} Hz'
        print(infostr)


    def _makeTTSupportImages(self) -> list:
        immod = 0
        suffixes = ['pb', 'psf', 'sumwt']
        try:
            self.imager.cube2tt(immod, suffixes=suffixes)
            self.imager.mfsImager.PStools[immod].makepsfbeamset()
        except Exception as e:
            raise(e)
        
        numterms = [1, 2 * self.nterms - 1, 2 * self.nterms - 1]
        for imext, N in zip(suffixes, numterms):
            self.outputImageList += [f'{self.baseimagename}.{imext}.tt{n}' for n in range(N)]


    def _makeTTResidualImages(self) -> list:
        immod = 0
        suffixes = ['residual']
        try:
            _su.remove_freq_dep_pb(cubename = self.baseimagename,
                                   mtname = self.baseimagename,
                                   pblimit = np.fabs(self.pblimit))
            self.imager.cube2tt(immod, suffixes = suffixes)
        except Exception as e:
            raise(e)
        
        numterms = [self.nterms]
        for imext, N in zip(suffixes, numterms):
            self.outputImageList += [f'{self.baseimagename}.{imext}.tt{n}' for n in range(N)]


    def _concatenateCubes(self, cycle: str) -> None:
        if cycle == 'initial':
            imtypelist = ['weight', 'sumwt', 'pb', 'psf']
        elif cycle == 'residual':
            imtypelist = ['residual']
    
        for imtype in imtypelist:
            imglist = [f'{imagename}.{imtype}' for imagename in self.concatImageList]
            cubename = f'{self.baseimagename}.{imtype}'
            if os.path.exists(cubename): shutil.rmtree(cubename)
            if not self.usefitsconcat:
                _ia.imageconcat(outfile = cubename, infiles = imglist)
            else:
                print(f'imglist: {imglist}')
                self._runfitsconcat(outfile = cubename, infiles = imglist)
            self.outputImageList.append(cubename)
            print(f'Done concatenating cube: {cubename}')
            self._printImageInfo(imagename = cubename, printFreq = True)


    def _makeCubeModel(self) -> None:
        immod = 0
        self.imager.tt2cube(immod)
        _su.apply_freq_dep_pb(cubename = self.baseimagename,
                              mtname = self.baseimagename,
                              pblimit = np.fabs(self.pblimit))
        self.outputImageList.append(f'{self.baseimagename}.model')
    
    
    def _extractPerSPWModel(self, spwlist : list = []) -> None:
        # this function needs a review. There's an implicit naming assumption that may break
        # if the spw key in the input json file is not of the form 'spw<index>'
        imtype = 'model'

        if spwlist == ['']:
            print(self.concatImageList)
            spwlist = range(len(self.concatImageList))

        for imagename,ispw in zip(self.concatImageList, spwlist):
            perSPWimage = f'{imagename}.{imtype}'
            perSPWNormImage = f'{imagename}.divmodel'
            print(f'Extracting plane: {perSPWimage}')
            if os.path.exists(perSPWimage): shutil.rmtree(perSPWimage)
            imsubimage(imagename = f'{self.baseimagename}.{imtype}', outfile = perSPWimage, chans = str(ispw))
            # and normalize:
            # workaround noop in dale due to normalized model
            os.system(f"sed -i 's/SubType =  normalized/SubType =/' {perSPWimage}/table.info")
            print(f'Normalizing image: {perSPWimage}')
            self._normalizeModel(imagename = perSPWimage, spw = f'spw{ispw}')
            self.outputImageList += [perSPWimage, perSPWNormImage]

    
    def _createImagerObj(self, inpars: dict) -> None:
        paramList = ImagerParameters(**inpars)
        self.imager = PyMtmfsViaCubeSynthesisImager(params = paramList)
        self.imager.initializeNormalizers()

    
    def _normalizeModel(self, imagename: str, spw: str) -> int:
        app = self.libra_install.getApp(jobmode.NORMALIZE)
        basename, ext = os.path.splitext(self.logname)
        logname = f'{self.rlogdir}/normmodel_{spw}_{self.datestr}.log'
        if ' ' in app:
            cmd = app.split(' ')
        else:
            cmd = [app]
        cmd.append(f'help=noprompt')
        cmd.append(f'imagename={imagename}')
        cmd.append(f'imtype=model')
        cmd.append(f'pblimit={self.pblimit}')
        cmdtext = ''
        for item in cmd:
            cmdtext += f'{item} '
        print(f'Running LibRA application via command line interface:\n {cmdtext}')
        stagelog = open(logname,'w')
        with Popen(cmd, stdout = PIPE, stderr = PIPE) as runner:
            stagelog.write(runner.communicate()[1].decode('ascii'))
            exitcode = runner.wait()
        stagelog.close()
        print()
        return exitcode
    

    def _runfitsconcat(self, outfile, infiles) -> None:
        print(f'Concatenating cube {outfile}')

        imglist = []
        fitscube = f'{outfile}.fits'
        cubetotr = f'{outfile}.tr'
        
        for filename in infiles:
            fitsname = f'{filename}.fits'
            print(f'filename: {filename}')
            exportfits(imagename = filename, fitsimage = fitsname)
            imglist.append(fitsname)

        self._fitsconcat.make_empty_image(imlist = imglist, outname = fitscube, nstokes = self.nstokes)
        self._fitsconcat.fill_cube_with_images(imlist = imglist, outname = fitscube, nstokes = self.nstokes)

        importfits(fitsimage = fitscube, imagename = cubetotr)
        imtrans(imagename = cubetotr, outfile = outfile, order = '0132')

        # cleanup intermediate files
        os.remove(fitscube)
        shutil.rmtree(cubetotr)
        for filename in imglist:
            os.remove(filename)
