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


#!/bin/bash

# $1: htc parameter definitions (htclean.params.htc)
# Following eval/sed cmds remove spaces to convert htcondor variables to shell variables
eval "`sed 's/ //g' $1 | sed 's/(/{/g' | sed 's/)/}/g'`"


DAGlist="makePSF makeDirtyImage runResidualCycle"

for DAG in ${DAGlist}
do
    JOBtext=""
    VARStext=""
    TEXT=""
    if [ "${DAG}" = "makeDirtyImage" ]
    then
        python_state="runResidualCycle"
    else
        python_state=${DAG}
    fi
    for i in `seq 1 ${nparts}`
    do
        JOBtext+="JOB         ${DAG}_n${i}          ${DAG}.htc"$'\n'
        VARStext+="VARS        ${DAG}_n${i}          partId=\"n${i}\""$'\n'
    done
    VARStext+="VARS        ALL_NODES          python_state=\"${python_state}\""$'\n'
    VARStext+="VARS        ALL_NODES          input_file=\"../bin/htclean.params\""$'\n'
    VARStext+="RETRY       ALL_NODES          2"
    TEXT=${JOBtext}$'\n\n'${VARStext}
    echo "${TEXT}" > ${DAG}.dag
done
