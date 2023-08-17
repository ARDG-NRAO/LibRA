#!/bin/bash
#
# Author: F. Madsen, 08/03/2021
# Defaults

# For easier debugging
set -e          # fail at first error
set -v          # echo commands


setupRoadRunner()
{
#    tar xf rr_80.tar; rm -rf rr_80.tar
#    tar xf rr_80_sing.tar; rm -rf rr_80_sing.tar
    chmod a+x rr_80.sh
    ${PWD}/rr_80.sh ${PWD}/rr_80

    tar xzf casa-data.tgz; rm -rf casa-data.tgz

    tar xf ${cfcache}.tar; rm -rf ${cfcache}.tar

    tar xf ${msname}_${partId}.ms.tar; rm -rf ${msname}_${partId}.ms.tar
    if [ ! -e ${msname}_${partId}.ms ]
    then
	MSNAME=`find . -maxdepth 1 -name *.ms`
	echo $MSNAME
	mv $MSNAME ${msname}_${partId}.ms
    fi

#    find ${PWD}/exodus -name 'libcuda.so.1' -exec rm -rf {} \+
#    find ${PWD}/exodus -name 'libcuda.so.1' -exec cp -f /.singularity.d/libs/libcuda.so.1 {} \;
#    cp -f /.singularity.d/libs/libcuda.so.1 ${PWD}/rr_sing/bundles/1738d12843cbea76303ea3029d202a084d1ed64c900b7b5090511aa6798ae8b3/.singularity.d/libs/

    echo "measures.directory:${PWD}/casa-data" > ~/.casarc

    echo "Setting up roadrunner on `hostname`. Output of nvidia-smi is:"
    echo "`nvidia-smi`"
    echo ""

    nvidia-smi --query-gpu=timestamp,name,utilization.memory,memory.used --format=csv -l 5 --id=0 > nvidia.${python_state}.${partId}.out &

    echo "LD_LIBRARY_PATH: ${LD_LIBRARY_PATH}"
    echo ""

    RRpars=`cat bin/roadrunner.cmd | awk '{$6=$2 ; $1=$2=$3=$4=$5="" ; print substr($0, 6)}'`

    # OMP_PROC_BIND should always be set to false. Setting it to true limits the data rate when
    # more than one instance of roadrunner runs on the same execute host
    export OMP_PROC_BIND=false

    RRBIN=${PWD}/rr_80/bin/roadrunner

    mkdir -p working/${imagename}.workdirectory
}

setupCASA()
{
    tar xf casa-6.5.0-15-py3.8.tar.xz; rm -rf casa-6.5.0-15-py3.8.tar.xz

    CASABIN=${PWD}/casa-6.5.0-15-py3.8/bin
    export OMP_NUM_THREADS=1
    isRHEL8=`grep \ 8\.[0-9] /etc/redhat-release | wc -l`
    if [ ${isRHEL8} -eq 1 ]
    then
        echo "Running on `cat /etc/redhat-release` - adding libnsl.so.1 to LD_LIBRARY_PATH"
        export LD_LIBRARY_PATH=${PWD}:${LD_LIBRARY_PATH}
    fi
}

[ -n "${1}" ] && python_state=${1}
[ -n "${2}" ] && input_file=${2}
[ -n "${3}" ] && partId=${3}
[ -n "${4}" ] && msname=${4}
[ -n "${5}" ] && imagename=${5}
[ -n "${6}" ] && cfcache=${6}
[ -n "${7}" ] && initialdir=${7}


echo "python_state is (${1})"
echo "input_file is (${2})"
echo "partId is (${3})"
echo "msname is (${4})"
echo "imagename is (${5})"
echo "cfcache is (${6})"
echo "initialdir is (${7})"


mkdir -p working/casalogs

export HOME=${HOME:=$TMPDIR}


cdate=$(date +%Y%m%d-%H%M%S)


case ${python_state} in
    makePSF)
        setupRoadRunner
        cd working
        eval "${RRBIN} ms=\"../${msname}_${partId}.ms\" imagename=\"${imagename}.workdirectory/${imagename}.${partId}.weight\" modelimagename=\"\" mode=\"weight\" cfcache=\"../${cfcache}\" ${RRpars}" &> casalogs/weight-${cdate}.${partId}.out
        weight=$?
        eval "${RRBIN} ms=\"../${msname}_${partId}.ms\" imagename=\"${imagename}.workdirectory/${imagename}.${partId}.psf\" modelimagename=\"\" mode=\"psf\" cfcache=\"../${cfcache}\" ${RRpars}" &> casalogs/psf-${cdate}.${partId}.out
        psf=$?
        # create tar files from image directories
        if [ $weight -eq 0 ] & [ $psf -eq 0 ]
        then
            echo "Adding images to tarballs:"
            cd ${imagename}.workdirectory
            tar cf ${imagename}.${partId}.weight.tar ${imagename}.${partId}.weight 
            tar cf ${imagename}.${partId}.sumwt.tar ${imagename}.${partId}.sumwt 
            tar cf ${imagename}.${partId}.psf.tar ${imagename}.${partId}.psf 	
            mv ${imagename}.${partId}.*.tar $_CONDOR_SCRATCH_DIR
        else
            echo "Failed to make images, check inputs carefully."
            exit 10
        fi
        ;;

    runResidualCycle)
        setupRoadRunner
