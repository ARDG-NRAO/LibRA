#!/bin/bash

# Default values
nprocs=40
chdir=""

# Parse command-line options
while getopts ":n:w:c:E:C:p:h" opt; do
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
    p )
      PYTHON_FILE_PATH=$OPTARG
      ;;
    h )
      echo "Usage: $0 
             [-n nprocs]
             [-w chdir]
             [-c cfcache_dir]
             [-E full path to exodus binary]
             [-C the directory that contains the casadata in a directory named data, i.e the CASAPATH ]
             [-p directory that contains the python file that launches the slurm process]
             [-h help]"
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

# check is chdir exists else make it before creating logs
if [ ! -d "$chdir" ]; then
  echo "chdir does not exist"
  mkdir -p $chdir
fi

# Check is a directory called logs exists inside chdir
if [ ! -d "$chdir/logs" ]; then
  echo "logs directory does not exist"
  mkdir -p $chdir/logs
fi

# Check the exodus bundle file is not present then exit
if [ ! -f "$coyote_app" ]; then
  echo "coyote_app does not exist"
  exit 1
fi

if [ ! -d "$CASAPATH" ]; then
  echo "CASAPATH does not exist"
  exit 1
fi

 if [ ! -d "$PYTHON_FILE_PATH" ]; then
   echo "PYTHON_FILE_PATH does not exist"
   echo "Setting it to workig directory"
    PYTHON_FILE_PATH=$chdir 
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

python3 ${PYTHON_FILE_PATH}/slurm_fillcf.py --cfcache_dir ${cfcache_dir} --nprocs ${nprocs} --coyote_app ${coyote_app}

EOF

# Submit the job

sbatch submit_sbatch.sh