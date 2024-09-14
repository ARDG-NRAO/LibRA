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


# Input arguments
imgname=$1
ncycle=${2:-10}
parfile=${3:-libra_htclean.def}

BIN=/users/fmadsen/htclean/
GO="\n inp \n go \n";

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
    change_parameters="\n imagename=${imgname}.weight \n mode=weight"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${griddingAPP} help=dbg 2>| weight.out
 
    # makePSF
    #echo "Making PSF using HPG...";
    change_parameters="\n imagename=${imgname}.psf \n mode=psf"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${griddingAPP} help=dbg 2>| psf.out
 
    # normalize the PSF and make primary beam
    change_parameters="\n imagename=${imgname} \n imtype=psf \n computepb=1"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${normalizationAPP} help=dbg 2>| norm_psf.out
 
    #echo "Making Dirty image using HPG...";
    # make dirty image
    change_parameters="\n imagename=${imgname}.residual \n mode=residual"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${griddingAPP} help=dbg 2>| dirty.out

    # normalize the residual
    change_parameters="\n imagename=${imgname} \n imtype=residual"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${normalizationAPP} help=dbg 2>| norm_dirty.out
else
    echo "Doing only the update step..."
fi

i=$start_index

while [ ! -f stopIMCycles ] && [ "${i}" -lt "${ncycle}" ]
do
    # run hummbee for updateModel deconvolution iterations
    change_parameters="\n imagename=${imgname} \n mode=deconvolve"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${deconvolutionAPP} help=dbg 2>| updateModel_cycle${i}.out
    grep "Reached n-sigma threshold" updateModel_cycle${i}.out
    [ "$?" -eq "0" ] && touch stopIMCycles

    # run acme to multiply model by weights
    change_parameters="\n imagename=${imgname} \n imtype=model"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${normalizationAPP} help=dbg 2>| divideModel_cycle${i}.out
    
    # run roadrunner for updateDir
    change_parameters="\n imagename=${imgname}.residual \n modelimagename=${imgname}.divmodel \n mode=residual"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${griddingAPP} help=dbg 2>| updateDir_cycle${i}.out

    # run acme to divide residual by weights
    change_parameters="\n imagename=${imgname} \n imtype=residual"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${normalizationAPP} help=dbg 2>| divideResidual_cycle${i}.out
       
    i=$((i+1))
done

    # run hummbee for restore
    change_parameters="\n imagename=${imgname} \n mode=restore"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${deconvolutionAPP} help=dbg 2>| makeFinalImages.out

