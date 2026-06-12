#!/bin/bash

# HTCondor version of launch_fillcf.sh. Generates a condor submit description
# file plus the worker python script, and submits with condor_submit.

# Default values
nprocs=40
chdir=""
mem="4G"

# Log prefix colors
LOG_PREFIX="\e[32m[LOG]\e[0m"
WARN_PREFIX="\e[33m[WARN]\e[0m"
ERROR_PREFIX="\e[31m[ERROR]\e[0m"

# Parse command-line options
while getopts ":n:w:c:E:C:u:e:m:h" opt; do
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
  m)
    mem=$OPTARG
    ;;
  h)
    echo -e "Usage: $0
             [-n nprocs]
             [-w chdir]
             [-c cfcache_dir]
             [-E full path to coyote binary or bundle directory]
             [-C the directory that contains the casadata in a directory named data,
              i.e the CASAPATH. If not present the script will check for
              /home/casa/data/trunk and create a symlink to it]
             [-u username]
             [-e email]
             [-m memory per job (default 4G)]
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
export cfcache_dir
export coyote_app
export CASAPATH
export chdir
export username
export email

#check if username and email are set
if [ -z "$username" ] || [ -z "$email" ]; then
  echo -e "${WARN_PREFIX} Username and email are not set"
  username=$(whoami)
  echo -e "${LOG_PREFIX} Username is set to: $username"
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
      CASAPATH="$chdir/data"
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

# Function to check if the coyote binary or bundle exists
check_coyote_app() {
  local app=$1
  local log_prefix=$2

  if [ -f "$app" ]; then
    echo -e "${log_prefix} ${app} is an executable file"
  elif [ -d "$app" ]; then
    local coyote_path="$app/bin/coyote"
    if [ -f "$coyote_path" ]; then
      echo -e "${log_prefix} ${app} is a directory"
      echo -e "${log_prefix} ${app}/bin/coyote exists"
      coyote_app="$coyote_path"
      echo -e "${log_prefix} coyote PATH is: $coyote_app"
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

# Check if htcondor_fillcf.py exists
PYTHON_FILE_PATH="$chdir/htcondor_fillcf.py"
if [ ! -f $PYTHON_FILE_PATH ]; then
  echo -e "${LOG_PREFIX} ${PYTHON_FILE_PATH} does not exist and will be created"
else
  echo -e "${LOG_PREFIX} ${PYTHON_FILE_PATH} exists removing it"
  rm -f $PYTHON_FILE_PATH
fi

# Write out submit_condor.sub
if [ -f "$chdir/submit_condor.sub" ]; then
  echo -e "${WARN_PREFIX} ${chdir}/submit_condor.sub exists and will be overwritten"
fi

PYTHON_EXE=$(command -v python3)

cat <<EOF >${chdir}/submit_condor.sub
universe        = vanilla
executable      = ${PYTHON_EXE}
arguments       = "${PYTHON_FILE_PATH} --cfcache_dir ${cfcache_dir} --nprocs ${nprocs} --coyote_app ${coyote_app} --task_id \$(Process)"
initialdir      = ${chdir}
getenv          = True
environment     = "CASAPATH=${CASAPATH}"
request_memory  = ${mem}
request_cpus    = 1
output          = logs/condor_\$(Cluster)_\$(Process).out
error           = logs/condor_\$(Cluster)_\$(Process).err
log             = logs/condor_\$(Cluster).log
batch_name      = fillcf
notify_user     = ${email}
notification    = Error
accounting_group_user = ${username}
should_transfer_files = IF_NEEDED
when_to_transfer_output = ON_EXIT

queue ${nprocs}
EOF

echo -e "${LOG_PREFIX} Creating python file"
cat <<-EOF >${PYTHON_FILE_PATH}
import argparse
import glob
import os
import sys
import subprocess

def check_path(path, path_name):
    if not os.path.exists(path):
        raise FileNotFoundError(f"The {path} does not exist. Please provide a valid path for {path_name}")

def worker(cfs, start, end, cfcache_dir, coyote_app, task_id):
    with open(f'logs/{task_id}.txt', 'w') as f:
        f.write(f"Starting index: {start} \n")
        f.write(f"Ending index: {end-1} \n")
        f.write(f"Number of cfs: {len(cfs)} \n")
        for cf in cfs:
            f.write(cf + '\n')
    command = [f'{coyote_app}', 'help=noprompt', 'mode=fillcf', f'cfcache={cfcache_dir}', f'cflist={",".join(cfs)}']
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
    worker(cfs[start:end], start, end, cfcache_dir, coyote_app, task_id)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Distribute tasks for fillcf under HTCondor')
    parser.add_argument('--nprocs', type=int, required=True, help='Number of processes')
    parser.add_argument('--cfcache_dir', required=True, help='Path to cfcache directory')
    parser.add_argument('--coyote_app', required=True, help='Path to coyote_app binary')
    parser.add_argument('--task_id', type=int, default=None, help='Task index (condor \$(Process))')
    args = parser.parse_args()

    cfs = sorted([os.path.basename(f) for f in glob.glob(os.path.join(args.cfcache_dir, 'CFS*')) if os.path.isdir(f)])
    task_id = args.task_id
    if task_id is None:
        task_id = int(os.getenv('CONDOR_PROCESS', '0'))

    distribute_tasks(args.nprocs, task_id, cfs, args.cfcache_dir, args.coyote_app)
EOF

# Submit the job
job_id=$(condor_submit ${chdir}/submit_condor.sub | grep -oP 'cluster \K[0-9]+')
echo -e "${LOG_PREFIX} Job submitted with cluster ID: $job_id"
