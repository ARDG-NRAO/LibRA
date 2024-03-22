#!/bin/bash
#SBATCH --export=ALL                          # Export all environment variables to job
#SBATCH --array=0-400
#SBATCH --chdir=/lustre/pjaganna/evla/cfcache/ # Working directory
#SBATCH --time=1-0:0:0                       # Request 30 days after which the job will be killed
#SBATCH --mem=2G                             # Memory per node
#SBATCH --nodes=1                             # Request exactly 1 node
#SBATCH --ntasks-per-node=1                   # Request 1 task per node
#SBATCH -o logs/slurm_%A_%a.out
#SBATCH -e logs/slurm_%A_%a.err


# Help function
function print_help {
  echo "Usage: $0 [cfcache_dir] [exodus_bundle] [CASAPATH] [cflist_file]"
  echo
  echo "cfcache_dir:    Path to the cfcache directory"
  echo "exodus_bundle:  Path to the exodus bundle directory"
  echo "CASAPATH:       Path to the CASA directory"
  echo "cflist_file:    Path to the cflist file"
  
  exit 1
}

# Check if -h or --help was passed
case "$1" in
  -h|--help)
    print_help
    ;;
esac

# Specify the directory to search. I am also exporting so you can set these as env and it would still work.                                                                                   
export cfcache_dir=$1
export exodus_bundle=$2
export CASAPATH=$3
#export cflist_file=$4

#Run this outside to get a filelist to send in
#for dir in ${cfcache_dir}/CFS*; do
#  basename $dir
#done > $cflist_file

export numprocs=400


# Check if paths exist
if [ ! -d "$cfcache_dir" ]; then
  echo "cfcache_dir does not exist"
  exit 1
fi

if [ ! -d "$exodus_bundle" ]; then
  echo "exodus_bundle does not exist"
  exit 1
fi

if [ ! -d "$CASAPATH" ]; then
  echo "CASAPATH does not exist"
  exit 1
fi

#export total_lines=`wc -l < $cflist_file`  # Get the total number of lines in the file



srun python3 slurm_fillcf.py ${cfcache_dir} ${numprocs} $SLURM_ARRAY_TASK_ID

#
#echo $numprocs
#export n=$(($(expr $total_lines / $numprocs)+1))

#echo "Total Lines : "$total_lines
#echo "Launching ${n} cfs per process over ${numprocs} processes"
#export start=$(((((SLURM_ARRAY_TASK_ID -1)*n))+1))
#export end=$((start + n ))
#echo "Beginning with line number: ${start} and ending with ${end}"
#export lines=`sed -n "${start},${end}p;${end}q" "$cflist_file" | tr '\n' ','`
#echo "The list of convolution functions being filled by ${SLURM_ARRAY_TASK_ID} is ${lines}"
#echo ${exodus_bundle}/bin/coyote help=noprompt cfcache=${cfcache_dir} mode=fillcf cflist="${lines}"
#${exodus_bundle}/bin/coyote help=noprompt cfcache=${cfcache_dir} mode=fillcf cflist="${lines}"

