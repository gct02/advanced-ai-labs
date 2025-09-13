#include <queue>
#include <cstdint>
#include <limits>
#include "utils.h"

bool AstarCompare::operator()(const std::shared_ptr<Node> lhs, const std::shared_ptr<Node> rhs) const {
    if (lhs->f != rhs->f) return lhs->f > rhs->f; // Prioritize smaller f-value
    if (lhs->h != rhs->h) return lhs->h > rhs->h; // Prioritize smaller h-value
    return lhs->id < rhs->id; // Use LIFO (larger ID -> higher priority)
}

bool GreedyCompare::operator()(const std::shared_ptr<Node> lhs, const std::shared_ptr<Node> rhs) const {
    if (lhs->h != rhs->h) return lhs->h > rhs->h; // Prioritize smaller h-value
    if (lhs->g != rhs->g) return lhs->g < rhs->g; // Prioritize higher g-value
    return lhs->id < rhs->id; // Use LIFO (larger ID -> higher priority)
}

// Hasher for uint32_t vectors.
// From https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector.
template<typename T>
T xorshift(const T& n, int i){
    return n^(n>>i);
}

uint32_t hash(const uint32_t& n) {
    uint32_t p = 0x55555555ul; // Pattern of alternating 0 and 1
    uint32_t c = 3423571495ul; // Random uneven integer constant
    return c*xorshift(p*xorshift(n,16),16);
}

std::size_t rotl(std::size_t n, std::size_t i){
    std::size_t m = (std::numeric_limits<std::size_t>::digits-1);
    std::size_t c = i&m;
    return (n<<c)|(n>>((std::size_t(0)-c)&m));
}

std::size_t StateHasher::operator()(std::vector<uint32_t> const& s) const {
    std::size_t ret = 0;
    for(auto& i : s) {
        ret = rotl(ret,11)^hash(i);
    }
    return ret;
}
