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
    h )
      echo "Usage: $0 
             [-n nprocs]
             [-w chdir]
             [-c cfcache_dir]
             [-E full path to exodus binary]
             [-C the directory that contains the casadata in a directory named data,
              i.e the CASAPATH. If not present the script will check for 
              /home/casa/data/trunk and create a symlink to it]
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
else
  echo "cfcache_dir is : $cfcache_dir"
fi

# check is chdir exists else make it before creating logs
if [ ! -d "$chdir" ]; then
  echo "chdir does not exist making it"
  mkdir -p $chdir
else
  echo "The working directory is set to: $chdir"
fi

# Check is a directory called logs exists inside chdir if not create it
if [ ! -d "$chdir/logs" ]; then
  echo "logs directory does not exist inside chdir"
  echo "Creating logs directory: $chdir/logs "
  mkdir -p $chdir/logs
else
  echo "logs directory exists and logs will be written to: $chdir/logs"
fi

# Check the exodus bundle file is not present then exit
if [ ! -f "$coyote_app" ]; then
  echo "coyote_app does not exist"
  exit 1
else
  echo "coyote_app PATH is : $coyote_app"
fi

# Check if CASAPATH was set then check if it exists
if [ -z "$CASAPATH" ]; then
  echo "CASAPATH is not set"
  echo "Checking for /home/casa/data/trunk"
  casa_data_dir="/home/casa/data/trunk"
  if [ -d "$casa_data_dir" ]; then
    echo "Found casa data directory at $casa_data_dir"
    echo "Creating symlink data inside $chdir to casa data directory"
    ln -s "$casa_data_dir" "$chdir/data"
  else
    echo "CASAPATH is not set & Casa data directory does not exist"
    exit 1
  fi
else
  if [ ! -d "$CASAPATH" ]; then
    echo "CASAPATH does not exist"
    exit 1
  else
    casa_data_dir="/home/casa/data/trunk"
    if [ -d "$casa_data_dir" ]; then
      echo "Found casa data directory at $casa_data_dir"
      echo "Creating symlink to casa data directory"
      ln -s "$casa_data_dir" "$chdir/data"
    else
      echo "casa data directory does not exist"
      exit 1
    fi
  fi
fi



# check if slurm_fillcf.py exists
PYTHON_FILE_PATH="$chdir/slurm_fillcf.py"
if [ ! -f $PYTHON_FILE_PATH ]; then
  echo "slurm_fillcf.py does not exist and will be created"
  create_python_file="True"
fi
# Write out submit_sbatch.sh
# check if submit_sbatch.sh exists warn that it will be overwritten
if [ -f "$chdir/submit_sbatch.sh" ]; then
  echo "submit_sbatch.sh exists and will be overwritten"
fi
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

python3 ${PYTHON_FILE_PATH} --cfcache_dir ${cfcache_dir} --nprocs ${nprocs} --coyote_app ${coyote_app}

EOF

if [ "$create_python_file" == "True" ]; then
  echo "Creating python file"
  # write out the python file
  cat <<EOF > ${PYTHON_FILE_PATH}
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
      base, remainder = divmod(len(cfs), n_processes)
      base, remainder = divmod(len(cfs), n_processes)
      quantities = [base + 1 if i < remainder else base for i in range(n_processes)]

      # Determine start and end indices for this task
      start = sum(quantities[:task_id])
      end = start + quantities[task_id]

      # Process cfs
      worker(cfs[start:end], start, end, cfcache_dir, coyote_app)

  if __name__ == "__main__":
      parser = argparse.ArgumentParser(description='Process the parallel cf filling.')
      parser.add_argument('--coyote_app', default='default', help='Coyote app')
      parser.add_argument('--cfcache_dir', required=True, help='cfcache_dir path')
      parser.add_argument('--nprocs', type=int, required=True, help='Number of processes')
      #parser.add_argument('--task_id', type=int, required=True, help='Slurm array task ID')

      args = parser.parse_args()

      # Check if the cfcache_dir exists
      check_path(args.cfcache_dir, 'cfcache_dir')

      # Check if the coyote_app exists
      check_path(args.coyote_app, 'coyote_app')

      # Get all cfs in the cfcache_dir
      cfs = sorted([os.path.basename(f) for f in glob.glob(os.path.join(args.cfcache_dir, 'CFS*')) if os.path.isdir(f)])

      # Get the task_id
      task_id = int(os.getenv('SLURM_ARRAY_TASK_ID', 'default'))


      # Distribute tasks among processes
      distribute_tasks(args.nprocs, task_id, cfs, cfcache_dir=args.cfcache_dir, coyote_app=args.coyote_app)
EOF
fi  
# Submit the job

sbatch submit_sbatch.sh
