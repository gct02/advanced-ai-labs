#ifndef HEURISTICS_H
#define HEURISTICS_H

#include <cstdint>
#include <vector>
#include <deque>
#include <memory>
#include "node.h"
#include "utils.h"

struct Solution {
    std::deque<State> path;
    std::shared_ptr<Node> root;
    int n_expanded = 0;
    int sol_depth = -1;
    int start_h = 0;
    double avg_h = 0.0;
    double time = 0.0;
};

Solution bfs_graph(const State& start_state);
Solution iter_deep(const State& start_state);
Solution greedy_best_first(const State& start_state);
Solution astar(const State& start_state);
Solution idastar(const State& start_state);

#endif // HEURISTICS_H