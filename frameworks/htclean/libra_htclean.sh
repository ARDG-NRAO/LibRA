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

usage()
{
    echo "$0 : Script to make images using LibRA components."
    echo "Usage: $0 -i <imagename> [-n <ncycles>] [-p <input file>] [-l <logdir>] [-L <LibRA root>] [-d] [-h]"$'\n'\
         "       -i set the basename of the images to be generated (without extension)"$'\n'\
         "       -n (optional) set the maximum number of imaging cycles. Default: 10 "$'\n'\
         "       -p (optional) set the name of the iput parameter file. Default: `basename $0 .sh`.def"$'\n'\
         "       -l (optional) directory to save .log and .def files. Default: save to current directory."$'\n'\
         "       -L (optional) LibRA root directory. Default is <user home directory>/libra."$'\n'\
         "       -d (optional) run as part of a distributed workflow. Default: False"$'\n'\
         "       -h prints help and exits"$'\n\n'\
	 "       Example: $0 -i test_image -d (for a job that makes test_image.* images and runs on a distributed computing environment.)"$'\n'
}

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
    echo "measures.directory:${PWD}/casa-data" > ~/.casarc

    libra_bundle=`find . -name 'libra.*.sh'`
    sh ${libra_bundle} libra;# rm -rf ${libra_bundle}
    libra_install=$?
    if [ ${libra_install} -ne "0" ]
    then
        return 5 # Error code to indicate failure to install libra
    else
        libraBIN=${PWD}/libra/bin
        return 0
    fi

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


OMP_NUM_THREADS=1

# Default parameters
OSGjob=false
imagename=""
ncycle=10
input_file=`basename $0 .sh`.def
LIBRAHOME=${HOME}/libra
logdir=${PWD}/

# Input arguments
while getopts "i:l:L:n:p:dh" option
do
    case "$option" in
        d) OSGjob=true                            ;;
        i) imagename=${OPTARG}                    ;;
        l) logdir=${OPTARG}/                      ;;
    	n) ncycle=${OPTARG}                       ;;
    	p) input_file=${OPTARG}                   ;;
    	L) LIBRAHOME=${OPTARG}                    ;;
    	h) usage
           exit 0                                 ;;
    	*) echo "${option}: Unknown option"
    	   usage
    	   exit 1                                 ;;
    esac
done

if [ -n "${imagename}" ]
then
    echo "Running $0 with the following input parameters:"
    echo "imagename = ${imagename}"
    echo "ncycle = ${ncycle}"
    echo "input_file = ${input_file}"
    echo ""
else
    echo "Imagename not set. Exiting now."$'\n'
    usage
    exit 1
fi

if ${OSGjob}
then
    setupLibra
    lsdir
else
    libraBIN=${LIBRAHOME}/install/bin
fi

mkdir -p ${logdir}

griddingAPP=${libraBIN}/roadrunner
deconvolutionAPP=${libraBIN}/hummbee
normalizationAPP=${libraBIN}/dale


# restart=1 will pick up files from an existing htclean run and continue CLEANing
# set start_index to the current number of imaging cycles + 1 (next imaging cycle after restart)
# set GPUENGINE=0 to run casatools based gridding (same as serialized htclean)
restart=0 
start_index=1

# Begin execution block

echo "Using gridding application:      "${griddingAPP}
echo "Using deconvolution application: "${deconvolutionAPP}
echo "Using normalization application: "${normalizationAPP}
echo ""

if [ "$restart" -eq "0" ]
then
    # makeWeights
    ${PWD}/runapp.sh ${griddingAPP} weight ${imagename} ${input_file} ${logdir}
  
    # makePSF
    ${PWD}/runapp.sh ${griddingAPP} psf ${imagename} ${input_file} ${logdir}

    # normalize the PSF and make primary beam
    ${PWD}/runapp.sh ${normalizationAPP} normalize ${imagename} ${input_file} ${logdir} -t psf

    # make dirty image
    ${PWD}/runapp.sh ${griddingAPP} residual ${imagename} ${input_file} ${logdir} -c 0

    # normalize the residual
    ${PWD}/runapp.sh ${normalizationAPP} normalize ${imagename} ${input_file} ${logdir} -t residual -c 0
else
    echo "Doing only the update step..."
fi

i=$start_index

while [ ! -f stopIMCycles ] && [ "${i}" -lt "${ncycle}" ]
do
    # run hummbee for updateModel deconvolution iterations
    ${PWD}/runapp.sh ${deconvolutionAPP} deconvolve ${imagename} ${input_file} ${logdir} -c ${i}

    # run dale to divide model by weights
    ${PWD}/runapp.sh ${normalizationAPP} normalize ${imagename} ${input_file} ${logdir} -t model -c ${i}

    # run roadrunner for updateDir
    ${PWD}/runapp.sh ${griddingAPP} residual ${imagename} ${input_file} ${logdir} -m ${imagename}.divmodel -c ${i}

    # run dale to divide residual by weights
    ${PWD}/runapp.sh ${normalizationAPP} normalize ${imagename} ${input_file} ${logdir} -t residual -c ${i}
     
    i=$((i+1))
done

# run hummbee for restore
${PWD}/runapp.sh ${deconvolutionAPP} restore ${imagename} ${input_file} ${logdir}

if [ "${OSGjob}" = "True" ]
then
    createTar weight sumwt psf mask model residual image image.pbcor
    lsdir
fi
