import argparse
import glob
import os
import subprocess
import sys
import logging
import argparse
import glob
import shutil
import getpass

#!/usr/bin/env python3

# Default values
nprocs = 40
chdir = ""

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Parse command-line options
parser = argparse.ArgumentParser(description='Launch the fillcf script.')
parser.add_argument('-n', '--nprocs', type=int, default=40, help='Number of processes will default to 40')
parser.add_argument('-w', '--chdir', required=True, help='Working directory')
parser.add_argument('-c', '--cfcache_dir', required=True, help='Path to cfcache directory')
parser.add_argument('-E', '--coyote_app', required=True, help='Path to coyote_app binary')
parser.add_argument('-C', '--CASAPATH', help='CASAPATH or the path to directory container data directory')
parser.add_argument('-e', '--email', help='User email')
parser.add_argument('-u', '--username', help='User name')
parser.add_argument('-s', '--submit', type=bool, default=False, help='Submit the job. Default is False')
args = parser.parse_args()

# Display help if -h option is used
if '-h' in sys.argv or '--help' in sys.argv:
    parser.print_help()
    sys.exit(0)

nprocs = args.nprocs
chdir = args.chdir
cfcache_dir = args.cfcache_dir
coyote_app = args.coyote_app
casapath = args.CASAPATH
email = args.email
username = args.username
submit = args.submit

if nprocs < 1:
    logger.warning("Number of processes must be greater than 0")
    nprocs = 10
    logger.info(f"Will use {nprocs} process")
startidx = 0
endidx = startidx + nprocs - 1

if username == "":
    logger.warning("No username provided")
    username = getpass.getuser()
    logger.info(f"Will use current user: {username}")
if email == "":
    logger.warning("No email provided")
    if username == getpass.getuser():
        email = f"{username}.nrao.edu"
        logger.info(f"Will use email: {email}")

start = 0
end = nprocs - 1

# Function to check if a path exists and return its type (file, directory, or link)
def check_path(path):
    if not os.path.exists(path):
        # logger.warning(f"{path} does not exist")
        return "not found"
    elif os.path.isfile(path):
        # logger.info(f"{path} is found and is a file")
        return "file"
    elif os.path.isdir(path):
        # logger.info(f"{path} is found and is a directory")
        return "directory"
    elif os.path.islink(path):
        # logger.info(f"{path} is found and is a symbolic link")
        return "link"

# Call the function
direxists = check_path(cfcache_dir)
if direxists == "directory":
    logger.info(f"cfcache_dir is set to {cfcache_dir}")
    # List number of directories with name CF*.im inside cfcache_dir
    cf_dirs = glob.glob(os.path.join(cfcache_dir, 'CF*.im'))
    num_cf_dirs = len(cf_dirs)
    logger.info(f"Number of CFs found is: {num_cf_dirs}")
else:
    logger.error(f"cfcache_dir {cfcache_dir} does not exist")
    sys.exit(1)

# Check if chdir exists
direxists = check_path(chdir)
if direxists == "directory":
    logger.info(f"Working directory is set to {chdir}")
elif direxists == "not found":
    logger.warning(f"Working directory {chdir} does not exist will attempt to create it")
    shutil.os.makedirs(chdir)
    logger.info(f"Creating working directory: {chdir}")

# Function to create the logs directory
direxists = check_path(os.path.join(chdir, "logs"))
if direxists != "directory":
    shutil.os.makedirs(os.path.join(chdir, "logs"))
    logger.info("Logs directory not found")
    logger.info(f"Creating logs directory {os.path.join(chdir, 'logs')}")
else:
    logger.info(f"Logs directory found at {os.path.join(chdir, 'logs')}")

# Check if coyote_app exists
checkfile = check_path(coyote_app)
if checkfile == "directory":
    logger.warning(f"{coyote_app} is a directory")
    logger.warning(f"Checking for coyote_app binary in {coyote_app}/bin/coyote")
    checkfile = check_path(os.path.join(coyote_app, "bin/coyote"))
    if checkfile == "file":
        coyote_app = os.path.join(coyote_app, "bin/coyote")
        logger.info(f"coyote_app binary found at {coyote_app}")
    else:
        logger.error(f"coyote_app binary not found in {coyote_app}. Please provide the correct path")
        sys.exit(1)
