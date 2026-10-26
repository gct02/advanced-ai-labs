#!/usr/bin/env python3
"""Summarize Fast Downward logs as CSV rows.

Extracts the problem, search, heuristic, initial heuristic value, plan cost,
expanded states, times and peak memory from each log, and appends one row per
log to the output CSV (writing the header if the file is new).

Usage:
    python3 scripts/parse_fd_log.py results/logs/2d/*.out -o results/summary_2d.csv
"""

import csv
import re
from argparse import ArgumentParser
from pathlib import Path

FIELDS = [
    "problem", "algorithm", "heuristic", "solution_found",
    "initial_h", "plan_cost", "expanded_states",
    "search_time_s", "total_time_s", "search_peak_memory_kb",
]


def first_match(pattern, text, cast):
    m = re.search(pattern, text)
    return cast(m.group(1)) if m else None


def parse_log(log: str) -> dict:
    data = dict.fromkeys(FIELDS)

    # e.g. ... translate.py castle/domain.pddl castle/castle-2-2-8-cards.pddl ...
    m = re.search(r"translate\.py\s+(?:\S*/)?\S+?\.pddl\s+(?:\S*/)?(\S+?)\.pddl", log)
    if m:
        data["problem"] = m.group(1)

    # e.g. --search 'eager_greedy([add()])'
    m = re.search(r"--search\s+'(\w+)\((.*)\)'", log)
    if m:
        data["algorithm"] = m.group(1)
        data["heuristic"] = m.group(2).replace("()", "").strip("[]")

    data["initial_h"] = first_match(r"Initial heuristic value for .+?: (\d+)", log, int)
    data["solution_found"] = "Solution found." in log
    if data["solution_found"]:
        data["plan_cost"] = first_match(r"Plan cost: (\d+)", log, int)
        data["expanded_states"] = first_match(r"Expanded (\d+) state\(s\)\.", log, int)
        data["total_time_s"] = first_match(r"Total time: ([\d.]+)s", log, float)
    data["search_time_s"] = first_match(r"Search time: ([\d.]+)s", log, float)
    data["search_peak_memory_kb"] = first_match(r"Peak memory: (\d+) KB", log, int)
    return data


def main():
    parser = ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("logs", nargs="+", type=Path, help="Fast Downward log files")
    parser.add_argument("-o", "--output", type=Path, default=Path("summary.csv"),
                        help="CSV file to append to (default: summary.csv)")
    args = parser.parse_args()

    new_file = not args.output.exists()
    with open(args.output, "a", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=FIELDS)
        if new_file:
            writer.writeheader()
        for log in args.logs:
            writer.writerow(parse_log(log.read_text()))
    print(f"Appended {len(args.logs)} row(s) to {args.output}")


if __name__ == "__main__":
    main()
