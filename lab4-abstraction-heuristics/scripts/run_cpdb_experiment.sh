#!/usr/bin/env bash
# Exercise 2(e): compare the built-in canonical PDB heuristic (cpdbs), a single
# built-in PDB with at most 1000 abstract states, and planopt_cpdbs on the
# nomystery instances. Both canonical heuristics get the same pattern
# collection (the collections provided with the assignment, up to 1000
# abstract states per pattern). Output: results/cpdb_runs.txt format.
#
# Usage (from the lab directory): scripts/run_cpdb_experiment.sh > results/cpdb_runs.txt

set -u
FD="${FD:-$(dirname "$0")/../../fast-downward/fast-downward.py}"
BENCHMARKS="$(dirname "$0")/../benchmarks/nomystery-opt11-strips"
FILTER="Expanded until last jump\|Total time\|Search time"
K=1000

PATTERNS=(
    '[[0, 2, 3, 4]]'
    '[[3, 4, 5], [2]]'
    '[[4, 5, 6], [2], [3]]'
    '[[5, 6, 7], [2], [3], [4]]'
    '[[6, 7, 8], [2], [3], [4], [5]]'
    '[[7, 8, 9], [2], [3], [4], [5], [6]]'
    '[[9, 10], [2], [3], [4], [5], [6], [7], [8]]'
)

run_limited() {  # 1 minute of CPU time and 2 GB of memory per run
    (ulimit -t 60; ulimit -v 2000000; "$FD" "$@" | grep "$FILTER")
}

for i in $(seq 1 ${#PATTERNS[@]}); do
    problem="$BENCHMARKS/p0$i.pddl"
    patterns="${PATTERNS[i-1]}"
    echo "Instance $i with k=$K"
    echo "$patterns"
    echo "Built-in implementation"
    run_limited "$problem" --search "astar(cpdbs(manual_patterns($patterns)))"
    echo "Built-in PDB implementation"
    run_limited "$problem" --search "astar(pdb(pattern=greedy($K)))"
    echo "PlanOpt implementation"
    run_limited "$problem" --search "astar(planopt_cpdbs(patterns=$patterns))"
    echo
done
