#!/bin/bash
# This is a script to test the taylor app and is similar to runlooptest.sh Only we are testing individual steps
# of the algorithm

# Default values for common parameters
IMSIZE="512"
CELL="12"
STOKES="I"
PHASECENTER="19:59:28.500000 +40.40.00.00000 J2000"
REFFREQ="1.4GHz"
WPLANES="1"
CFCACHE="awp.cf"
WBAWP="1"
ATERM="1"
PSTERM="0"
CONJBEAMS="0"
MUELLERTYPE="diagonal"
DPA="360"
FIELD="*"
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

cat <<EOL > source.mask
#CRTFv0
global coord=J2000
circle[[19:59:28.5, +40.40.00], 0.5pix]
#circle[[19:59:28.5, +40.45.00], 0.5pix]
#circle[[19:58:05.5, +40.55.00], 0.5pix]
#circle[[19:59:28.5, +40.35.00], 0.5pix]
EOL

# path to binaries
BIN_PATH="/home/nemo/Software/libra_taylor/install/bin"
roadrunner=$BIN_PATH/roadrunner
coyote=$BIN_PATH/coyote
taylor=$BIN_PATH/taylor
dale=$BIN_PATH/dale
hummbee=$BIN_PATH/hummbee

# start by creating the convolution function from the measurement set

mset="sim_VLA.ms"

# Create the convolution function cache

#$coyote help=noprompt vis="$mset" telescope="EVLA" imsize="$IMSIZE" cell="$CELL" stokes="$STOKES" reffreq="$REFFREQ" phasecenter="$PHASECENTER" wplanes="$WPLANES" cfcache="$CFCACHE" wbawp="$WBAWP" aterm="$ATERM" psterm="$PSTERM" conjbeams="$CONJBEAMS" muellertype="$MUELLERTYPE" dpa="$DPA" field="$FIELD" spw="$SPW" buffersize="$BUFFERSIZE" oversampling="$OVERSAMPLING" mode="$dryrun" cflist="$CFLIST"
#$coyote help=noprompt vis="$mset" telescope="EVLA" imsize="$IMSIZE" cell="$CELL" stokes="$STOKES" reffreq="$REFFREQ" phasecenter="$PHASECENTER" wplanes="$WPLANES" cfcache="$CFCACHE" wbawp="$WBAWP" aterm="$ATERM" psterm="$PSTERM" conjbeams="$CONJBEAMS" muellertype="$MUELLERTYPE" dpa="$DPA" field="$FIELD" spw="$SPW" buffersize="$BUFFERSIZE" oversampling="$OVERSAMPLING" mode="fillcf" cflist="CFS*"



# Function to run roadrunner with different modes
run_roadrunner() {
    local mode=$1
    local suffix=$2
    local phasecenter=${3:-$PHASECENTER}
    for spw in 0 1 2 3 4; do
        $roadrunner help=noprompt vis="$mset" imagename="dirty_spw${spw}.${suffix}" modelimagename="" datacolumn="data" sowimageext="sumwt" complexgrid="" imsize="$IMSIZE" cell="$CELL" stokes="$STOKES" reffreq="" phasecenter="$phasecenter" weighting="natural" rmode="none" robust="0" wprojplanes="$WPLANES" gridder="awphpg" cfcache="$CFCACHE" mode="$mode" wbawp="false" field="$FIELD" spw="$spw" uvrange="" pbcor="1" conjbeams="1" pblimit="$PBLIMIT" usepointing="0" pointingoffsetsigdev="300,300"
        if [ $? -ne 0 ]; then
            echo "Error: roadrunner failed for spw=${spw}, mode=${mode}" >&2
            exit 1
        fi
    done
}

run_roadrunner_model() {
    local mode=$1
    local suffix=$2
    local phasecenter=${3:-$PHASECENTER}
    for spw in 0 1 2 3 4; do
        $roadrunner help=noprompt vis="$mset" imagename="dirty_spw${spw}.${suffix}" modelimagename="dirty_spw${spw}.model" datacolumn="data" sowimageext="sumwt" complexgrid="" imsize="$IMSIZE" cell="$CELL" stokes="$STOKES" reffreq="" phasecenter="$phasecenter" weighting="natural" rmode="none" robust="0" wprojplanes="$WPLANES" gridder="awphpg" cfcache="$CFCACHE" mode="$mode" wbawp="false" field="$FIELD" spw="$spw" uvrange="" pbcor="1" conjbeams="1" pblimit="$PBLIMIT" usepointing="0" pointingoffsetsigdev="300,300"
    done
}

# Function to run dale
run_dale() {
    local suffix=$1
    for spw in 0 1 2 3 4; do
        $dale help=noprompt imagename="dirty_spw${spw}.${suffix}" imtype="$suffix" weightimage="dirty_spw${spw}.weight" sowimage="dirty_spw${spw}.sumwt" pblimit="0.0001" computepb="1"
    done
}

