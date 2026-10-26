# Fast Downward plugin: `planopt_heuristics`

The heuristics for [lab 3](../lab3-delete-relaxation) and [lab 4](../lab4-abstraction-heuristics),
written as a plugin for [Fast Downward](https://github.com/aibasel/downward) 24.06.1.

## Setup

```bash
fast-downward-plugin/setup.sh        # clones, patches and builds Fast Downward in ./fast-downward
```

The script:
1. clones Fast Downward 24.06.1;
2. applies [`course-hooks.patch`](course-hooks.patch);
3. links this folder's `planopt_heuristics` into `src/search/`;
4. builds the release configuration.

The patch is the course's change to Fast Downward, for both labs. It registers the plugin in
CMake and adds the test flags `--test-and-or-graphs` (lab 3) and `--test-projections` (lab 4).
Fast Downward's own requirements apply: CMake, a C++20 compiler and Python 3.

## What is mine

The plugin skeleton comes from the course: class declarations, heuristic wrappers, the TNF and
relaxed-task conversions, and the tests. My solutions are the parts marked `Lab 3, exercise …`
or `Lab 4, exercise …` in these files:

| File | Lab | Exercise |
|---|---|---|
| [`and_or_graph.cc`](planopt_heuristics/and_or_graph.cc) | 3 | 2(a) most conservative valuation; 2(c) and 2(e) weighted valuation with best achievers |
| [`relaxed_task_graph.cc`](planopt_heuristics/relaxed_task_graph.cc) | 3 | 2(b) graph construction; 2(c) h<sup>add</sup>; 2(e) h<sup>FF</sup> |
| [`projection.cc`](planopt_heuristics/projection.cc) | 4 | 2(a) projected initial state, goal and operators |
| [`pdb.cc`](planopt_heuristics/pdb.cc) | 4 | 2(b) backward Dijkstra over abstract states |
| [`canonical_pdbs.cc`](planopt_heuristics/canonical_pdbs.cc) | 4 | 2(d) compatibility graph and canonical heuristic |

## License

Fast Downward is licensed under the GNU General Public License v3 or later. The plugin builds on
Fast Downward's code, so it is distributed under the same license.
