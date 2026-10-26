/*
 * search.h - Search algorithms for the sliding-tile puzzle.
 *
 * All algorithms use unit action costs and the Manhattan distance heuristic,
 * and none of them generates the parent of the node being expanded. Ties in
 * the priority queues are broken as the assignment requires:
 *   A*:   min f, then min h, then LIFO (most recently generated first)
 *   GBFS: min h, then max g, then LIFO
 */
#ifndef SEARCH_H
#define SEARCH_H

#include "puzzle.h"

#include <cstdint>
#include <vector>

struct SearchResult {
    uint64_t expanded = 0;      // number of expanded nodes
    int solution_length = -1;   // -1 if no solution was found
    double average_h = 0.0;     // mean of all heuristic values computed (0 for blind searches)
    int initial_h = 0;          // heuristic value of the initial state
    std::vector<State> path;    // states from the initial state to the goal
};

SearchResult breadth_first_search(const Puzzle &puzzle, State start);
SearchResult iterative_deepening_search(const Puzzle &puzzle, State start);
SearchResult greedy_best_first_search(const Puzzle &puzzle, State start);
SearchResult astar_search(const Puzzle &puzzle, State start);
SearchResult idastar_search(const Puzzle &puzzle, State start);

#endif  // SEARCH_H
