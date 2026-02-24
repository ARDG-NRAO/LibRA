#!/bin/bash

OMP_NUM_THREADS=1

CASABIN="casa -r 6.1.0-118"

# deconvolver for nemo. wisclean in libra
HBIN=../../install/bin/hummbee

partname="serial"
imgname="htclean_wasp_G55" 
parfile="wasp_G55.params"
ncycle=5
nparts=1
restart=0
BIN=pylib/
GO="\n inp \n go \n";
GPUENGINE=0


if [ $# -lt 1 ];
then
    echo "Usage: $0 #ofModelCycles [0/1]"
    exit 1;
else
    ncycle=$1;
    if [ $# -gt 1 ];
    then
	restart=$2;
    fi
fi
#
#----------------------------------------------------------------------------------------
#
echo "Using CASA binary: "${CASABIN}

# clean .out so we can detect error messages better 
rm -rf *.out
rm -rf casa-2026*.log
rm -rf ${imgname}*
rm -rf state.txt

if [ "$restart" -eq "0" ]
then
    if [ "$GPUENGINE" -eq "1" ]
    then
        #echo "Computing initial images...";
        # makeWeights - generate .weight and gridded visibilities .vis
        # the gridded residual vis is changed every time roadrunner is called.
        echo "Making weight images using HPG...";
        echo -e "load weight_revE_newpsf.def $GO" | ${RRBIN} help=dbg 2>| weight.out  # runs roadrunner
    
        # makePSF - generate .psf and .sumwt
        echo "Making PSF using HPG...";
        echo -e "load psf_revE_newpsf.def $GO" | ${RRBIN} help=dbg 2>| psf.out
        # save the initial psf
        cp -r ${imgname}.psf ${imgname}_cycle0.psf

        # normalize the PSF - update .psf/.sumwt/.weight
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py gatherPSF ${parfile} serial >& gatherPSF.out
        # copy original .weight and .sumwt for later use
        cp -r ${imgname}.weight ${imgname}.weight.cycle0
        cp -r ${imgname}.sumwt ${imgname}.sumwt.cycle0

        # make the Primary beam
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py makePrimaryBeam ${parfile} serial >& makePB.out
    
        # make dirty image (.residual)
        echo "Making Dirty image using HPG...";
        echo -e "load dirty_revE_newpsf.def $GO" | ${RRBIN} help=dbg 2>| dirty.out

        # save the gridded vis
        cp -r ${imgname}_gridv.vis ${imgname}_gridv_cycle0.vis

    else
        echo "Doing the set up makePSF on cpu"
        # CPU-based PSF 
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py makePSF ${parfile} serial >& makePSF.out
        
        echo "Doing the setup gatherPSF on cpu"
        # normalize the PSF
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py gatherPSF ${parfile} serial >& gatherPSF.out

        echo "Doing the setup makePrimaryBeam on cpu"
        # make the Primary beam
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py makePrimaryBeam ${parfile} serial >& makePB.out
    
        echo "Doing the setup runResidualCycle on cpu"
        # CPU-based residual computation
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py runResidualCycle ${parfile} serial >& runResidualCycle_cycle0.out
    fi
    echo "Doing the set up gather"
    # normalize the residual
    ${CASABIN} --nologger --nogui -c ${BIN}htclean.py gather ${parfile} serial >& gather_cycle0.out

else
    echo "Doing only the update step..."
fi

i=1

# debug message
echo "ncycle ${ncycle}, stopIM ${stopIMCycles}, i ${i}"


#while [ ! -f stopIMCycles ] && [ "${i}" -lt "${ncycle}" ]
while [ "${i}" -lt "${ncycle}" ]
do  

    ##rm -rf ${imgname}.mask
    # make model image (this changes .residual as well and creates .mask)
    echo "Run model on gpu at cycle ${i}"
    ##${CASABIN} --nologger --nogui -c ${BIN}htclean.py runModelCycle ${parfile} ${partname} >& runModelCycle_cycle${i}.out
    echo -e "load model.def $GO" | ${HBIN} help=dbg 2>| runModelCycle_cycle${i}.out
    ${CASABIN} --nologger --nogui -c ${BIN}htclean.py scatter ${parfile} ${partname} >& scatter_cycle${i}.out

    # updates the residual
    if [ "$GPUENGINE" -eq "1" ]
    then
        echo "load residual on gpu at cycle ${i}"
        echo -e "load residual_revE_newpsf.def $GO" | ${RRBIN} help=dbg 2>| runResidualCycle_cycle${i}.out
    else
        echo "run residual cycle ${i}"
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py runResidualCycle ${parfile} ${partname} >& runResidualCycle_cycle${i}.out
    fi

    # save the gridded vis
    #cp -r ${imgname}_gridv.vis ${imgname}_gridv_cycle${i}.vis
    #cp -r ${imgname}.model ${imgname}.model.cycle${i}

    # normalized the residual (this changes .sumwt and .model as well)
    ${CASABIN} --nologger --nogui -c ${BIN}htclean.py gather ${parfile} ${partname} >& gather_cycle${i}.out


    i=$((i+1))
done


${CASABIN} --nologger --nogui -c ${BIN}htclean.py makeFinalImages ${parfile} ${partname} >& makeFinalImages.out
echo "Finish make final images"
