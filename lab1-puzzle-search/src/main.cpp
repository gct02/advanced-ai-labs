/*
 * main.cpp - Command-line interface.
 *
 *   ./main <algorithm> <puzzle>[, <puzzle> ...]
 *
 * <algorithm> is one of -bfs, -idfs, -astar, -idastar, -gbfs. Each puzzle is a
 * list of 9 or 16 tiles (the tile in each cell, row-major, 0 = blank), and
 * puzzles are separated by commas. For each puzzle, one line is printed:
 *
 *   expanded nodes, solution length, time (s), average h, initial h
 */
#include "puzzle.h"
#include "search.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

using SearchFunction = std::function<SearchResult(const Puzzle &, State)>;

const std::map<std::string, SearchFunction> kAlgorithms = {
    {"-bfs", breadth_first_search},
    {"-idfs", iterative_deepening_search},
    {"-astar", astar_search},
    {"-idastar", idastar_search},
    {"-gbfs", greedy_best_first_search},
};

void print_usage(std::ostream &out) {
    out << "Usage: ./main <algorithm> <puzzle>[, <puzzle> ...]\n"
           "Algorithms: -bfs, -idfs, -astar, -idastar, -gbfs\n"
           "A puzzle lists the tile in each cell (row-major, 0 = blank);\n"
           "puzzles are separated by commas. Examples:\n"
           "  ./main -astar 0 6 1 7 4 2 3 8 5\n"
           "  ./main -bfs 5 0 2 6 4 8 1 7 3, 2 4 7 0 3 6 8 1 5\n";
}

// Joins the arguments after the algorithm and splits them into puzzles.
std::vector<std::vector<int>> parse_puzzles(int argc, char *argv[]) {
    std::string joined;
    for (int i = 2; i < argc; ++i) {
        joined += argv[i];
        joined += ' ';
    }
    std::vector<std::vector<int>> puzzles;
    std::stringstream by_comma(joined);
    std::string chunk;
    while (std::getline(by_comma, chunk, ',')) {
        std::istringstream by_space(chunk);
        std::vector<int> tiles;
        std::string token;
        while (by_space >> token) {
            try {
                tiles.push_back(std::stoi(token));
            } catch (const std::exception &) {
                throw std::runtime_error("invalid tile '" + token + "'");
            }
        }
        if (!tiles.empty()) puzzles.push_back(tiles);
    }
    return puzzles;
}

}  // namespace

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage(std::cerr);
        return 1;
    }
    auto algorithm = kAlgorithms.find(argv[1]);
    if (algorithm == kAlgorithms.end()) {
        std::cerr << "Error: unknown algorithm " << argv[1] << "\n";
        print_usage(std::cerr);
        return 1;
    }

    std::vector<std::vector<int>> puzzles;
    try {
        puzzles = parse_puzzles(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    // Validate every puzzle before solving any, so a bad input fails fast.
    for (size_t i = 0; i < puzzles.size(); ++i) {
        std::string error = Puzzle::validate(puzzles[i]);
        if (error.empty() && puzzles[i].size() != puzzles[0].size()) {
            error = "all puzzles must have the same size";
        }
        if (!error.empty()) {
            std::cerr << "Error in puzzle " << i + 1 << ": " << error << "\n";
            return 1;
        }
    }

    for (const auto &tiles : puzzles) {
        Puzzle puzzle(static_cast<int>(tiles.size()));
        State start = Puzzle::pack(tiles);

        auto t0 = std::chrono::steady_clock::now();
        SearchResult r = algorithm->second(puzzle, start);
        std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - t0;

        std::cout << r.expanded << "," << r.solution_length << "," << elapsed.count()
                  << "," << r.average_h << "," << r.initial_h << std::endl;
    }
    return 0;
}
