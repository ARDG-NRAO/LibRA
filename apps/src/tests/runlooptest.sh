#!/bin/bash

# Default values for common parameters
IMSIZE="1024"
CELL="5"
STOKES="I"
PHASECENTER="19:59:28.523 +40.54.01.152 J2000"
REFFREQ="1.5GHz"
WPLANES="1"
CFCACHE="awp.cf"
WBAWP="1"
ATERM="1"
PSTERM="0"
CONJBEAMS="0"
MUELLERTYPE="diagonal"
DPA="360"
FIELD="0"
SPW="*"
BUFFERSIZE="0"
OVERSAMPLING="20"
MODE="dryrun"
CFLIST=""
PBLIMIT="0.001"
NTERMS="2"
GAIN="0.1"
THRESHOLD="0.00001"
CYCLENITER="100"
CYCLEFACTOR="1"
SPECMODE="mfs"
PBCOR="0"

# Parse command line arguments
while [ "$#" -gt 0 ]; do
    case $1 in
        --bin) BIN_PATH="$2"; shift ;;
        --data) DATA_PATH="$2"; shift ;;
        --coyote) COYOTE="1" ;;
        --help)
            echo "Usage: $0 --bin <path_to_bin> --data <path_to_data>"
            echo "  --bin   Path to the bin area"
            echo "  --data  Path to the data area"
            echo "  --coyote Run Coyote to make cf"
            echo "  --help  Display this help message"
            exit 0
            ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

# Ensure BIN_PATH and DATA_PATH are set
if [ -z "$BIN_PATH" ] || [ -z "$DATA_PATH" ]; then
    echo "Error: --bin and --data parameters are required."
    echo "Use --help for more information."
    exit 1
fi

roadrunner=$BIN_PATH/roadrunner
coyote=$BIN_PATH/coyote
taylor=$BIN_PATH/taylor
dale=$BIN_PATH/dale
hummbee=$BIN_PATH/hummbee

# start by creating the convolution function from the measurement set

mset="$DATA_PATH/refim_oneshiftpoint.mosaic.ms"

# Create the convolution function cache

if [ "$COYOTE" = "1" ]; then
    $coyote help=noprompt vis="$mset" telescope="EVLA" imsize="$IMSIZE" cell="$CELL" stokes="$STOKES" reffreq="$REFFREQ" phasecenter="$PHASECENTER" wplanes="$WPLANES" cfcache="$CFCACHE" wbawp="$WBAWP" aterm="$ATERM" psterm="$PSTERM" conjbeams="$CONJBEAMS" muellertype="$MUELLERTYPE" dpa="$DPA" field="$FIELD" spw="$SPW" buffersize="$BUFFERSIZE" oversampling="$OVERSAMPLING" mode="$dryrun" cflist="$CFLIST"
    $coyote help=noprompt vis="$mset" telescope="EVLA" imsize="$IMSIZE" cell="$CELL" stokes="$STOKES" reffreq="$REFFREQ" phasecenter="$PHASECENTER" wplanes="$WPLANES" cfcache="$CFCACHE" wbawp="$WBAWP" aterm="$ATERM" psterm="$PSTERM" conjbeams="$CONJBEAMS" muellertype="$MUELLERTYPE" dpa="$DPA" field="$FIELD" spw="$SPW" buffersize="$BUFFERSIZE" oversampling="$OVERSAMPLING" mode="fillcf" cflist="CFS*"
fi


# Function to run roadrunner with different modes
run_roadrunner() {
    local mode=$1
    local suffix=$2
    local phasecenter=${3:-$PHASECENTER}
    for spw in 0 1 2; do
        $roadrunner help=noprompt vis="$mset" imagename="dirty_spw${spw}.${suffix}" modelimagename="${4:-}" datacolumn="" sowimageext="sumwt" complexgrid="" imsize="$IMSIZE" cell="$CELL" stokes="$STOKES" reffreq="" phasecenter="$phasecenter" weighting="natural" rmode="none" robust="0" wprojplanes="$WPLANES" gridder="awphpg" cfcache="$CFCACHE" mode="$mode" wbawp="false" field="$FIELD" spw="$spw" uvrange="" pbcor="1" conjbeams="1" pblimit="$PBLIMIT" usepointing="0" pointingoffsetsigdev="300,300"
    done
}

