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
import pandas as pd
import plotly.express as px
from datetime import datetime, timedelta

from glob import glob

from jobTiming import jobTiming

# Function definitions

def sortAppLogList(appLogNamePattern):
    """
    Creates a sorted list of CASA/roadrunner logs based on an input name pattern.

    Parameters
    ----------
    appLogNamePattern : str
        The CASA/roadrunner log name pattern

    Returns
    -------
    appLog : list
        A sorted list of CASA/roadrunner log names
    """
    applog = glob(appLogNamePattern)
    applog.sort()
    return applog


class htcleanTiming():
    """
    Class used to retrieve and store information from htclean logs.

    ...

    Attributes
    ----------
    jobTimings : list of jobTiming objects (from jobTiming.py)
        A list containing jobTiming objects for each job in the htclean run


    Methods
    -------
    concurrencyPlot()
        Makes a detailed plot showing all the jobs in the run as bars in a timeline
    queryTotalTimes(jobType = '', jobSite = '', output_format = 'table', header = True)
        Prints job names, site names, total transfer times (input + output) and job walltimes
    queryTranferTimes(self, jobType = '', jobSite = '', output_format = 'table', header = True)
        Prints job names, site names, input transfer times and output transfer times
    """

    def __init__(self, input_dir = '.', running_executable = False):
        """
        Parameters
        ----------
        input_dir : str
            The directory containing all the htclean logs
        running_executable : bool, optional
            A flag used to modify output text based on whether the function is called from an executable
            or a python shell (default is False)
        """

        condor_logs = []
        app_logs = []
        jobTimings = []
        
        if input_dir != '.': os.chdir(input_dir)
        print('Analyzing contents of input directory: ' + str(os.getcwd()))
        n_parts  = len(glob('makePSF*.condor.log'))
        imcycles = len(glob('runModelCycle.*.condor.log'))
        print('Processing logs from ' + str(n_parts) + ' partitions and ' + str(imcycles) + ' imaging cycles.')

        # Append makePSF logs to log lists
        for i in range(1, n_parts + 1):
            condor_logs += ['makePSF_n' + str(i)]
            applog = glob('casalogs/psf-*.n' + str(i) + '.out')
            app_logs += applog
        
        # Append gatherPSF logs to log lists
        if len(glob('gatherPSF.*')) > 0:
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
            if imcycles == 0: imcycle = 1
            condor_logs += ['makeFinalImages.imcycle' + str(imcycle)]
            app_logs += glob('casalogs/makeFinalImages-*.log')
        
        for i, jobname in enumerate(condor_logs):
            jobTimings += [jobTiming(jobname, app_logs[i])]

        if not running_executable:
            print("Done. All timing information is available in <object_name>.jobTimings as a list of jobTiming objects.\n\
Some simple queries that can be done on the data are (where N is the job index, <> are optional arguments):\n\
\t>>> <object_name>.jobTimings[N].jobname\n\
\t>>> <object_name>.jobTimings[N].sitename\n\
\t>>> <object_name>.jobTimings[N].getTransferTime(<direction>)\n\
\t>>> <object_name>.jobTimings[N].getTotalTime()\n\n\
Higher level functions available are:\n\
\t>>> <object_name>.queryTransferTimes(<jobType>, <jobSite>, <output_format>, <header>)\n\
\t>>> <object_name>.queryTotalTimes(<jobType>, <jobSite>, <output_format>, <header>)\n\
\t>>> <object_name>.concurrencyPlot()")
        else:
            print("This output is generated by htcleanTiming._getJobTimings().\n\
Import module htcleanTiming in a Python shell or program for more log parsing functionality.")

        self.jobTimings = jobTimings

    def plotConcurrencyCount(self, timebin = 1):
        """
        Makes a detailed plot showing the number of jobs running in each timebin interval.

        Parameters
        ----------
        timebin : float, optional
            Duration of the bin interval, in minutes
            Default is 1
        """

        df = pd.DataFrame()

        start_time = self.jobTimings[0].transferIn['start']
        end_time   = self.jobTimings[-1].transferOut['end']

        time = start_time

        while time <= end_time:
            bin_count = 0
            max_time = time + timedelta(minutes = timebin)
            center_time = time + timedelta(minutes = timebin / 2)
            for job in self.jobTimings:
                if job.transferIn['start'] < time and job.transferOut['end'] > max_time :
                    bin_count += 1
            df = df.append(dict(Time = center_time, Count = bin_count), ignore_index = True)
            time += timedelta(minutes = timebin)

        print(df)

        fig = px.bar(df, x = 'Time', y = 'Count')
        fig.show()


    def concurrencyPlot(self):
        """
        Makes a detailed plot showing all the jobs in the run as bars in a timeline
        """

        df = pd.DataFrame()
        for job in self.jobTimings:
            df = df.append(job.getDataFrame(), ignore_index = True)

        print(df)

        # Make and display concurrency plot
        colors = {'Transfer In': 'rgb(251, 188, 4)',
                  'Transfer Out': 'rgb(251, 188, 4)',
                  'Job executing': 'rgb(66, 133, 244)',
                  'App executing': 'rgb(183, 225, 205)',
                  'Gridding': 'rgb(234, 67, 53)'}
        
        fig = px.timeline(df, x_start = 'Start', x_end = 'Finish', y = 'Task', color = 'Action', color_discrete_map = colors, text = 'SiteName')
        fig.update_yaxes(autorange = 'reversed')
        fig.update_traces(textposition='outside')
        fig.show()

    def queryTotalTimes(self, jobType = '', jobSite = '', output_format = 'table', header = True):
        """
        Prints job names, site names, total transfer times (input + output) and job walltimes

        Parameters
        ----------
        jobType : str, optional
            Print lines matching "jobType in <object_name>.jobTimings[N].jobname"
            Default is '' (print all lines)
        jobSite : str, optional
            Print lines matching "jobSite in <object_name>.jobTimings[N].sitename"
            Default is '' (print all lines)
        output_format : str, optional
            The formatting of the printed output
            Accepted values are 'table' and 'csv'. Default is 'table'.
        header : bool, optional
            A flag used to print column headers to the console (default is True)
        """

        if output_format not in ['table', 'csv']:
            raise Exception("Unknown output format. Accepted values are 'table' and 'csv'.")

        reported = 0

        if output_format == 'table':
            print_str = '{:40} {:23} {:23} {:20}'
        elif output_format == 'csv':
            print_str = '{}, {}, {}, {}'

        for job in self.jobTimings:
            if jobType.lower() in job.jobname.lower() and jobSite.lower() in job.sitename.lower():
                reported += 1
                if reported == 1 and header == True:
                    print(print_str.format('Job name', 'Site name', 'Transfer time (s)', 'Job walltime (s)'))
                if output_format == 'table': print_str = '{:40} {:20} {:20.1f} {:22.1f}'
                print(print_str.format(job.jobname, job.sitename, job.getTransferTime(), job.getTotalTime()))

        if reported == 0:
            print("No jobs were found matching jobType = " + repr(jobType) + " and jobSite = " + repr(jobSite) + ", check query inputs.")

    def queryTransferTimes(self, jobType = '', jobSite = '', output_format = 'table', header = True):
        """
        Prints job names, site names, input transfer times and output transfer times

        Parameters
        ----------
        jobType : str, optional
            Print lines matching "jobType in <object_name>.jobTimings[N].jobname"
            Default is '' (print all lines)
        jobSite : str, optional
            Print lines matching "jobSite in <object_name>.jobTimings[N].sitename"
            Default is '' (print all lines)
        output_format : str, optional
            The formatting of the printed output
            Accepted values are 'table' and 'csv'. Default is 'table'.
        header : bool, optional
            A flag used to print column headers to the console (default is True)

        """

        if output_format not in ['table', 'csv']:
            raise Exception("Unknown output format. Accepted values are 'table' and 'format'.")

        reported = 0

        if output_format == 'table':
            print_str = '{:40} {:20} {:21} {:20}'
        elif output_format == 'csv':
            print_str = '{}, {}, {}, {}'

        for job in self.jobTimings:
            if jobType.lower() in job.jobname.lower() and jobSite.lower() in job.sitename.lower():
                reported += 1
                if reported == 1 and header == True:
                    print(print_str.format('Job name', 'Site name', 'Transfer IN time (s)', 'Transfer OUT time (s)'))
                if output_format == 'table': print_str = '{:40} {:20} {:20.1f} {:22.1f}'
                print(print_str.format(job.jobname, job.sitename, job.getTransferTime('in'), job.getTransferTime('out')))

        if reported == 0:
            print("No jobs were found matching jobType = " + repr(jobType) + " and jobSite = " + repr(jobSite) + ", check query inputs.")
