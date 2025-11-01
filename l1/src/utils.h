#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <queue>
#include <cstdint>
#include <limits>
#include "node.h"

#define QUEUE_SIZE 1000

// Custom Compare for A* priority queue.
struct AstarCompare : public std::binary_function<Node*, Node*, bool> {
  bool operator()(const Node* lhs, const Node* rhs) const;
};

// Custom Compare for Greedy Best-First Search priority queue.
struct GreedyCompare : public std::binary_function<Node*, Node*, bool> {
  bool operator()(const Node* lhs, const Node* rhs) const;
};

class AstarPQueue : public std::priority_queue<Node*, std::vector<Node*>, AstarCompare> {
public:
  AstarPQueue(size_t reserve_size = QUEUE_SIZE) {
    this->c.reserve(reserve_size);
  }
};

class GreedyPQueue : public std::priority_queue<Node*, std::vector<Node*>, GreedyCompare> {
public:
  GreedyPQueue(size_t reserve_size = QUEUE_SIZE) {
    this->c.reserve(reserve_size);
  }
};

#endif // MY_UTILS_H