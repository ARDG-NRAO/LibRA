#!/bin/bash
#
# Author: F. Madsen, 08/03/2021
# Defaults

[ -n "${1}" ] && python_state=${1}
[ -n "${2}" ] && input_file=${2}
[ -n "${3}" ] && partId=${3}
[ -n "${4}" ] && msname=${4}
[ -n "${5}" ] && imagename=${5}
[ -n "${6}" ] && cfcache=${6}
[ -n "${7}" ] && user=${7}
[ -n "${8}" ] && ssh_key=${8}
[ -n "${9}" ] && initialdir=${9}


echo "python_state is (${1})"
echo "input_file is (${2})"
echo "partId is (${3})"
echo "msname is (${4})"
echo "imagename is (${5})"
echo "cfcache is (${6})"
echo "user is (${7})"
echo "ssh_key is (${8})"
echo "initialdir is (${9})"


mkdir -p working

export HOME=${HOME:=$TMPDIR}
export OMP_NUM_THREADS=1

# run CASA
CASAHOME=/home/casa/packages/RHEL7/prerelease/casa-6.5.0-8-py3.6/bin/

if [ "${python_state}" = "runResidualCycle" ] || [ "${python_state}" = "makePSF" ]
then
    echo ""
    echo "Copying cfcache on ${cfcache} to /dev/shm..."
    mkdir -p /dev/shm/cfcache
    cp -r /mnt/scratch/cfcache/${cfcache} /dev/shm/cfcache/
    if [ $? -eq 0 ]
    then
        echo "cfcache successfully copied to /dev/shm."
    else
        echo "Failed to copy cfcache. Exiting now."
        exit 1
    fi
fi

cd working
mkdir casalogs

cdate=$(date +%Y%m%d-%H%M%S)

if [ "${python_state}" = "runResidualCycle" ]
then
    logname="${python_state}_${partId}-${cdate}"
    mkdir ${imagename}.workdirectory
    mv ../${imagename}.${partId}.* ${imagename}.workdirectory
else
    logname="${python_state}-${cdate}"
    [ "${python_state}" = "makePSF" ] && logname="${python_state}_${partId}-${cdate}"
fi

logname=casalogs/${logname}

${CASAHOME}/casa --nologger --nogui --logfile ${logname}.log -c ../bin/htclean.py ${python_state} ${input_file} ${partId} &> ${logname}.out

#sleep 299


# cleanup
#rm -rf *.workdirectory
rm -rf cachedir
rm -f *.last

## copy data back to submit host
#cd ..
if [ "${python_state}" = "runResidualCycle" ] || [ "${python_state}" = "makePSF" ]
then
#    rsync -aq -e "ssh -l ${user} -i ${ssh_key} -q -o StrictHostKeyChecking=no" working/${logname}.* gibson-10g2.aoc.nrao.edu:${initialdir}/working/
#    rsync -aq --relative -e "ssh -l ${user} -i ${ssh_key} -q -o StrictHostKeyChecking=no" working/${imagename}.workdirectory/${imagename}.${partId}.* gibson-10g2.aoc.nrao.edu:${initialdir}
    rm -rf /dev/shm/cfcache
#else
#    rsync -aq --relative -e "ssh -l ${user} -i ${ssh_key} -q -o StrictHostKeyChecking=no" working gibson-10g2.aoc.nrao.edu:${initialdir}
fi
