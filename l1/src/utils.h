#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <queue>
#include <cstdint>
#include <limits>
#include "node.h"

// Custom Compare for A* priority queue.
struct AstarCompare : public std::binary_function<Node*, Node*, bool> 
{
  bool operator()(const Node* lhs, const Node* rhs) const;
};

// Custom Compare for Greedy Best-First Search priority queue.
struct GreedyCompare : public std::binary_function<Node*, Node*, bool> 
{
  bool operator()(const Node* lhs, const Node* rhs) const;
};

// Hasher for States (uint8_t vectors).
// Adapted from https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector.
class StateHasher 
{
public:
  std::size_t operator()(std::vector<uint8_t> const& s) const;
};

#endif // MY_UTILS_H