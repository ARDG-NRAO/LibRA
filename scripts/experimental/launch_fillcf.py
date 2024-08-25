#!/usr/bin/env python3

import argparse
import glob
import os
import subprocess
import sys

# Default values
nprocs = 40
chdir = ""

# Parse command-line options
parser = argparse.ArgumentParser(description='Launch the fillcf script.')
parser.add_argument('-n', '--nprocs', type=int, default=40, help='Number of processes')
parser.add_argument('-w', '--chdir', default="", help='Working directory')
parser.add_argument('-c', '--cfcache_dir', required=True, help='Path to cfcache directory')
parser.add_argument('-E', '--coyote_app', required=True, help='Path to coyote_app binary')
parser.add_argument('-C', '--CASAPATH', help='Path to CASAPATH directory')
args = parser.parse_args()

nprocs = args.nprocs
chdir = args.chdir
cfcache_dir = args.cfcache_dir
coyote_app = args.coyote_app
CASAPATH = args.CASAPATH

start = 0
end = nprocs - 1

# Function to check if a directory exists
def check_directory(dir, log_prefix):
  if not os.path.isdir(dir):
    print(f"{log_prefix} {dir} does not exist")
    sys.exit(1)
  else:
    print(f"{log_prefix} {dir} is: {dir}")

# Function to create a directory if it doesn't exist
def create_directory(dir, log_prefix):
  if not os.path.isdir(dir):
    print(f"{log_prefix} {dir} does not exist, creating it")
    os.makedirs(dir)
  else:
    print(f"{log_prefix} The directory {dir} already exists")

# Function to create the logs directory
def create_logs_directory(dir, log_prefix):
  logs_dir = os.path.join(dir, "logs")
  if not os.path.isdir(logs_dir):
    print(f"{log_prefix} logs directory does not exist inside {dir}")
    print(f"{log_prefix} Creating logs directory: {logs_dir}")
    os.makedirs(logs_dir)
  else:
    print(f"{log_prefix} logs directory exists and logs will be written to: {logs_dir}")

# Function to check if the coyote_app file exists
def check_coyote_app(app, log_prefix):
  if not os.path.isfile(app):
    print(f"{log_prefix} coyote_app does not exist")
    sys.exit(1)
  else:
    print(f"{log_prefix} coyote_app PATH is: {app}")

# Function to check if CASAPATH is set and exists
def check_casapath(casapath, chdir, log_prefix):
  if not casapath:
    print(f"{log_prefix} CASAPATH is not set")
    print(f"{log_prefix} Checking for /home/casa/data/trunk")
    casa_data_dir = "/home/casa/data/trunk"
    if os.path.isdir(casa_data_dir):
      print(f"{log_prefix} Found casa data directory at {casa_data_dir}")
      print(f"{log_prefix} Creating symlink data inside {chdir} to casa data directory")
      if not os.path.islink(os.path.join(chdir, "data")):
        os.symlink(casa_data_dir, os.path.join(chdir, "data"))
        print(f"{log_prefix} Symlink 'data' created")
      else:
        print(f"{log_prefix} Symlink 'data' already exists")
    else:
      print(f"{log_prefix} CASAPATH is not set & Casa data directory does not exist")
      sys.exit(1)
  else:
    print(f"{log_prefix} CASAPATH is set to: {casapath}")
    if not os.path.isdir(casapath):
      print(f"{log_prefix} CASAPATH directory does not exist")
      sys.exit(1)

# Call the functions
check_directory(cfcache_dir, "[LOG]")
create_directory(chdir, "[LOG]")
create_logs_directory(chdir, "[LOG]")
check_coyote_app(coyote_app, "[LOG]")
check_casapath(CASAPATH, chdir, "[LOG]")

# Check if slurm_fillcf.py exists
PYTHON_FILE_PATH = os.path.join(chdir, "slurm_fillcf.py")
if not os.path.isfile(PYTHON_FILE_PATH):
  print("[LOG] slurm_fillcf.py does not exist and will be created")
  create_python_file = True

# Write out submit_sbatch.sh
# Check if submit_sbatch.sh exists, warn that it will be overwritten
submit_sbatch_path = os.path.join(chdir, "submit_sbatch.sh")
if os.path.isfile(submit_sbatch_path):
  print("[WARN] submit_sbatch.sh exists and will be overwritten")

