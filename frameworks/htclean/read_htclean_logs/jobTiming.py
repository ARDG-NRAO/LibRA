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


# jobTiming.py
# class definition to read and report timings of htclean jobs running on HTCondor

from datetime import datetime, timedelta
import pandas as pd

# Defaults
time_fmt = '%Y-%m-%d %H:%M:%S'


def getCondorLogTimeStr(line, index):
    """
    Gets the time string (in format defined in time_fmt) from a line of an HTCondor log

    Parameters
    ----------
    line : str
        The input line from the HTCondor log
    index : int
        A reference index representing the end of the time string

    Returns
    -------
    str
        a time string as defined in time_fmt
    """
    end   = index - 1
    start = end - 19

    return line[start:end]


def getCASALogTimeStr(line):
    """
    Gets the time string (in format defined in time_fmt) from a line of a CASA or roadrunner log

    Parameters
    ----------
    line : str
        The input line from the CASA/roadrunner log

    Returns
    -------
    str
        a time string as defined in time_fmt

    """
    return line[0:19]


def getTime(timestr):
    """
    Creates a datetime object from a time string (format defined in time_fmt)

    Parameters
    ----------
    timestr : str
        A time string as defined in time_fmt

    Returns
    -------
    datetime
        A datetime object
    """
    return datetime.strptime(timestr, time_fmt)

def getSiteName(line):
    """
    Gets the site name from the line containing 'SlotName' in the HTCondor logs (current implementation
    will only work with PATh slot names)

    Parameters
    ----------
    line : str
        Input line from the HTCondor log that contains 'SlotName'

    Returns
    -------
    siteName : str
        The name of the PATh site where the job ran
    """
    siteName = ((line.partition('@')[2]).partition('osgvo')[0])[:-1]
    return siteName


