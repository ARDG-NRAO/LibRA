#!/usr/bin/python3.6

# getIMCycleTimings.py
# script to read Imaging Cycle timings from htclean logs in given input directory

# Import modules
import os
import sys


from glob import glob

from jobTiming import jobTiming


# Read input arguments
try:
    logdir = sys.argv[1]
except:
    logdir = '.'


# Function definitions

def sortAppLogList(appLogNamePattern):
    applog = glob(appLogNamePattern)
    applog.sort()
    return applog


# Begin execution block
condor_logs_model = []
app_logs_model = []
jobTimings_model = []
condor_logs_residual = []
app_logs_residual = []
jobTimings_residual = []


os.chdir(logdir)
print('Analyzing contents of input directory: ' + logdir)

n_parts  = len(glob('runResidualCycle*.imcycle1.condor.log'))
imcycles = len(glob('runModelCycle.*.condor.log'))

print('Processing logs from ' + str(n_parts) + ' partitions and ' + str(imcycles) + ' imaging cycles.')


# Iterate on imaging cycles to append logs to log lists
for imcycle in range(1, imcycles + 1):
    list_index = imcycle - 1
    condor_logs_model += ['runModelCycle.imcycle' + str(imcycle)]
    app_logs_model += [sortAppLogList('casalogs/runModelCycle-*.log')[list_index]]
    for i in range(1, n_parts + 1):
        condor_logs_residual += ['runResidualCycle_n' + str(i) + '.imcycle' + str(imcycle)]
        app_logs_residual += [sortAppLogList('casalogs/runResidualCycle-*.n' + str(i) + '.out')[list_index]]



for i, jobname in enumerate(condor_logs_model):
    jobTimings_model += [jobTiming(jobname, app_logs_model[i])]
    print(jobTimings_model[-1].jobname + ', ' + str(jobTimings_model[-1].getTransferTime()) + ', ' + str(jobTimings_model[-1].getTotalTime()))

for i, jobname in enumerate(condor_logs_residual):
    jobTimings_residual += [jobTiming(jobname, app_logs_residual[i])]
    print(jobTimings_residual[-1].jobname + ', ' + str(jobTimings_residual[-1].getTransferTime()) + ', ' + str(jobTimings_residual[-1].getTotalTime()))






