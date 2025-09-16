#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// #include <memory>
// #include <queue>
#include "heuristics.h"
#include <cstring>

State split(std::string s) {
  size_t pos_start = 0, pos_end;
  std::string token;
  State res;
  while ((pos_end = s.find(" ", pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + 1;
    res.push_back((uint32_t)std::stoi(token));
  }
  res.push_back((uint32_t)std::stoi(s.substr(pos_start)));
  return res;
}

// Not used currently, uncomment the "astar" part in the run_algorithm function
// to use again
void print_graph(const std::string &filename, std::shared_ptr<Node> root) {
  std::ofstream out(filename);
  std::queue<std::shared_ptr<Node>> queue;
  std::shared_ptr<Node> pred = nullptr;
  std::shared_ptr<Node> curr = nullptr;
  queue.push(root);

  while (!queue.empty()) {
    std::shared_ptr<Node> curr = queue.front();
    queue.pop();
    if (pred) {
      if (pred->g < curr->g)
        out << "\n";
      else
        out << " ";
    }
    out << state_to_string(curr->state);
    for (auto child : curr->children) {
      queue.push(child);
    }
    pred = curr;
  }
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

std::vector<State> parse_command_line_puzzles(int argc, char *argv[]) {
  std::vector<State> puzzles;

  if (argc < 3) {
    print_usage();
    exit(1);
  }

  // Combine all arguments after algorithm into one string
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

  // Parse each puzzle string using your existing split function
  for (const auto &puzzle_str : puzzle_strings) {
    State puzzle = split(puzzle_str);

    // Validate puzzle size
    if (puzzle.size() != 9 && puzzle.size() != 16) {
      std::cerr
          << "Error: Puzzle must have 9 (8-puzzle) or 16 (15-puzzle) tiles\n";
      std::cerr << "Received: " << puzzle.size()
                << " tiles: " << state_to_string(puzzle) << "\n";
      exit(1);
    }

    puzzles.push_back(puzzle);
  }

  return puzzles;
}

void run_algorithm(const std::string &algorithm, const State &state) {
  Solution sol;
  if (algorithm == "-astar") {
    sol = astar(state);
//    std::shared_ptr<Node> root_astar = sol.root;
//    print_graph("graph_astar.txt", root_astar);

  } else if (algorithm == "-bfs") {
    sol = bfs_graph(state);
  } else if (algorithm == "-idastar") {
    sol = idastar(state);
  } else if (algorithm == "-idfs") {
    sol = iter_deep(state);
  } else if (algorithm == "-gbfs") {
    sol = greedy_best_first(state);
  } else {
    std::cerr << "Error: Unknown algorithm " << algorithm << "\n";
    print_usage();
    exit(1);
  }
  std::cout << sol.n_expanded << "," << sol.sol_depth << "," << sol.time << ","
            << sol.avg_h << "," << sol.start_h << std::endl;
}
int main(int argc, char *argv[]) {
  if (argc < 3) {
    print_usage();
    return 1;
  }

  std::string algorithm = argv[1];
  std::vector<State> puzzles = parse_command_line_puzzles(argc, argv);

  // Run the specified algorithm on each puzzle
  for (const auto &puzzle : puzzles) {
    run_algorithm(algorithm, puzzle);
  }

  return 0;
}
