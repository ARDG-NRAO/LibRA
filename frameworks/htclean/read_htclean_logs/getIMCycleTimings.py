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


#!/usr/bin/python3.6

# getIMCycleTimings.py
# script to read Imaging Cycle timings from htclean logs in given input directory

# Import modules

import sys

from htcleanTiming import htcleanTiming

# Read input arguments
try:
    logdir = sys.argv[1]
except:
    logdir = '.'

try:
    output = sys.argv[2]
except:
    output = 'csv'

this_run = htcleanTiming(running_executable = True)
this_run.queryTotalTimes(jobType = 'model', output_format = output)
this_run.queryTotalTimes(jobType = 'residual', output_format = output, header = False)
