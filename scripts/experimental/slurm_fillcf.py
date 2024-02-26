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
    subprocess.run([f'{coyote_app}', 'help=noprompt', 'mode=fillcf', f'cfcache={cfcache_dir}',f'cflist={", ".join(cfs[start:end])}'])

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
    cfs = sorted([f for f in glob.glob(os.path.join(args.cfcache_dir, 'CFS*')) if os.path.isdir(f)])

    # Get the task_id
    task_id = int(os.getenv('SLURM_ARRAY_TASK_ID', 'default'))

    # Distribute tasks among processes
    distribute_tasks(args.nprocs, task_id, cfs, cfcache_dir=args.cfcache_dir, coyote_app=args.coyote_app)