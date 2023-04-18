#!/bin/bash
#
# Author: F. Madsen, 08/03/2021
# Defaults

setupRoadRunner()
{
#    CASABUILD="CASA-HPG-TEST"               # multi-threaded grid/cfc load, branch of 13581
#    CASABUILD="CAS-13581-rattler"
#    CASAHOME=/home/yaghan3/sanjay/${CASABUILD}/casa6/casa5
    CASABUILD="CAS-13581"
    CASAHOME=/home/yaghan3/sanjay/${CASABUILD}/casa5
#    CUDACAP=7x
#    RRBIN=${CASAHOME}/code/build-linux_64b/synthesis/roadrunner${CUDACAP}
    RRBIN=${CASAHOME}/code/build-linux_64b/synthesis/roadrunner
    source /home/yaghan3/setup-spack.bash && source /home/yaghan3/sanjay/HPG/libhpg/hpg/setup.hpg
    RRpars=`cat ../bin/roadrunner.cmd | awk '{$6=$2 ; $1=$2=$3=$4=$5="" ; print substr($0, 6)}'`
#    GO="\n inp \n go \n"
    export OMP_PROC_BIND=true
#    printenv > env.txt 
}

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
export PATH=${PATH}:/usr/local/cuda/bin

#if [ "${python_state}" = "runResidualCycle" ] || [ "${python_state}" = "makePSF" ]
#then
#    echo ""
#    echo "Copying cfcache on ${cfcache} to /dev/shm..."
#    mkdir -p /dev/shm/cfcache
#    cp -r ${cfcache} /dev/shm/cfcache/
#    if [ $? -eq 0 ]
#    then
#        echo "cfcache successfully copied to /dev/shm."
#    else
#        echo "Failed to copy cfcache. Exiting now."
#        exit 1
#    fi
#    echo `ls /dev/shm`
#fi

cd working
mkdir -p casalogs

cdate=$(date +%Y%m%d-%H%M%S)

#if [ "${python_state}" = "runResidualCycle" ]
#then
#    logname="casalogs/${python_state}_${partId}-${cdate}"
#    mkdir ${imagename}.workdirectory
#    mv ../${imagename}.${partId}.* ${imagename}.workdirectory
#else
#    logname="casalogs/${python_state}-${cdate}"
#    [ "${python_state}" = "makePSF" ] && logname="${python_state}_${partId}-${cdate}"
#fi

case ${python_state} in
    makePSF)
        setupRoadRunner
        mkdir -p ${imagename}.workdirectory
        xvfb-run -d eval "${RRBIN} ms=\"../${msname}_${partId}.ms\" imagename=\"${imagename}.workdirectory/${imagename}.${partId}.weight\" modelimagename=\"\" mode=\"weight\" cfcache=\"/export/home/`hostname`/fmadsen/cfcache/${cfcache}\" ${RRpars}" &> casalogs/weight-${cdate}.${partId}.out
        xvfb-run -d eval "${RRBIN} ms=\"../${msname}_${partId}.ms\" imagename=\"${imagename}.workdirectory/${imagename}.${partId}.psf\" modelimagename=\"\" mode=\"psf\" cfcache=\"/export/home/`hostname`/fmadsen/cfcache/${cfcache}\" ${RRpars}" &> casalogs/psf-${cdate}.${partId}.out
        ;;
    runResidualCycle)
        setupRoadRunner
        mkdir -p ${imagename}.workdirectory 
        mv ../${imagename}.${partId}.* ${imagename}.workdirectory 
        if [ ! -e ${imagename}.workdirectory/${imagename}.${partId}.model ]
        then
            xvfb-run -d eval "${RRBIN} ms=\"../${msname}_${partId}.ms\" imagename=\"${imagename}.workdirectory/${imagename}.${partId}.residual\" modelimagename=\"\" mode=\"residual\" cfcache=\"/export/home/`hostname`/fmadsen/cfcache/${cfcache}\" ${RRpars}" &> casalogs/dirty-${cdate}.${partId}.out
        else
            xvfb-run -d eval "${RRBIN} ms=\"../${msname}_${partId}.ms\" imagename=\"${imagename}.workdirectory/${imagename}.${partId}.residual\" modelimagename=\"${imagename}.workdirectory/${imagename}.${partId}.model\" mode=\"residual\" cfcache=\"/export/home/`hostname`/fmadsen/cfcache/${cfcache}\" ${RRpars}" &> casalogs/runResidualCycle-${cdate}.${partId}.out
        fi
        ;;
    *)
        CASABIN=/home/casa/packages/RHEL7/release/casa-6.5.0-15-py3.6/bin/

        logname="casalogs/${python_state}-${cdate}"
        xvfb-run -d ${CASABIN}/casa --nologger --nogui --logfile ${logname}.log -c ../bin/htclean.py ${python_state} ${input_file} ${partId} &> ${logname}.out
        ;;
esac


#sleep 299


# cleanup
rm -rf cachedir
rm -f *.last

## delete cfcache copy on /dev/shm
#if [ "${python_state}" = "runResidualCycle" ] || [ "${python_state}" = "makePSF" ]
#then
#    rm -rf /dev/shm/cfcache
#fi
