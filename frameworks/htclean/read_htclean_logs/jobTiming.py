# jobTiming.py
# class definition to read and report timings of htclean jobs running on HTCondor

from datetime import datetime, timedelta
import pandas as pd

# Defaults
time_fmt = '%Y-%m-%d %H:%M:%S'


def getCondorLogTimeStr(line):
    return line[19:38]


def getCASALogTimeStr(line):
    return line[0:19]


def getTime(timestr):
    return datetime.strptime(timestr, time_fmt)


class jobTiming(object):
    def __init__(self, jobname, applogfile):
        self.jobname                  = jobname
        self.jobSubmitted             = self._readSubmissionTime()
        self.transferIn               = self._readTransferTime(direction = 'input')
        self.jobExecuting             = self._readJobExecutionTime()
        self.appExecuting             = self._readAppExecutionTime(applogfile)
        self.gridding                 = self._readGriddingTime(applogfile)
        self.transferOut              = self._readTransferTime(direction = 'output')
        self._local2UTC()


    def _readSubmissionTime(self):
        logfile = self.jobname + '.condor.log'
        with open(logfile, 'r') as log:
            for line in log.readlines():
                if 'Job submitted' in line:
                    time_str = getCondorLogTimeStr(line)
                    submitted_time = getTime(time_str)
                    return submitted_time


    def _readTransferTime(self, direction):
        logfile =  self.jobname + '.condor.log'
        start_str = 'Started transferring ' + direction + ' files'
        end_str = 'Finished transferring ' + direction + ' files'
        with open(logfile, 'r') as log:
            for line in log.readlines():
                if start_str in line:
                    time_str = getCondorLogTimeStr(line)
                    transfer_start_time = getTime(time_str)
                elif end_str in line:
                    time_str = getCondorLogTimeStr(line)
                    transfer_end_time = getTime(time_str)
                    transfer_time = { 'start': transfer_start_time, 'end' : transfer_end_time }
                    return transfer_time


    def _readJobExecutionTime(self):
        logfile = self.jobname + '.condor.log'
        with open(logfile, 'r') as log:
            for line in log.readlines():
                if 'Job executing' in line:
                    time_str = getCondorLogTimeStr(line)
                    execution_start_time = getTime(time_str)
                elif 'Started transferring output files' in line:
                    time_str = getCondorLogTimeStr(line)
                    execution_end_time = getTime(time_str)
                    jobExecuting = { 'start' : execution_start_time , 'end' : execution_end_time }
                    return jobExecuting


    def _readAppExecutionTime(self, applogfile):
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
                start_time_str = getCASALogTimeStr(log.readline())
                for line in log.readlines()[1:-1]:
                    if 'roadrunner::main' in line and '...done' in line:
                        end_time_str = getCASALogTimeStr(line)

        app_start_time = getTime(start_time_str)
        app_end_time = getTime(end_time_str)
        appExecuting = { 'start' : app_start_time, 'end' : app_end_time }
        return appExecuting


    def _readGriddingTime(self, applogfile):
        jobname = self.jobname
        if 'makePSF' in jobname or 'makeDirtyImage' in jobname or 'runResidualCycle' in jobname:
            with open(applogfile, 'r') as log:
                for line in log.readlines():
                    if 'AWProjectFT2::init[R&D]' in line: # keyword derived from gridding duration, double check
                        start_time_str = getCASALogTimeStr(line)
                        gridding_start_time = getTime(start_time_str)
                    elif 'rows/s' in line:
                        end_time_str = getCASALogTimeStr(line)
                        gridding_end_time = getTime(end_time_str)
                        gridding = { 'start' : gridding_start_time, 'end' : gridding_end_time }
        else:
            gridding = None
        return gridding


    def getJobSubmitted(self):
        return self.jobSubmitted

    def getTransferIn(self, key):
        return self.transferIn[key]

    def getTransferOut(self, key):
        return self.transferOut[key]

    def getJobExecuting(self, key):
        return self.jobExecuting[key]

    def getAppExecuting(self, key):
        return self.appExecuting[key]

    def getGridding(self, key):
        return self.gridding[key]

    def getIdleTime(self):
        return (self.transferIn['start'] - self.jobSubmitted).total_seconds()

    def getTotalTime(self):
        return (self.transferOut['end'] - self.transferIn['start'])

    def _local2UTC(self):
        utc = self.appExecuting['start']
        local = self.jobExecuting['start']
        local2UTC = int((local - utc).total_seconds() / 3600 )
        for key in ['start', 'end']:
            self.transferIn[key]   -= timedelta(hours = local2UTC)
            self.transferOut[key]  -= timedelta(hours = local2UTC)
            self.jobExecuting[key] -= timedelta(hours = local2UTC)


    def getDataFrame(self):
        jobname = self.jobname
        startTransferIn   = datetime.strftime(self.transferIn['start'], time_fmt)
        endTransferIn     = datetime.strftime(self.transferIn['end'], time_fmt)
        startTransferOut  = datetime.strftime(self.transferOut['start'], time_fmt)
        endTransferOut    = datetime.strftime(self.transferOut['end'], time_fmt)
        startJobExecuting = datetime.strftime(self.jobExecuting['start'], time_fmt)
        endJobExecuting   = datetime.strftime(self.jobExecuting['end'], time_fmt)
        startAppExecuting = datetime.strftime(self.appExecuting['start'], time_fmt)
        endAppExecuting   = datetime.strftime(self.appExecuting['end'], time_fmt)
        if self.gridding is not None:
            startGridding = datetime.strftime(self.gridding['start'], time_fmt)
            endGridding   = datetime.strftime(self.gridding['end'], time_fmt)

        df = pd.DataFrame([
            dict(Task = jobname, Start = startTransferIn, Finish= endTransferIn, Action = 'Transfer In'),
            dict(Task = jobname, Start = startTransferOut, Finish = endTransferOut, Action = 'Transfer Out'),
            dict(Task = jobname, Start = startJobExecuting, Finish = endJobExecuting, Action = 'Job executing'),
            dict(Task = jobname, Start = startAppExecuting, Finish = endAppExecuting, Action = 'App executing'),
        ])
        if 'startGridding' in dir():
            df = df.append(dict(Task = jobname,
                Start = startGridding, Finish = endGridding, Action = 'Gridding'),
                ignore_index = True)

        return df
