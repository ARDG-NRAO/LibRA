#!/bin/bash
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


# $1: parameter file (imageSolver.def)
# $2: retry / imcycle number (imcycle.htc)
# Following eval/sed cmds remove spaces to convert htcondor variables to shell variables
eval "`sed 's/ //g' $1 | sed 's/(/{/g' | sed 's/)/}/g'`"
eval "`sed 's/ //g' $2`"
job=${3:-psf}

msnamelist=( `echo ${msnamelist} | sed 's/,/ /g'` )

file_types="weight sumwt psf"

[ ${job} != "psf" ] && file_types="residual"

input_files=""
gatherimagelist=""

for i in ${!msnamelist[@]}
do
    i=$((i+1))
    if [ "${gatherimagelist}" != "" ]
    then
        gatherimagelist="${gatherimagelist},"
    fi
    gatherimagelist="${gatherimagelist}${baseimagename}.n${i}"
    for TYPE in ${file_types}
    do
        if [ "$input_files" != "" ]
        then
            input_files="${input_files}, "
        fi
        input_files="${input_files}${imagesdir}/${baseimagename}.workdirectory.imcycle${imcycle}/${baseimagename}.n${i}.${TYPE}.tgz?pack=auto"
    done
done

echo "input_files = ${input_files}" > input_files.htc
echo "gatherimagelist = ${gatherimagelist}" >> input_files.htc
