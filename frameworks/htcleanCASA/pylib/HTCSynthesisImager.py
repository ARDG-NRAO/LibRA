# Copyright (C) 2021
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


#from imagerhelpers.imager_base import PySynthesisImager
#from imagerhelpers.input_parameters import ImagerParameters
#from casac import casac
#from mstransform_cli import mstransform_cli as mstransform
import glob
import shutil


import os
import sys

# get is_CASA6 and is_python3
try:
    from casatasks.private.casa_transition import *
    if is_CASA6:
        from casatasks import casalog
    
        from casatasks.private.imagerhelpers.imager_base import PySynthesisImager
        from casatasks.private.imagerhelpers.input_parameters import ImagerParameters
        from casatools import synthesisimager, synthesisdeconvolver, synthesisnormalizer, iterbotsink, ctsys, table
    
        ctsys_hostinfo = ctsys.hostinfo
        _tb = table()
    else:
        raise Exception('not CASA6, importing CASA5 modules')
except:
    from taskinit import *

    from imagerhelpers.imager_base import PySynthesisImager
    from imagerhelpers.input_parameters import ImagerParameters
    synthesisimager = casac.synthesisimager
    synthesisdeconvolver = casac.synthesisdeconvolver
    synthesisnormalizer = casac.synthesisnormalizer
    # make it look like the CASA6 version even though it's using the CASA5 named tool not present in CASA6
    iterbotsink = casac.synthesisiterbot

    ctsys_hostinfo = casac.cu.hostinfo

    _tb = tb




class HTCSynthesisImager(PySynthesisImager):

    def _mkImagePartList(self, imgname, imtype, partname):
        partnames=[];
        tmp = glob.glob(imgname + '.workdirectory/' + imgname + "." + partname + "*." + imtype + '*');
        for imname in tmp:
            if '.tt' not in imname:
                partnames.append(imname.replace('.' + imtype, ''))
            elif '.tt0' in imname:
                partnames.append(imname.replace('.' + imtype + '.tt0', ''))
        return partnames;


    def _gather(self, imtype, partname, scatter):
        imgname = self.allimpars['0']['imagename'];
        partlist = self._mkImagePartList(imgname, imtype, partname);

        for immod in range(self.NF):
            normpars = self.allnormpars[str(immod)];
            if len(partlist) > 1:
                normpars['partimagenames'] = partlist;
            print('gather_normpars: {}'.format(normpars))

        self.PStools.append(synthesisnormalizer())

        for immod in range(self.NF):
            self.PStools[immod].setupnormalizer(normpars=normpars)
            self.IBtool.endmajorcycle()
            if imtype == 'psf':
                self.PStools[immod].gatherpsfweight();
                self.PStools[immod].dividepsfbyweight();
            else:
                print('Reusing existing psf and weights')


            if imtype == 'residual':
                self.PStools[immod].gatherresidual();
                self.PStools[immod].divideresidualbyweight();
                if os.path.isdir(imgname + '.model') or os.path.isdir(imgname + '.model.tt0'):
                    self.PStools[immod].multiplymodelbyweight()

#           PB computation to be done separately
                if not os.path.isdir(imgname + '.pb') and not os.path.isdir(imgname + '.pb.tt0'):
                    self.PStools[immod].normalizeprimarybeam();
                else:
                    print('Reusing existing primary beam')
        if not scatter:
            for immod in range(self.NF):
                self.PStools[immod].done()

    def gather(self, imtype = 'residual', partname = 'SPW', scatter = False):
        self.initializeDeconvolvers()
        self.initializeIterationControl()

        self._gather(imtype, partname, scatter)

    def _scatter(self, imtype, partname, gather):
        imgname = self.allimpars['0']['imagename'];
        partlist = self._mkImagePartList(imgname, imtype, partname);

        for immod in range(self.NF):
            normpars = self.allnormpars[str(immod)];
            if len(partlist) > 1:
                normpars['partimagenames'] = partlist;
            print('scatter_normpars: {}'.format(normpars))
    
        if not gather:
            self.PStools.append(synthesisnormalizer())

        for immod in range(self.NF):
            self.PStools[immod].setupnormalizer(normpars=normpars)
            self.PStools[immod].dividemodelbyweight()
            self.PStools[immod].scattermodel()

        if gather:
            for immod in range(self.NF):
                self.PStools[immod].done()

    def scatter(self, imtype = 'residual', partname = 'SPW', gather = False):
        self.initializeDeconvolvers()
        self.initializeIterationControl()

        self._scatter(imtype, partname, gather)

    def makePSF(self):
        self.initializeImagers()
        self.initializeNormalizers()
        self.setWeighting()

        self.makePSFCore()

    def makePrimaryBeam(self):
        self.initializeImagers()
        self.initializeNormalizers()
        self.setWeighting()

        self.makePB()

    def runResidualCycle(self, lastcycle = False):
        imgname = self.allimpars['0']['imagename']

        print('residualCycle:parameters: {}'.format(self.allimpars['0']))

        self.initializeImagers()
        self.setWeighting()

        self.runMajorCycleCore(lastcycle)


    def runModelCycle(self, imtype = 'residual', partname = 'SPW', gather = True, scatter = True):
        self.initializeDeconvolvers()
        self.initializeIterationControl()

        if gather:
            self._gather(imtype, partname, scatter)
            imgname = self.allimpars['0']['imagename'];
            shutil.copytree(imgname + '.residual', imgname + '.residual.noiter')

        stop = self.hasConverged()

        self.updateMask()
    
        print('modelCycle:parameters: {}'.format(self.allimpars['0']))
    
        self.runMinorCycle()
        if scatter:
            self._scatter(imtype, partname, gather)

        if self.hasConverged():
            os.system('touch stopIMCycles')

    def makeFinalImages(self, imtype = 'residual', partname = 'SPW', gather = True, scatter = False):
        self.initializeDeconvolvers()
        self.initializeIterationControl()

        if gather:
            self._gather(imtype, partname, scatter = False)

        self.restoreImages()
        self.pbcorImages()

    def runModelCycle2(self, imtype = 'residual', partname = 'SPW'):
        self.initializeDeconvolvers()
        self.initializeIterationControl()

        stop = self.hasConverged()

        self.updateMask()

        print('modelCycle:parameters: {}'.format(self.allimpars['0']))

        if not self.hasConverged():
            self.runMinorCycle()
            self._scatter(imtype, partname)
        else:
            os.system('touch stopIMCycles')

    def runModelCycle3(self, imtype = 'residual', partname = 'SPW'):
        self.initializeDeconvolvers()
        self.initializeIterationControl()

        isit = self.hasConverged()

        self.updateMask()

        print('modelCycle:parameters: {}'.format(self.allimpars['0']))

        self.runMinorCycle()
        self._scatter(imtype, partname)

        self.updateMask()

        if self.hasConverged():
            os.system('touch stopIMCycles')