# Function to run dale
run_dale() {
    local suffix=$1
    for spw in 0 1 2; do
        $dale help=noprompt imagename="dirty_spw${spw}.${suffix}" imtype="$suffix" weightimage="dirty_spw${spw}.weight" sowimage="dirty_spw${spw}.sumwt" pblimit="0.0001" computepb="1"
    done
}

# Function to run taylor
run_taylor() {
    local mode=$1
    local imType=$2
    $taylor help=noprompt taylorImages="dirty_spw0.${imType}" cubeImage="dirty_spw0.${imType},dirty_spw1.${imType},dirty_spw2.${imType}" pbimage="dirty_spw0.pb,dirty_spw1.pb,dirty_spw2.pb" sumwtImage="dirty_spw0.sumwt,dirty_spw1.sumwt,dirty_spw2.sumwt" overwrite="1" mode="$mode" reffreq="$REFFREQ" nTerms="$NTERMS" pblimit="$PBLIMIT" imType="$imType"
}

# Run roadrunner for residual, psf, and weight modes
run_roadrunner "residual" "residual"
run_roadrunner "psf" "psf" "19:59:28.500000 +40.40.01.50000 J2000" 
run_roadrunner "weight" "weight"

# Normalize with dale
run_dale "psf"
run_dale "residual"

# Rename psf.pb files to pb
for spw in 0 1 2; do
    mv "dirty_spw${spw}.psf.pb" "dirty_spw${spw}.pb"
done

# Convert cubes to taylor images
run_taylor "cube2taylor" "residual"
run_taylor "cube2taylor" "pb"
run_taylor "cube2taylor" "psf"

# Clean using hummbee
$hummbee help=noprompt imagename="dirty_spw0" modelimagename="" deconvolver="mtmfs" scales="" largestscale="-1" fusedthreshold="0" nterms="$NTERMS" gain="$GAIN" nsigma="0" threshold="$THRESHOLD" cycleniter="$CYCLENITER" cyclefactor="$CYCLEFACTOR" mask="" specmode="$SPECMODE" pbcor="$PBCOR" mode="deconvolve"

# Create empty copies of the .model by copying the .residual
for spw in 0 1 2; do
    mv "dirty_spw${spw}.residual" "dirty_spw${spw}.model"
done

# Convert taylor to cube
run_taylor "taylor2cube" "model"

# Create residual images per spw with roadrunner
run_roadrunner "residual" "residual" "${PHASECENTER}" "dirty_spw0.model"

# Normalize with dale the residuals
run_dale "residual"

# Convert cube to taylor
run_taylor "cube2taylor" "residual"

# Run hummbee again
$hummbee help=noprompt imagename="dirty_spw0" modelimagename="dirty_spw0.model" deconvolver="mtmfs" scales="" largestscale="-1" fusedthreshold="0" nterms="$NTERMS" gain="$GAIN" nsigma="0" threshold="$THRESHOLD" cycleniter="$CYCLENITER" cyclefactor="$CYCLEFACTOR" mask="" specmode="$SPECMODE" pbcor="$PBCOR" mode="deconvolve"

# Convert taylor to cube again
run_taylor "taylor2cube" "model"

# Create residual images per spw with roadrunner again
run_roadrunner "residual" "residual" "${PHASECENTER}" "dirty_spw0.model"

# Normalize with dale the residuals again
run_dale "residual"

# Convert cube to taylor again
run_taylor "cube2taylor" "residual"

# Run hummbee again
$hummbee help=noprompt imagename="dirty_spw0" modelimagename="dirty_spw0.model" deconvolver="mtmfs" scales="" largestscale="-1" fusedthreshold="0" nterms="$NTERMS" gain="$GAIN" nsigma="0" threshold="$THRESHOLD" cycleniter="$CYCLENITER" cyclefactor="$CYCLEFACTOR" mask="" specmode="$SPECMODE" pbcor="$PBCOR" mode="deconvolve"

# Convert taylor to cube again
run_taylor "taylor2cube" "model"

# Create residual images per spw with roadrunner again
run_roadrunner "residual" "residual" "${PHASECENTER}" "dirty_spw0.model"

# Normalize with dale the residuals again
run_dale "residual"
