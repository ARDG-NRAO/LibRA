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
