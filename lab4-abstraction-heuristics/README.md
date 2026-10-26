# Lab 4: Abstraction Heuristics

Pattern database heuristics, implemented as Fast Downward heuristics on tasks in transition
normal form (TNF):

| Exercise | Implementation | Fast Downward heuristic |
|---|---|---|
| 2(a) | Projection of a TNF task onto a pattern: initial state, goal and operators | – |
| 2(b) | Pattern database: goal distances of every abstract state | `planopt_pdb(pattern=P)` |
| 2(d) | Canonical PDB heuristic: compatibility graph and maximum over additive sets | `planopt_cpdbs(patterns=C)` |

The code is in [`fast-downward-plugin/planopt_heuristics`](../fast-downward-plugin/planopt_heuristics):
`projection.cc`, `pdb.cc` and `canonical_pdbs.cc`.

## How it works

- **Projection.** Keeps only the pattern's variables. Operators lose their entries on other
  variables, and operators left without entries are dropped. Abstract states are ranked into
  `0 .. N-1` by perfect hashing.
- **Pattern database.** Runs Dijkstra's algorithm backwards from the abstract goal over ranked
  states. In TNF, an operator leads into a state exactly when the state matches all of the
  operator's effect values. The predecessor then takes the precondition values instead.
- **Canonical PDBs.**
  - Two patterns are additive when no operator changes variables in both, and these pairs are
    the edges of the compatibility graph.
  - The heuristic is the maximum, over all maximal cliques of that graph, of the sum of the
    clique's PDB values.

## Running the experiments

From this folder, after `../fast-downward-plugin/setup.sh`:

```bash
scripts/run_pdb_experiment.sh  > results/pdb_runs.txt    # exercise 2(c)
scripts/run_cpdb_experiment.sh > results/cpdb_runs.txt   # exercise 2(e)
python3 scripts/analyze_results.py results/pdb_runs.txt
```

Each run has a limit of 1 minute and 2 GB. The course test for exercise 2(a) runs with
`../fast-downward/fast-downward.py --test-projections`.

## Results

### 2(c): single PDB, built-in vs. mine (castle, 16 instances)

Both implementations were given the same pattern: the one chosen by Fast Downward's greedy
pattern generator for each instance. Preprocessing time is total time minus search time.

| Max. abstract states | Implementation | Mean expanded (until last f-layer) | Mean search time (s) | Mean preprocessing time (s) |
|---|---|---:|---:|---:|
| 1,000 | built-in `pdb` | 1,608,245 | 5.650 | 0.011 |
| 1,000 | `planopt_pdb` | 1,608,245 | 5.736 | 0.017 |
| 100,000 | built-in `pdb` | 486,187 | 1.588 | 0.031 |
| 100,000 | `planopt_pdb` | 486,187 | 1.626 | 1.377 |

- **Expansions are identical** on all 32 runs, so both implementations compute the same
  heuristic.
- **Search time is within a few percent.**
- **Preprocessing is slower with larger patterns.** With 100,000 abstract states, my PDB
  construction takes about 1.4 s against 0.03 s. For every abstract state it scans all operators
  and unranks or ranks full state vectors. Fast Downward instead precomputes, for each operator,
  the hash offset it applies.

### 2(e): canonical PDBs (nomystery, pattern collections from the assignment)

| Instance | Expanded, built-in `cpdbs` | Expanded, `planopt_cpdbs` | Expanded, single PDB (≤ 1,000 states) | Search time, `cpdbs` / mine (s) |
|---|---:|---:|---:|---:|
| p01 | 43 | 43 | 43 | 0.000 / 0.001 |
| p02 | 2,406 | 2,406 | 10,405 | 0.006 / 0.007 |
| p03 | 2,231 | 2,231 | 31,011 | 0.005 / 0.006 |
| p04 | 31,350 | 31,350 | 1,379,615 | 0.086 / 0.089 |
| p05 | 391,698 | 391,698 | – | 1.42 / 1.59 |
| p06 | 3,323,144 | 3,323,144 | – | 13.5 / 15.5 |

- **The canonical heuristics match** the built-in implementation exactly in expansions, and run
  at most 15% slower.
- **Canonical PDBs beat a single PDB of the same size.** Combining several small additive
  patterns expands up to 44 times fewer states than one PDB on these instances. The single PDB
  failed to solve p05 and p06 within the limits.
- **p07 was not solved** by any configuration.

## Theory

[`theory/transition-systems-and-abstractions.pdf`](theory/transition-systems-and-abstractions.pdf)
answers exercise 1:
- graphs that are isomorphic or graph-equivalent to a given transition system;
- graphs that are strict or non-strict homomorphisms of it;
- an induced abstraction, one of its coarsenings and one of its refinements.

## Code changes after the experiments

The compatibility graph had two bugs, fixed during the cleanup:
- it checked operator `i` against pattern `i` instead of every operator against every pattern;
- it stored each edge in one direction only, while Fast Downward's clique algorithm expects
  both.

On these nomystery collections, the expansions still match the built-in heuristic, so the bugs
did not affect the results above. In general, they could join patterns that are not additive,
which makes the heuristic inadmissible, or miss additive pairs, which makes it weaker.
