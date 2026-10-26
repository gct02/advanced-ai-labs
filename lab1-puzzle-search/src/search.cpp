/*
 * search.cpp - BFS (graph search), iterative deepening DFS, greedy best-first
 * search, A* and IDA* for the sliding-tile puzzle.
 *
 * Graph searches (BFS, GBFS, A*) keep the generated nodes in a pool and refer
 * to them by index; a node's index is also its generation order, which gives
 * the LIFO tie-breaking. Iterative deepening and IDA* are recursive depth-first
 * searches that only keep the current path in memory.
 *
 * Average heuristic value: every heuristic value computed during the search is
 * accumulated. For a state that is already closed, the stored value is counted
 * instead of recomputing it.
 */
#include "search.h"

#include <algorithm>
#include <deque>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace {

constexpr uint32_t kNoParent = std::numeric_limits<uint32_t>::max();

struct Node {
    State state;
    uint32_t parent;  // index of the parent node in the pool
    uint16_t g;
    uint8_t h;
};

// std::deque never relocates its elements, so the pool can grow to millions
// of nodes without the copy spikes of std::vector.
using NodePool = std::deque<Node>;

State parent_state(const NodePool &nodes, uint32_t index) {
    uint32_t parent = nodes[index].parent;
    return parent == kNoParent ? kNoState : nodes[parent].state;
}

