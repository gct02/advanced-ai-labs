#ifndef NODE_H
#define NODE_H

#include <cstdint>
#include <algorithm>
#include <vector>
#include <string>
#include <memory>

using State = std::vector<uint32_t>;

// Heuristic: Manhattan Distance.
int manhattan_dist(const State& state);

// Check if the given state is a goal state.
bool is_goal_state(const State& state);

// Generates all possible neighbor states from the given state 
// by sliding tiles into the empty space (0).
std::vector<State> get_neighbors(const State& state, const State* parent_state = nullptr);

// Check if state `a` is equal to state `b`.
bool comp_states(const State& a, const State& b);

// Creates a string representation of the given state. 
std::string state_to_string(const State& state);

struct Node {
    State state;
    bool is_goal;
    int g; // Cost from start to current node
    int h; // Heuristic cost estimate to goal
    int f; // Total estimated cost
    std::shared_ptr<Node> parent; // Pointer to parent node for path reconstruction
    std::vector<std::shared_ptr<Node>> children;
    uint64_t id; // Unique sequence number for LIFO tie-breaking

    Node(const State& s, int g_cost, std::shared_ptr<Node> par, uint64_t id_num = 0)
        : state(s), g(g_cost), parent(par), id(id_num) { 
        h = manhattan_dist(state);
        is_goal = is_goal_state(state);
        f = g + h; 
    }
};

#endif // NODE_H