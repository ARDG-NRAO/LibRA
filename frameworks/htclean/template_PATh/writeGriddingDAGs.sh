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