#        for IMG in `find . -name "${imagename}.${partId}.*.tar"`
        for IMG in ${imagename}.${partId}.*.tar
        do
            echo "Expanding tarball: $IMG"
            tar xf $IMG; rm -rf $IMG
        done
        mv ${imagename}.${partId}.* working/${imagename}.workdirectory 
        cd working
        if [ ! -e ${imagename}.workdirectory/${imagename}.${partId}.model ]
        then
            eval "${RRBIN} ms=\"../${msname}_${partId}.ms\" imagename=\"${imagename}.workdirectory/${imagename}.${partId}.residual\" modelimagename=\"\" mode=\"residual\" cfcache=\"../${cfcache}\" ${RRpars}" &> casalogs/dirty-${cdate}.${partId}.out
            residual=$?
        else
            eval "${RRBIN} ms=\"../${msname}_${partId}.ms\" imagename=\"${imagename}.workdirectory/${imagename}.${partId}.residual\" modelimagename=\"${imagename}.workdirectory/${imagename}.${partId}.model\" mode=\"residual\" cfcache=\"../${cfcache}\" ${RRpars}" &> casalogs/runResidualCycle-${cdate}.${partId}.out
            residual=$?
        fi
        if [ $residual -eq 0 ]
        then
            echo "Adding images to tarballs:"
            cd ${imagename}.workdirectory
            tar cf ${imagename}.${partId}.residual.tar ${imagename}.${partId}.residual
            tar cf ${imagename}.${partId}.weight.tar ${imagename}.${partId}.weight 
            tar cf ${imagename}.${partId}.sumwt.tar ${imagename}.${partId}.sumwt 
            tar cf ${imagename}.${partId}.psf.tar ${imagename}.${partId}.psf 	
            mv ${imagename}.${partId}.*.tar $_CONDOR_SCRATCH_DIR
        else
            echo "Failed to make images, check inputs carefully."
            exit 10
        fi
        ;;

    *)
        setupCASA
        for IMG in `find . -name "${imagename}.*.tar"`
        do
            echo "Extracting $IMG"
            tar xf $IMG; rm -rf $IMG
        done
        mv ${imagename}.* working

        cd working 
        echo ""
        echo "Now listing contents of ${PWD}:"
        echo "`ls`"

        mkdir ${imagename}.workdirectory
        mv ${imagename}.${partId}* ${imagename}.workdirectory
        echo ""
        echo "Now listing contents of ${PWD}/${imagename}.workdirectory:"
        echo "`ls ${imagename}.workdirectory`"

        logname="casalogs/${python_state}-${cdate}"

        echo ""
        echo "Starting CASA"
        echo ""

        ${CASABIN}/casa --nologger --nogui --logfile ${logname}.log -c ../pylib/htclean.py ${python_state} ${input_file} ${partId} &> ${logname}.out

        echo ""
        echo "CASA session ended. Listing contents of ${PWD}"
        echo "`ls`"
        echo ""

        for IMG in `find . -maxdepth 1 \( -name "${imagename}.*" -a ! -name "${imagename}.workdirectory" \)`
        do
            echo "Adding ${IMG} to ${IMG}.tar"
            tar cf ${IMG}.tar ${IMG}
        done

        if [ -f stopIMCycles ]
        then
            mv stopIMCycles $_CONDOR_SCRATCH_DIR
        fi

        if [ -e ${imagename}.model ] & [ "${python_state}" = "runModelCycle" ]
        then
            cd ${imagename}.workdirectory

            echo ""
            echo "Now listing contents of ${PWD}:"
            echo "`ls`"

            for IMG in `find . -name "${imagename}.*.model"`
            do
                echo "Adding ${IMG} to ${IMG}.tar"
                tar cf ${IMG}.tar ${IMG}
            done
            mv ${imagename}.*.tar $_CONDOR_SCRATCH_DIR
            cd ..
        fi

        mv ${imagename}.*.tar $_CONDOR_SCRATCH_DIR
        ;;
esac

#create tgz file containing casalogs directory
cd $_CONDOR_SCRATCH_DIR
mv working/casalogs $_CONDOR_SCRATCH_DIR
tar czf casalogs.${partId}.${python_state}.tgz casalogs
echo ""
echo "Now listing contents of ${PWD}:"
echo "`ls`"
