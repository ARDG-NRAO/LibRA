#!/bin/bash

# Default values
nprocs=40
chdir=""

# Log prefix colors
LOG_PREFIX="\e[32m[LOG]\e[0m"
WARN_PREFIX="\e[33m[WARN]\e[0m"
ERROR_PREFIX="\e[31m[ERROR]\e[0m"

# Parse command-line options
while getopts ":n:w:c:E:C:u:e:h" opt; do
  case ${opt} in
  n)
    nprocs=$OPTARG
    ;;
  w)
    chdir=$OPTARG
    ;;
  c)
    cfcache_dir=$OPTARG
    ;;
  E)
    coyote_app=$OPTARG
    ;;
  C)
    CASAPATH=$OPTARG
    ;;
  u)
    username=$OPTARG
    ;;
  e)
    email=$OPTARG
    ;;
  h)
    echo -e "Usage: $0 
             [-n nprocs]
             [-w chdir]
             [-c cfcache_dir]
             [-E full path to exodus binary]
             [-C the directory that contains the casadata in a directory named data,
              i.e the CASAPATH. If not present the script will check for 
              /home/casa/data/trunk and create a symlink to it]
             [-u username]
             [-e email]
             [-h help]"
    exit 0
    ;;
  \?)
    echo -e "${WARN_PREFIX} Invalid option: $OPTARG" 1>&2
    exit 1
    ;;
  :)
    echo -e "${WARN_PREFIX} Invalid option: $OPTARG requires an argument" 1>&2
    exit 1
    ;;
  esac
done
shift $((OPTIND - 1))

export nprocs
export start=0
export end=$((nprocs - 1))
export cfcache_dir
export coyote_app
export CASAPATH
export chdir
export username
export email

#check if username and email are set
if [ -z "$username" ] || [ -z "$email" ]; then
  echo -e "${WARN_PREFIX} Username and email are not set"
  # Get the username of the user
  username=$(whoami)
  echo -e "${LOG_PREFIX} Username is set to: $username"
  # Get the email of the user
  email=$(whoami)@nrao.edu
  echo -e "${LOG_PREFIX} Email is set to: $email"
  echo -e "${LOG_PREFIX} If this is incorrect. 
  Please provide the correct username and email using the -u and -e flags"
fi

# Function to check if a directory exists
check_directory() {
  local dir=$1
  local log_prefix=$2

  if [ ! -d "$dir" ]; then
    echo -e "${ERROR_PREFIX} $dir does not exist"
    exit 1
  else
    echo -e "${log_prefix} $dir is: $dir"
  fi
}

# Function to create a directory if it doesn't exist
create_directory() {
  local dir=$1
  local log_prefix=$2

  if [ ! -d "$dir" ]; then
    echo -e "${WARN_PREFIX} $dir does not exist, creating it"
    mkdir -p "$dir"
  else
    echo -e "${log_prefix} The directory $dir already exists"
  fi
}

# Function to create the logs directory
create_logs_directory() {
  local dir=$1
  local log_prefix=$2

  if [ ! -d "$dir/logs" ]; then
    echo -e "${WARN_PREFIX} logs directory does not exist inside $dir"
    echo -e "${log_prefix} Creating logs directory: $dir/logs"
    mkdir -p "$dir/logs"
  else
    echo -e "${log_prefix} logs directory exists and logs will be written to: $dir/logs"
  fi
}

# Function to check if CASAPATH is set and exists
check_casapath() {
  local casapath=$1
  local chdir=$2
  local log_prefix=$3

  if [ -z "$casapath" ]; then
    echo -e "${WARN_PREFIX} CASAPATH is not set"
    echo -e "${log_prefix} Checking for /home/casa/data/trunk"
    casa_data_dir="/home/casa/data/trunk"
    if [ -d "$casa_data_dir" ]; then
      echo -e "${log_prefix} Found casa data directory at $casa_data_dir"
      echo -e "${log_prefix} Creating symlink data inside $chdir to casa data directory"
      if [ -L "$chdir/data" ]; then
        echo -e "${log_prefix} Symlink 'data' already exists"
      else
        ln -s "$casa_data_dir" "$chdir/data"
        echo -e "${log_prefix} Symlink 'data' created"
      fi
    else
      echo -e "${ERROR_PREFIX} CASAPATH is not set & Casa data directory does not exist"
      exit 1
    fi
  else
    echo -e "${log_prefix} CASAPATH is set to: $casapath"
    if [ ! -d "$casapath" ]; then
      echo -e "${ERROR_PREFIX} CASAPATH directory does not exist"
      exit 1
    fi
  fi
}

# Call the functions
check_directory "$cfcache_dir" "$LOG_PREFIX"
create_directory "$chdir" "$LOG_PREFIX"
create_logs_directory "$chdir" "$LOG_PREFIX"

