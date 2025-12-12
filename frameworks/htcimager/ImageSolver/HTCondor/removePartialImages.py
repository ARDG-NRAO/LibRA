#!/usr/bin/python3

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
import shutil
import sys
from glob import glob

inputfile = sys.argv[1]

projectdir = ''
dataorigin = ''
imagesdir = ''
imagename = ''
keepPrevious = 1

try:
    with open(inputfile, 'r') as infile:
        for line in infile.readlines():
            for param in ['projectdir','imagesdir','dataorigin','imagename']:
                if param == line.split('=')[0].strip():
                    exec(f"{param} = '{line.split('=')[1].strip()}'")
    
    imagesdir = imagesdir.replace('osdf://','')
    imagesdir = imagesdir.replace('$(dataorigin)',f'{dataorigin}')
    imagesdir = imagesdir.replace('$(projectdir)',f'{projectdir}')

    partialImagesDirList = glob(f'{imagesdir}/{imagename}.workdirectory.imcycle*')
    partialImagesDirList.sort()

    for imdir in partialImagesDirList[:-keepPrevious]:
#        print(f"Remove directory: {imdir}")
        shutil.rmtree(imdir)

    
except Exception as e:
    raise e
