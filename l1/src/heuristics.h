#ifndef HEURISTICS_H
#define HEURISTICS_H

#include <cstdint>
#include <vector>
#include <deque>
#include <memory>
#include "node.h"
#include "utils.h"

struct Solution {
  std::vector<State> path;
  Node* root;
  int n_expanded = 0;
  int sol_depth = -1;
  int start_h = 0;
  double avg_h = 0.0;
  double time = 0.0;
};

Solution bfs_graph(const State& start_state);
Solution iter_deep(const State& start_state);
Solution greedy_best_first(const State& start_state);
Solution astar(State start_state, int size);
Solution idastar(State start_state, int size);

#endif // HEURISTICS_H