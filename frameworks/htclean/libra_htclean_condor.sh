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


# For easier debugging
#set -e          # fail at first error
set -v          # echo commands

lsdir()
{
    path=$1
    echo ""
    echo "Listing contents of ${path}:"
    echo "`ls -l ${path}`"$'\n\n'
}

setupLibra()
{
    # Add casa-data to .casarc 
    tar xzf casa-data.tgz; rm -rf casa-data.tgz
    echo "measures.directory:${PWD}/casa-data" > ~/.casarc

    libra_bundle=`find . -name 'libra.*.sh'`
    sh ${libra_bundle} libra;# rm -rf ${libra_bundle}
    libra_install=$?
    if [ ${libsetup} -ne "0" ]
    then
        return 5 # Error code to indicate failure to install libra
    else
        libraBIN=${PWD}/libra/bin
        return 0
    fi
}

setupRoadRunner()
{
    echo "Setting up roadrunner on `hostname`. Output of nvidia-smi is:"
    echo "`nvidia-smi`"
    echo "`nvidia-smi --query-gpu=name,compute_cap --format=csv --id=${NVIDIA_VISIBLE_DEVICES}`"
    echo ""

    nvidia-smi --query-gpu=timestamp,name,utilization.memory,memory.used --format=csv -l 5 --id=${NVIDIA_VISIBLE_DEVICES} > working/logs/nvidia.${jobmode}.${partId}.out &

    bundles_dir=`ls libra/bundles`
    echo "bundles_dir: $bundles_dir"
    if [ -e /.singularity.d/libs/libcuda.so.1 ]
    then
        cp -f /.singularity.d/libs/libcuda.so.1 libra/bundles/${bundles_dir}/lib64
    else
        if [ -e /lib64/libcuda.so.1 ]
        then
            cp -f /lib64/libcuda.so.1 libra/bundles/${bundles_dir}/lib64
        else
            echo "libcuda.so.1: File not found on knwon paths. Trying with packaged version"
        fi
    fi

    echo "LD_LIBRARY_PATH: ${LD_LIBRARY_PATH}"
    echo ""

    # OMP_PROC_BIND should always be set to false. Setting it to true limits the data rate when
    # more than one instance of roadrunner runs on the same execute host
    export OMP_PROC_BIND=false
    export NPROCS=0

    mkdir -p working/${imagename}.workdirectory
}

checkInputMSName()
{
    lsdir ${PWD}
    if [ ! -e ${msname} ]
    then
        _msname=`find . -maxdepth 1 -name *.ms`
        if [ "${_msname}" != "" ]
        then
            echo "Found ${_msname}, renaming to ${msname}"
            mv ${_msname} ${msname}
        else # workaround for transfer plugin not renaming expanded tarball
            if [ -d ${msname}.tar ]
            then
                mv ${msname}.tar ${msname}
                mv ${cfcache}.tar ${cfcache}
            else
                if [ -f ${msname}.tar ]
                then
                    echo "Expanding tarballs: ${msname}.tar, ${cfcache}.tar"
                    tar xf ${msname}.tar; rm -rf ${msname}.tar
                    tar xf ${cfcache}.tar; rm -rf ${cfcache}.tar
                fi
            fi
        fi
    fi
}

