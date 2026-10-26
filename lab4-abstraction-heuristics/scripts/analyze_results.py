#!/usr/bin/env python3
"""Summarize the output of run_pdb_experiment.sh or run_cpdb_experiment.sh.

For each implementation, prints the mean and standard deviation over all runs
of: states expanded until the last f-layer, search time and total time, plus
the number of successful runs.

A run that failed (time or memory limit) is imputed with the largest value
any implementation obtained on that instance, so that every implementation is
averaged over the same set of runs.

Usage: python3 scripts/analyze_results.py results/pdb_runs.txt
"""

import re
import sys
from collections import defaultdict

import numpy as np

METRICS = {
    "expanded": re.compile(r"Expanded until last jump: (\d+) state"),
    "search_time": re.compile(r"Search time: ([\d.]+)s"),
    "total_time": re.compile(r"Total time: ([\d.]+)s"),
}


def parse_results(path):
    """Return {implementation: {metric: {run_label: value}}}."""
    results = defaultdict(lambda: defaultdict(dict))
    runs = []
    run = implementation = None
    with open(path) as f:
        for line in f:
            if line.startswith("Instance "):
                instance, k = line.split()[1], line.split("=")[-1].strip()
                run = f"{instance}-{k}"
                runs.append(run)
                implementation = None
            elif " implementation" in line:
                implementation = line.split(" implementation")[0].strip()
                results[implementation]  # register it even if all its runs failed
            elif run and implementation:
                for metric, pattern in METRICS.items():
                    m = pattern.search(line)
                    if m:
                        value = float(m.group(1))
                        results[implementation][metric][run] = int(value) if metric == "expanded" else value
    return results, runs


def impute_failures(results, runs):
    successful = {impl: len(metrics["search_time"]) for impl, metrics in results.items()}
    for run in runs:
        for metric in METRICS:
            values = [m[metric][run] for m in results.values() if run in m[metric]]
            worst = max(values, default=-1)
            for metrics in results.values():
                metrics[metric].setdefault(run, worst)
    return successful


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    results, runs = parse_results(sys.argv[1])
    successful = impute_failures(results, runs)
    for implementation, metrics in results.items():
        print(f"Implementation: {implementation}")
        for metric in METRICS:
            values = np.array(list(metrics[metric].values()))
            print(f"  {metric}: mean = {values.mean():.4f}, std = {values.std():.4f}")
        print(f"  Successful runs: {successful[implementation]}\n")


if __name__ == "__main__":
    main()
