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
    echo "$0 : Script to change imaging parameters and start individual imaging steps."
    echo "Usage: $0 <application> <mode> <imagename> <input file> [-t <imptype>] [-m modelimagename] [-c <index of imaging cycle>] [-l <logdir>] [-h]"$'\n'\
         "   Required parameters:"$'\n'\
         "       application  : full path to the application executable."$'\n'\
         "       mode         : application specific mode if running roadrunner or hummbee. Set to normalize if running dale."$'\n'\
         "       imagename    : basename of the image to be generated or processed"$'\n'\
         "       input file   : name of input parameter file"$'\n'\
	     "       logdir       : directory to save .log and .def files."$'\n'\
         "   Optional parameters:"$'\n'\
         "       -t set imtype when running dale."$'\n'\
         "       -f set name of convolution function cache (cfcache) to use with mode = weight | psf | residual."$'\n'\
         "       -s set modelimagename for model subtraction if running roadrunner with mode=residual."$'\n'\
         "       -c index of the current imaging cycle."$'\n'\
         "       -h prints help and exits"$'\n'\
	     "   Example: $0 /path/to/roadrunner weight my_image my_parameters.def -l /path/to/my_logs (for running roadrunner to compute weights and save logs to given location.)"$'\n\n'
}

app=$1
mode=$2
imagename=$3
input_file=$4
logdir=$5
shift 5

while true
do
    case "$1" in
    -t) imtype=$2               ; shift 2  ;;
    -f) cfcache=$2              ; shift 2  ;;
    -s) modelimagename=$2       ; shift 2  ;;
    -c) imcycle=_imcycle$2      ; shift 2  ;;
    -l) logdir=$2/              ; shift 2  ;;
    *) break ;;
    esac
done

case "$mode" in
	weight | psf | residual)
        logname=${logdir}${mode}-$(date +%Y%m%d-%H%M%S)${imcycle}
        cp ${input_file} ${logname}.def
	    echo "imagename = ${imagename}.${mode}" >> ${logname}.def
	    echo "mode = ${mode}" >> ${logname}.def
        [ -n "${modelimagename}" ] && echo "modelimagename = ${modelimagename}" >> ${logname}.def
        [ -n "${cfcache}" ] && echo "cfcache = ${cfcache}" >> ${logname}.def
	    ;;
    normalize)
        logname=${logdir}${mode}_${imtype}-$(date +%Y%m%d-%H%M%S)${imcycle}
        cp ${input_file} ${logname}.def
	    echo "imagename = ${imagename}" >> ${logname}.def
	    echo "imtype = ${imtype}" >> ${logname}.def
	    [ "${imtype}" = "psf" ] && echo "computepb = 1" >> ${logname}.def
	    ;;
    deconvolve | restore)
	    if [ $mode = deconvolve ]
	    then
            logname=${logdir}modelUpdate-$(date +%Y%m%d-%H%M%S)${imcycle}
	    else
		    logname=${logdir}makeFinalImages-$(date +%Y%m%d-%H%M%S)${imcycle}
	    fi
        cp ${input_file} ${logname}.def
	    echo "imagename = ${imagename}" >> ${logname}.def
	    echo "mode = ${mode}" >> ${logname}.def
	    ;;
esac

${app} help=def,${logname}.def &> ${logname}.log
if [ $mode = deconvolve ]
then
    grep "Reached global stopping criteria\|Reached n-sigma threshold" ${logname}.log
    [ "$?" -eq "0" ] && touch stopIMCycles
fi