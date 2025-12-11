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

sys.path.insert(0, bin_dir)

from htclean_helperfunctions import partitionMS


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
msname     = sys.argv[index]
partaxis   = sys.argv[index + 1]
nparts     = int(sys.argv[index + 2])

partitionMS(msname, partaxis, nparts)
