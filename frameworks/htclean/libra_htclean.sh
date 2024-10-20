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


OMP_NUM_THREADS=1
OMP_PROC_BIND=False

LIBRAHOME=/home/gpuhost003/fmadsen/libra
griddingAPP=${LIBRAHOME}/install/bin/roadrunner
deconvolutionAPP=${LIBRAHOME}/install/bin/hummbee
normalizationAPP=${LIBRAHOME}/install/bin/dale

name=`basename $0 .sh`

# Input arguments
imagename=$1
ncycle=${2:-10}
input_file=${3:-${name}.def}


# restart=1 will pick up files from an existing htclean run and continue CLEANing
# set start_index to the current number of imaging cycles + 1 (next imaging cycle after restart)
# set GPUENGINE=0 to run casatools based gridding (same as serialized htclean)
restart=0 
start_index=1

# Begin execution block

echo "Using gridding application: "${griddingAPP}
echo "Using deconvolution application: "${deconvolutionAPP}
echo "Using normalization application: "${normalizationAPP}


if [ "$restart" -eq "0" ]
then
    #echo "Computing initial images...";
    # makeWeights
    #echo "Making weight images using HPG...";
    logname=weight-$(date +%Y%m%d-%H%M%S)
    cp ${input_file} ${logname}.def
    echo "imagename = ${imagename}.weight" >> ${logname}.def
    echo "mode = weight" >> ${logname}.def
    ${griddingAPP} help=def,${logname}.def &> ${logname}.log
 
    # makePSF
    #echo "Making PSF using HPG...";
    logname=psf-$(date +%Y%m%d-%H%M%S)
    cp ${input_file} ${logname}.def
    echo "imagename = ${imagename}.psf" >> ${logname}.def
    echo "mode = psf" >> ${logname}.def
    ${griddingAPP} help=def,${logname}.def &> ${logname}.log
 
    # normalize the PSF and make primary beam
    logname=norm_psf-$(date +%Y%m%d-%H%M%S)
    cp ${input_file} ${logname}.def
    echo "imagename = ${imagename}" >> ${logname}.def
    echo "imtype = psf" >> ${logname}.def
    echo "computepb = 1" >> ${logname}.def
    ${normalizationAPP} help=def,${logname}.def &> ${logname}.log

    #echo "Making Dirty image using HPG...";
    # make dirty image
    logname=dirty-$(date +%Y%m%d-%H%M%S)
    cp ${input_file} ${logname}.def
    echo "imagename = ${imagename}.residual" >> ${logname}.def
    echo "mode = residual" >> ${logname}.def
    ${griddingAPP} help=def,${logname}.def &> ${logname}.log

    # normalize the residual
    logname=norm_dirty-$(date +%Y%m%d-%H%M%S)
    cp ${input_file} ${logname}.def
    echo "imagename = ${imagename}" >> ${logname}.def
    echo "imtype = residual" >> ${logname}.def
    ${normalizationAPP} help=def,${logname}.def &> ${logname}.log
else
    echo "Doing only the update step..."
fi

i=$start_index

while [ ! -f stopIMCycles ] && [ "${i}" -lt "${ncycle}" ]
do
    # run hummbee for updateModel deconvolution iterations
    logname=modelUpdate-$(date +%Y%m%d-%H%M%S)_imcycle${i}
    cp ${input_file} ${logname}.def
    echo "imagename = ${imagename}" >> ${logname}.def
    echo "mode = deconvolve" >> ${logname}.def
    ${deconvolutionAPP} help=def,${logname}.def &> ${logname}.log
    grep "Reached global stopping criteria\|Reached n-sigma threshold" ${logname}.log
    [ "$?" -eq "0" ] && touch stopIMCycles

    # run dale to divide model by weights
    logname=norm_model-$(date +%Y%m%d-%H%M%S)_imcycle${i}
    cp ${input_file} ${logname}.def
    echo "imagename = ${imagename}" >> ${logname}.def
    echo "imtype = model" >> ${logname}.def
    ${normalizationAPP} help=def,${logname}.def &> ${logname}.log

    # run roadrunner for updateDir
    logname=residual-$(date +%Y%m%d-%H%M%S)_imcycle${i}
    cp ${input_file} ${logname}.def
    echo "imagename = ${imagename}.residual" >> ${logname}.def
    echo "modelimagename=${imagename}.divmodel" >> ${logname}.def
    echo "mode = residual" >> ${logname}.def
    ${griddingAPP} help=def,${logname}.def &> ${logname}.log

    # run dale to divide residual by weights
    logname=norm_residual-$(date +%Y%m%d-%H%M%S)_imcycle${i}
    cp ${input_file} ${logname}.def
    echo "imagename = ${imagename}" >> ${logname}.def
    echo "imtype = residual" >> ${logname}.def
    ${normalizationAPP} help=def,${logname}.def &> ${logname}.log

    i=$((i+1))
done

# run hummbee for restore
logname=makeFinalImages-$(date +%Y%m%d-%H%M%S)
cp ${input_file} ${logname}.def
echo "imagename = ${imagename}" >> ${logname}.def
echo "mode = restore" >> ${logname}.def
${deconvolutionAPP} help=def,${logname}.def &> ${logname}.log

