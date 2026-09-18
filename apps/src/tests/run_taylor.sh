#!/bin/bash
# This is a script to test the taylor app and is similar to runlooptest.sh.
# Only we are testing individual steps of the algorithm.

# --- Configuration Variables ---
# Default values for common parameters.
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
MODE="dryrun" # Note: This default might be overridden in specific calls
CFLIST=""
PBLIMIT="0.001"
NTERMS="2"
GAIN="0.1"
THRESHOLD="0.00001"
CYCLENITER="100"
CYCLEFACTOR="1"
SPECMODE="mfs"
PBCOR="0"
MSET="sim_VLA.ms"

# --- Image Naming Configuration ---
# Define base names for your images.
# These will be used to construct the full image paths.
DIRTY_IMAGE_BASE="dirty_spw" # Example: dirty_spw0, dirty_spw1
MTMFS_IMAGE_BASE="initial_mtmfs" # Example: initial_mtmfs.residual, initial_mtmfs.model.tt0

# Path to binaries. Using a clear variable for the binary path.
BIN_PATH="/home/nemo/Software/libra_taylor/install/bin"
ROADRUNNER="$BIN_PATH/roadrunner"
COYOTE="$BIN_PATH/coyote"
TAYLOR="$BIN_PATH/taylor"
DALE="$BIN_PATH/dale"
HUMMBEE="$BIN_PATH/hummbee"

# Source mask definition. Using a here-document for clarity.
SOURCE_MASK_FILE="source.mask"
cat <<EOL > "$SOURCE_MASK_FILE"
#CRTFv0
global coord=J2000
circle[[19:59:28.5, +40.40.00], 0.5pix]
#circle[[19:59:28.5, +40.45.00], 0.5pix]
#circle[[19:58:05.5, +40.55.00], 0.5pix]
#circle[[19:59:28.5, +40.35.00], 0.5pix]
EOL

# --- Helper Functions ---

# Function to get a comma-separated list of SPW-specific images for a given suffix.
# Parameters:
#   $1: suffix (e.g., "residual", "psf", "pb", "model")
# Returns: a string like "dirty_spw0.residual,dirty_spw1.residual,..."
get_spw_image_list() {
    local suffix="$1"
    local image_list=""
    for spw in 0 1 2 3 4; do
        if [[ -n "$image_list" ]]; then
            image_list+=","
        fi
        image_list+="${DIRTY_IMAGE_BASE}${spw}.${suffix}"
    done
    echo "$image_list"
}

# Function to get a comma-separated list of MTMFS model Taylor images.
# Parameters:
#   $1: image_type (e.g., "tt0", "tt1")
# Returns: a string like "initial_mtmfs.model.tt0,initial_mtmfs.model.tt1"
get_mtmfs_model_taylor_list() {
    local taylor_terms=""
    for term in $(seq 0 $((NTERMS - 1))); do
        if [[ -n "$taylor_terms" ]]; then
            taylor_terms+=","
        fi
        taylor_terms+="${MTMFS_IMAGE_BASE}.model.tt${term}"
    done
    echo "$taylor_terms"
}

# Function to run roadrunner.
# Parameters:
#   $1: mode (e.g., "residual", "psf", "weight")
#   $2: suffix (e.g., "residual", "psf")
#   $3: phasecenter (optional, defaults to global PHASECENTER)
#   $4: model_image_name_base (optional, if provided, constructs dirty_spwX.model for modelimagename)
run_roadrunner() {
    local mode="$1"
    local suffix="$2"
    local current_phasecenter="${3:-$PHASECENTER}"
    local model_image_name_base="${4:-}" # Pass "dirty_spw" if a model is needed

    echo "Running roadrunner in mode: ${mode}, suffix: ${suffix}"
    for spw in 0 1 2 3 4; do
        local imagename="${DIRTY_IMAGE_BASE}${spw}.${suffix}"
        local full_model_imagename=""
        if [[ -n "$model_image_name_base" ]]; then
            full_model_imagename="${model_image_name_base}${spw}.divmodel"
        fi

        "$ROADRUNNER" help=noprompt \
            vis="$MSET" \
            imagename="$imagename" \
            modelimagename="$full_model_imagename" \
            datacolumn="data" \
            sowimageext="sumwt" \
            complexgrid="" \
            imsize="$IMSIZE" \
            cell="$CELL" \
            stokes="$STOKES" \
            reffreq="" \
            phasecenter="$current_phasecenter" \
            weighting="natural" \
            rmode="none" \
            robust="0" \
            wprojplanes="$WPLANES" \
            gridder="awphpg" \
            cfcache="$CFCACHE" \
            mode="$mode" \
            wbawp="false" \
            field="$FIELD" \
            spw="$spw" \
            uvrange="" \
            pbcor="1" \
            conjbeams="1" \
            pblimit="$PBLIMIT" \
            usepointing="0" \
            pointingoffsetsigdev="300,300"

        if [ $? -ne 0 ]; then
            echo "Error: roadrunner failed for spw=${spw}, mode=${mode}" >&2
            exit 1
        fi
    done
}

