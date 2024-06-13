#!/bin/bash

# show us where we have failed you
set -o nounset -o errexit 

# report issues
usage() {
  echo "Usage: ./start.sh [ roadrunner | hummbee | coyote | acme ] <... arguments ...>"
  echo "  --help: show this message"
  echo "  --override: start a bash shell"
  echo "  --profiler [ncu | nsys]: start the profiler"
  exit 1
}

# check for help flag
if [[ "$1" == "--help" ]]; then
  usage
fi

# check for override flag
if [[ "$1" == "--override" ]]; then
  exec /bin/bash
fi

# check for the --profiler flag and set the profiler as ncu or nsys
if [[ "$1" == "--profiler" ]]; then
  if [[ "$2" == "ncu" ]]; then
    export PROFILER="ncu"
  elif
    [[ "$2" == "nsys" ]]; then
    export PROFILER="nsys"
  else
    echo "Error: Invalid profiler. Please use either ncu or nsys"
    usage
  fi


# not enough arguments? we have failed
if [ "$#" -lt 1 ]; then
    usage
fi

# what app are we trying to use?
APP=$1; shift

# does it exist?
if [ ! -x /data/libra/apps/install/$APP ]; then
    "Error: unable to locate $APP in /data/libra/apps/install"
    usage
fi

# it must exist, so let's proceed
echo "passing command line arguments to $APP: $@"

/data/libra/apps/install/$APP $@ 