std::vector<State> trace_path(const NodePool &nodes, uint32_t index) {
    std::vector<State> path;
    for (; index != kNoParent; index = nodes[index].parent) {
        path.push_back(nodes[index].state);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// Accumulates heuristic values to report their mean.
struct HeuristicStats {
    int64_t sum = 0;
    int64_t count = 0;
    void add(int h) { sum += h; ++count; }
    double mean() const { return count ? static_cast<double>(sum) / count : 0.0; }
};

// Open-list entry for the best-first searches. `node` is the pool index, so a
// larger value means a more recently generated node.
struct OpenEntry {
    uint32_t node;
    uint16_t g;
    uint8_t h;
};

// std::priority_queue pops the "largest" element: compare(a, b) is true when
// a has lower priority than b.
struct AstarOrder {  // min f, then min h, then LIFO
    bool operator()(const OpenEntry &a, const OpenEntry &b) const {
        int fa = a.g + a.h, fb = b.g + b.h;
        if (fa != fb) return fa > fb;
        if (a.h != b.h) return a.h > b.h;
        return a.node < b.node;
    }
};

struct GreedyOrder {  // min h, then max g, then LIFO
    bool operator()(const OpenEntry &a, const OpenEntry &b) const {
        if (a.h != b.h) return a.h > b.h;
        if (a.g != b.g) return a.g < b.g;
        return a.node < b.node;
    }
};

}  // namespace

SearchResult breadth_first_search(const Puzzle &puzzle, State start) {
    SearchResult result;
    result.initial_h = puzzle.manhattan(start);
    if (puzzle.is_goal(start)) {
        result.solution_length = 0;
        result.path = {start};
        return result;
    }

    NodePool nodes{{start, kNoParent, 0, 0}};
    std::queue<uint32_t> frontier;
    std::unordered_set<State> reached{start};
    std::array<State, 4> succ;
    frontier.push(0);

    // The goal test happens when a node is generated.
    while (!frontier.empty()) {
        uint32_t index = frontier.front();
        frontier.pop();
        ++result.expanded;

        int n = puzzle.successors(nodes[index].state, parent_state(nodes, index), succ);
        for (int i = 0; i < n; ++i) {
            if (!reached.insert(succ[i]).second) continue;
            uint16_t g = nodes[index].g + 1;
            nodes.push_back({succ[i], index, g, 0});
            uint32_t child = static_cast<uint32_t>(nodes.size() - 1);
            if (puzzle.is_goal(succ[i])) {
                result.solution_length = g;
                result.path = trace_path(nodes, child);
                return result;
            }
            frontier.push(child);
        }
    }
    return result;
}

namespace {
// Depth-limited DFS used by iterative deepening. The goal test happens when a
// node is visited, before the depth limit is checked.
struct DepthLimitedSearch {
    const Puzzle &puzzle;
    SearchResult &result;
    int limit = 0;
    std::vector<State> path;

    bool visit(State state, State parent, int g) {
        if (puzzle.is_goal(state)) {
            result.solution_length = g;
            result.path = path;
            return true;
        }
        if (g > limit) return false;
        ++result.expanded;

        std::array<State, 4> succ;
        int n = puzzle.successors(state, parent, succ);
        for (int i = 0; i < n; ++i) {
            path.push_back(succ[i]);
            if (visit(succ[i], state, g + 1)) return true;
            path.pop_back();
        }
        return false;
    }
};
}  // namespace

SearchResult iterative_deepening_search(const Puzzle &puzzle, State start) {
    SearchResult result;
    result.initial_h = puzzle.manhattan(start);
    DepthLimitedSearch dls{puzzle, result, 0, {}};
    for (dls.limit = 0; dls.limit <= std::numeric_limits<uint16_t>::max(); ++dls.limit) {
        dls.path = {start};
        if (dls.visit(start, kNoState, 0)) break;
    }
    return result;
}

SearchResult greedy_best_first_search(const Puzzle &puzzle, State start) {
    SearchResult result;
    const uint8_t h0 = static_cast<uint8_t>(puzzle.manhattan(start));
    result.initial_h = h0;
    HeuristicStats h_stats;
    h_stats.add(h0);
    if (h0 == 0) {
        result.solution_length = 0;
        result.average_h = h_stats.mean();
        result.path = {start};
        return result;
    }

    // Duplicates are detected when generated: each state enters the open list
    // at most once. `seen` maps every generated state to its heuristic value.
    NodePool nodes{{start, kNoParent, 0, h0}};
    std::priority_queue<OpenEntry, std::vector<OpenEntry>, GreedyOrder> open;
    std::unordered_map<State, uint8_t> seen{{start, h0}};
    std::array<State, 4> succ;
    open.push({0, 0, h0});

    while (!open.empty()) {
        OpenEntry top = open.top();
        open.pop();
        const Node node = nodes[top.node];
        if (node.h == 0) {
            result.solution_length = node.g;
            result.average_h = h_stats.mean();
            result.path = trace_path(nodes, top.node);
            return result;
        }
        ++result.expanded;

        int n = puzzle.successors(node.state, parent_state(nodes, top.node), succ);
        for (int i = 0; i < n; ++i) {
            auto it = seen.find(succ[i]);
            if (it != seen.end()) {
                h_stats.add(it->second);
                continue;
            }
            uint8_t h = static_cast<uint8_t>(puzzle.manhattan(succ[i]));
            h_stats.add(h);
            uint16_t g = node.g + 1;
            nodes.push_back({succ[i], top.node, g, h});
            open.push({static_cast<uint32_t>(nodes.size() - 1), g, h});
            seen.emplace(succ[i], h);
        }
    }
    return result;
}

SearchResult astar_search(const Puzzle &puzzle, State start) {
    SearchResult result;
    const uint8_t h0 = static_cast<uint8_t>(puzzle.manhattan(start));
    result.initial_h = h0;
    HeuristicStats h_stats;
    h_stats.add(h0);

    // Duplicates are detected when a node is popped (lazy deletion), and
    // successors that are already closed are not generated again. `closed`
    // maps each expanded state to its heuristic value.
    NodePool nodes{{start, kNoParent, 0, h0}};
    std::priority_queue<OpenEntry, std::vector<OpenEntry>, AstarOrder> open;
    std::unordered_map<State, uint8_t> closed;
    std::array<State, 4> succ;
    open.push({0, 0, h0});

    while (!open.empty()) {
        OpenEntry top = open.top();
        open.pop();
        const Node node = nodes[top.node];
        if (node.h == 0) {
            result.solution_length = node.g;
            result.average_h = h_stats.mean();
            result.path = trace_path(nodes, top.node);
            return result;
        }
        if (!closed.emplace(node.state, node.h).second) continue;
        ++result.expanded;

        int n = puzzle.successors(node.state, parent_state(nodes, top.node), succ);
        for (int i = 0; i < n; ++i) {
            auto it = closed.find(succ[i]);
            if (it != closed.end()) {
                h_stats.add(it->second);
                continue;
            }
            uint8_t h = static_cast<uint8_t>(puzzle.manhattan(succ[i]));
            h_stats.add(h);
            uint16_t g = node.g + 1;
            nodes.push_back({succ[i], top.node, g, h});
            open.push({static_cast<uint32_t>(nodes.size() - 1), g, h});
        }
    }
    result.average_h = h_stats.mean();
    return result;
}

namespace {
// One cost-bounded DFS iteration of IDA*. The goal test happens when a node is
// visited, before the f-bound is checked. Heuristic values are counted every
// time a node is visited, across all iterations.
struct BoundedSearch {
    const Puzzle &puzzle;
    SearchResult &result;
    HeuristicStats &h_stats;
    int bound = 0;
    int next_bound = 0;
    std::vector<State> path;

    bool visit(State state, State parent, int g, int h) {
        h_stats.add(h);
        if (h == 0) {
            result.solution_length = g;
            result.average_h = h_stats.mean();
            result.path = path;
            return true;
        }
        int f = g + h;
        if (f > bound) {
            next_bound = std::min(next_bound, f);
            return false;
        }
        ++result.expanded;

        std::array<State, 4> succ;
        int n = puzzle.successors(state, parent, succ);
        for (int i = 0; i < n; ++i) {
            path.push_back(succ[i]);
            if (visit(succ[i], state, g + 1, puzzle.manhattan(succ[i]))) return true;
            path.pop_back();
        }
        return false;
    }
};
}  // namespace

SearchResult idastar_search(const Puzzle &puzzle, State start) {
    SearchResult result;
    const int h0 = puzzle.manhattan(start);
    result.initial_h = h0;
    HeuristicStats h_stats;
    BoundedSearch search{puzzle, result, h_stats, 0, 0, {}};
    constexpr int kInfinity = std::numeric_limits<uint16_t>::max();

    for (search.bound = h0; search.bound != kInfinity; search.bound = search.next_bound) {
        search.next_bound = kInfinity;
        search.path = {start};
        if (search.visit(start, kNoState, 0, h0)) break;
    }
    return result;
}