# Run roadrunner for residual, psf, and weight modes
run_roadrunner "residual" "residual" "$PHASECENTER"
run_roadrunner "psf" "psf" "$PHASECENTER"
run_roadrunner "weight" "weight" "$PHASECENTER"

# Normalize with dale
run_dale "psf"
run_dale "residual"


# Rename psf.pb files to pb
for spw in 0 1 2 3 4; do
    mv "dirty_spw${spw}.psf.pb" "dirty_spw${spw}.pb"
done

# Run taylor in mode flatsky
$taylor help=noprompt taylorImages="" cubeImage="dirty_spw0.residual,dirty_spw1.residual,dirty_spw2.residual,dirty_spw3.residual,dirty_spw4.residual" pbimage="dirty_spw0.pb,dirty_spw1.pb,dirty_spw2.pb,dirty_spw3.pb,dirty_spw4.pb" sumwtImage="" overwrite="1" mode="flatsky" reffreq="" nTerms="0" pblimit="0" imType="residual" avgpbname="" minfreqpbname="" avgpbmode="mean"

# Run taylor in mode computeavgpb to compute the average pb mode is mean for now. Have not tried the median yet.
$taylor help=noprompt taylorImages="" cubeImage="dirty_spw0.residual,dirty_spw1.residual,dirty_spw2.residual,dirty_spw3.residual,dirty_spw4.residual" pbimage="dirty_spw0.pb,dirty_spw1.pb,dirty_spw2.pb,dirty_spw3.pb,dirty_spw4.pb" sumwtImage="" overwrite="1" mode="computeavgpb" reffreq="" nTerms="0" pblimit="0" imType="residual" avgpbname="dirty_spw0.avgpb" minfreqpbname="dirty_spw0.minfreq" avgpbmode="mean"

#Run taylor in mode multiplyPB. Multiply the average pb with the residual image for all the spws
for spw in 0 1 2 3 4; do
    $taylor help=noprompt taylorImages="" cubeImage="dirty_spw${spw}.residual" pbimage="dirty_spw0.avgpb" sumwtImage="" overwrite="1" mode="multiplyPB" reffreq="" nTerms="0" pblimit="0" imType="residual" avgpbname="" minfreqpbname="" avgpbmode="mean"
done

#Run taylor in mode cube2taylor. This will create the taylor images for all the spws at one shot.
#So the inputs are all the residual images and the pb image which should be avgpb and sumwt images
# Repeat for imType=psf and imtype=PB
$taylor help=noprompt cubeImage="dirty_spw0.residual,dirty_spw1.residual,dirty_spw2.residual,dirty_spw3.residual,dirty_spw4.residual" taylorImages="initial_mtmfs.residual" pbimage="dirty_spw0.avgpb" sumwtImage="dirty_spw0.sumwt, dirty_spw1.sumwt,dirty_spw2.sumwt,dirty_spw3.sumwt,dirty_spw4.sumwt" overwrite="1" mode="cube2taylor" reffreq="1.4e9" nTerms="2" pblimit="0" imType="residual" avgpbname="" minfreqpbname="" avgpbmode="mean"
$taylor help=noprompt cubeImage="dirty_spw0.psf,dirty_spw1.psf,dirty_spw2.psf,dirty_spw3.psf,dirty_spw4.psf" taylorImages="initial_mtmfs.psf" pbimage="dirty_spw0.avgpb" sumwtImage="dirty_spw0.sumwt, dirty_spw1.sumwt,dirty_spw2.sumwt,dirty_spw3.sumwt,dirty_spw4.sumwt" overwrite="1" mode="cube2taylor" reffreq="1.4e9" nTerms="2" pblimit="0" imType="psf" avgpbname="" minfreqpbname="" avgpbmode="mean"
$taylor help=noprompt cubeImage="dirty_spw0.pb,dirty_spw1.pb,dirty_spw2.pb,dirty_spw3.pb,dirty_spw4.pb" taylorImages="initial_mtmfs.pb" pbimage="dirty_spw0.avgpb" sumwtImage="dirty_spw0.sumwt, dirty_spw1.sumwt,dirty_spw2.sumwt,dirty_spw3.sumwt,dirty_spw4.sumwt" overwrite="1" mode="cube2taylor" reffreq="1.4e9" nTerms="2" pblimit="0" imType="pb" avgpbname="" minfreqpbname="" avgpbmode="mean"

# Run hummbee to deconvolve.
$hummbee help=noprompt imagename="initial_mtmfs" modelimagename="" deconvolver="mtmfs" scales="" largestscale="-1" fusedthreshold="0" nterms="$NTERMS" gain="$GAIN" nsigma="0" threshold="$THRESHOLD" cycleniter="$CYCLENITER" cyclefactor="$CYCLEFACTOR" mask="source.mask" specmode="$SPECMODE" pbcor="$PBCOR" mode="deconvolve"

