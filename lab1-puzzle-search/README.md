# Lab 1: Heuristic Search for the Sliding-Tile Puzzle

Five search algorithms for the 8-puzzle and the 15-puzzle, in C++ with no external libraries:

- breadth-first graph search (`-bfs`)
- iterative deepening depth-first search (`-idfs`)
- greedy best-first search (`-gbfs`)
- A\* (`-astar`)
- IDA\* (`-idastar`)

The informed searches use the Manhattan distance heuristic. The goal is the ordered grid with
the blank in the top-left corner.

## Building and running

```bash
make                                   # builds ./main
./main -astar 0 6 1 7 4 2 3 8 5        # one puzzle
./main -bfs 5 0 2 6 4 8 1 7 3, 2 4 7 0 3 6 8 1 5   # several puzzles, separated by commas
```

A puzzle lists the tile in each cell, row by row, with 0 for the blank. For each puzzle,
`main` prints one line:

```
expanded nodes, solution length, time (s), average h, initial h
```

The average h is the mean of every heuristic value computed during the search; it is 0 for the
blind searches.

`scripts/run_instances.sh` solves every puzzle in an instance file, each in its own process with
a time and memory limit (30 s and 8 GB by default), and prints one CSV line per puzzle. Puzzles
that hit a limit get `-,-,-,-,-`.

```bash
scripts/run_instances.sh -astar instances/15puzzle.txt > astar-15puzzle.csv
TIME_LIMIT=60 MEMORY_LIMIT_GB=4 scripts/run_instances.sh -idfs instances/8puzzle.txt
```

`make test` checks the program against the example outputs from the assignment: expanded nodes,
solution length and initial h must match exactly.

## Implementation

- **State representation.** A state is a 64-bit integer with 4 bits per cell. Moving the blank
  is a shift and two masks, and states are hashed and compared as plain integers.
- **Successors** are generated in the required order: the blank moves up, left, right, down. The
  parent of the node being expanded is never generated.
- **Tie-breaking**, as required by the assignment:
  - A\*: lowest f, then lowest h, then the most recently generated node.
  - GBFS: lowest h, then highest g, then the most recently generated node.

  A node's index in the node pool is its generation order, which gives the LIFO rule.
- **Duplicate detection.**
  - BFS and GBFS discard states already generated.
  - A\* discards a popped node whose state is closed. This is lazy deletion with re-opening
    avoided, which is safe because the Manhattan distance is consistent.
- **Memory.**
  - BFS, GBFS and A\* store nodes in a pool (`std::deque`) and refer to them by 32-bit index.
  - Iterative deepening and IDA\* are recursive and only keep the current path, so they use
    memory linear in the solution depth.
- **Input validation.** Puzzles that are not permutations, or that are unsolvable (the
  permutation parity must match the parity of the blank's distance to its goal cell), are
  rejected before searching.

## Results on the 100 8-puzzle instances

Averages over [`instances/8puzzle.txt`](instances/8puzzle.txt):

| Algorithm | Expanded nodes | Solution length | Average h | Initial h |
|---|---:|---:|---:|---:|
| BFS | 81,459.5 | 22.16 | – | 13.88 |
| Iterative deepening | 2,578,290.6 | 22.16 | – | 13.88 |
| A\* | 895.0 | 22.16 | 10.03 | 13.88 |
| IDA\* | 2,373.0 | 22.16 | 10.43 | 13.88 |
| Greedy best-first | 370.6 | 137.16 | 6.91 | 13.88 |

The four optimal algorithms find solutions of the same length. Greedy best-first search expands
far fewer nodes, but its solutions are about six times longer.

The output matches the assignment's examples exactly in expanded nodes, solution length and
initial h. The one difference is IDA\*'s average h, which is slightly off: for example, 7.12766
here against 7.13781 in the example. The two implementations count heuristic evaluations
slightly differently.
