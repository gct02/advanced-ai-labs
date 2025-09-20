#ifndef NODE_H
#define NODE_H

#include <cstdint>
#include <algorithm>
#include <vector>
#include <string>
#include <memory>

using State = uint64_t;

void precompute_goal_positions(int size);

// Heuristic: Manhattan Distance.
uint8_t manhattan_dist(State state, int size);

// Generates all possible neighbor states from the given state 
// by sliding tiles into the empty space (0).
void get_neighbors(State state, int size, State parent_state, 
                   std::vector<State>& neighbors_buffer);

State make_compact_state(const std::vector<uint8_t>& state_vec);

// Creates a string representation of the given state. 
std::string state_to_string(State state);

struct Node 
{
  State state;
  Node* parent; // Pointer to parent node for path reconstruction
  uint32_t id; // Unique sequence number for LIFO tie-breaking
  uint16_t g; // Cost from start to current node
  uint8_t h; // Heuristic cost estimate to goal

  Node(State s, uint16_t g_cost, Node* par, int size, uint8_t h_cost, uint32_t id_num = 0)
    : state(s), g(g_cost), parent(par), h(h_cost), id(id_num) {}
};

#endif // NODE_H