#!/bin/bash
# compare_fixes.sh — Run roadrunner with each fix combination and report timing.
#
# Usage:
#   ./scripts/compare_fixes.sh /path/to/roadrunner vis="..." cfcache="..." [other params...]
#
# Pass the full roadrunner invocation (minus imagename=) as arguments.
# imagename= is set per-run automatically.
#
# Fix env vars tested:
#   LIBRA_SKIP_VBROW2CFBMAP  Fix 1: skip redundant makeVBRow2CFBMap
#   LIBRA_SKIP_PA_ROTATION   Fix 2: skip paChangeDetector per VB
#   LIBRA_SKIP_FREQ_INTERP   Fix 3: bypass interpolateFrequencyTogrid

set -euo pipefail

if [ $# -lt 2 ]; then
  echo "Usage: $0 /path/to/roadrunner vis=... cfcache=... [other params...]"
  exit 1
fi

RR="$1"; shift
RR_ARGS=("$@")

if [ ! -x "$RR" ]; then
  echo "ERROR: roadrunner binary not found or not executable: $RR"
  exit 1
fi

WORKDIR=/tmp/fix_compare
mkdir -p "$WORKDIR"

iter_time() {
  grep "^Data iteration time:" "$1" | awk '{print $4}'
}

# run LABEL [KEY=VALUE ...]
# Remaining args after LABEL are env vars passed to roadrunner via env(1).
run() {
  local label="$1"; shift
  local logfile="$WORKDIR/${label}.log"
  echo -n "  [$label] ... "
  env "$@" "$RR" "${RR_ARGS[@]}" imagename="$WORKDIR/${label}" \
    > "$logfile" 2>&1
  local rc=$?
  local t
  t=$(iter_time "$logfile")
  if [ $rc -eq 0 ]; then
    echo "OK  data_iter=${t}s"
  else
    echo "FAILED (see $logfile)"
  fi
  echo "$t"
}

echo ""
echo "========================================================"
echo "  roadrunner: $RR"
echo "  workdir:    $WORKDIR"
echo "========================================================"
echo ""
echo "--- Running ---"

declare -A TIMES

t=$(run "baseline")
TIMES[baseline]=$t

t=$(run "fix1" "LIBRA_SKIP_VBROW2CFBMAP=1")
TIMES[fix1]=$t

t=$(run "fix2" "LIBRA_SKIP_PA_ROTATION=1")
TIMES[fix2]=$t

t=$(run "fix3" "LIBRA_SKIP_FREQ_INTERP=1")
TIMES[fix3]=$t

t=$(run "fix1+fix2" "LIBRA_SKIP_VBROW2CFBMAP=1" "LIBRA_SKIP_PA_ROTATION=1")
TIMES[fix1+fix2]=$t

t=$(run "fix1+fix3" "LIBRA_SKIP_VBROW2CFBMAP=1" "LIBRA_SKIP_FREQ_INTERP=1")
TIMES[fix1+fix3]=$t

t=$(run "fix2+fix3" "LIBRA_SKIP_PA_ROTATION=1" "LIBRA_SKIP_FREQ_INTERP=1")
TIMES[fix2+fix3]=$t

t=$(run "fix1+fix2+fix3" "LIBRA_SKIP_VBROW2CFBMAP=1" "LIBRA_SKIP_PA_ROTATION=1" "LIBRA_SKIP_FREQ_INTERP=1")
TIMES[fix1+fix2+fix3]=$t

echo ""
echo "========================================================"
echo "  RESULTS (data iteration time)"
printf "  %-20s  %12s  %10s\n" "Label" "iter_time(s)" "speedup"
printf "  %-20s  %12s  %10s\n" "--------------------" "------------" "----------"

BASE=${TIMES[baseline]}
for label in baseline fix1 fix2 fix3 fix1+fix2 fix1+fix3 fix2+fix3 fix1+fix2+fix3; do
  t=${TIMES[$label]}
  if [ -n "$t" ] && [ -n "$BASE" ] && awk "BEGIN{exit ($BASE==0)}" 2>/dev/null; then
    speedup=$(awk "BEGIN{printf \"%.2fx\", $BASE/$t}")
  else
    speedup="n/a"
  fi
  printf "  %-20s  %12s  %10s\n" "$label" "${t:-FAILED}" "$speedup"
done
echo "========================================================"
echo ""
