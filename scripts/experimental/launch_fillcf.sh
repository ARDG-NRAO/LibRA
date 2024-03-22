#!/bin/bash

# Default values
nprocs=40
chdir="/lustre/pjaganna/evla/cfcache/"

# Parse command-line options
while getopts ":n:w:c:E:C:h" opt; do
  case ${opt} in
    n )
      nprocs=$OPTARG
      ;;
    w )
      chdir=$OPTARG
      ;;
    c )
      cfcache_dir=$OPTARG
      ;;
    E )
      coyote_app=$OPTARG
      ;;
    C )
      CASAPATH=$OPTARG
      ;;
    h )
      echo "Usage: $0 [-n nprocs] [-w chdir] [-c cfcache_dir] [-E exodus_bundle] [-C CASAPATH] [-h]"
      exit 0
      ;;
    \? )
      echo "Invalid option: $OPTARG" 1>&2
      exit 1
      ;;
    : )
      echo "Invalid option: $OPTARG requires an argument" 1>&2
      exit 1
      ;;
  esac
done
shift $((OPTIND -1))

export nprocs
export start=0
export end=$((nprocs - 1))
export cfcache_dir
export coyote_app
export CASAPATH

# Check if paths exist
if [ ! -d "$cfcache_dir" ]; then
  echo "cfcache_dir does not exist"
  exit 1
fi

# if [ ! -d "$exodus_bundle" ]; then
#   echo "exodus_bundle does not exist"
#   exit 1
# fi

if [ ! -d "$CASAPATH" ]; then
  echo "CASAPATH does not exist"
  exit 1
fi

# Write out submit_sbatch.sh
cat <<EOF > submit_sbatch.sh
#!/bin/bash
#SBATCH --export=ALL
#SBATCH --array="$start-$end"
#SBATCH --chdir="$chdir"
#SBATCH --time="1-0:0:0"
#SBATCH --mem="2G"
#SBATCH --nodes="1"
#SBATCH --ntasks-per-node="1"
#SBATCH --output="logs/slurm_%A_%a.out"
#SBATCH --error="logs/slurm_%A_%a.err"
#SBATCH --job-name="fillcf"
#SBATCH --account="pjaganna"
#SBATCH --mail-user="pjaganna@nrao.edu"
#SBATCH --mail-type="FAIL"

python3 slurm_fillcf.py --cfcache_dir ${cfcache_dir} --nprocs ${nprocs} --coyote_app ${coyote_app}

EOF

# Submit the job
sbatch submit_sbatch.sh
