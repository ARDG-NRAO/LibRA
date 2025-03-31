# This script is used to launch the `fillcf` script for processing CFs (Convolution Functions) in a distributed manner 
# using SLURM job arrays. It validates inputs, checks directories, prepares necessary files, and optionally submits 
# the job to a SLURM cluster.

# Classes:
#     - CustomFormatter: A custom logging formatter for color-coded logs.
#     - FillCFLauncher: Main class to handle argument parsing, input validation, and job preparation.

# Functions:
#     - write_submit_sbatch: Generates the SLURM batch script for job submission.
#     - write_python_file: Creates a Python script to distribute tasks among processes.
#     - generate_and_submit_job: Combines the above functions to generate and optionally submit the job.

# Usage:
#     Run the script with the required arguments to prepare and optionally submit the job.

#     Example:
#         python3 launch_fillcf.py -n 40 -w /path/to/working_dir -c /path/to/cfcache_dir \
#         -E /path/to/coyote_app -C /path/to/CASAPATH -e user@example.com -u username -s True

# Arguments:
#     -n, --nprocs: Number of processes to use (default: 40).
#     -w, --chdir: Working directory (required).
#     -c, --cfcache_dir: Path to the cfcache directory (required).
#     -E, --coyote_app: Path to the coyote_app binary (required).
#     -C, --CASAPATH: Path to the CASA data directory (optional).
#     -e, --email: User email for SLURM notifications (optional).
#     -u, --username: Username for SLURM account (optional).
#     -s, --submit: Whether to submit the job after preparation (default: False).

# Notes:
#     - The script validates the provided paths and creates necessary directories if they do not exist.
#     - If the `CASAPATH` is not provided, it defaults to a symlink pointing to `/home/casa/data/trunk`.
#     - The SLURM batch script and Python task distribution script are created in the working directory.
#     - Logs are stored in the `logs` subdirectory of the working directory.

import argparse
import glob
import os
import subprocess
import sys
import logging

# Configure logging with color logs this from stackoverflow.
class CustomFormatter(logging.Formatter):
    grey = "\x1b[38;20m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    bold_red = "\x1b[31;1m"
    reset = "\x1b[0m"
    format = "%(asctime)s - %(name)s - %(levelname)s - %(message)s (%(filename)s:%(lineno)d)"

    FORMATS = {
        logging.DEBUG: grey + format + reset,
        logging.INFO: grey + format + reset,
        logging.WARNING: yellow + format + reset,
        logging.ERROR: red + format + reset,
        logging.CRITICAL: bold_red + format + reset
    }

    def format(self, record):
        log_fmt = self.FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt)
        return formatter.format(record)

