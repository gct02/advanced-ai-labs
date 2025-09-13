#include "node.h"

// Heuristic: Manhattan Distance.
int manhattan_dist(const State& state) {
    int dist = 0;
    int size = (state.size() == 9) ? 3 : 4;
    for (int i = 0; i < (int)state.size(); ++i) {
        if (state[i] != 0) {
            int value = state[i];
            int row = i / size;
            int col = i % size;
            int goal_row = value / size;
            int goal_col = value % size;
            dist += abs(row - goal_row) + abs(col - goal_col);
        }
    }
    return dist;
}

// Check if the given state is a goal state.
bool is_goal_state(const State& state) {
    for (uint32_t i = 0; i < (uint32_t)state.size(); i++) {
        if (state[i] != i) return false;
    }
    return true;
}

// Generates all possible neighbor states from the given state 
// by sliding tiles into the empty space (0).
std::vector<State> get_neighbors(const State& state) {
    std::vector<State> neighbors;
    int size = state.size() == 9 ? 3 : 4;
    int zero_index = std::find(state.begin(), state.end(), 0) - state.begin();
    int row = zero_index / size;
    int col = zero_index % size;

    std::vector<std::pair<int, int>> directions = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};
    for (const auto& dir : directions) {
        int new_row = row + dir.first;
        int new_col = col + dir.second;
        if (new_row >= 0 && new_row < size && new_col >= 0 && new_col < size) {
            State new_state(state);
            std::swap(new_state[zero_index], new_state[new_row * size + new_col]);
            neighbors.push_back(new_state);
        }
    }
    return neighbors;
}

// Creates a string representation of the given state. 
std::string state_to_string(const State& state) {
    std::string s;
    // bool first = true;
    for (int num : state) {
        // if (!first) s += ",";
        // first = false;
        s += std::to_string(num);
    }
    return s;
}