class jobTiming(object):
    """
    Class used to collect timestamps that enable profiling of htclean runs

    Attributes
    ----------
    jobname : str
        Name of the job
    sitename : str
        Name of the site where the job ran
    jobSubmitted : datetime
        Job submission time as a datetime object
    transferIn : dict('start' : datetime, 'end' : datetime)
        Transfer input files start/end as a dictionary of datetime objects
    jobExecuting : dict('start' : datetime, 'end' : datetime)
        Job execution start/end as a dictionary of datetime objects
    appExecuting : dict('start' : datetime, 'end' : datetime)
        Application (eg. CASA/roadrunner) execution start/end as a dictionary of datetime objects
    gridding : dict('start' : datetime, 'end' : datetime)
        Gridding start/end (roadrunner only) as a dictionary of datetime objects
    transferOut : dict('start' : datetime, 'end' : datetime)
        Transfer output files start/end as a dictionary of datetime objects

    Methods
    -------
    getAppExecuting(key)
        Returns the start/end of application execution as defined in key
    getDataFrame()
        Returns a Pandas DataFrame object with job timing information
    getGridding(key)
        Returns the start/end of gridding (roadrunner only) as defined in key
    getIdleTime()
        Returns the elapsed time in seconds for which the job was idle
    getJobExecuting(key)
        Returns the start/end of job execution as defined in key
    getTotalTime()
        Retuns the total duration of the job in seconds
    getTransferIn(key)
        Returns the start/end of transfer of input files as defined in key
    getTransferOut(key)
        Returns the start/end of transfer of output files as defined in key
    getTransferTime(direction='')
        Returns the transfer time in seconds, for input, output or both
    """

    def __init__(self, jobname, applogfile):
        """
        Parameters
        ----------
        jobname : str
            The name of the job (as defined in the DAG definitions - htclean.dag and subDAGs)
        applogfile : str
            The name of the application (CASA/roadrunner) logfile
        """
        self.jobname                  = jobname
        self.sitename                 = self._readSiteName()
        self.jobSubmitted             = self._readSubmissionTime()
        self.transferIn               = self._readTransferTime(direction = 'input')
        self.jobExecuting             = self._readJobExecutionTime()
        self.appExecuting             = self._readAppExecutionTime(applogfile)
        self.gridding                 = self._readGriddingTime(applogfile)
        self.transferOut              = self._readTransferTime(direction = 'output')
        self._local2UTC()


    def _readSiteName(self):
        """
        Reads the name of the site where the job executed from the HTCondor log
        """
        logfile = self.jobname + '.condor.log'
        with open(logfile, 'r') as log:
            for line in log.readlines():
                if 'SlotName' in line:
                    siteName = getSiteName(line)
        return siteName
        

    def _readSubmissionTime(self):
        """
        Reads the job submission time from the HTCondor log
        """
        logfile = self.jobname + '.condor.log'
        with open(logfile, 'r') as log:
            for line in log.readlines():
                if 'Job submitted' in line:
                    index = line.find('Job submitted')
                    time_str = getCondorLogTimeStr(line, index)
                    submitted_time = getTime(time_str)
        return submitted_time


    def _readTransferTime(self, direction):
        """
        Reads the start/end times of a transfer of input or output files from the HTCondor log

        Parmeters
        ---------
        direction : str
            The direction of the transfer. Allowed values are 'input' and 'output'

        Returns
        -------
        dict('start' : datetime, 'end' : datetime)
            The start/end of the transfer as defined in direction
        """
        logfile =  self.jobname + '.condor.log'
        start_str = 'Started transferring ' + direction + ' files'
        end_str = 'Finished transferring ' + direction + ' files'
        with open(logfile, 'r') as log:
            for line in log.readlines():
                if start_str in line:
                    index = line.find(start_str)
                    time_str = getCondorLogTimeStr(line, index)
                    transfer_start_time = getTime(time_str)
                elif end_str in line:
                    index = line.find(end_str)
                    time_str = getCondorLogTimeStr(line, index)
                    transfer_end_time = getTime(time_str)
                    transfer_time = { 'start': transfer_start_time, 'end' : transfer_end_time }
        return transfer_time


    def _readJobExecutionTime(self):
        """
        Reads the start/end times of job execution from the HTCondor log

        Returns
        -------
        dict('start' : datetime, 'end' : datetime)
            The start/end of the job execution
        """
        logfile = self.jobname + '.condor.log'
        with open(logfile, 'r') as log:
            for line in log.readlines():
                if 'Job executing' in line:
                    index = line.find('Job executing')
                    time_str = getCondorLogTimeStr(line, index)
                    execution_start_time = getTime(time_str)
                elif 'Started transferring output files' in line:
                    index = line.find('Started transferring output files')
                    time_str = getCondorLogTimeStr(line, index)
                    execution_end_time = getTime(time_str)
                    jobExecuting = { 'start' : execution_start_time , 'end' : execution_end_time }
        return jobExecuting


    def _readAppExecutionTime(self, applogfile):
        """
        Reads the start/end times of app (CASA/roadrunner) execution from the app log

        Returns
        -------
        dict('start' : datetime, 'end' : datetime)
            The start/end of the app execution
        """
        jobname = self.jobname
        if 'runModelCycle' in jobname or 'gather' in jobname or 'makeFinalImages' in jobname:
            # CASA job - read timestamps from .log file; 1st and last line
            with open(applogfile, 'r') as log:
                loglines = log.readlines()
                start_time_str = getCASALogTimeStr(loglines[0])
                end_time_str = getCASALogTimeStr(loglines[-1])
        elif 'makePSF' in jobname or 'makeDirtyImage' in jobname or 'runResidualCycle' in jobname:
            # Roadrunner job - read timestamps from .out file; 1st line and line containing roadrunner::main.*done
            with open(applogfile, 'r') as log:
                loglines = log.readlines()
                for line in loglines[0:5]:
                    if 'Opening' in line:
                        start_time_str = getCASALogTimeStr(line)
                for line in loglines[1:-1]:
                    if ('roadrunner::main' in line or 'roadrunner::Roadrunner_func' in line) and '...done' in line:
                        end_time_str = getCASALogTimeStr(line)

        try:
            app_start_time = getTime(start_time_str)
            app_end_time = getTime(end_time_str)
            appExecuting = { 'start' : app_start_time, 'end' : app_end_time }
        except:
            print('Failed to read application start/end times. Check ' + applogfile + ' for errors.')
            appExecuting = None
        return appExecuting


    def _readGriddingTime(self, applogfile):
        """
        Reads the start/end times of gridding from the app (roadrunner) log

        Returns
        -------
        dict('start' : datetime, 'end' : datetime)
            The start/end of gridding
        """
        jobname = self.jobname
        if 'makePSF' in jobname or 'makeDirtyImage' in jobname or 'runResidualCycle' in jobname:
            with open(applogfile, 'r') as log:
                for line in log.readlines():
                    if 'AWProjectFT2::init[R&D]' in line: # keyword derived from gridding duration, double check
                        start_time_str = getCASALogTimeStr(line)
