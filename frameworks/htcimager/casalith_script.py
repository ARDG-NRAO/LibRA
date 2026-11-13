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

from subprocess import Popen,PIPE

class casalith_script(object):
    def __init__(self, inputArgs):
        self.scriptcmd = inputArgs.casa_script_cmd
        self.casapath = inputArgs.casapath
        self.logname = inputArgs.jobname

        self._executeCASAscript()
        self.outputImageList = [
        # could write a simple input file to help users configure this,
        # with variables such as script name, parameter file and output product list
        ]

    def _executeCASAscript(self):
        cmd = f'{self.casapath}/casa --nogui -c {self.scriptcmd}'.split()
        stagelog = open(f'{self.logname}.log','w')
        with Popen(cmd, stdout = PIPE, stderr = PIPE) as runner:
            stagelog.write(runner.communicate()[1].decode('ascii'))
        stagelog.close()