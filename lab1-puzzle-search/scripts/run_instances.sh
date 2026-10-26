#!/usr/bin/env bash
# Runs ./main on every puzzle of an instance file (one puzzle per line), each
# in its own process with a time and memory limit, and prints one CSV line per
# puzzle:
#
#   expanded,length,time,average_h,initial_h
#
# Puzzles that fail or hit a limit get "-,-,-,-,-", as the assignment's report
# required. Progress and a summary go to stderr.
#
# Usage:   scripts/run_instances.sh <algorithm> <instance_file> > results.csv
# Example: scripts/run_instances.sh -astar instances/15puzzle.txt > astar15.csv
#
# Limits (defaults follow the assignment's 15-puzzle limits):
#   TIME_LIMIT=30 MEMORY_LIMIT_GB=8 scripts/run_instances.sh ...

set -u

TIME_LIMIT="${TIME_LIMIT:-30}"
MEMORY_LIMIT_GB="${MEMORY_LIMIT_GB:-8}"
MAIN="$(dirname "$0")/../main"

if [ $# -ne 2 ]; then
    echo "Usage: $0 <-bfs|-idfs|-astar|-idastar|-gbfs> <instance_file>" >&2
    exit 1
fi
algorithm="$1"
instances="$2"
case "$algorithm" in
    -bfs|-idfs|-astar|-idastar|-gbfs) ;;
    *) echo "Error: unknown algorithm '$algorithm'" >&2; exit 1 ;;
esac
[ -f "$instances" ] || { echo "Error: '$instances' not found" >&2; exit 1; }
[ -x "$MAIN" ] || { echo "Error: $MAIN not found; run make first" >&2; exit 1; }

echo "$algorithm on $instances (limits: ${TIME_LIMIT}s, ${MEMORY_LIMIT_GB} GB)" >&2
total=0
solved=0
while IFS= read -r line || [ -n "$line" ]; do
    line="$(echo "$line" | xargs)"
    [ -z "$line" ] || [[ "$line" == \#* ]] && continue
    total=$((total + 1))
    # shellcheck disable=SC2086  # the tiles must be split into separate arguments
    if result=$(ulimit -v $((MEMORY_LIMIT_GB * 1024 * 1024)) &&
                timeout "$TIME_LIMIT" "$MAIN" "$algorithm" $line 2>/dev/null); then
        echo "$result"
        solved=$((solved + 1))
        echo "  [$total] $result" >&2
    else
        echo "-,-,-,-,-"
        echo "  [$total] failed or hit a limit" >&2
    fi
done < "$instances"
echo "Solved $solved of $total puzzles." >&2
