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
# Defaults

[ -n "${1}" ] && msname=${1}
[ -n "${2}" ] && partaxis=${2}
[ -n "${3}" ] && nparts=${3}
[ -n "${4}" ] && user=${4}
[ -n "${5}" ] && ssh_key=${5}
[ -n "${6}" ] && initialdir=${6}


echo "msname is (${1})"
echo "partaxis is (${2})"
echo "nparts is (${3})"
echo "user is (${4})"
echo "ssh_key is (${5})"
echo "initialdir is (${6})"


mkdir -p data

export HOME=${HOME:=$TMPDIR}
export OMP_NUM_THREADS=1


cd data

cdate=$(date +%Y%m%d-%H%M%S)


CASABIN=/home/casa/packages/RHEL7/release/casa-6.5.0-15-py3.6/bin/

logname="MSpartition-${cdate}"
${CASABIN}/casa --nologger --nogui --logfile ${logname}.log -c ../pylib/MSpartition.py ../${msname}.ms ${partaxis} ${nparts} &> ${logname}.out