# Create empty copies of the .model by copying the .residual
for spw in 0 1 2 3 4; do
    cp -r "dirty_spw${spw}.residual" "dirty_spw${spw}.model"
done

# Convert taylor to cube
$taylor help=noprompt cubeImage="dirty_spw0.model,dirty_spw1.model,dirty_spw2.model,dirty_spw3.model,dirty_spw4.model" taylorImages="initial_mtmfs.model.tt0,initial_mtmfs.model.tt1" pbimage="dirty_spw0.avgpb" sumwtImage="dirty_spw0.sumwt, dirty_spw1.sumwt,dirty_spw2.sumwt,dirty_spw3.sumwt,dirty_spw4.sumwt" overwrite="1" mode="taylor2cube" reffreq="1.4e9" nTerms="2" pblimit="0" imType="residual" avgpbname="" minfreqpbname="" avgpbmode="mean"

# Divide the model by avgpb for all spws using mode=flatsky
#$taylor help=noprompt taylorImages="" cubeImage="dirty_spw0.model,dirty_spw1.model,dirty_spw2.model,dirty_spw3.model,dirty_spw4.model" pbimage="dirty_spw0.avgpb,dirty_spw0.avgpb,dirty_spw0.avgpb,dirty_spw0.avgpb,dirty_spw0.avgpb" sumwtImage="" overwrite="1" mode="flatsky" reffreq="" nTerms="0" pblimit="0" imType="residual" avgpbname="" minfreqpbname="" avgpbmode="mean"


# # dale to make model
# for spw in 0 1 2 3 4; do
#     $dale help=noprompt imagename="dirty_spw${spw}.model" imtype="model" weightimage="dirty_spw${spw}.weight" sowimage="dirty_spw${spw}.sumwt" pblimit="0.0001" computepb="1"
# done
repetitive_actions() {
run_roadrunner_model "residual" "residual" "$PHASECENTER"
run_dale "residual"

#flatsky
$taylor help=noprompt taylorImages="" cubeImage="dirty_spw0.residual,dirty_spw1.residual,dirty_spw2.residual,dirty_spw3.residual,dirty_spw4.residual" pbimage="dirty_spw0.pb,dirty_spw1.pb,dirty_spw2.pb,dirty_spw3.pb,dirty_spw4.pb" sumwtImage="" overwrite="1" mode="flatsky" reffreq="" nTerms="0" pblimit="0" imType="residual" avgpbname="" minfreqpbname="" avgpbmode="mean"

#Run taylor in mode multiplyPB. Multiply the average pb with the residual image for all the spws
for spw in 0 1 2 3 4; do
    $taylor help=noprompt taylorImages="" cubeImage="dirty_spw${spw}.residual" pbimage="dirty_spw0.avgpb" sumwtImage="" overwrite="1" mode="multiplyPB" reffreq="" nTerms="0" pblimit="0" imType="residual" avgpbname="" minfreqpbname="" avgpbmode="mean"
done

#cubetotaylor
$taylor help=noprompt cubeImage="dirty_spw0.residual,dirty_spw1.residual,dirty_spw2.residual,dirty_spw3.residual,dirty_spw4.residual" taylorImages="initial_mtmfs.residual" pbimage="dirty_spw0.avgpb" sumwtImage="dirty_spw0.sumwt, dirty_spw1.sumwt,dirty_spw2.sumwt,dirty_spw3.sumwt,dirty_spw4.sumwt" overwrite="1" mode="cube2taylor" reffreq="1.4e9" nTerms="2" pblimit="0" imType="residual" avgpbname="" minfreqpbname="" avgpbmode="mean"

$hummbee help=noprompt imagename="initial_mtmfs" modelimagename="" deconvolver="mtmfs" scales="" largestscale="-1" fusedthreshold="0" nterms="$NTERMS" gain="$GAIN" nsigma="0" threshold="$THRESHOLD" cycleniter="$CYCLENITER" cyclefactor="$CYCLEFACTOR" mask="source.mask" specmode="$SPECMODE" pbcor="$PBCOR" mode="deconvolve"

$taylor help=noprompt cubeImage="dirty_spw0.model,dirty_spw1.model,dirty_spw2.model,dirty_spw3.model,dirty_spw4.model" taylorImages="initial_mtmfs.model.tt0,initial_mtmfs.model.tt1" pbimage="dirty_spw0.avgpb" sumwtImage="dirty_spw0.sumwt, dirty_spw1.sumwt,dirty_spw2.sumwt,dirty_spw3.sumwt,dirty_spw4.sumwt" overwrite="1" mode="taylor2cube" reffreq="1.4e9" nTerms="2" pblimit="0" imType="residual" avgpbname="" minfreqpbname="" avgpbmode="mean"
}


# Run the repetitive actions multiple times
num_iterations=7 # The sequence is repeated 7 times in the original script
for i in $(seq 1 "$num_iterations"); do
    echo "Iteration: $i"
    repetitive_actions
done