class FillCFLauncher:
    def __init__(self):
        self.args = self.parse_arguments()
        self.logger = self.setup_logger()
        self.nprocs = self.args.nprocs
        self.chdir = self.args.chdir
        self.cfcache_dir = self.args.cfcache_dir
        self.coyote_app = self.args.coyote_app
        self.casapath = self.args.CASAPATH
        self.email = self.args.email
        self.username = self.args.username
        self.submit = self.args.submit
        self.start = 0
        self.end = self.nprocs - 1
        self.python_file_path = os.path.join(self.chdir, "slurm_fillcf.py")
        if not os.path.isabs(self.python_file_path):
            # self.logger.warning(f"Python file path {self.python_file_path} is relative. Converting to absolute path.")
            self.python_file_path = os.path.abspath(self.python_file_path)
            # self.logger.info(f"Python file path is now set to absolute path: {self.python_file_path}")
        self.create_python_file = True
        self.args.time = getattr(self.args, "time", None)
        self.args.mem = getattr(self.args, "mem", None)
        self.args.nodes = getattr(self.args, "nodes", None)
        self.args.ntasks_per_node = getattr(self.args, "ntasks_per_node", None)
        self.args.output = getattr(self.args, "output", None)
        self.args.error = getattr(self.args, "error", None)
        self.args.job_name = getattr(self.args, "job_name", None)

    @staticmethod
    def parse_arguments():
        parser = argparse.ArgumentParser(
            description="Launch the fillcf script.",
            epilog=(
                "Example command:\n"
                "python3 launch_fillcf.py -n 40 -w /path/to/working_dir -c /path/to/cfcache_dir "
                "-E /path/to/coyote_app -C /path/to/CASAPATH -e user@example.com -u username -s True"
            ),
            formatter_class=argparse.RawTextHelpFormatter
        )
        parser.add_argument(
            "-n", "--nprocs", type=int, default=40, help="Number of processes (default: 40)"
        )
        parser.add_argument("-w", "--chdir", required=True, help="Working directory")
        parser.add_argument("-c", "--cfcache_dir", required=True, help="Path to cfcache directory")
        parser.add_argument("-E", "--coyote_app", required=True, help="Path to coyote_app binary")
        parser.add_argument("-C", "--CASAPATH", help="CASAPATH or the path to data directory")
        parser.add_argument("-e", "--email", help="User email")
        parser.add_argument("-u", "--username", help="User name")
        parser.add_argument(
            "-s", "--submit", type=bool, default=False, help="Submit the job (default: False)"
        )
        args = parser.parse_args()
        return args

    @staticmethod
    def setup_logger():
        logger = logging.getLogger(__name__)
        handler = logging.StreamHandler()
        handler.setFormatter(CustomFormatter())
        logger.addHandler(handler)
        logger.setLevel(logging.INFO)
        return logger

    @staticmethod
    def check_path(path):
        if not os.path.exists(path):
            return "not found"
        elif os.path.isfile(path):
            if os.access(path, os.X_OK):
                path_type = "file (executable)"
            else:
                path_type = "file (not executable)"
        elif os.path.isdir(path):
            path_type = "directory"
        elif os.path.islink(path):
            path_type = "link"
        else:
            path_type = "unknown"

        if os.path.isabs(path):
            return f"{path_type} (absolute)"
        else:
            absolute_path = os.path.abspath(path)
            return f"{path_type} (relative), absolute path: {absolute_path}"

    def validate_inputs(self):
        if self.nprocs < 1:
            self.logger.warning("Number of processes must be greater than 0")
            self.nprocs = 10
            self.logger.info(f"Will use {self.nprocs} processes")

        if not self.username:
            self.logger.warning("No username provided")
            self.username = os.getenv("USER") or subprocess.getoutput("whoami")
            self.logger.info(f"Will use current user: {self.username}")

        if not self.email:
            self.logger.warning("No email provided")
            self.email = f"{self.username}@nrao.edu"
            self.logger.info(f"Will use email: {self.email}")

    def check_directories(self):
        # Check cfcache_dir
        direxists = self.check_path(self.cfcache_dir)
        if "relative" in direxists:
            self.logger.warning(f"cfcache_dir {self.cfcache_dir} is a relative path. Converting to absolute path.")
            self.cfcache_dir = os.path.abspath(self.cfcache_dir)
            self.logger.info(f"cfcache_dir is now set to absolute path: {self.cfcache_dir}")
            direxists = self.check_path(self.cfcache_dir)

        if "directory" in direxists:
            self.logger.info(f"cfcache_dir is set to {self.cfcache_dir}")
            cf_dirs = glob.glob(os.path.join(self.cfcache_dir, "CF*.im"))
            num_cf_dirs = len(cf_dirs)
            self.logger.info(f"Number of CFs found: {num_cf_dirs}")
        else:
            self.logger.error(f"cfcache_dir {self.cfcache_dir} does not exist or is invalid")
            sys.exit(1)
        # Check working directory
        direxists = self.check_path(self.chdir)
        if "relative" in direxists:
            self.logger.warning(f"Working directory {self.chdir} is a relative path. Converting to absolute path.")
            self.chdir = os.path.abspath(self.chdir)
            self.logger.info(f"Working directory is now set to absolute path: {self.chdir}")
            direxists = self.check_path(self.chdir)
        if direxists == "directory":
            self.logger.info(f"Working directory is set to {self.chdir}")
        elif direxists == "not found":
            self.logger.warning(f"Working directory {self.chdir} does not exist. Creating it.")
            os.makedirs(self.chdir)
            self.logger.info(f"Created working directory: {self.chdir}")
        # Create logs directory
        logs_dir = os.path.join(self.chdir, "logs")
        direxists = self.check_path(logs_dir)
        if "relative" in direxists:
            self.logger.warning(f"Logs directory {logs_dir} is a relative path. Converting to absolute path.")
            logs_dir = os.path.abspath(logs_dir)
            self.logger.info(f"Logs directory is now set to absolute path: {logs_dir}")
            direxists = self.check_path(logs_dir)
        if direxists != "directory":
            os.makedirs(logs_dir, exist_ok=True)
            self.logger.info(f"Created logs directory: {logs_dir}")
        else:
            self.logger.info(f"Logs directory found at: {logs_dir}")

    def check_coyote_app(self):
        if not os.path.isabs(self.coyote_app):
            self.logger.warning(f"coyote_app path {self.coyote_app} is relative. Converting to absolute path.")
            self.coyote_app = os.path.abspath(self.coyote_app)
            self.logger.info(f"coyote_app path is now set to absolute path: {self.coyote_app}")

        checkfile = self.check_path(self.coyote_app)
        if checkfile == "directory":
            self.logger.warning(f"{self.coyote_app} is a directory. Checking for binary in {self.coyote_app}/bin/coyote")
            checkfile = self.check_path(os.path.join(self.coyote_app, "bin/coyote"))
            if checkfile == "file":
                self.coyote_app = os.path.join(self.coyote_app, "bin/coyote")
                self.logger.info(f"coyote_app binary found at {self.coyote_app}")
        elif "file (executable)" in checkfile:
                self.logger.warning(f"coyote_app binary not found in {self.coyote_app}. Please provide the correct path.")
        elif checkfile == "file":
            self.logger.info(f"coyote_app binary found at {self.coyote_app}")
            return
        else:
            self.logger.error(f"coyote_app binary not found at {self.coyote_app}")
            sys.exit(1)

    def check_casapath(self):
        if not self.casapath:
            self.logger.warning("CASAPATH is not set. Setting it to default.")
            self.casapath = os.path.join(self.chdir, "data")

        direxists = self.check_path(self.casapath)
        if direxists == "symlink":
            self.logger.info("CASAPATH is set and is a symlink")
        elif direxists == "directory":
            self.logger.info("CASAPATH is set and is a directory")
        else:
            casa_data_dir = "/home/casa/data/trunk"
            if os.path.isdir(casa_data_dir):
                self.logger.info(f"Found CASA data directory at {casa_data_dir}. Creating symlink.")
                if not os.path.islink(os.path.join(self.chdir, "data")):
                    os.symlink(casa_data_dir, os.path.join(self.chdir, "data"))
                    self.logger.info("Symlink 'data' created")
                else:
                    self.logger.info("Symlink 'data' already exists. Using it.")
            else:
                self.logger.error("CASAPATH is not set and CASA data directory does not exist.")
                sys.exit(1)

    def prepare_python_file(self):
        if not os.path.isfile(self.python_file_path):
            self.logger.info(f"{self.python_file_path} does not exist and will be created.")
            self.create_python_file = True
        else:
            self.logger.info(f"{self.python_file_path} exists and will be overwritten.")
            os.remove(self.python_file_path)
            self.create_python_file = True

    def run(self):
        self.validate_inputs()
        self.check_directories()
        self.check_coyote_app()
        self.check_casapath()
        self.prepare_python_file()


    def write_submit_sbatch(self, chdir, start, end, username, email, python_file_path, cfcache_dir, nprocs, coyote_app):
        submit_sbatch_path = os.path.join(chdir, "submit_sbatch.sh")
        if os.path.isfile(submit_sbatch_path):
            launcher.logger.warning("submit_sbatch.sh exists and will be overwritten")
            launcher.logger.warning(f"Overwriting {submit_sbatch_path}")
        with open(submit_sbatch_path, "w") as f:
            f.write(
                f"""#!/bin/bash
#SBATCH --export=ALL
#SBATCH --array={self.start}-{self.end}
#SBATCH --chdir={self.chdir}
#SBATCH --time={self.args.time or "1-0:0:0"}
#SBATCH --mem={self.args.mem or "2G"}
#SBATCH --nodes={self.args.nodes or 1}
#SBATCH --ntasks-per-node={self.args.ntasks_per_node or 1}
#SBATCH --output={self.args.output or "logs/slurm_%A_%a.out"}
#SBATCH --error={self.args.error or "logs/slurm_%A_%a.err"}
#SBATCH --job-name={self.args.job_name or "fillcf"}
#SBATCH --account={self.username}
#SBATCH --mail-user={self.email}
#SBATCH --mail-type=FAIL
python3 {self.python_file_path} --cfcache_dir {self.cfcache_dir} --nprocs {self.nprocs} --coyote_app {self.coyote_app}
                """
            )
        launcher.logger.info(f"submit_sbatch.sh written to {submit_sbatch_path}")
        return submit_sbatch_path


    def write_python_file(self):
        python_file_path = os.path.join(self.chdir, "slurm_fillcf.py")
        if os.path.isfile(python_file_path):
            launcher.logger.warning(f"{python_file_path} exists and will be overwritten")
            os.remove(python_file_path)
        launcher.logger.info(f"Writing Python file to {python_file_path}")
        with open(python_file_path, "w") as f:
            f.write(
"""import argparse
import glob
import os
import sys
import subprocess

def check_path(path, path_name):
    if not os.path.exists(path):
        raise FileNotFoundError(f"The {path_name} does not exist at {path}. Please provide a valid path.")

def worker(cfs, start, end, cfcache_dir, coyote_app):
    pid = os.getenv('SLURM_ARRAY_TASK_ID', '0')  # Default to '0' if SLURM_ARRAY_TASK_ID is not set
    log_file = os.path.join('logs', f'{pid}.txt')
    with open(log_file, 'w') as f:
        f.write(f"Starting index: {start}\\n")
        f.write(f"Ending index: {end - 1}\\n")
        f.write(f"Number of CFs: {len(cfs)}\\n")
        for cf in cfs:
            f.write(cf + '\\n')
    command = [
        coyote_app, 'help=noprompt', 'mode=fillcf',
        f'cfcache={cfcache_dir}', f'cflist={",".join(cfs)}'
    ]
    print(f"Running command: {' '.join(command)}")
    subprocess.run(command, check=True)

def distribute_tasks(n_processes, task_id, cfs, cfcache_dir, coyote_app):
    # Distribute tasks among processes
    base, remainder = divmod(len(cfs), n_processes)
    quantities = [base + 1 if i < remainder else base for i in range(n_processes)]

    # Determine start and end indices for this task
    start = sum(quantities[:task_id])
    end = start + quantities[task_id]

    # Process CFs
    worker(cfs[start:end], start, end, cfcache_dir, coyote_app)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process the parallel CF filling.')
    parser.add_argument('--coyote_app', required=True, help='Path to the coyote_app binary')
    parser.add_argument('--cfcache_dir', required=True, help='Path to the cfcache directory')
    parser.add_argument('--nprocs', type=int, required=True, help='Number of processes')

    args = parser.parse_args()

    # Validate paths
    check_path(args.cfcache_dir, 'cfcache_dir')
    check_path(args.coyote_app, 'coyote_app')

    # Get all CFs in the cfcache_dir
    cfs = sorted([os.path.basename(f) for f in glob.glob(os.path.join(args.cfcache_dir, 'CF*.im')) if os.path.isdir(f)])

    # Get the SLURM task ID
    task_id = int(os.getenv('SLURM_ARRAY_TASK_ID', '0'))  # Default to '0' if SLURM_ARRAY_TASK_ID is not set

    # Distribute tasks among processes
    distribute_tasks(args.nprocs, task_id, cfs, cfcache_dir=args.cfcache_dir, coyote_app=args.coyote_app)
""")
        launcher.logger.info(f"Python file written to {python_file_path}")


    def generate_and_submit_job(self):
        submit_sbatch_path = self.write_submit_sbatch(self.chdir, self.start, self.end, self.username, self.email, self.python_file_path, self.cfcache_dir, self.nprocs, self.coyote_app)
        self.write_python_file()

        if self.submit:
            launcher.logger.info("Submitting the job")
            job_id = subprocess.call(["sbatch", submit_sbatch_path])
            launcher.logger.info(f"Job submitted with ID: {job_id}")
        else:
            launcher.logger.info("Job preparation complete. Use the following command to submit the job:")
            launcher.logger.info(f"sbatch {submit_sbatch_path}")

# Submit the job
if __name__ == "__main__":
    launcher = FillCFLauncher()
    launcher.run()
    launcher.generate_and_submit_job()