# Function to check if the exodus bundle file exists
check_coyote_app() {
  local app=$1
  local log_prefix=$2

  if [ -f "$app" ]; then
    echo -e "${log_prefix} ${app} is an executable file"
    echo "$app" # Return the original path
  elif [ -d "$app" ]; then
    local coyote_path="$app/bin/coyote"
    if [ -f "$coyote_path" ]; then
      echo -e "${log_prefix} ${app} is a directory"
      echo -e "${log_prefix} ${app}/bin/coyote exists"
      echo -e "${log_prefix} ${app} PATH is: $coyote_path"
      echo "$coyote_path" # Return the updated path
    else
      echo -e "${ERROR_PREFIX} ${app}/bin/coyote does not exist"
      exit 1
    fi
  else
    echo -e "${ERROR_PREFIX} ${app} does not exist"
    exit 1
  fi
}
check_coyote_app "$coyote_app" "$LOG_PREFIX"

check_casapath "$CASAPATH" "$chdir" "$LOG_PREFIX"

# Check if slurm_fillcf.py exists
PYTHON_FILE_PATH="$chdir/slurm_fillcf.py"
if [ ! -f $PYTHON_FILE_PATH ]; then
  echo -e "${LOG_PREFIX} ${chdir}/slurm_fillcf.py does not exist and will be created"
  create_python_file="True"
else
  create_python_file="True"
  echo -e "${LOG_PREFIX} ${chdir}/slurm_fillcf.py exists removing it"
  rm -f $PYTHON_FILE_PATH
fi

# Write out submit_sbatch.sh
# Check if submit_sbatch.sh exists, warn that it will be overwritten
if [ -f "$chdir/submit_sbatch.sh" ]; then
  echo -e "${WARN_PREFIX} ${chdir}/submit_sbatch.sh exists and will be overwritten"
fi

cat <<EOF >${chdir}/submit_sbatch.sh
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

python3 ${PYTHON_FILE_PATH} --cfcache_dir ${cfcache_dir} --nprocs ${nprocs} --coyote_app ${coyote_app}

EOF

if [ "$create_python_file" == "True" ]; then
  echo -e "${LOG_PREFIX} Creating python file"
  # Write out the python file
  cat <<-EOF >${PYTHON_FILE_PATH}
import argparse
import glob
import os
import sys
import subprocess

def check_path(path, path_name):
    if not os.path.exists(path):
        raise FileNotFoundError(f"The {path} does not exist. Please provide a valid path for {path_name}")

def worker(cfs, start, end, cfcache_dir, coyote_app):
    pid = os.getenv('SLURM_ARRAY_TASK_ID', 'default')
    with open(f'logs/{pid}.txt', 'w') as f:
        f.write(f"Starting index: {start} \n")
        f.write(f"Ending index: {end-1} \n")
        f.write(f"Number of cfs: {len(cfs[start:end])} \n")
        for cf in cfs:
            f.write(cf + '\n')
    f.close()
    command = [f'{coyote_app}', 'help=noprompt', 'mode=fillcf', f'cfcache={cfcache_dir}', f'cflist={", ".join(cfs)}']
    print("Running command:", " ".join(command))
    subprocess.run(command)

def distribute_tasks(n_processes, task_id, cfs, cfcache_dir, coyote_app):
    # Distribute tasks among processes
    print("Number of cfs:", len(cfs))
    base, remainder = divmod(len(cfs), n_processes)
    quantities = [base + 1 if i < remainder else base for i in range(n_processes)]
    # Determine start and end indices for this task
    start = sum(quantities[:task_id])
    end = start + quantities[task_id]
    # Process cfs
    worker(cfs[start:end], start, end, cfcache_dir, coyote_app)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Distribute tasks for fillcf')
    parser.add_argument('--nprocs', type=int, required=True, help='Number of processes')
    parser.add_argument('--cfcache_dir', required=True, help='Path to cfcache directory')
    parser.add_argument('--coyote_app', required=True, help='Path to coyote_app binary')
    args = parser.parse_args()

    cfs = sorted([os.path.basename(f) for f in glob.glob(os.path.join(args.cfcache_dir, 'CFS*')) if os.path.isdir(f)])
    task_id = os.getenv('SLURM_ARRAY_TASK_ID')
    if task_id is None:
        print("SLURM_ARRAY_TASK_ID is not set")
        task_id = 0
    else:
        print("SLURM_ARRAY_TASK_ID is set to:", task_id)
        print("Number of cfs:", len(cfs))
        print("Number of processes:", args.nprocs)
        print("Number of cfs per process:", len(cfs) // args.nprocs)
        print("Using binary:", args.coyote_app)
        print("Using cfcache directory:", args.cfcache_dir)
        print("Running task:", task_id)
        task_id = int(task_id)

    distribute_tasks(args.nprocs, task_id, cfs, args.cfcache_dir, args.coyote_app)
EOF
fi
# Submit the job
job_id=$(sbatch ${chdir}/submit_sbatch.sh | awk '{print $NF}')
echo -e "${LOG_PREFIX} Job submitted with ID: $job_id"