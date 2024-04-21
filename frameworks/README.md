## Description

Framework code for iterative image reconstruction using LibRA [apps](/README.md#currently-available-apps).

# To Do List

- [ ] Make a Python version of libra_htclean.sh.  This will allow some error handling as well.
- [ ] Script to deploy libra_htclean in parallel via Slurm.  Similar to the one that deploys it on the PATh network using HTCondor scheduler.
- [x] review structure of framework/htclean directory to eliminate duplicates - fmadsen
- [x] integrate new scatter mode from ```/lustre/aoc/sciops/fmadsen/tickets/scg-136/imaging_tests/bin/``` (HTCSynthesisImager.py and htclean.py) - fmadsen
- [ ] merge different templates into a general implementation - fmadsen
- [ ] ~~check if ```roadrunner``` can compute cfcache if it's not provided - sbhatnag~~
- [x] ~~check mode = predict on ```roadrunner``` - sbhatnag~~
