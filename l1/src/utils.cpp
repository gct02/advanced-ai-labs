#include <queue>
#include <cstdint>
#include <limits>
#include "utils.h"

bool AstarCompare::operator()(const Node* lhs, const Node* rhs) const {
  uint16_t lhs_f = (uint16_t)lhs->g + lhs->h;
  uint16_t rhs_f = (uint16_t)rhs->g + rhs->h;
  if (lhs_f != rhs_f) return lhs_f > rhs_f; // Prioritize smaller f-value
  if (lhs->h != rhs->h) return lhs->h > rhs->h; // Prioritize smaller h-value
  return lhs->id < rhs->id; // Use LIFO (larger ID -> higher priority)
}

bool GreedyCompare::operator()(const Node* lhs, const Node* rhs) const {
  if (lhs->h != rhs->h) return lhs->h > rhs->h; // Prioritize smaller h-value
  if (lhs->g != rhs->g) return lhs->g < rhs->g; // Prioritize higher g-value
  return lhs->id < rhs->id; // Use LIFO (larger ID -> higher priority)
}