# Function to run dale.
# Parameters:
#   $1: imtype (e.g., "psf", "residual", "model")
#   $2: image suffix (optional, defaults to $1). Use "" to pass just the base name.
run_dale() {
    local imtype="$1"
    local suffix="${2-$1}"
    echo "Running dale for imtype: ${imtype}, suffix: ${suffix}"
    for spw in 0 1 2 3 4; do
        local imgname
        if [[ -n "$suffix" ]]; then
            imgname="${DIRTY_IMAGE_BASE}${spw}.${suffix}"
        else
            imgname="${DIRTY_IMAGE_BASE}${spw}"
        fi
        "$DALE" help=noprompt \
            imagename="$imgname" \
            imtype="$imtype" \
            pblimit="$PBLIMIT" \
            computepb="1"
    done
}

# Function to run taylor in various modes.
# Parameters:
#   $1: mode (e.g., "flatsky", "computeavgpb", "applyPB", "cube2taylor", "taylor2cube")
#   ... other parameters specific to taylor, passed as an array or individual arguments.
run_taylor() {
    local mode="$1"
    shift # Remove the mode from the arguments
    local taylor_args=("$@") # Capture remaining arguments

    echo "Running taylor in mode: ${mode}"

    "$TAYLOR" help=noprompt mode="$mode" "${taylor_args[@]}"
}

# Function to run hummbee.
# Parameters:
#   $1: imagename_base (e.g., "initial_mtmfs")
#   $2: deconvolver
#   $3: mask file
run_hummbee() {
    local imagename_base="$1"
    local deconvolver="$2"
    local mask="$3"

    echo "Running hummbee for imagename: ${imagename_base}, deconvolver: ${deconvolver}"

    "$HUMMBEE" help=noprompt \
        imagename="${imagename_base}" \
        modelimagename="" \
        deconvolver="$deconvolver" \
        scales="" \
        largestscale="-1" \
        fusedthreshold="0" \
        nterms="$NTERMS" \
        gain="$GAIN" \
        nsigma="0" \
        threshold="$THRESHOLD" \
        cycleniter="$CYCLENITER" \
        cyclefactor="$CYCLEFACTOR" \
        mask="$mask" \
        specmode="$SPECMODE" \
        pbcor="$PBCOR" \
        mode="deconvolve"
}

# --- Main Workflow Steps ---

# Step 1: Create initial residual, PSF, and weight images
create_initial_images() {
    echo "--- Creating Initial Images (Residual, PSF, Weight) ---"
    run_roadrunner "residual" "residual" "$PHASECENTER" "" # No model for initial residual
    run_roadrunner "psf" "psf" "$PHASECENTER"
    run_roadrunner "weight" "weight" "$PHASECENTER"

    run_dale "psf"
    run_dale "residual"

    # Rename psf.pb files to pb
    for spw in 0 1 2 3 4; do
        mv "${DIRTY_IMAGE_BASE}${spw}.psf.pb" "${DIRTY_IMAGE_BASE}${spw}.pb"
    done
}

