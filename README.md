# Advanced Artificial Intelligence: Search and Planning Labs

Lab assignments from *Inteligência Artificial Avançada* (Advanced Artificial Intelligence),
UFRGS, 2025/2, taught by Prof. André Grahl Pereira. Labs 3 and 4 are based on exercises by
Malte Helmert from the *Planning and Optimization* course at the University of Basel, and are
implemented inside the [Fast Downward](https://www.fast-downward.org) planning system.

| Lab | Topic | What it contains | Language |
|---|---|---|---|
| [1](lab1-puzzle-search) | Heuristic search | BFS, iterative deepening, A\*, IDA\* and greedy best-first search for the 8- and 15-puzzle | C++ |
| [2](lab2-pddl-modeling) | Planning models | A PDDL domain with conditional effects (vampire and slayer) | PDDL |
| [3](lab3-delete-relaxation) | Delete relaxation | AND/OR graphs, relaxed task graphs, h<sup>add</sup> and h<sup>FF</sup> as Fast Downward heuristics | C++ |
| [4](lab4-abstraction-heuristics) | Abstraction heuristics | Projections, pattern databases and the canonical PDB heuristic as Fast Downward heuristics | C++ |

## Repository layout

```
.
├── lab1-puzzle-search/            standalone C++ program (make)
├── lab2-pddl-modeling/            PDDL domain
├── lab3-delete-relaxation/        benchmarks, experiment scripts and results
├── lab4-abstraction-heuristics/   benchmarks, experiment scripts and results
└── fast-downward-plugin/          the planopt_heuristics plugin with the code for labs 3 and 4
```

Labs 3 and 4 share one Fast Downward build. Instead of a copy of Fast Downward, the repository
only holds the course's plugin folder and a small patch. `fast-downward-plugin/setup.sh` clones
Fast Downward 24.06.1 into `fast-downward/`, adds the plugin and builds it. See
[fast-downward-plugin](fast-downward-plugin) for which plugin files contain my solutions.

## Credits

- Labs 1 and 2 were done in a pair with Makoto Ishikawa.
- The plugin skeleton, the test graphs and the benchmark instances are course materials.
- Fast Downward is developed by the Fast Downward contributors and licensed under the GNU GPL
  v3. The plugin is a derivative of Fast Downward and is distributed under the same license.
