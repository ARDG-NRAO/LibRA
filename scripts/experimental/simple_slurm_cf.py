import datetime
import os
import subprocess
import argparse


def make_slurm_object(args):
'''#create a slurm object to fill the following
# export all environment variables to job from args that are slurm related.
# array range for the job
# working directory
# time for the job
# memory per node
# number of nodes
# number of tasks per node
# output file name
# error file name
# job name
# mail type
# mail user'''
# Check if simple_slurm is installed else use default slurm
try:
    from simple_slurm import Slurm
    slurm_type = 'simple_slurm'
except ImportError:
    print("simple_slurm is not installed. Please install it using pip install simple_slurm to \
          enable simple_slurm functionality.")
    print("Using default slurm, consequently slurm_job.sh will be created and submitted to slurm.\
          If slurm_job.sh already exists it will be overwritten.")
    if os.path.exists('slurm_job.sh'):
        os.remove('slurm_job.sh')
    slurm_type = 'default'


def make_slurm_object(args, slurm_type='default', slurm_script='slurm_job.sh'):
    if slurm_type == 'simple_slurm':
        slurm_options = {
        'export': args.export,
        'array': args.array,
        'chdir': args.chdir,
        'time': args.time,
        'mem': args.mem,
        'nodes': args.nodes,
        'ntasks-per-node': args.ntasks_per_node,
        'output': args.output,
        'error': args.error,
        'job-name': args.job_name,
        'mail-type': args.mail_type,
        'mail-user': args.mail_user
        }
        slurm = Slurm(slurm_options)
        return slurm
    
    elif slurm_type == 'default':
        slurm_script = f"""#!/bin/bash
        #SBATCH --export={args.export}
        #SBATCH --array=0-{args.array}
        #SBATCH --chdir={args.chdir}
        #SBATCH --time={args.time}
        #SBATCH --mem={args.mem}
        #SBATCH --nodes={args.nodes}
        #SBATCH --ntasks-per-node={args.ntasks_per_node}
        #SBATCH -o {args.output}
        #SBATCH -e {args.error}
        #SBATCH --job-name={args.job_name}
        #SBATCH --mail-type={args.mail_type}
        #SBATCH --mail-user={args.mail_user}

        # Your job commands go here
        """

        with open('slurm_job.sh', 'w') as f:
            f.write(slurm_script)

# Create the parser

def parse_arguments():
    parser = argparse.ArgumentParser(description='Process some integers.')

    # Add the arguments
    parser.add_argument('--export', type=str, default='ALL', help='Export all environment variables to job')
    parser.add_argument('--array', type=str, default='0-400', help='Array range for the job')
    parser.add_argument('--chdir', type=str, help='Working directory')
    parser.add_argument('--time', type=str, default='1-0:0:0', help='Time for the job')
    parser.add_argument('--mem', type=str, default='2G', help='Memory per node')
    parser.add_argument('--nodes', type=int, default=1, help='Number of nodes')
    parser.add_argument('--ntasks-per-node', type=int, default=1, help='Number of tasks per node')
    parser.add_argument('-o', '--output', type=str, default='logs/slurm_%A_%a.out', help='Output file name')
    parser.add_argument('-e', '--error', type=str, default='logs/slurm_%A_%a.err', help='Error file name')
    parser.add_argument('--job-name', type=str, default='job', help='Job name')
    parser.add_argument('--mail-type', type=str, default='END', help='Mail type')
    parser.add_argument('--mail-user', type=str, default='', help='Mail user')
    parser.add_argument('--vis', type=str, help='Measurement Set')
    parser.add_argument('--cfcache', type=str, help='CFCache directory')
    parser.add_argument('--mode', type=str, default='fillcf', help='Mode')
    parser.add_argument('--coyote_app', type=str, default='default', help='Coyote app')
    # parser.add_argument('--telescope', type=str, default='EVLA', help='Telescope name')
    # parser.add_argument('--imsize', type=int, default=2048, help='Image size')
    # parser.add_argument('--cell', type=str, default='0.1', help='Cell size')
    # parser.add_argument('--stokes', type=str, default='I', help='Stokes parameters')
    # parser.add_argument('--reffreq', type=str, default='3.0e9', help='Reference frequency')
    # parser.add_argument('--phasecenter', type=str, help='Phase center')
    # parser.add_argument('--wplanes', type=int, default=1, help='Number of w-projection planes')
    # parser.add_argument('--nameprefix', type=str, default='', help='Name prefix')
    # parser.add_argument('--wbawp', type=int, default=1, help='Wideband A-projection')
    # parser.add_argument('--aterm', type=int, default=1, help='A-term correction')
    # parser.add_argument('--psterm', type=int, default=1, help='Polarization leakage term')
    # parser.add_argument('--conjbeams', type=int, default=1, help='Conjugate beams')
    # parser.add_argument('--muellertype', type=str, default='diagonal', help='Mueller matrix type')
    # parser.add_argument('--dpa', type=int, default=360, help='Degrees per aperture')
    # parser.add_argument('--field', type=str, help='Field name')
    # parser.add_argument('--spw', type=str, help='Spectral window')
    # parser.add_argument('--buffersize', type=int, default=3076, help='Buffer size')
    # parser.add_argument('--oversampling', type=int, default=20, help='Oversampling factor')


    # check for the existence of direcotries given in args.chdir args.cfcache args.vis
    directory_list = [args.chdir, args.cfcache, args.vis]
    for directory in directory_list:
        if directory is not None:
            if not os.path.exists(directory):
                raise argparse.ArgumentTypeError(f"Directory {directory} does not exist")


    return parser.parse_args()

if __name__ == "__main__":

    args=parse_arguments()

    