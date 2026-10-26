#!/usr/bin/env python3
"""Run Fast Downward with one search and heuristic on every benchmark instance.

Each run gets the assignment's limits (1 minute of CPU time, 2 GB of memory)
and its log is written to OUTPUT_DIR/<instance>-<search>-<heuristic>.out.

Examples (from the lab directory, after fast-downward-plugin/setup.sh):
    python3 scripts/run_experiments.py astar blind -o results/logs/2b
    python3 scripts/run_experiments.py astar planopt_relaxed_task_graph -o results/logs/2b
    python3 scripts/run_experiments.py eager_greedy planopt_add -o results/logs/2d
    python3 scripts/run_experiments.py eager_greedy planopt_ff -o results/logs/2f
"""

import resource
import subprocess
from argparse import ArgumentParser
from pathlib import Path

LAB_DIR = Path(__file__).resolve().parent.parent
DEFAULT_FD = LAB_DIR.parent / "fast-downward" / "fast-downward.py"
DEFAULT_BENCHMARKS = LAB_DIR / "benchmarks" / "castle"

TIME_LIMIT_S = 60
MEMORY_LIMIT_BYTES = 2_000_000 * 1024  # ulimit -v 2000000 (KiB)


def apply_limits():
    """Same effect as `ulimit -t 60; ulimit -v 2000000` for the child process."""
    resource.setrlimit(resource.RLIMIT_CPU, (TIME_LIMIT_S, TIME_LIMIT_S))
    resource.setrlimit(resource.RLIMIT_AS, (MEMORY_LIMIT_BYTES, MEMORY_LIMIT_BYTES))


def search_option(search: str, heuristic: str) -> str:
    # A* takes one evaluator; the greedy searches take a list of them.
    if search == "astar":
        return f"astar({heuristic}())"
    return f"{search}([{heuristic}()])"


def main():
    parser = ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("search", help="search algorithm, e.g. astar or eager_greedy")
    parser.add_argument("heuristic", help="heuristic, e.g. blind, add, planopt_add")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("results/logs"))
    parser.add_argument("--benchmarks", type=Path, default=DEFAULT_BENCHMARKS,
                        help="folder with domain.pddl and the problem files")
    parser.add_argument("--fast-downward", type=Path, default=DEFAULT_FD,
                        help="path to fast-downward.py")
    args = parser.parse_args()

    args.output_dir.mkdir(parents=True, exist_ok=True)
    domain = args.benchmarks / "domain.pddl"
    problems = sorted(p for p in args.benchmarks.glob("*.pddl") if p.name != "domain.pddl")

    for problem in problems:
        log = args.output_dir / f"{problem.stem}-{args.search}-{args.heuristic}.out"
        command = [str(args.fast_downward), str(domain), str(problem),
                   "--search", search_option(args.search, args.heuristic)]
        with open(log, "w") as out:
            # A failed search (limit reached, no plan) is recorded in the log.
            subprocess.run(command, stdout=out, stderr=subprocess.STDOUT,
                           preexec_fn=apply_limits, check=False)
        print(f"{problem.name}: {log}")


if __name__ == "__main__":
    main()
