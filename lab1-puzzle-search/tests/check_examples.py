#!/usr/bin/env python3
"""Check ./main against the example outputs given in the assignment.

The number of expanded nodes, the solution length and the initial heuristic
value must match exactly. Average heuristic values and times are printed for
comparison but not checked (times depend on the machine).

Usage (from the lab directory, after `make`):
    python3 tests/check_examples.py
"""

import subprocess
import sys
from pathlib import Path

MAIN = Path(__file__).resolve().parent.parent / "main"

EIGHT = "0 6 1 7 4 2 3 8 5, 5 0 2 6 4 8 1 7 3, 2 4 7 0 3 6 8 1 5"
FIFTEEN = ("7 11 8 3 14 0 6 15 1 4 13 9 5 12 2 10, "
           "12 9 0 6 8 3 5 14 2 4 11 7 10 1 15 13, "
           "13 0 9 12 11 6 3 5 15 8 1 10 4 14 2 7")

# (algorithm, puzzles, expected lines: expanded, length, time, average h, initial h)
EXAMPLES = [
    ("astar", EIGHT, ["91,16,0.0000479221,6.72561,10",
                      "612,21,0.000200987,9.81136,11",
                      "511,23,0.000167847,9.97655,15"]),
    ("bfs", EIGHT, ["5209,16,0.00154996,0,10",
                    "42704,21,0.011677,0,11",
                    "86288,23,0.022454,0,15"]),
    ("idastar", EIGHT, ["165,16,0.0000319481,7.13781,10",
                        "439,21,0.0000369549,10.75,11",
                        "724,23,0.0000538826,11.7241,15"]),
    ("idfs", EIGHT, ["20305,16,0.000360012,0,10",
                     "336804,21,0.00481009,0,11",
                     "1183127,23,0.017087,0,15"]),
    ("gbfs", EIGHT, ["16,16,0.0000259876,5.5,10",
                     "96,27,0.0000660419,8.05422,11",
                     "90,57,0.0000729561,6.80982,15"]),
    ("astar", FIFTEEN, ["154092,46,0.0424099,22.9871,36",
                        "2731989,50,0.799799,24.6075,32",
                        "744209,53,0.206705,28.0517,39"]),
]


def main() -> int:
    if not MAIN.exists():
        print(f"{MAIN} not found; run `make` first", file=sys.stderr)
        return 1

    failures = 0
    header = f"{'algorithm':<9} {'expanded':>9} {'length':>6} {'h0':>3} {'avg h (got / expected)':>26} {'time (s)':>10}"
    print(header)
    print("-" * len(header))
    for algorithm, puzzles, expected in EXAMPLES:
        out = subprocess.run([str(MAIN), f"-{algorithm}", *puzzles.split()],
                             capture_output=True, text=True, timeout=300, check=True)
        for got_line, exp_line in zip(out.stdout.split(), expected):
            got, exp = got_line.split(","), exp_line.split(",")
            ok = (got[0], got[1], got[4]) == (exp[0], exp[1], exp[4])
            failures += not ok
            avg = f"{float(got[3]):.5g} / {float(exp[3]):.5g}"
            print(f"{algorithm:<9} {got[0]:>9} {got[1]:>6} {got[4]:>3} {avg:>26} "
                  f"{float(got[2]):>10.4g}  {'ok' if ok else 'MISMATCH (expected ' + exp_line + ')'}")

    print(f"\n{'All examples match.' if failures == 0 else f'{failures} mismatch(es).'}")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