expandInputImageTarballs()
{
    if [ $# -gt 0 ]
    then
        imglist="$@"
        for img in ${imglist}
        do
            echo "Expanding tarball: ${img}"
            tar xf ${img}; rm -rf ${img}
        done
    else
        echo "Didn't find any image tarballs to expand."
        lsdir ${PWD}
    fi
}

createTar()
{
    if [ $# -gt 0 ]
    then
        imglist="$@"
        for img in ${imglist}
        do
	    name=${imagename}.${img}
	    echo "Making tarball: ${name}.tar"
            if [ -e ${name} ]
            then
                tar cf ${name}.tar ${name}
            else
                echo "${name}: file not found."
                return 1
            fi
        done
        return 0
    else
        echo "$0 expects at least one argument, none given."
        return 2
    fi
}

checkLog()
{
    logfile=$1
    grep SEVERE ${logfile}
    if [ $? != 0 ]
    then
        return 0
    else
        return 1
    fi
}

makeInputImageList()
{
    ext=$1
    unset partimagenames

    eval "`grep msnamelist.*\= ${input_file} | sed 's/ //g'`"
    msnamelist=`echo ${msnamelist} | sed 's/,/ /g'`

    partimagenames=${imagename}.n1.${ext}
    i=2
    for msname in `echo $msnamelist | awk '{$1=""}{print substr($0, 2)}'`
    do
        partimagenames=${partimagenames},${imagename}.n${i}.${ext}
        i=$((i+1))
    done
}

readStartImageFlags()
{
    eval "`grep useStartModel.*\= ${input_file} | sed 's/ //g'`"
    useStartModel=`echo ${useStartModel} | sed 's/,/ /g'`
    eval "`grep useStartMask.*\= ${input_file} | sed 's/ //g'`"
    useStartMask=`echo ${useStartMask} | sed 's/,/ /g'`
}

runGather()
{
    extlist=$@
    gatherApp=${libraBIN}/chip

    excode=0
    for ext in ${extlist}
    do
        makeInputImageList ${ext}
        logname=logs/gather_${ext}-$(date +%Y%m%d-%H%M%S)
        cp ${input_file} ${logname}.def
        # Change input parameters
        echo "imagename = ${partimagenames}" >> ${logname}.def
        echo "outputimage = ${imagename}.${ext}" >> ${logname}.def
        if [ ${useStartMask} = "True" ] || [ ${useStartModel} = "True" ] && [ "${ext}" != "sumwt" ]
        then
            echo "overwrite = 1" >> ${logname}.def
        fi
        echo "resetoutput = 1" >> ${logname}.def
        echo "stats = all" >> ${logname}.def
        ${gatherApp} help=def,${logname}.def &> ${logname}.log
        checkLog ${logname}.log
        ex=$?
        
        logname=${logname}.checkAmp
        cp ${input_file} ${logname}.def
        # Change input parameters
        echo "imagename = ${imagename}.${ext}" >> ${logname}.def
        ${libraBIN}/acme help=def,${logname}.def &> ${logname}.log
        checkAmp=$?
        if [ ${checkAmp} -gt ${ex} ]
        then    
            ex=${checkAmp}
        fi

        [ ${ex} -gt ${excode} ] && excode=${ex}
    done
    return ${excode}
}

runNormalize()
{
    f_imtype=$1
    norm_app=${libraBIN}/dale

    #Change input parameters
    echo "imtype = ${f_imtype}" >> ${input_file}
    if [ ${f_imtype} = "psf" ]
    then
        echo "computepb = 1" >> ${input_file}
    # Workaround for NOOP in dale
    elif [ ${f_imtype} = "model" ]
    then
        sed -i 's/SubType.*=.*//g' ${imagename}.model/table.info
    fi

    # Normalize psf and weight and compute pb
    logname=logs/norm_${f_imtype}-$(date +%Y%m%d-%H%M%S)
    cp ${input_file} ${logname}.def
    ${norm_app} help=def,${input_file} &> ${logname}.log
    checkLog ${logname}.log
    return $?
}

makeStartImageCopies()
{
    cpext=$1
    extlist="weight psf residual"
    for ext in ${extlist}
    do
        cp -r ${imagename}.${cpext} ${imagename}.${ext}
    done
    tar cf ${imagename}.residual.start.tar ${imagename}.residual
}

htclean_finalize()
{
    #create tgz file containing casalogs directory
    cd $_CONDOR_SCRATCH_DIR
    mv working/logs $_CONDOR_SCRATCH_DIR
    tar czf logs.${jobmode}.tgz logs
    lsdir ${PWD}
}

[ -n "${1}" ] && jobmode=${1}
[ -n "${2}" ] && input_file=${2}
[ -n "${3}" ] && partId=${3}
[ -n "${4}" ] && msname=${4}
[ -n "${5}" ] && imagename=${5}
[ -n "${6}" ] && cfcache=${6}


echo "jobmode is (${1})"
echo "input_file is (${2})"
echo "partId is (${3})"
echo "msname is (${4})"
echo "imagename is (${5})"
echo "cfcache is (${6})"

echo "OS version: `cat /etc/redhat-release`"
echo "Hostname: `hostname -f`"
echo "Physical Host Name: `cat $_CONDOR_MACHINE_AD | grep -i k8sphysicalhostname | awk '{print $3}'`"

mkdir -p working/logs
export HOME=${HOME:=$TMPDIR}
export OMP_NUM_THREADS=1
setupLibra
libra_install=$?
if [ ${libra_install} -ne "0" ]
then
    exit ${libra_install}
fi	

case ${jobmode} in
    makePSF)
        checkInputMSName
        setupRoadRunner
        mv ${cfcache} working
        cd working

        # Change parameter values by writing to the end of ${input_file}
        # Make .weight image
        echo "vis = ../${msname}" >> ${input_file}
        echo "imagename = ${imagename}.workdirectory/${imagename}.${partId}.weight" >> ${input_file}
        echo "mode = weight" >> ${input_file}
        echo "cfcache = ${cfcache}" >> ${input_file}
        
        logname=logs/weight-$(date +%Y%m%d-%H%M%S).${partId}
        cp ${input_file} ${logname}.def
        cp ${logname}.def roadrunner.def
        ${libraBIN}/roadrunner help=def,${input_file} &> ${logname}.log
        weight=$?

        logname=${logname}.checkAmp
        ${libraBIN}/acme help=def,${input_file} &> ${logname}.log
        checkAmp=$?
        if [ ${checkAmp} -gt ${weight} ]
        then
            weight=${checkAmp}
        fi
        
        # Make .psf image
        echo "imagename = ${imagename}.workdirectory/${imagename}.${partId}.psf" >> ${input_file}
        echo "mode = psf" >> ${input_file}
        
        logname=logs/psf-$(date +%Y%m%d-%H%M%S).${partId}
        cp ${input_file} ${logname}.def
        ${libraBIN}/roadrunner help=def,${input_file} &> ${logname}.log
        psf=$?

        logname=${logname}.checkAmp
        ${libraBIN}/acme help=def,${input_file} &> ${logname}.log
        checkAmp=$?
        if [ ${checkAmp} -gt ${psf} ]
        then    
            psf=${checkAmp}
        fi      


        # create tar files from image directories
        if [ $weight -eq 0 ] && [ $psf -eq 0 ] && [ -e ${imagename}.workdirectory/${imagename}.${partId}.weight ] && [ -e ${imagename}.workdirectory/${imagename}.${partId}.psf ] 
        then
            echo "Adding images to tarballs:"
            cd ${imagename}.workdirectory
            echo "Contents of ${PWD}:"
            echo "`ls`"$'\n\n'
            createTar ${partId}.weight ${partId}.sumwt ${partId}.psf
            makePSF=$?
            mv ${imagename}.${partId}.*.tar $_CONDOR_SCRATCH_DIR
        else
            echo "Failed to make images, check inputs carefully. Exit-codes are weight: ${weight} and psf: ${psf}."
            if [ $weight -gt $psf ]
            then
                makePSF=$weight
            else
                makePSF=$psf
                if [ $psf -eq 0 ]
                then
                    makePSF=1
                fi
            fi
        fi
        htclean_finalize
        exit ${makePSF}
        ;;

    runResidualCycle)
        checkInputMSName
        imagelist=`find . -maxdepth 1 -name "${imagename}.*.tar"`
        expandInputImageTarballs ${imagelist}
        setupRoadRunner
        mv ${cfcache} working
        mv ${imagename}.* working
        cd working
        mv ${imagename}.${partId}.* ${imagename}.workdirectory 
        

        # Change parameters by writing to the end of ${input_file}
        echo "vis = ../${msname}" >> ${input_file}
        echo "imagename = ${imagename}.workdirectory/${imagename}.${partId}.residual" >> ${input_file}
        echo "mode = residual" >> ${input_file}
        echo "cfcache = ${cfcache}" >> ${input_file}
        
        # If  model image exists, also update modelimagename
        # Attention to the name of the model image, modify workflow accordingly
        if [ -e ${imagename}.divmodel ]
        then
            echo "modelimagename = ${imagename}.divmodel" >> ${input_file}
        fi

        # Compute .residual
        logname=logs/residual-$(date +%Y%m%d-%H%M%S).${partId}
        cp ${input_file} ${logname}.def
        ${libraBIN}/roadrunner help=def,${input_file} &> ${logname}.log
        residual=$?

        logname=${logname}.checkAmp
        ${libraBIN}/acme help=def,${input_file} &> ${logname}.log
        checkAmp=$?
        if [ ${checkAmp} -gt ${residual} ]
        then    
            residual=${checkAmp}
        fi      


        if [ $residual -eq 0 ] && [ -e ${imagename}.workdirectory/${imagename}.${partId}.residual ]
        then
            echo "Adding images to tarballs:"
            cd ${imagename}.workdirectory
            createTar ${partId}.residual
            mv ${imagename}.${partId}.residual.tar $_CONDOR_SCRATCH_DIR
        else
            echo "Failed to make images, check inputs carefully."
            if [ $residual -eq 0 ]
            then
                residual=1
            fi
        fi
        htclean_finalize
        exit ${residual}
        ;;
    
    # Gather and normalize PSF and weight; compute pb
    gatherPSF)
        imagelist=`find . -maxdepth 1 -name "${imagename}.*.tar"`
        expandInputImageTarballs ${imagelist}
        mv ${imagename}.* working
        cd working
        readStartImageFlags

        if [ ${useStartModel} = "True" ]
        then
            makeStartImageCopies model
        else
            if [ ${useStartMask} = "True" ]
            then
        	makeStartImageCopies mask
            fi
        fi
        
        runGather psf weight sumwt
        gatherPSF=$?
        
        runNormalize psf
        normPSF=$?

        if [ "${useStartModel}" = "True" ] && [ ${normPSF} -eq 0 ] && [ -e ${imagename}.model ]
        then
            runNormalize model
            divModel=$?
        
            if [ ${divModel} -ne 0 ]
            then
        	normPSF=${divModel}
            else
        	createTar divmodel
            fi
        fi
        
        if [ ${gatherPSF} -eq 0 ] && [ -e ${imagename}.psf ] && [ ${normPSF} -eq 0 ]
        then
            createTar weight sumwt psf pb
            mv ${imagename}.*.tar $_CONDOR_SCRATCH_DIR
            gathernormPSF=0
        else
            echo "Failed in gather/normalize, check inputs carefully. Exit-codes are gather: ${gatherPSF} and normalize: ${normPSF}."
            if [ $gatherPSF -gt $normPSF ]
            then
                gathernormPSF=$gatherPSF
            else
                gathernormPSF=$normPSF
                if [ $normPSF -eq 0 ]
                then
                   gathernormPSF=1
                fi
            fi
        fi
        htclean_finalize
        exit ${gathernormPSF}
        ;;

    gather)
        imagelist=`find . -maxdepth 1 -name "${imagename}.*.tar"`
        expandInputImageTarballs ${imagelist}
        mv ${imagename}.* working
        cd working
        readStartImageFlags

        runGather residual
        gather=$?

        runNormalize residual
        norm=$?

        if [ ${gather} -eq 0 ] && [ -e ${imagename}.residual ] && [ ${norm} -eq 0 ]
        then
        createTar residual
        mv ${imagename}.residual.tar $_CONDOR_SCRATCH_DIR
        else
            echo "Failed in gather/normalize, check inputs carefully. Exit-codes are gather: ${gather} and normalize: ${norm}."
            if [ $gather -gt $norm ]
            then
                gathernorm=$gather
            else
                gathernorm=$norm
                if [ $norm -eq 0 ]
                then
                    gathernorm=1
                fi
            fi
        fi
        htclean_finalize
        exit ${gathernorm}
        ;;
	
    runModelCycle)
        imagelist=`find . -maxdepth 1 -name "${imagename}.*.tar"`
        expandInputImageTarballs ${imagelist}
        mv ${imagename}.* working
        cd working
        lsdir ${PWD}

        # workaround for rest frequency mismatch when using external model/mask
        mv ${imagename}.sumwt ${imagename}.sumwt.noop

        logname=logs/modelUpdate-$(date +%Y%m%d-%H%M%S)
        cp ${input_file} ${logname}.def
        ${libraBIN}/hummbee help=def,${input_file} &> ${logname}.log
        checkLog ${logname}.log
        updateModel=$?
        
        # Test convergence
        grep "Reached n-sigma threshold\|Reached global stopping criteria" ${logname}.log
        [ "$?" -eq "0" ] && touch $_CONDOR_SCRATCH_DIR/stopIMCycles

        # Undo rename sumwt so that acme gets required inputs
        mv ${imagename}.sumwt.noop ${imagename}.sumwt

        # Change parameters and run acme to divide model by weight
        runNormalize model
        divModel=$?
	
        if [ ${updateModel} -eq 0 ] && [ ${divModel} -eq 0 ] && [ -e ${imagename}.divmodel ]
        then
            createTar model divmodel residual mask
            mv ${imagename}.*.tar $_CONDOR_SCRATCH_DIR
            model=0
        else
            echo "Failed to update model, check inputs carefully. Exit-codes are updateModel: ${updateModel} and normalize: ${divModel}."
            if [ $updateModel -gt $divModel ]
            then
                model=$updateModel
            else
                model=$divModel
                if [ $divModel -eq 0 ]
                then
                    model=1
                fi
            fi
        fi
        htclean_finalize
        exit ${model}
        ;;
	    
    makeFinalImages)
        imagelist=`find . -maxdepth 1 -name "${imagename}.*.tar"`
        expandInputImageTarballs ${imagelist}
        mv ${imagename}.* working
        cd working
        lsdir ${PWD}

        # workaround for rest frequency mismatch when using external model/mask
        mv ${imagename}.sumwt ${imagename}.sumwt.noop

        echo "mode = restore" >> ${input_file}
        logname=logs/makeFinalImages-$(date +%Y%m%d-%H%M%S)
        cp ${input_file} ${logname}.def
        ${libraBIN}/hummbee help=def,${input_file} &> ${logname}.log
        checkLog ${logname}.log
        makeFinalImages=$?

        if [ ${makeFinalImages} -eq 0 ] && [ -e ${imagename}.image ]
        then
            createTar image image.pbcor
            mv ${imagename}.image*.tar $_CONDOR_SCRATCH_DIR
        else
            echo "Failed to make images, check inputs carefully."
            if [ ${makeFinalImages} -eq 0 ]
            then
                makeFinalImages=1
            fi 
        fi

        htclean_finalize
        exit ${makeFinalImages}
        ;;

    *) 
        echo "${jobmode}: unknown jobmode. Exiting."
        exit 1
        ;;
esac
