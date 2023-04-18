#!/bin/bash

# $1: htc parameter definitions (htclean.params.htc)
# $2: retry / imcycle number (imcycle.htc)
# Following eval/sed cmds remove spaces to convert htcondor variables to shell variables
eval "`sed 's/ //g' $1 | sed 's/(/{/g' | sed 's/)/}/g'`"
eval "`sed 's/ //g' $2`"

output_files=""

for I in `seq 1 ${nparts}`
do
    if [ "$output_files" != "" ]
    then
        output_files="${output_files}; "
    fi
    output_files="${output_files}${imagename}.n${I}.model.tar = ${imagesdir}/${imagename}.workdirectory.imcycle${imcycle}/${imagename}.n${I}.model.tar"
done

echo "output_files = ${output_files}" > output_files.htc
