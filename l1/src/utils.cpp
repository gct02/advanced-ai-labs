#include <queue>
#include <cstdint>
#include <limits>
#include "utils.h"

bool AstarCompare::operator()(const Node* lhs, const Node* rhs) const 
{
  uint16_t lhs_f = (uint16_t)lhs->g + lhs->h;
  uint16_t rhs_f = (uint16_t)rhs->g + rhs->h;
  if (lhs_f != rhs_f) return lhs_f > rhs_f; // Prioritize smaller f-value
  if (lhs->h != rhs->h) return lhs->h > rhs->h; // Prioritize smaller h-value
  return lhs->id < rhs->id; // Use LIFO (larger ID -> higher priority)
}

bool GreedyCompare::operator()(const Node* lhs, const Node* rhs) const 
{
  if (lhs->h != rhs->h) return lhs->h > rhs->h; // Prioritize smaller h-value
  if (lhs->g != rhs->g) return lhs->g < rhs->g; // Prioritize higher g-value
  return lhs->id < rhs->id; // Use LIFO (larger ID -> higher priority)
}

template<typename T>
T xorshift(const T& n, int i)
{
  return n ^ (n >> i);
}

uint32_t hash(const uint32_t& n) 
{
  uint32_t p = 0x55555555ul; // Pattern of alternating 0 and 1
  uint32_t c = 3423571495ul; // Random uneven integer constant
  return c * xorshift(p * xorshift(n, 16), 16);
}

template <typename T, typename S>
typename std::enable_if<std::is_unsigned<T>::value, T>::type
constexpr rotl(const T n, const S i) 
{
  const T m = (std::numeric_limits<T>::digits - 1);
  const T c = i & m;
  return (n << c) | (n >> ((T(0) - c) & m));
}

// Hasher for States (uint8_t vectors).
// Adapted from https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector.
std::size_t StateHasher::operator()(std::vector<uint8_t> const& s) const 
{
  std::size_t ret = 0;
  for (auto& i : s)
    ret = rotl(ret, 5) ^ hash(static_cast<uint32_t>(i));
  return ret;
}
