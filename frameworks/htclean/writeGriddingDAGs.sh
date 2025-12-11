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


getFileSize()
{
    inputFile=$1
    if [ -f ${inputFile} ]
    then
        size=`du -sL ${inputFile} | awk '{print int($1/(1024**2))+1}'`
    else
        size=5
    fi
    echo $size
}

# $1: parameter definitions (libra_htclean.def)
# Following eval/sed cmds remove spaces to convert htcondor variables to shell variables
input_file=$1
eval "`sed 's/ //g' ${input_file} | sed 's/(/{/g' | sed 's/)/}/g' | sed 's/osdf\:\/\///g'`"

DAGlist="makePSF makeDirtyImage runResidualCycle"

# Create arrays to iterate on ms and cfcache names
msnamelist=( `echo ${msnamelist} | sed 's/,/ /g'` )
cfcachelist=( `echo ${cfcachelist} | sed 's/,/ /g'` )

if [ ${#cfcachelist[@]} -gt 1 ] && [ ${#cfcachelist[@]} -ne ${#msnamelist[@]} ]
then
    echo "Review inputs: cfcachelist has multiple entries, but the number of entries does not match msnamelist"
    exit 255
else
    if [ ${#cfcachelist[@]} -eq 1 ]
    then
        cfcache=${cfcachelist}
        for i in ${!msnamelist[@]}
        do
            cfcachelist[i]=${cfcache}
        done
    fi
fi


for DAG in ${DAGlist}
do
    JOBtext=""
    VARStext=""
    TEXT=""
    if [ "${DAG}" = "makeDirtyImage" ]
    then
        jobmode="runResidualCycle"
    else
        jobmode=${DAG}
    fi
    for i in ${!msnamelist[@]}
    do
        k=$((i+1))
        msname=${msnamelist[i]}
        cfcache=${cfcachelist[i]}
        JOBtext+="JOB         ${DAG}_n${k}          ${DAG}.htc"$'\n'
        VARStext+="VARS        ${DAG}_n${k}          msname=\"${msname}\""$'\n'
        VARStext+="VARS        ${DAG}_n${k}          cfcache=\"${cfcache}\""$'\n'
        VARStext+="VARS        ${DAG}_n${k}          partId=\"n${k}\""$'\n'
#        cfsize=`du -sL ${cfcachedir}/${cfcache}.tgz | awk '{print int($1/(1024**2))+1}'`
#        mssize=`du -sL ${msdir}/${msname}.tgz | awk '{print int($1/(1024**2))+1}'`
        cfsize=`getFileSize ${cfcachedir}/${cfcache}.tgz`
	mssize=`getFileSize ${msdir}/${msname}.tgz`
        diskrq=$((2 * (cfsize + mssize) + 10))
        VARStext+="VARS        ${DAG}_n${k}          diskResidual=\"${diskrq} G\""$'\n'
    done
    VARStext+="VARS        ALL_NODES          jobmode=\"${jobmode}\""$'\n'
    VARStext+="VARS        ALL_NODES          input_file=\"../bin/${input_file}\""$'\n'
    VARStext+="RETRY       ALL_NODES          2"
    TEXT=${JOBtext}$'\n\n'${VARStext}
    echo "${TEXT}" > ${DAG}.dag
done
