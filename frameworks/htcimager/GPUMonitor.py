# Copyright (C) 2024
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
from subprocess import Popen,PIPE

class GPUMonitor(object):

    def __init__(self):
        self.device = self.getDevice()
        self.GPUName = self.getDeviceProperty('name')
        self.GPUCapability = self.getDeviceProperty('compute_cap')
        self.listSystemGPUs()

    def listSystemGPUs(self):
        with Popen(['nvidia-smi'], stdout = PIPE) as nvidia_smi:
            print(nvidia_smi.stdout.read().decode('ascii'))
        
    def getDevice(self):
        try:
            device = os.environ['NVIDIA_VISIBLE_DEVICES']
        except KeyError:
            print('Variable ${NVIDIA_VISIBLE_DEVICES} not set, obtaining device id with uuid. This may be a problem in systems with multiple GPUs.')
            cmd = ['nvidia-smi','--query-gpu=uuid','--format=csv,noheader']
            with Popen(cmd, stdout = PIPE) as nvidia_smi:
                device = nvidia_smi.stdout.read().decode('ascii').strip()
        except Exception as e:
            raise e
        return device
    
    def getDeviceProperty(self, property):
        cmd = ['nvidia-smi','--query-gpu=' + property,'--id=' + self.device,'--format=csv,noheader']
        with Popen(cmd, stdout = PIPE) as nvidia_smi:
            devprop = nvidia_smi.stdout.read().decode('ascii').strip()
        return devprop
        
    def startMonitor(self, interval = 1, outputfile = 'GPUMonitor.out'):
        cmd = ['nvidia-smi','--query-gpu=timestamp,name,utilization.gpu,utilization.memory,memory.used',
               '--format=csv','-l',str(interval),'--id=' + self.device,'--filename=' + outputfile]
        self.monitor = Popen(cmd)

    def stopMonitor(self):
        self.monitor.send_signal(2)
