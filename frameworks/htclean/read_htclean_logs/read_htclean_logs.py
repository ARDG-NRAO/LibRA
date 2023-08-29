#!/usr/bin/python3.6

# read_htclean_logs.py
# script to read htclean logs from an input directory

# Import modules
import sys

from htcleanTiming import htcleanTiming


# Read input arguments
try:
    logdir = sys.argv[1]
except:
    logdir = '.'


this_run = htcleanTiming(input_dir = logdir, running_executable = True)
this_run.concurrencyPlot()