#                        gridding_start_time = getTime(start_time_str)
                    elif 'rows/s' in line:
                        end_time_str = getCASALogTimeStr(line)
#                        gridding_end_time = getTime(end_time_str)
#                        gridding = { 'start' : gridding_start_time, 'end' : gridding_end_time }
                try:
                    gridding_start_time = getTime(start_time_str)
                    gridding_end_time = getTime(end_time_str)
                    gridding = { 'start' : gridding_start_time, 'end' : gridding_end_time }
                except:
                    gridding = None
                    print('Failed to read gridding start/end times. Check ' + applogfile + ' for errors.')
        else:
            gridding = None
        return gridding


    def getTransferIn(self, key):
        """
        Returns the start/end of transfer of input files as defined in key

        Parameters
        ----------
        key : str
            A keyword specifying the timestamp. Use 'start' or 'end'.

        Returns
        -------
        datetime
            A datetime object containing the specified timestamp (start/end) of the input transfer
        """
        return self.transferIn[key]

    def getTransferOut(self, key):
        """
        Returns the start/end of transfer of output files as defined in key

        Parameters
        ----------
        key : str
            A keyword specifying the timestamp. Use 'start' or 'end'.

        Returns
        -------
        datetime
            A datetime object containing the specified timestamp (start/end) of the output transfer
        """
        return self.transferOut[key]

    def getJobExecuting(self, key):
        """
        Returns the start/end of job execution as defined in key

        Parameters
        ----------
        key : str
            A keyword specifying the timestamp. Use 'start' or 'end'.

        Returns
        -------
        datetime
            A datetime object containing the specified timestamp (start/end) of the job execution
        """
        return self.jobExecuting[key]

    def getAppExecuting(self, key):
        """
        Returns the start/end of application execution as defined in key

        Parameters
        ----------
        key : str
            A keyword specifying the timestamp. Use 'start' or 'end'.

        Returns
        -------
        datetime
            A datetime object containing the specified timestamp (start/end) of the app execution
        """
        return self.appExecuting[key]

    def getGridding(self, key):
        """
        Returns the start/end of gridding (roadrunner only) as defined in key

        Parameters
        ----------
        key : str
            A keyword specifying the timestamp. Use 'start' or 'end'.

        Returns
        -------
        datetime
            A datetime object containing the specified timestamp (start/end) of gridding
        """
        return self.gridding[key]

    def getIdleTime(self):
        """
        Returns the elapsed time in seconds for which the job was idle

        Returns
        -------
        float
            Elapsed seconds for which the job was idle (between submission time and transfer in start)
        """
        return (self.transferIn['start'] - self.jobSubmitted).total_seconds()

    def getTransferTime(self, direction = ''):
        """
        Returns the transfer time in seconds, for input, output or both

        Parameters
        ----------
        direction : str
            A keyword specifying the direction of the transfer.
            Allowed values are 'input', 'output' and ''.
            Default is '', which means aggregate (input + output)

        Returns
        -------
        float
            Duration of the transfer (or aggregate transfer time if direction='') in seconds
        """
        transfer_in = (self.transferIn['end'] - self.transferIn['start']).total_seconds()
        transfer_out = (self.transferOut['end'] - self.transferOut['start']).total_seconds()
        if direction.lower() in ['input', 'in']:
            return transfer_in
        elif direction.lower() in ['output', 'out']:
            return transfer_out
        else:
            return (transfer_in + transfer_out)

    def getTotalTime(self):
        """
        Retuns the total duration of the job in seconds

        Returns
        -------
        float
            Duration of the job in seconds
        """
        return (self.transferOut['end'] - self.transferIn['start']).total_seconds()

    def _local2UTC(self):
        """
        Converts local times from the HTCondor log to UTC
        """
        if self.appExecuting is not None:
            utc = self.appExecuting['start']
            local = self.jobExecuting['start']
