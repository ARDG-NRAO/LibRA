#!/bin/bash

# Ask for the bin path if not provided as argument
if [ -z "$1" ]; then
    read -p "Enter the path to the libra bin directory: " BIN_PATH
else
    BIN_PATH="$1"
fi

COYOTE="$BIN_PATH/coyote"
ROADRUNNER="$BIN_PATH/roadrunner"

if [ ! -x "$COYOTE" ] || [ ! -x "$ROADRUNNER" ]; then
    echo "Error: coyote or roadrunner not found or not executable in $BIN_PATH"
    exit 1
fi

# Check for --cf flag
RUN_CF=0
for arg in "$@"; do
    if [ "$arg" == "--cf" ]; then
        RUN_CF=1
    fi
done

export CASAPATH=$PWD

# Define frequencies and spws
freqs=("1.0" "1.2" "1.4" "1.6" "1.8")
spws=("0" "1" "2" "3" "4")

# Only run CF generation if --cf is specified
if [ "$RUN_CF" -eq 1 ]; then
    for i in "${!freqs[@]}"; do
        freq="${freqs[$i]}"
        spw="${spws[$i]}"
        cfname="sim_${freq}.cf"
        reffreq="${freq}GHz"
        $coyote help=noprompt vis="sim_VLA.ms" telescope="EVLA" imsize="512" cell="12" stokes="I" reffreq="$reffreq" phasecenter="19:59:28.500000 +40.40.00.00000 J2000" wplanes="1" cfcache="$cfname" wbawp="1" aterm="1" psterm="0" conjbeams="0" muellertype="diagonal" dpa="360" field="" spw="$spw" buffersize="0" oversampling="20" mode="dryrun" cflist=""
        $coyote help=noprompt vis="sim_VLA.ms" telescope="EVLA" imsize="512" cell="12" stokes="I" reffreq="$reffreq" phasecenter="19:59:28.500000 +40.40.00.00000 J2000" wplanes="1" cfcache="$cfname" wbawp="1" aterm="1" psterm="0" conjbeams="0" muellertype="diagonal" dpa="360" field="" spw="$spw" buffersize="0" oversampling="20" mode="fillcf" cflist="CFS*"
    done
fi

for i in "${!freqs[@]}"; do
    freq="${freqs[$i]}"
    spw="${spws[$i]}"
    cfname="sim_${freq}.cf"
    modelimage="sim_${freq}GHz.im"
    $roadrunner help=noprompt vis="sim_VLA.ms" imagename="test" modelimagename="$modelimage" datacolumn="data" sowimageext="sumwt" complexgrid="" imsize="512" cell="12" stokes="I" reffreq="mean" phasecenter="19:59:28.500000 +40.40.00.00000 J2000" weighting="natural" rmode="none" robust="0" wprojplanes="1" gridder="awphpg" cfcache="$cfname" mode="predict" wbawp="1" field="" spw="$spw" uvrange="" pbcor="1" conjbeams="0" pblimit="0.001" usepointing="0" pointingoffsetsigdev="300,300"
done