elif checkfile == "file":
    logger.info(f"{coyote_app} is found")

# Check if CASAPATH is set and exists
if casapath is None:
    logger.warning("CASAPATH is not set")
    logger.warning(f"Setting CASAPATH to {chdir}/data")
    casapath = os.path.join(chdir, "data")
direxists = check_path(casapath)
if direxists == "symlink":
    logger.info("CASAPATH is set and is a symlink")
elif direxists == "directory":
    logger.info("CASAPATH is set and is a directory")
else:
    logger.warning("CASAPATH is not set")
    logger.warning("Checking for /home/casa/data/trunk")
    casa_data_dir = "/home/casa/data/trunk"
    if os.path.isdir(casa_data_dir):
        logger.info(f"Found casa data directory at {casa_data_dir}")
        logger.info(f"Creating symlink data inside {chdir} to casa data directory")
        if not os.path.islink(os.path.join(chdir, "data")):
            os.symlink(casa_data_dir, os.path.join(chdir, "data"))
            logger.info("Symlink 'data' created")
        else:
            logger.info("Symlink 'data' already exists will use it")
    else:
        logger.error("CASAPATH is not set & casa data directory does not exist")
        sys.exit(1)


# Check if slurm_fillcf.py exists
python_file_path = os.path.join(chdir, "slurm_fillcf.py")
if not os.path.isfile(python_file_path):
    logger.info("slurm_fillcf.py does not exist and will be created")
    create_python_file = True
else:
    logger.info("slurm_fillcf.py exists and will be overwritten")
    os.remove(python_file_path)
    create_python_file = True


# Write out submit_sbatch.sh
# Check if submit_sbatch.sh exists, warn that it will be overwritten
submit_sbatch_path = os.path.join(chdir, "submit_sbatch.sh")
if os.path.isfile(submit_sbatch_path):
    logger.warning("submit_sbatch.sh exists and will be overwritten")
    logger.warning(f"Overwriting {submit_sbatch_path}")
# print("submit_sbatch_path:", submit_sbatch_path)
# print("chdir:", chdir)
# print("start:", start)
# print("end:", end)
with open(submit_sbatch_path, "w") as f:
    f.write(f'''#!/bin/bash
#SBATCH --export=ALL
#SBATCH --array={start}-{end}
#SBATCH --chdir={chdir}
#SBATCH --time=1-0:0:0
#SBATCH --mem=2G
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=logs/slurm_%A_%a.out
#SBATCH --error=logs/slurm_%A_%a.err
#SBATCH --job-name=fillcf
#SBATCH --account={username}
#SBATCH --mail-user={email}
#SBATCH --mail-type=FAIL
python3 {python_file_path} --cfcache_dir {cfcache_dir} --nprocs {nprocs} --coyote_app {coyote_app}
    ''')

if create_python_file:
    logger.info(f"Creating python file in the working directory to distribute tasks : {python_file_path}")
    # Write out the python file
    with open(python_file_path, "w") as f:
        f.write("""import argparse
import glob
import os
import sys
import subprocess

def check_path(path, path_name):
    if not os.path.exists(path):
        raise FileNotFoundError(f"The {path} does not exist. Please provide a valid path for {path_name}")

def worker(cfs, start, end, cfcache_dir, coyote_app):
    pid = os.getenv('SLURM_ARRAY_TASK_ID', 'default')
    print("Current working directory:", os.getcwd())
    print("SLURM_ARRAY_TASK_ID:", pid)
    with open(f'logs/{pid}.txt', 'w') as f:
        f.write(f"Starting index: {start} \\n")
        f.write(f"Ending index: {end-1} \\n")
        f.write(f"Number of cfs: {len(cfs[start:end])} \\n")
        for cf in cfs:
            f.write(cf + '\\n')
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
""")

# Submit the job
if submit:
    logger.info("Submitting the job")
    job_id = subprocess.call(["sbatch", submit_sbatch_path])
    # logger.info(f"Job submitted with ID: {job_id}")
    # slurm_array_task_id = int(job_id.split('.')[0])
    # logger.info(f"SLURM_ARRAY_TASK_ID: {slurm_array_task_id}")
    # Cancel the job
    # logger.info("Cancelling the job")
    # subprocess.run(["scancel", job_id])
