#!/usr/bin/env bash
# Exercise 2(c): compare the built-in PDB heuristic with planopt_pdb on the
# castle instances, for patterns with at most 1000 and 100000 abstract states.
#
# For each instance and size limit, the pattern chosen by the built-in greedy
# pattern generator is extracted and given to planopt_pdb, so both
# implementations use the same pattern. Output: results/pdb_runs.txt format.
#
# Usage (from the lab directory): scripts/run_pdb_experiment.sh > results/pdb_runs.txt

set -u
FD="${FD:-$(dirname "$0")/../../fast-downward/fast-downward.py}"
BENCHMARKS="$(dirname "$0")/../benchmarks/castle"
FILTER="Expanded until last jump\|Total time\|Search time"

run_limited() {  # 1 minute of CPU time and 2 GB of memory per run
    (ulimit -t 60; ulimit -v 2000000; "$FD" "$@" | grep "$FILTER")
}

for k in 1000 100000; do
    for problem in $(ls "$BENCHMARKS"/castle-8-5-*-cards.pddl | sort -V); do
        i="$(basename "$problem" | sed -E 's/castle-8-5-([0-9]+)-cards.pddl/\1/')"
        echo "Instance $i with k=$k"

        pattern=$("$FD" "$problem" --search "astar(pdb(pattern=greedy($k)), bound=0)" |
                  grep -oP 'greedy pattern generator pattern: \K.*')
        if [ -z "$pattern" ]; then
            echo "Could not extract the pattern; skipping."
            echo
            continue
        fi
        echo "Extracted PATTERN: $pattern"

        echo "Built-in implementation"
        run_limited "$problem" --search "astar(pdb(pattern=greedy($k)))"
        echo "PlanOpt implementation"
        run_limited "$problem" --search "astar(planopt_pdb(pattern=$pattern))"
        echo
    done
done
