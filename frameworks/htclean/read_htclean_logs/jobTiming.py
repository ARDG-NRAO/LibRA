# jobTiming.py
# class definition to read and report timings of htclean jobs running on HTCondor

from datetime import datetime, timedelta
import pandas as pd

# Defaults
time_fmt = '%Y-%m-%d %H:%M:%S'


def getCondorLogTimeStr(line, index):
    end   = index - 1
    start = end - 19

    return line[start:end]


def getCASALogTimeStr(line):
    return line[0:19]


def getTime(timestr):
    return datetime.strptime(timestr, time_fmt)

def getSiteName(line):
    siteName = ((line.partition('@')[2]).partition('osgvo')[0])[:-1]
    return siteName


class jobTiming(object):
    def __init__(self, jobname, applogfile):
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
        logfile = self.jobname + '.condor.log'
        with open(logfile, 'r') as log:
            for line in log.readlines():
                if 'SlotName' in line:
                    siteName = getSiteName(line)
        return siteName
        

    def _readSubmissionTime(self):
        logfile = self.jobname + '.condor.log'
        with open(logfile, 'r') as log:
            for line in log.readlines():
                if 'Job submitted' in line:
                    index = line.find('Job submitted')
                    time_str = getCondorLogTimeStr(line, index)
                    submitted_time = getTime(time_str)
        return submitted_time


    def _readTransferTime(self, direction):
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
                start_time_str = getCASALogTimeStr(loglines[1])
                for line in loglines[1:-1]:
                    if ('roadrunner::main' in line or 'roadrunner::Roadrunner_func' in line) and '...done' in line:
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

    def getTransferTime(self, direction = ''):
        transfer_in = (self.transferIn['end'] - self.transferIn['start']).total_seconds()
        transfer_out = (self.transferOut['end'] - self.transferOut['start']).total_seconds()
        if direction.lower() in ['input', 'in']:
            return transfer_in
        elif direction.lower() in ['output', 'out']:
            return transfer_out
        else:
            return (transfer_in + transfer_out)

    def getTotalTime(self):
        return (self.transferOut['end'] - self.transferIn['start']).total_seconds()

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
        siteName = self.sitename
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
            dict(Task = jobname, Start = startTransferIn, Finish= endTransferIn, Action = 'Transfer In',
                SiteName = ''),
            dict(Task = jobname, Start = startTransferOut, Finish = endTransferOut, Action = 'Transfer Out',
                SiteName = siteName),
            dict(Task = jobname, Start = startJobExecuting, Finish = endJobExecuting, Action = 'Job executing',
                SiteName = ''),
            dict(Task = jobname, Start = startAppExecuting, Finish = endAppExecuting, Action = 'App executing',
                SiteName = ''),
        ])
        if 'startGridding' in dir():
            df = df.append(dict(Task = jobname,
                Start = startGridding, Finish = endGridding, Action = 'Gridding', SiteName = ''),
                ignore_index = True)

        return df
