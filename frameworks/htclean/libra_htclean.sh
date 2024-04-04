#!/bin/bash


OMP_NUM_THREADS=1
OMP_PROC_BIND=False

LIBRAHOME=/home/dhruva2/disk3/fmadsen/libra_acme/libra
roadrunnerBIN=${LIBRAHOME}/install/bin/roadrunner
hummbeeBIN=${LIBRAHOME}/install/bin/hummbee
acmeBIN=${LIBRAHOME}/build/Libra/apps/src/acme


# Input arguments
imgname=$1
ncycle=${2:-10}
parfile=${3:-libra_htclean.def}

nparts=1
BIN=/users/fmadsen/htclean/
GO="\n inp \n go \n";

# restart=1 will pick up files from an existing htclean run and continue CLEANing
# set start_index to the current number of imaging cycles + 1 (next imaging cycle after restart)
# set GPUENGINE=0 to run casatools based gridding (same as serialized htclean)
restart=0 
start_index=1

# Begin execution block

echo "Using roadrunner binary: "${roadrunnerBIN}
echo "Using hummbee binary: "${hummbeeBIN}
echo "Using acme binary: "${acmeBIN}


if [ "$restart" -eq "0" ]
then
    #echo "Computing initial images...";
    # makeWeights
    #echo "Making weight images using HPG...";
    change_parameters="\n imagename=${imgname}.weight \n mode=weight"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${roadrunnerBIN} help=dbg 2>| weight.out
 
    # makePSF
    #echo "Making PSF using HPG...";
    change_parameters="\n imagename=${imgname}.psf \n mode=psf"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${roadrunnerBIN} help=dbg 2>| psf.out
 
    # normalize the PSF and make primary beam
    change_parameters="\n imagename=${imgname} \n imtype=psf \n computepb=1"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${acmeBIN} help=dbg 2>| norm_psf.out
 
    #echo "Making Dirty image using HPG...";
    # make dirty image
    change_parameters="\n imagename=${imgname}.residual \n mode=residual"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${roadrunnerBIN} help=dbg 2>| dirty.out

    # normalize the residual
    change_parameters="\n imagename=${imgname} \n imtype=residual"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${acmeBIN} help=dbg 2>| norm_dirty.out
else
    echo "Doing only the update step..."
fi

i=$start_index

while [ ! -f stopIMCycles ] && [ "${i}" -lt "${ncycle}" ]
do
    # run hummbee for updateModel deconvolutions
    change_parameters="\n imagename=${imgname}"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${hummbeeBIN} help=dbg 2>| updateModel_cycle${i}.out
    grep "Reached n-sigma threshold" updateModel_cycle${i}.out
    [ "$?" -eq "0" ] && touch stopIMCycles

    # run acme to multiply model by weights
    change_parameters="\n imagename=${imgname} \n imtype=model"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${acmeBIN} help=dbg 2>| divideModel_cycle${i}.out
    
    # run roadrunner for updateDir
    change_parameters="\n imagename=${imgname}.residual \n modelimagename=${imgname}.divmodel \n mode=residual"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${roadrunnerBIN} help=dbg 2>| updateDir_cycle${i}.out

    # run acme to divide residual by weights
    change_parameters="\n imagename=${imgname} \n imtype=residual"
    echo -e "load ${parfile} ${change_parameters} $GO" | ${acmeBIN} help=dbg 2>| divideResidual_cycle${i}.out
       
    i=$((i+1))
done

echo "Finished running imaging cycles, restore stage not yet implemented in libRA."
