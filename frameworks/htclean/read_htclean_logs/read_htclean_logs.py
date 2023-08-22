#!/usr/bin/python3.6

# read_htclean_logs.py
# script to read htclean logs from an input directory

# Import modules
import os
import sys
import pandas as pd
import plotly.express as px

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
condor_logs = []
app_logs = []
jobTimings = []
df = pd.DataFrame()


os.chdir(logdir)
print('Analyzing contents of input directory: ' + logdir)

n_parts  = len(glob('makePSF*.condor.log'))
imcycles = len(glob('runModelCycle.*.condor.log'))

print('Processing logs from ' + str(n_parts) + ' partitions and ' + str(imcycles) + ' imaging cycles.')

# Append makePSF logs to log lists
for i in range(1, n_parts + 1):
    condor_logs += ['makePSF_n' + str(i)]
    applog = glob('casalogs/psf-*.n' + str(i) + '.out')
    app_logs += applog

# Append gatherPSF logs to log lists
condor_logs += ['gatherPSF.imcycle0']
app_logs += glob('casalogs/gatherPSF-*.log')

# Append makeDirtyImage logs to log lists
for i in range(1, n_parts + 1):
    condor_logs += ['makeDirtyImage_n' + str(i)]
    applog = glob('casalogs/dirty-*.n' + str(i) + '.out')
    app_logs += applog

# If separate gather, append gather.imcycle0 to log lists
if len(glob('gather.imcycle*')) > 0:
    condor_logs += ['gather.imcycle0']
    app_logs += [sortAppLogList('casalogs/gather-*.log')[0]]

# Iterate on imaging cycles to append logs to log lists
for imcycle in range(1, imcycles + 1):
    list_index = imcycle - 1
    condor_logs += ['runModelCycle.imcycle' + str(imcycle)]
    app_logs += [sortAppLogList('casalogs/runModelCycle-*.log')[list_index]]
    for i in range(1, n_parts + 1):
        condor_logs += ['runResidualCycle_n' + str(i) + '.imcycle' + str(imcycle)]
        app_logs += [sortAppLogList('casalogs/runResidualCycle-*.n' + str(i) + '.out')[list_index]]
    # If separate gather, append gather.imcycle0 to log lists
    if len(glob('gather.imcycle*')) > 0:
        condor_logs += ['gather.imcycle' + str(imcycle)]
        app_logs += [sortAppLogList('casalogs/gather-*.log')[imcycle]]

# Append makeFinalImages to log lists
if len(glob('makeFinalImages.*.condor.log')) > 0:
    condor_logs += ['makeFinalImages.imcycle' + str(imcycle)]
    app_logs += glob('casalogs/makeFinalImages-*.log')


for i, jobname in enumerate(condor_logs):
    jobTimings += [jobTiming(jobname, app_logs[i])]

for job in jobTimings:
    df = df.append(job.getDataFrame(), ignore_index = True)

print(df)

# Make and display concurrency plot
colors = {'Transfer In': 'rgb(251, 188, 4)',
          'Transfer Out': 'rgb(251, 188, 4)',
          'Job executing': 'rgb(66, 133, 244)',
          'App executing': 'rgb(183, 225, 205)',
          'Gridding': 'rgb(234, 67, 53)'}

fig = px.timeline(df, x_start = 'Start', x_end = 'Finish', y = 'Task', color = 'Action', color_discrete_map = colors)
fig.update_yaxes(autorange = 'reversed')
fig.show()

