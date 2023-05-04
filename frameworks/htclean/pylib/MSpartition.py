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
