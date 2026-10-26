/*
 * puzzle.cpp - Packed-state operations for the sliding-tile puzzle.
 *
 * Moving the blank swaps it with a neighboring tile. On the packed state this
 * is: copy the neighbor's nibble into the blank's position (by shifting the
 * whole state by one cell, or by one row, and masking), then clear the nibble
 * at the neighbor's old position.
 */
#include "puzzle.h"

#include <cstdlib>
#include <numeric>

namespace {
constexpr State kNibble = 0xFull;

int blank_index(State s) {
    int i = 0;
    while ((s >> (4 * i)) & kNibble) ++i;
    return i;
}
}  // namespace

Puzzle::Puzzle(int num_cells)
    : num_cells_(num_cells), width_(num_cells == 9 ? 3 : 4), goal_(0) {
    for (int tile = 0; tile < num_cells_; ++tile) {
        goal_row_[tile] = tile / width_;
        goal_col_[tile] = tile % width_;
        goal_ |= static_cast<State>(tile) << (4 * tile);
    }
}

int Puzzle::manhattan(State s) const {
    int h = 0;
    for (int cell = 0; cell < num_cells_; ++cell, s >>= 4) {
        int tile = static_cast<int>(s & kNibble);
        if (tile != 0) {
            h += std::abs(cell / width_ - goal_row_[tile]) +
                 std::abs(cell % width_ - goal_col_[tile]);
        }
    }
    return h;
}

int Puzzle::successors(State s, State parent, std::array<State, 4> &out) const {
    const int blank = blank_index(s);
    const int row = blank / width_;
    const int col = blank % width_;
    const State blank_mask = kNibble << (4 * blank);
    int count = 0;

    // The tile at `neighbor` moves into the blank's cell. `shifted` is the state
    // shifted so that this tile lines up with the blank.
    auto add = [&](int neighbor, State shifted) {
        State moved = (s | (shifted & blank_mask)) & ~(kNibble << (4 * neighbor));
        if (moved != parent) out[count++] = moved;
    };

    if (row > 0) add(blank - width_, s << (4 * width_));           // up
    if (col > 0) add(blank - 1, s << 4);                           // left
    if (col < width_ - 1) add(blank + 1, s >> 4);                  // right
    if (row < width_ - 1) add(blank + width_, s >> (4 * width_));  // down
    return count;
}

State Puzzle::pack(const std::vector<int> &tiles) {
    State s = 0;
    for (size_t i = 0; i < tiles.size(); ++i) {
        s |= static_cast<State>(tiles[i]) << (4 * i);
    }
    return s;
}

std::string Puzzle::validate(const std::vector<int> &tiles) {
    const int n = static_cast<int>(tiles.size());
    if (n != 9 && n != 16) {
        return "a puzzle needs 9 (8-puzzle) or 16 (15-puzzle) tiles, got " +
               std::to_string(n);
    }
    std::vector<bool> seen(n, false);
    for (int t : tiles) {
        if (t < 0 || t >= n || seen[t]) {
            return "the tiles must be a permutation of 0.." + std::to_string(n - 1);
        }
        seen[t] = true;
    }
    // Every move swaps the blank with a tile, which flips the parity of the
    // permutation and the parity of the blank's distance to its goal cell
    // (top-left). Both are even at the goal, so they must agree.
    int inversions = 0;
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            inversions += tiles[i] > tiles[j];
    const int width = n == 9 ? 3 : 4;
    int blank = 0;
    while (tiles[blank] != 0) ++blank;
    const int blank_distance = blank / width + blank % width;
    if ((inversions + blank_distance) % 2 != 0) {
        return "this puzzle is unsolvable (wrong permutation parity)";
    }
    return "";
}
