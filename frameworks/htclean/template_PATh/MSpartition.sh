#!/bin/bash
#
# Author: F. Madsen, 08/03/2021
# Defaults

[ -n "${1}" ] && msname=${1}
[ -n "${2}" ] && partaxis=${2}
[ -n "${3}" ] && nparts=${3}
[ -n "${4}" ] && user=${4}
[ -n "${5}" ] && ssh_key=${5}
[ -n "${6}" ] && initialdir=${6}


echo "msname is (${1})"
echo "partaxis is (${2})"
echo "nparts is (${3})"
echo "user is (${4})"
echo "ssh_key is (${5})"
echo "initialdir is (${6})"


mkdir -p data

export HOME=${HOME:=$TMPDIR}
export OMP_NUM_THREADS=1


cd data

cdate=$(date +%Y%m%d-%H%M%S)


CASABIN=/home/casa/packages/RHEL7/release/casa-6.5.0-15-py3.6/bin/

logname="MSpartition-${cdate}"
${CASABIN}/casa --nologger --nogui --logfile ${logname}.log -c ../bin/MSpartition.py ../${msname}.ms ${partaxis} ${nparts} &> ${logname}.out
