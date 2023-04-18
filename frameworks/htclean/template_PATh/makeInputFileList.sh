#!/bin/bash

# $1: htc parameter definitions (htclean.params.htc)
# $2: retry / imcycle number (imcycle.htc)
# Following eval/sed cmds remove spaces to convert htcondor variables to shell variables
eval "`sed 's/ //g' $1 | sed 's/(/{/g' | sed 's/)/}/g'`"
eval "`sed 's/ //g' $2`"
job=${3:-psf}

file_types="weight sumwt psf"

if [ $job = "dirty" ]
then
    file_types="${file_types} residual"
else 
    if [ $job = "residual" ]
    then
        file_types="${file_types} ${job} model"
    fi
fi

input_files=""

for I in `seq 1 ${nparts}`
do
    for TYPE in ${file_types}
    do
        if [ "$input_files" != "" ]
        then
            input_files="${input_files}, "
        fi
        input_files="${input_files}${imagesdir}/${imagename}.workdirectory.imcycle${imcycle}/${imagename}.n${I}.${TYPE}.tar"
    done
done

echo "input_files = ${input_files}" > input_files.htc
