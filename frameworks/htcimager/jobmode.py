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

from enum import Enum,Flag,auto

class jobmode(Flag):
    WEIGHT           = auto()
    PSF              = auto()
    RESIDUAL         = auto()
    MODEL            = auto()
    GATHER           = auto()
    NORMALIZE        = auto()
    RESTORE          = auto()
    MAKEPSF          = WEIGHT | PSF
    GATHERPSF        = GATHER | NORMALIZE
    RUNRESIDUALCYCLE = RESIDUAL
    MAKEDIRTYIMAGE   = RESIDUAL
    RUNMODELCYCLE    = MODEL | NORMALIZE

    @classmethod
    def ismode(self, mode):
        return mode.upper() in list(self.__members__)
    
    @classmethod
    def list(self, mode):
        modelist = []
        for imode in self:
            if imode in mode:
                modelist.append(imode)
        if len(modelist) > 1 and mode in modelist:
            modelist.remove(mode)
        return modelist
    
    @property
    def imtype(self):
        imtype = []
        if self == self.GATHERPSF:
            imtype = [['weight', 'sumwt', 'psf'], ['psf']]
        elif self == self.RUNMODELCYCLE:
            imtype = [[''], ['model']]
        elif self == self.MAKEPSF:
            imtype = [['weight'], ['psf']]
        else:
            imtype = [[self.name.lower()]]
        return imtype
    

class execModes(Enum):
    gridding = [
        jobmode.MAKEDIRTYIMAGE,
        jobmode.MAKEPSF,
        jobmode.PSF,
        jobmode.RESIDUAL,
        jobmode.RUNRESIDUALCYCLE,
        jobmode.WEIGHT
    ]
    gather = [
        jobmode.GATHER,
        jobmode.GATHERPSF
    ]
    normalization = [
        jobmode.NORMALIZE
    ]
    deconvolution = [
        jobmode.MODEL,
        jobmode.RESTORE,
        jobmode.RUNMODELCYCLE
    ]