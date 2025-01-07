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

usage()
{
    echo "$0 : Script to make a normalized image using LibRA components."
    echo "Usage: $0 -i <imagename> [-p <input file>] [-l <logdir>] [-L <LibRA root>] [-h]"$'\n'\
         "       -i set the basename of the images to be generated (without extension)"$'\n'\
         "       -p (optional) set the name of the iput parameter file. Default: `basename $0 .sh`.def"$'\n'\
         "       -l (optional) directory to save .log and .def files. Default: save to current directory."$'\n'\
         "       -L (optional) LibRA root directory. Default is <user home directory>/libra."$'\n'\
         "       -h prints help and exits"$'\n\n'\
	 "       Example: $0 -i ksg1_natural_norm -L <PATH TO LIBRA APP BINARIES> -l logs.4K_natural -p imaging.def"$'\n'
}
if [ $# -eq 0 ] 
then 
	usage
	exit 0
fi

# Default is where this script resides
LIBRAHOME=$(dirname $(readlink -f $0));

while getopts "i:l:L:p:h" option
do
    case "$option" in
        i) imagename=${OPTARG}                    ;;
        l) logdir=${OPTARG}/                      ;;
    	p) input_file=${OPTARG}                   ;;
    	L) LIBRAHOME=${OPTARG}                    ;;
    	h) usage
           exit 0                                 ;;
    	*) echo "${option}: Unknown option"
    	   usage
    	   exit 1                                 ;;
    esac
done

runApp=${LIBRAHOME}/runapp.sh
libraBIN=${LIBRAHOME}/bin
griddingAPP=${libraBIN}/roadrunner
normalizationAPP=${libraBIN}/dale
mkdir -p ${logdir}

# Begin execution block

echo "Using gridding application:      "${griddingAPP}
echo "Using normalization application: "${normalizationAPP}
echo ""

# # makeWeights
${runApp} ${griddingAPP} weight ${imagename} ${input_file} ${logdir}

# # # makePSF
${runApp} ${griddingAPP} psf ${imagename} ${input_file} ${logdir}

# # # normalize the PSF and make primary beam
${runApp} ${normalizationAPP} normalize ${imagename} ${input_file} ${logdir} -t psf

# # # make dirty image
${runApp} ${griddingAPP} residual ${imagename} ${input_file} ${logdir} -c 0

# # # normalize the residual
${runApp} ${normalizationAPP} normalize ${imagename} ${input_file} ${logdir} -t residual -c 0