# Step 2: Taylor operations (flatsky, computeavgpb, applyPB, cube2taylor)
perform_taylor_initial_steps() {
    echo "--- Performing Initial Taylor Steps ---"

    local residual_images=$(get_spw_image_list "residual")
    local pb_images=$(get_spw_image_list "pb")
    local psf_images=$(get_spw_image_list "psf")
    local sumwt_images=$(get_spw_image_list "sumwt")
    local avgpb_image="${DIRTY_IMAGE_BASE}0.avgpb"
    local minfreq_image="${DIRTY_IMAGE_BASE}0.minfreq"

    # Run taylor in mode flatsky
    run_taylor "flatsky" \
        taylorImages="" \
        cubeImage="$residual_images" \
        pbimage="$pb_images" \
        sumwtImage="" \
        overwrite="1" \
        reffreq="" \
        nTerms="0" \
        pblimit="0" \
        imtype="residual" \
        avgpbname="" \
        minfreqpbname="" \
        avgpbmode="mean"

    # Run taylor in mode computeavgpb to compute the average pb mode is mean for now.
    run_taylor "computeavgpb" \
        taylorImages="" \
        cubeImage="$residual_images" \
        pbimage="$pb_images" \
        sumwtImage="" \
        overwrite="1" \
        reffreq="" \
        nTerms="0" \
        pblimit="0" \
        imtype="residual" \
        avgpbname="$avgpb_image" \
        minfreqpbname="$minfreq_image" \
        avgpbmode="mean"

    # Run taylor in mode applyPB (multiply). Multiply the average pb with the residual image for all the spws
    for spw in 0 1 2 3 4; do
        run_taylor "applyPB" \
            taylorImages="" \
            cubeImage="${DIRTY_IMAGE_BASE}${spw}.residual" \
            pbimage="$avgpb_image" \
            sumwtImage="" \
            overwrite="1" \
            reffreq="" \
            nTerms="0" \
            pblimit="$PBLIMIT" \
            imtype="multiply" \
            avgpbname="" \
            minfreqpbname="" \
            avgpbmode="mean"
    done

    # Run taylor in mode cube2taylor for residual, psf, and pb images.
    run_taylor "cube2taylor" \
        cubeImage="$residual_images" \
        taylorImages="${MTMFS_IMAGE_BASE}.residual" \
        pbimage="$avgpb_image" \
        sumwtImage="$sumwt_images" \
        overwrite="1" \
        reffreq="1.4e9" \
        nTerms="2" \
        pblimit="0" \
        imtype="residual" \
        avgpbname="" \
        minfreqpbname="" \
        avgpbmode="mean"

    run_taylor "cube2taylor" \
        cubeImage="$psf_images" \
        taylorImages="${MTMFS_IMAGE_BASE}.psf" \
        pbimage="$avgpb_image" \
        sumwtImage="$sumwt_images" \
        overwrite="1" \
        reffreq="1.4e9" \
        nTerms="2" \
        pblimit="0" \
        imtype="psf" \
        avgpbname="" \
        minfreqpbname="" \
        avgpbmode="mean"

    run_taylor "cube2taylor" \
        cubeImage="$pb_images" \
        taylorImages="${MTMFS_IMAGE_BASE}.pb" \
        pbimage="$avgpb_image" \
        sumwtImage="$sumwt_images" \
        overwrite="1" \
        reffreq="1.4e9" \
        nTerms="2" \
        pblimit="0" \
        imtype="pb" \
        avgpbname="" \
        minfreqpbname="" \
        avgpbmode="mean"
}

# Step 3: Deconvolve and convert taylor to cube
deconvolve_and_convert() {
    echo "--- Deconvolving and Converting Taylor to Cube ---"

    run_hummbee "$MTMFS_IMAGE_BASE" "mtmfs" "$SOURCE_MASK_FILE"

    # Create empty copies of the .model by copying the .residual
    for spw in 0 1 2 3 4; do
        cp -r "${DIRTY_IMAGE_BASE}${spw}.residual" "${DIRTY_IMAGE_BASE}${spw}.model"
    done

    # Convert taylor to cube
    local model_images=$(get_spw_image_list "model")
    local taylor_model_images=$(get_mtmfs_model_taylor_list)
    local sumwt_images=$(get_spw_image_list "sumwt")
    local avgpb_image="${DIRTY_IMAGE_BASE}0.avgpb"

    run_taylor "taylor2cube" \
        cubeImage="$model_images" \
        taylorImages="$taylor_model_images" \
        pbimage="$avgpb_image" \
        sumwtImage="$sumwt_images" \
        overwrite="1" \
        reffreq="1.4e9" \
        nTerms="2" \
        pblimit="$PBLIMIT" \
        imtype="model" \
        avgpbname="" \
        minfreqpbname="" \
        avgpbmode="mean"

    # Apply per-SPW PB to model cubes (avgPB was removed inside taylor2cube).
    for spw in 0 1 2 3 4; do
        run_taylor "applyPB" \
            taylorImages="" \
            cubeImage="${DIRTY_IMAGE_BASE}${spw}.model" \
            pbimage="${DIRTY_IMAGE_BASE}${spw}.pb" \
            sumwtImage="" \
            overwrite="1" \
            reffreq="" \
            nTerms="0" \
            pblimit="$PBLIMIT" \
            imtype="multiply" \
            avgpbname="" \
            minfreqpbname="" \
            avgpbmode="mean"

        # Strip SubType from table.info so casacore treats this as a plain image,
        # not a residual or other typed image, before dale normalization.
        sed -i 's/SubType.*=.*//g' "${DIRTY_IMAGE_BASE}${spw}.model/table.info"
    done

    # Dale normalization once over all SPWs to produce divmodel for roadrunner.
    run_dale "model" ""
}

