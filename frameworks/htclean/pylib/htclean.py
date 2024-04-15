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


import os
import sys

pylib_dir = '../pylib'

sys.path.insert(0, pylib_dir)
from HTCSynthesisImager import HTCSynthesisImager

from htclean_helperfunctions import *


# Older versions of CASA do not have the transition module, so an exception is raised
# also the arguments index change from CASA5 to CASA6
try:
    from casatasks.private.casa_transition import *
    if is_CASA6:
        index = 1
    else:
        raise Exception('not CASA6, using CASA5 argument indexing')
except:
    index = 3


# Read arguments
mode = sys.argv[index]
infile = sys.argv[index + 1]
partId = sys.argv[index + 2]

# import imager parameters
inpars = import_tclean_parameters(infile)
vis = inpars['msname']
imagename = inpars['imagename']

if 'SPW' in partId:
    spw = partId.replace('SPW', '')

# Adjust parameter values according to mode
if mode in ['makePSF', 'runResidualCycle']:
    if not 'serial' in partId:
        inpars['msname'] = vis.split('.ms')[0] + '_' + partId + '.ms' # think about the serial case
        inpars['imagename'] = imagename + '.workdirectory/' + imagename + '.' + partId
elif mode in ['gatherPSF', 'gather', 'scatter', 'runModelCycle', 'makeFinalImages', 'makePrimaryBeam']:
    inpars['msname'] = ''
else:
    raise Exception('unknown mode: ' + mode + '. Check inputs.')

# parameter list has to change if htclean_helperfunctions becomes an import
paramList = ImagerParameters(**inpars)
imager = HTCSynthesisImager(params = paramList)

if 'serial' in partId: partId = ''

# Mode selection
if mode in ['makePSF', 'makePrimaryBeam', 'runResidualCycle']:
    eval('imager.' + mode + '()')
elif mode == 'runModelCycle':
    imager.runModelCycle(partname = partId)
elif mode == 'gatherPSF':
    imager.gather(partname = partId, imtype = 'psf')
elif mode == 'gather':
    imager.gather(partname = partId)
elif mode == 'scatter':
    imager.scatter(partname = partId)
elif mode == 'makeFinalImages':
    imager.makeFinalImages(partname = partId)
else:
    raise Exception('unknown mode: ' + mode + '. Check inputs.')
