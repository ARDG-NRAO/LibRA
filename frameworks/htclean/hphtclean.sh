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

#############################################################################################
#      Following needs review to work with relocatable roadrunner and parameter file        #
#############################################################################################

# CASA for gpuhost00x
#CASABUILD="CASA-HPG-TEST-GPUFFT"	# stable build used with scg-136/145
#CASABUILD="CAS-13581-Raw"		# oriented to OTF mosaic (VLASS)
#CASABUILD="CASA-HPG-TEST"		# multi-threaded grid/cfc load, branch of 13581
#CASAHOME=/home/yaghan3/sanjay/${CASABUILD}/casa6/casa5

#source /home/yaghan3/setup-spack.bash && source /home/yaghan3/sanjay/HPG/libhpg/hpg/setup.hpg
#source ${CASAHOME}/casainit.sh

#CASABIN=${CASAHOME}/linux_64b/bin/casa
#CASABIN=/home/casa/packages/RHEL7/release/casa-release-5.8.0-109.el7/bin/casa
#CUDACAP=8x
#RRBIN=${CASAHOME}/code/build-linux_64b/synthesis/roadrunner${CUDACAP}
RRBIN=/home/dhruva2/disk3/fmadsen/libra_acme/libra/install/bin/roadrunner
CASABIN=casa

#############################################################################################

# Input arguments
parfile=$1
imgname=$2
ncycle=$3

nparts=1
BIN=/users/fmadsen/libra/frameworks/htclean/pylib/
GO="\n inp \n go \n";

# To use the following options, make a local copy of hphtclean and launch manually
# restart=1 will pick up files from an existing htclean run and continue CLEANing
# set start_index to the current number of imaging cycles + 1 (next imaging cycle after restart)
# set GPUENGINE=0 to run casatools based gridding (same as serialized htclean)
restart=0 
start_index=1
GPUENGINE=1

# Begin execution block

echo "Using CASA binary: "${CASABIN}
echo "Using roadrunner binary: "${RRBIN}


if [ "$restart" -eq "0" ]
then
    if [ "$GPUENGINE" -eq "1" ]
    then
        #echo "Computing initial images...";
        # makeWeights
        #echo "Making weight images using HPG...";
	change_parameters="\n imagename=${imgname}.weight \n mode=weight"
        echo -e "load libra_htclean.def ${change_parameters} $GO" | ${RRBIN} help=dbg 2>| weight.out
    
        # makePSF
        #echo "Making PSF using HPG...";
	change_parameters="\n imagename=${imgname}.psf \n mode=psf"
        echo -e "load libra_htclean.def ${change_parameters} $GO" | ${RRBIN} help=dbg 2>| psf.out

        # normalize the PSF
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py gatherPSF ${parfile} serial >& gatherPSF.out

        # make the Primary beam
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py makePrimaryBeam ${parfile} serial >& makePB.out
    
        #echo "Making Dirty image using HPG...";
        # make dirty image
	change_parameters="\n imagename=${imgname}.residual \n mode=residual"
        echo -e "load libra_htclean.def ${change_parameters} $GO" | ${RRBIN} help=dbg 2>| dirty.out

    else
        # CPU-based PSF 
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py makePSF ${parfile} serial >& makePSF.out

        # normalize the PSF
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py gatherPSF ${parfile} serial >& gatherPSF.out

        # make the Primary beam
        #${CASABIN} --nologger --nogui -c ${BIN}htclean.py makePrimaryBeam ${parfile} serial >& makePB.out
    
        # CPU-based residual computation
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py runResidualCycle ${parfile} serial >& runResidualCycle_cycle0.out
    fi
    # normalize the residual
    ${CASABIN} --nologger --nogui -c ${BIN}htclean.py gather ${parfile} serial >& gather_cycle0.out
else
    echo "Doing only the update step..."
fi

i=$start_index

while [ ! -f stopIMCycles ] && [ "${i}" -lt "${ncycle}" ]
do
    ${CASABIN} --nologger --nogui -c ${BIN}htclean.py runModelCycle ${parfile} serial >& runModelCycle_cycle${i}.out

    if [ "$GPUENGINE" -eq "1" ]
    then
	change_parameters="\n imagename=${imgname}.residual \n modelimagename=${imgname}.model \n mode=residual"
        echo -e "load libra_htclean.def ${change_parameters} $GO" | ${RRBIN} help=dbg 2>| runResidualCycle_cycle${i}.out
    else
        ${CASABIN} --nologger --nogui -c ${BIN}htclean.py runResidualCycle ${parfile} serial >& runResidualCycle_cycle${i}.out
    fi

    ${CASABIN} --nologger --nogui -c ${BIN}htclean.py gather ${parfile} serial >& gather_cycle${i}.out
       
    i=$((i+1))
done

${CASABIN} --nologger --nogui -c ${BIN}htclean.py makeFinalImages ${parfile} serial >& makeFinalImages.out
