/*
 * puzzle.h - Sliding-tile puzzle (8-puzzle and 15-puzzle).
 *
 * A state is packed into a 64-bit integer with 4 bits per cell: the nibble at
 * position i holds the tile in cell i (row-major order), and tile 0 is the
 * blank. The goal is the ordered grid 0 1 2 ... (blank in the top-left cell).
 *
 * Moving a tile is a few shifts and masks on the packed state, and states can
 * be hashed and compared as plain integers.
 */
#ifndef PUZZLE_H
#define PUZZLE_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

using State = uint64_t;

// Marks "no parent" when generating the successors of the initial state.
// No valid puzzle state packs to 0 (it would have the blank in every cell).
constexpr State kNoState = 0;

class Puzzle {
public:
    // `num_cells` is 9 (8-puzzle) or 16 (15-puzzle).
    explicit Puzzle(int num_cells);

    int num_cells() const { return num_cells_; }
    State goal() const { return goal_; }
    bool is_goal(State s) const { return s == goal_; }

    // Sum of the Manhattan distances of all tiles (blank excluded) to their goal cells.
    int manhattan(State s) const;

    // Writes the successors of `s` to `out` in the order required by the
    // assignment (blank moves up, left, right, down), skipping `parent`.
    // Returns the number of successors written.
    int successors(State s, State parent, std::array<State, 4> &out) const;

    // Packs a list of tiles (one per cell) into a state.
    static State pack(const std::vector<int> &tiles);

    // Returns an error message if `tiles` is not a solvable permutation of
    // 0 .. num_cells-1, or an empty string if it is valid.
    static std::string validate(const std::vector<int> &tiles);

private:
    int num_cells_;
    int width_;
    State goal_;
    std::array<int, 16> goal_row_{};
    std::array<int, 16> goal_col_{};
};

#endif  // PUZZLE_H
