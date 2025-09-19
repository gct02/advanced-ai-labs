#include <iostream>
#include <inttypes.h>
#include "node.h"

// Heuristic: Manhattan Distance.
uint8_t manhattan_dist(State state, int size) {
  uint8_t h = 0;
  uint8_t n = size == 9 ? 3 : 4;
  for (uint8_t i = 0; i < (uint8_t)size; i++) {
    uint8_t val = (uint8_t)(state & 0xfull);
    state >>= 4;
    if (val != 0) {
      h += abs(i / n - val / n) + abs(i % n - val % n);
    }
  }
  return h;
}

int get_zero_index(State state, int size) {
  int i = 0;
  while ((state >> (i * 4)) & 0xfull) i++;
  return i;
}

// Generates all possible neighbor states from the given state 
// by sliding tiles into the empty space (0).
void get_neighbors(State state, int size, State parent_state, 
                   std::vector<State>& neighbors_buffer) {
  int n = size == 9 ? 3 : 4;
  int zero_index = get_zero_index(state, size);
  int row = zero_index / n;
  int col = zero_index % n;
  uint64_t mask_zero = 0xfull << (zero_index * 4);

  // Up
  if (row > 0) {
    uint64_t mask_tile = ~(0xfull << ((zero_index - n) * 4));
    uint64_t new_tile = (state << (4 * n)) & mask_zero;
    uint64_t new_state = (state | new_tile) & mask_tile;
    if (new_state != parent_state)
      neighbors_buffer.push_back(new_state);
  }
  // Left
  if (col > 0) {
    uint64_t mask_tile = ~(0xfull << ((zero_index - 1) * 4));
    uint64_t new_tile = (state << 4) & mask_zero;
    uint64_t new_state = (state | new_tile) & mask_tile;
    if (new_state != parent_state)
      neighbors_buffer.push_back(new_state);
  }
  // Right
  if (col < n - 1) {
    uint64_t mask_tile = ~(0xfull << ((zero_index + 1) * 4));
    uint64_t new_tile = (state >> 4) & mask_zero;
    uint64_t new_state = (state | new_tile) & mask_tile;
    if (new_state != parent_state)
      neighbors_buffer.push_back(new_state);
  }
  // Down
  if (row < n - 1) {
    uint64_t mask_tile = ~(0xfull << ((zero_index + n) * 4));
    uint64_t new_tile = (state >> (4 * n)) & mask_zero;
    uint64_t new_state = (state | new_tile) & mask_tile;
    if (new_state != parent_state)
      neighbors_buffer.push_back(new_state);
  }
}

State make_compact_state(const std::vector<uint8_t>& state_vec) {
  State state = 0x0llu;
  for (int i = 0; i < (int)state_vec.size(); i++) {
    state |= ((uint64_t)state_vec[i]) << (i * 4);
  }
  // printf("state = 0x%" PRIx64 "\n", state);
  return state;
}

// Creates a string representation of the given state. 
std::string state_to_string(State state) {
  // std::string s;
  // for (int num : state) {
  //   s += std::to_string(num);
  // }
  // return s;
  return "";
}