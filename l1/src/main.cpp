#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "heuristics.h"
#include <cstring>
#include <chrono>

using timer = std::chrono::high_resolution_clock;
using time_point = std::chrono::high_resolution_clock::time_point;
using duration = std::chrono::duration<double>;

double get_elapsed_time(time_point start_time) {
  time_point end_time = timer::now();
  duration elapsed = end_time - start_time;
  return elapsed.count();
}

std::vector<uint8_t> split(std::string s) {
  size_t pos_start = 0, pos_end;
  std::string token;
  std::vector<uint8_t> res;
  while ((pos_end = s.find(" ", pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + 1;
    res.push_back((uint8_t)std::stoi(token));
  }
  res.push_back((uint8_t)std::stoi(s.substr(pos_start)));
  return res;
}

void print_usage() {
  std::cout << "Usage: ./main <algorithm> <puzzle1> <puzzle2> ...\n";
  std::cout << "Algorithms: -astar, -bfs, -idastar, -idfs, -gbfs\n";
  std::cout << "Puzzle format: numbers separated by spaces, puzzles separated "
               "by commas\n";
  std::cout << "Examples:\n";
  std::cout << "  ./main -astar 0 6 1 7 4 2 3 8 5\n";
  std::cout << "  ./main -bfs 5 0 2 6 4 8 1 7 3, 2 4 7 0 3 6 8 1 5\n";
}

std::vector<std::vector<uint8_t>> parse_command_line_puzzles(int argc, char *argv[]) {
  std::vector<std::vector<uint8_t>> puzzles;

  if (argc < 3) {
    print_usage();
    exit(1);
  }

  // Combine all arguments after <algorithm> into one string
  std::string puzzle_args;
  for (int i = 2; i < argc; i++) {
    puzzle_args += argv[i];
    puzzle_args += " ";
  }

  // Split by commas to get individual puzzles
  std::vector<std::string> puzzle_strings;
  std::stringstream ss(puzzle_args);
  std::string token;

  while (std::getline(ss, token, ',')) {
    // Remove leading/trailing whitespace
    size_t start = token.find_first_not_of(" \t\n\r");
    size_t end = token.find_last_not_of(" \t\n\r");

    if (start != std::string::npos && end != std::string::npos) {
      puzzle_strings.push_back(token.substr(start, end - start + 1));
    }
  }

  // Parse each puzzle string
  for (const auto &puzzle_str : puzzle_strings) {
    std::vector<uint8_t> puzzle = split(puzzle_str);

    // Validate puzzle size
    if (puzzle.size() != 9 && puzzle.size() != 16) {
      std::cerr << "Error: Puzzle must have 9 (8-puzzle) or 16 (15-puzzle) tiles\n";
      std::cerr << "Received: " << puzzle.size() << "\n";
                // << " tiles: " << state_to_string(puzzle) << "\n";
      exit(1);
    }

    puzzles.push_back(puzzle);
  }

  return puzzles;
}

void run_algorithm(const std::string &algorithm, const std::vector<uint8_t> &state) {
  Solution sol;
  State compact_state = make_compact_state(state);
  time_point start_time = timer::now();
  if (algorithm == "-astar") {
    sol = astar(compact_state, state.size());
  } else if (algorithm == "-bfs") {
    sol = bfs_graph(compact_state, state.size());
  } else if (algorithm == "-idastar") {
    sol = idastar(compact_state, state.size());
  } else if (algorithm == "-idfs") {
    sol = iter_deep(compact_state, state.size());
  } else if (algorithm == "-gbfs") {
    sol = greedy_best_first(compact_state, state.size());
  } else {
    std::cerr << "Error: Unknown algorithm " << algorithm << "\n";
    print_usage();
    exit(1);
  }
  sol.time = get_elapsed_time(start_time);
  std::cout << sol.n_expanded << "," << sol.sol_depth << "," << sol.time << ","
            << sol.avg_h << "," << sol.start_h << std::endl;
}
int main(int argc, char *argv[]) {
  if (argc < 3) {
    print_usage();
    return 1;
  }

  std::string algorithm = argv[1];
  std::vector<std::vector<uint8_t>> puzzles = parse_command_line_puzzles(argc, argv);

  // Run the specified algorithm on each puzzle
  for (const auto &puzzle : puzzles) {
    run_algorithm(algorithm, puzzle);
  }

  return 0;
}