# Step 4: Repetitive clean cycle (residual calculation, taylor ops, deconvolution, taylor2cube)
perform_clean_cycle() {
    echo "--- Starting Clean Cycle Iteration ---"

    local residual_images=$(get_spw_image_list "residual")
    local pb_images=$(get_spw_image_list "pb")
    local psf_images=$(get_spw_image_list "psf") # Though not strictly needed in the cycle, good to have it defined.
    local sumwt_images=$(get_spw_image_list "sumwt")
    local avgpb_image="${DIRTY_IMAGE_BASE}0.avgpb"
    local model_images=$(get_spw_image_list "model")
    local taylor_model_images=$(get_mtmfs_model_taylor_list)

    # Note: roadrunner in 'residual' mode now takes 'dirty_spwX.model' as modelimagename
    run_roadrunner "residual" "residual" "$PHASECENTER" "$DIRTY_IMAGE_BASE"
    run_dale "residual"

    # flatsky
    run_taylor "flatsky" \
        taylorImages="" \
        cubeImage="$residual_images" \
        pbimage="$pb_images" \
        sumwtImage="" \
        overwrite="1" \
        reffreq="" \
        nTerms="0" \
        pblimit="0" \
        imtype="residual" \
        avgpbname="" \
        minfreqpbname="" \
        avgpbmode="mean"

    # Run taylor in mode applyPB (multiply).
    for spw in 0 1 2 3 4; do
        run_taylor "applyPB" \
            taylorImages="" \
            cubeImage="${DIRTY_IMAGE_BASE}${spw}.residual" \
            pbimage="$avgpb_image" \
            sumwtImage="" \
            overwrite="1" \
            reffreq="" \
            nTerms="0" \
            pblimit="$PBLIMIT" \
            imtype="multiply" \
            avgpbname="" \
            minfreqpbname="" \
            avgpbmode="mean"
    done

    # cube2taylor
    run_taylor "cube2taylor" \
        cubeImage="$residual_images" \
        taylorImages="${MTMFS_IMAGE_BASE}.residual" \
        pbimage="$avgpb_image" \
        sumwtImage="$sumwt_images" \
        overwrite="1" \
        reffreq="1.4e9" \
        nTerms="2" \
        pblimit="0" \
        imtype="residual" \
        avgpbname="" \
        minfreqpbname="" \
        avgpbmode="mean"

    run_hummbee "$MTMFS_IMAGE_BASE" "mtmfs" "$SOURCE_MASK_FILE"

    run_taylor "taylor2cube" \
        cubeImage="$model_images" \
        taylorImages="$taylor_model_images" \
        pbimage="$avgpb_image" \
        sumwtImage="$sumwt_images" \
        overwrite="1" \
        reffreq="1.4e9" \
        nTerms="2" \
        pblimit="$PBLIMIT" \
        imtype="model" \
        avgpbname="" \
        minfreqpbname="" \
        avgpbmode="mean"

    # Apply per-SPW PB to model cubes (avgPB was removed inside taylor2cube).
    for spw in 0 1 2 3 4; do
        run_taylor "applyPB" \
            taylorImages="" \
            cubeImage="${DIRTY_IMAGE_BASE}${spw}.model" \
            pbimage="${DIRTY_IMAGE_BASE}${spw}.pb" \
            sumwtImage="" \
            overwrite="1" \
            reffreq="" \
            nTerms="0" \
            pblimit="$PBLIMIT" \
            imtype="multiply" \
            avgpbname="" \
            minfreqpbname="" \
            avgpbmode="mean"

        # Strip SubType from table.info so casacore treats this as a plain image,
        # not a residual or other typed image, before dale normalization.
        sed -i 's/SubType.*=.*//g' "${DIRTY_IMAGE_BASE}${spw}.model/table.info"
    done

    # Dale normalization once over all SPWs to produce divmodel for roadrunner.
    run_dale "model" ""
}

# Function to run coyote.
# Parameters:
#   $1: mode (e.g., "dryrun", "fillcf")
run_coyote() {
    local mode="$1"
    echo "Running coyote in mode: ${mode}"
    "$COYOTE" help=noprompt \
        vis="$MSET" \
        telescope="EVLA" \
        imsize="$IMSIZE" \
        cell="$CELL" \
        stokes="$STOKES" \
        reffreq="$REFFREQ" \
        phasecenter="$PHASECENTER" \
        wplanes="$WPLANES" \
        cfcache="$CFCACHE" \
        wbawp="$WBAWP" \
        aterm="$ATERM" \
        psterm="$PSTERM" \
        conjbeams="$CONJBEAMS" \
        muellertype="$MUELLERTYPE" \
        dpa="$DPA" \
        field="$FIELD" \
        spw="$SPW" \
        buffersize="$BUFFERSIZE" \
        oversampling="$OVERSAMPLING" \
        mode="$mode" \
        cflist="CFS*"
}

# --- Main Script Execution ---

main() {
    echo "--- Creating Convolution Function Cache with Coyote ---"
    run_coyote "dryrun"
    run_coyote "fillcf"

    create_initial_images
    perform_taylor_initial_steps
    deconvolve_and_convert

    num_iterations=7
    for i in $(seq 1 "$num_iterations"); do
        echo "--- Iteration: $i of $num_iterations ---"
        perform_clean_cycle
	done
    echo "Script finished successfully!"
}
# Call the main function
main "$@"
