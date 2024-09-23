
# A script to detect unique SPW indeces in the given CFC, and make
# partitioned CFCs per SPW index with only the CFs for one SPW.
#
CFCName=$1

CFNAMING="CFS_T_B_CF_SPW_W_POL.im"
WTCFNAMING="WT"$CFNAMING

copyCF(){
    ln -s $1
}

# Collect CF names
cfs=`ls -d $CFCName/* | grep CFS*`

# Collect names of misc info files/dirs
misc=`ls -d $CFCName/* | grep -v CFS*`
#echo $misc;

#
# Make a list of unique SPW indices found
#
SWPL=();
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
    PartName="SPW"$s".cfc";
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
