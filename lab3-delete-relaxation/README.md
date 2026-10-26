# Lab 3: Delete Relaxation Heuristics

Heuristics based on the delete relaxation, implemented as Fast Downward heuristics on top of an
AND/OR graph:

| Exercise | Implementation | Fast Downward heuristic |
|---|---|---|
| 2(a) | Most conservative valuation of an AND/OR graph (generalized Dijkstra) | – |
| 2(b) | Relaxed task graph of a STRIPS task, used to prune relaxed-unsolvable states | `planopt_relaxed_task_graph()` |
| 2(c) | Weighted most conservative valuation, which computes h<sup>add</sup> | `planopt_add()` |
| 2(e) | Best achievers and the relaxed plan they induce, which computes h<sup>FF</sup> | `planopt_ff()` |

The code is in [`fast-downward-plugin/planopt_heuristics`](../fast-downward-plugin/planopt_heuristics):
`and_or_graph.cc` and `relaxed_task_graph.cc`.

## How it works

- **Relaxed task graph.**
  - Every proposition is an OR node.
  - Every operator is an AND node whose successors are its preconditions, with the operator cost
    as its direct cost. Each of its effects is an OR node with an edge to it.
  - The initial-state propositions point to an initial AND node without successors, which is
    always true. The goal is an AND node over the goal propositions.
- **Generalized Dijkstra.** A node is forced true when all its successors are (AND node) or one
  of them is (OR node). The weighted version expands nodes in order of cost:
  - an AND node costs its direct cost plus the sum of its successors' costs;
  - an OR node costs the cheapest of its successors.

  The goal's cost is h<sup>add</sup>. Replacing the sum with a maximum gives h<sup>max</sup>.
- **h<sup>FF</sup>.** Each OR node remembers the successor that gave its cost (its best
  achiever). Starting from the goal, the search collects every successor of an AND node and the
  best achiever of an OR node. h<sup>FF</sup> is the total cost of the collected operators, each
  counted once.

## Running the experiments

The experiments use the [castle](benchmarks/castle) instances, with the limits set by the
assignment: 1 minute of CPU time and 2 GB of memory per run. From this folder, after
`../fast-downward-plugin/setup.sh`:

```bash
python3 scripts/run_experiments.py astar blind -o results/logs/2b
python3 scripts/run_experiments.py astar planopt_relaxed_task_graph -o results/logs/2b
python3 scripts/run_experiments.py eager_greedy add -o results/logs/2d
python3 scripts/run_experiments.py eager_greedy planopt_add -o results/logs/2d
python3 scripts/run_experiments.py eager_greedy ff -o results/logs/2f
python3 scripts/run_experiments.py eager_greedy planopt_ff -o results/logs/2f
python3 scripts/parse_fd_log.py results/logs/2d/*.out -o results/summary_2d.csv
```

The course test for exercise 2(a) runs with `../fast-downward/fast-downward.py --test-and-or-graphs`.

## Results

The raw logs are in [`results/logs`](results/logs) and the summaries in `results/summary_*.csv`.
"–" means the instance was not solved; instances that no configuration solved are left out.

### 2(b): pruning relaxed-unsolvable states in A\*

| Instance | Plan cost | Expanded (blind) | Expanded (relaxed task graph) | Search time, blind (s) | Search time, relaxed task graph (s) |
|---|---:|---:|---:|---:|---:|
| castle-2-2-8 | 4 | 8 | 8 | 0.001 | 0.001 |
| castle-3-3-8 | 9 | 84 | 84 | 0.000 | 0.001 |
| castle-4-3-5 | 15 | 263 | 263 | 0.001 | 0.004 |
| castle-5-4-9 | 19 | 7,350 | 7,350 | 0.025 | 0.152 |
| castle-6-4-7 | 26 | 24,146 | 24,022 | 0.063 | 0.551 |
| castle-7-5-4 | 35 | 3,680,953 | – | 13.6 | – |
| castle-8-5-9 | 39 | 4,839,625 | – | 17.8 | – |

Pruning rarely helps on these instances. Only castle-6-4-7 has relaxed-unsolvable states that
blind search would expand, and there pruning saved 124 of 24,146 expansions. Building and
evaluating the relaxed task graph in every state makes each expansion several times slower. As a
result, blind search solved two instances that the pruned search did not finish.

### 2(d) and 2(f): greedy best-first search with h<sup>add</sup> and h<sup>FF</sup>

| Instance | Optimal cost (2b) | h<sup>add</sup>(I) | Plan cost (h<sup>add</sup>) | h<sup>FF</sup>(I), built-in / mine | Plan cost (h<sup>FF</sup>), built-in / mine |
|---|---:|---:|---:|---:|---:|
| castle-2-2-8 | 4 | 6 | 4 | 4 / 4 | 4 / 4 |
| castle-3-3-8 | 9 | 53 | 9 | 9 / 11 | 9 / 9 |
| castle-4-3-5 | 15 | 161 | 15 | 19 / 20 | 15 / 15 |
| castle-5-4-9 | 19 | 232 | 21 | 25 / 25 | 21 / 20 |
| castle-6-4-7 | 26 | 555 | 26 | 35 / 37 | 30 / 27 |
| castle-7-5-4 | 35 | 595 | 40 | 41 / 43 | 38 / 39 |
| castle-8-5-9 | 39 | 1,704 | 46 | 48 / 49 | 43 / 46 |
| castle-10-6-7 | – | 2,257 | 82 | 62 / 64 | 72 / 74 |

- **`planopt_add` matches the built-in `add` exactly** on every instance: the same initial h,
  plan costs and expansions.
- **h<sup>add</sup> grossly overestimates.** For example, it gives 2,257 for a plan of cost 82,
  because it counts shared subgoals once for every goal that needs them. Even so, it guides
  greedy search to an optimal plan on four of the seven instances with a known optimum.
- **h<sup>FF</sup> is much closer to the real cost.** It counts every operator of one relaxed
  plan once.
- **Mine vs. the built-in `ff`.** The two can choose different best achievers on ties, so their
  values differ slightly. With mine, greedy search expanded fewer states on most instances.
- **Where the plans lose cost.** The suboptimal plans mostly contain "useless" moves that are
  undone later. For example, on castle-5-4-9 the h<sup>add</sup> plan moves `club4` to `club5`
  and later on to `spade5`, while the optimal plan moves it to `spade5` directly. The optimal
  relaxed plans (A\* with LM-cut on the delete relaxation) cost 19 on castle-5-4-9 and 26 on
  castle-6-4-7.

## Theory

[`theory/visitall-relaxed-task-graph.png`](theory/visitall-relaxed-task-graph.png) is the relaxed
task graph of the Visitall instance in [`benchmarks/visitall-untyped`](benchmarks/visitall-untyped),
labeled with h<sup>max</sup> and h<sup>add</sup> costs:

| h\*(I) | h<sup>+</sup>(I) | h<sup>max</sup>(I) | h<sup>add</sup>(I) |
|---:|---:|---:|---:|
| 6 | 4 | 2 | 6 |

## Code changes after the experiments

These results come from the original runs. Two things were fixed during the cleanup:

- **The h<sup>add</sup> valuation in the committed code returned infinity.** The weighted
  valuation reused the successor counters left by the unweighted one, and could process a node
  more than once. Each node is now expanded once, in order of cost, which computes the same
  values as the built-in `add` that the logged runs match.
- **`run_experiments.py` now applies the time and memory limits.** The original script set
  `ulimit` in a separate shell, so the logged runs had no limits.
