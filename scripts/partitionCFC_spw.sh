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
# License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning AIPS++ should be addressed as follows:
#        Internet email: aips2-request@nrao.edu.
#        Postal address: AIPS++ Project Office
#                        National Radio Astronomy Observatory
#                        520 Edgemont Road
#                        Charlottesville, VA 22903-2475 USA
#

# A script to detect unique SPW indeces in the given CFC, and make
# partitioned CFCs per SPW index with only the CFs for one SPW.
#

set -e

CFCName=$1
copymode=${2:-""}

cfcbasename=`basename ${CFCName} .cfc`

if [ "$copymode" = "deepcopy" ]
then
    deepcopy=true
else
    deepcopy=false
fi

CFNAMING="CFS_T_B_CF_SPW_W_POL.im"
WTCFNAMING="WT"$CFNAMING

copyCF(){
    if ${deepcopy}
    then
        cp -r $1 .
    else
        ln -s $1
    fi
}

# Collect CF names
cfs=`ls -d $CFCName/* | grep CFS*`

# Collect names of misc info files/dirs
misc=`ls -d $CFCName/* | grep -v CFS*`
#echo $misc;

#
# Make a list of unique SPW indices found
#
SPWL=();
for cfname in $cfs; do
    nm=`basename $cfname`;
    n=`echo $nm | awk -F_ '{print $5}' -`
    SPWL+=($n);
done;
#echo ${SPWL[@]};
L=()
L+=( `echo ${SPWL[@]} | tr ' ' '\n' | sort -nu` );
#echo ${L[@]};

for s in ${L[@]}; do
    #
    # For each SWP index, make a pattern that matches the CFS for the given SPW index.
    #
    PATTERN="CFS_*_*_CF_"$s"_*_*.im"
    tt=`ls -d $CFCName/$PATTERN|wc -l`
    echo "$PATTERN: SPW "$s" has "$tt" CFS."

    #
    # Make the partitionend CFC dir, and copy the misc files
    #
    PartName="${cfcbasename}_SPW"$s".cfc";
    mkdir -p $PartName;
    for m in $misc; do
	echo $m;
	cp -r $m $PartName;
    done

    #
    # Make a list of CFS and WTCFS for the current SPW, and make a
    # symbolic link in the partitioned CFC dir.  If necessary, the
    # symb link can be replaced with deep copy.
    #
    CFList=`realpath $CFCName/$PATTERN`
    WTCFList=`realpath $CFCName/"WT"$PATTERN`
    cd $PartName;
    for cf in $CFList; do
	copyCF $cf;
    done
    for wtcf in $WTCFList; do
	copyCF $wtcf;
    done
    cd ..
done;