#            if abs((local - utc).total_seconds()) > 3600:
            local2UTC = int((local - utc).total_seconds() / 3600 )
            for key in ['start', 'end']:
                self.appExecuting[key] += timedelta(hours = local2UTC)
                if self.gridding is not None:
                    self.gridding[key] += timedelta(hours = local2UTC)
#                self.transferIn[key]   -= timedelta(hours = local2UTC)
#                self.transferOut[key]  -= timedelta(hours = local2UTC)
#                self.jobExecuting[key] -= timedelta(hours = local2UTC)
        else:
            print('No application time for ' + self.jobname + '. Local time not converted to UTC.')


    def getDataFrame(self):
        """
        Returns a Pandas DataFrame object with job timing information

        Returns
        -------
        pandas.DataFrame
            DataFrame object. Fields are Task, Start, End, Action, SiteName.
        """
        jobname = self.jobname
        siteName = self.sitename
        startTransferIn   = datetime.strftime(self.transferIn['start'], time_fmt)
        endTransferIn     = datetime.strftime(self.transferIn['end'], time_fmt)
        startTransferOut  = datetime.strftime(self.transferOut['start'], time_fmt)
        endTransferOut    = datetime.strftime(self.transferOut['end'], time_fmt)
        startJobExecuting = datetime.strftime(self.jobExecuting['start'], time_fmt)
        endJobExecuting   = datetime.strftime(self.jobExecuting['end'], time_fmt)
        if self.appExecuting is not None:
            startAppExecuting = datetime.strftime(self.appExecuting['start'], time_fmt)
            endAppExecuting   = datetime.strftime(self.appExecuting['end'], time_fmt)
        if self.gridding is not None:
            startGridding = datetime.strftime(self.gridding['start'], time_fmt)
            endGridding   = datetime.strftime(self.gridding['end'], time_fmt)

        df = pd.DataFrame([
            dict(Task = jobname, Start = startTransferIn, Finish= endTransferIn, Action = 'Transfer In',
                SiteName = ''),
            dict(Task = jobname, Start = startTransferOut, Finish = endTransferOut, Action = 'Transfer Out',
                SiteName = siteName),
            dict(Task = jobname, Start = startJobExecuting, Finish = endJobExecuting, Action = 'Job executing',
                SiteName = ''),
#            dict(Task = jobname, Start = startAppExecuting, Finish = endAppExecuting, Action = 'App executing',
#                SiteName = ''),
        ])
        if 'startAppExecuting' in dir():
            df = df.append(dict(Task = jobname,
                Start = startAppExecuting, Finish = endAppExecuting, Action = 'App executing', SiteName = ''),
                ignore_index = True)
        if 'startGridding' in dir():
            df = df.append(dict(Task = jobname,
                Start = startGridding, Finish = endGridding, Action = 'Gridding', SiteName = ''),
                ignore_index = True)

        return df