with open(submit_sbatch_path, "w") as f:
  f.write("#!/bin/bash\n")
  f.write("#SBATCH --export=ALL\n")
  f.write(f"#SBATCH --array={start}-{end}\n")
  f.write(f"#SBATCH --chdir={chdir}\n")
  f.write("#SBATCH --time=1-0:0:0\n")
  f.write("#SBATCH --mem=2G\n")
  f.write("#SBATCH --nodes=1\n")
  f.write("#SBATCH --ntasks-per-node=1\n")
  f.write("#SBATCH --output=logs/slurm_%A_%a.out\n")
  f.write("#SBATCH --error=logs/slurm_%A_%a.err\n")
  f.write("#SBATCH --job-name=fillcf\n")
  f.write("#SBATCH --account=pjaganna\n")
  f.write("#SBATCH --mail-user=pjaganna@nrao.edu\n")
  f.write("#SBATCH --mail-type=FAIL\n")
  f.write(f"python3 {PYTHON_FILE_PATH} --cfcache_dir {cfcache_dir} --nprocs {nprocs} --coyote_app {coyote_app}\n")

if create_python_file:
  print("[LOG] Creating python file")
  # Write out the python file
  with open(PYTHON_FILE_PATH, "w") as f:
    f.write("import argparse\n")
    f.write("import glob\n")
    f.write("import os\n")
    f.write("import sys\n")
    f.write("import subprocess\n")
    f.write("\n")
    f.write("def check_path(path, path_name):\n")
    f.write("    if not os.path.exists(path):\n")
    f.write("        raise FileNotFoundError(f\"The {path} does not exist. Please provide a valid path for {path_name}\")\n")
    f.write("\n")
    f.write("def worker(cfs, start, end, cfcache_dir, coyote_app):\n")
    f.write("    pid = os.getenv('SLURM_ARRAY_TASK_ID', 'default')\n")
    f.write("    with open(f'logs/{pid}.txt', 'w') as f:\n")
    f.write("        f.write(f\"Starting index: {start} \\n\")\n")
    f.write("        f.write(f\"Ending index: {end-1} \\n\")\n")
    f.write("        f.write(f\"Number of cfs: {len(cfs[start:end])} \\n\")\n")
    f.write("        for cf in cfs:\n")
    f.write("            f.write(cf + '\\n')\n")
    f.write("    f.close()\n")
    f.write("    command = [f'{coyote_app}', 'help=noprompt', 'mode=fillcf', f'cfcache={cfcache_dir}', f'cflist={\", \".join(cfs)}']\n")
    f.write("    print(\"Running command:\", \" \".join(command))\n")
    f.write("    subprocess.run(command)\n")
    f.write("\n")
    f.write("def distribute_tasks(n_processes, task_id, cfs, cfcache_dir, coyote_app):\n")
    f.write("    # Distribute tasks among processes\n")
    f.write("    base, remainder = divmod(len(cfs), n_processes)\n")
    f.write("    quantities = [base + 1 if i < remainder else base for i in range(n_processes)]\n")
    f.write("\n")
    f.write("    # Determine start and end indices for this task\n")
    f.write("    start = sum(quantities[:task_id])\n")
    f.write("    end = start + quantities[task_id]\n")
    f.write("\n")
    f.write("    # Process cfs\n")
    f.write("    worker(cfs[start:end], start, end, cfcache_dir, coyote_app)\n")
    f.write("\n")
    f.write("if __name__ == '__main__':\n")
    f.write("    parser = argparse.ArgumentParser(description='Process the parallel cf filling.')\n")
    f.write("    parser.add_argument('--coyote_app', default='default', help='Coyote app')\n")
    f.write("    parser.add_argument('--cfcache_dir', required=True, help='cfcache_dir path')\n")
    f.write("    parser.add_argument('--nprocs', type=int, required=True, help='Number of processes')\n")
    f.write("\n")
    f.write("    args = parser.parse_args()\n")
    f.write("\n")
    f.write("    # Check if the cfcache_dir exists\n")
    f.write("    check_path(args.cfcache_dir, 'cfcache_dir')\n")
    f.write("\n")
    f.write("    # Check if the coyote_app exists\n")
    f.write("    check_path(args.coyote_app, 'coyote_app')\n")
    f.write("\n")
    f.write("    # Get all cfs in the cfcache_dir\n")
    f.write("    cfs = sorted([os.path.basename(f) for f in glob.glob(os.path.join(args.cfcache_dir, 'CFS*')) if os.path.isdir(f)])\n")
    f.write("\n")
    f.write("    # Get the task_id\n")
    f.write("    task_id = int(os.getenv('SLURM_ARRAY_TASK_ID', 'default'))\n")
    f.write("\n")
    f.write("    # Distribute tasks among processes\n")
    f.write("    distribute_tasks(args.nprocs, task_id, cfs, cfcache_dir=args.cfcache_dir, coyote_app=args.coyote_app)\n")

# Submit the job
job_id = subprocess.check_output(["sbatch", submit_sbatch_path]).decode("utf-8").split()[-1]
print(f"[LOG] Job submitted with ID: {job_id}")
