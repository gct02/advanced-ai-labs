/*
 * heuristics.cpp
 *
 * Autores: Gabriel C. Tavares
 * Data: 2025-9-5
 * Descrição: Implementação das heurísticas BFS-Graph, Iterative Deepening, A*,
 * IDA* e Greedy Best-First Search para os problemas 8-puzzle e 15-puzzle.
 */

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <stack>
#include <memory>
#include <chrono>
#include "heuristics.h"

using timer = std::chrono::high_resolution_clock;
using time_point = std::chrono::high_resolution_clock::time_point;
using duration = std::chrono::duration<double>;

#define MAX_DEPTH 1000
#define MAX_H 100


double get_elapsed_time(time_point start_time) {
  time_point end_time = timer::now();
  duration elapsed = end_time - start_time;
  return elapsed.count();
}

Solution bfs_graph(const State& start_state) {
  Solution sol;
  time_point start_time = timer::now();

  std::shared_ptr<Node> start_node(new Node(start_state, 0, nullptr));
  sol.start_h = start_node->h;

  if (start_node->is_goal) {
    sol.time = get_elapsed_time(start_time);
    sol.sol_depth = start_node->g;
    sol.path.push_front(start_state);
    return sol;
  }
  std::queue<std::shared_ptr<Node>> frontier;
  std::unordered_map<State, bool, StateHasher> explored;

  frontier.push(start_node);
  explored[start_state] = true;

  while (!frontier.empty()) {
    std::shared_ptr<Node> curr_node = frontier.front();
    frontier.pop();
    sol.n_expanded++;

    for (const auto& nb : get_neighbors(curr_node->state)) {
      if (explored.find(nb) == explored.end()) {
        std::shared_ptr<Node> nb_node(new Node(nb, curr_node->g + 1, curr_node));
        if (nb_node->is_goal) {
          sol.time = get_elapsed_time(start_time);
          sol.sol_depth = nb_node->g;
          std::shared_ptr<Node> temp = nb_node;
          while (temp != nullptr) {
            sol.path.push_front(temp->state);
            temp = temp->parent;
          }
          return sol;
        }
        frontier.push(nb_node);
        explored[nb] = true;
      }
    }
  }
  sol.time = get_elapsed_time(start_time);
  return sol;
}

Solution iter_deep(const State& start_state) {
  Solution sol;
  time_point start_time = timer::now();

  std::shared_ptr<Node> start_node(new Node(start_state, 0, nullptr));
  sol.start_h = start_node->h;
  sol.root = start_node;

  int depth_lim = 0;

  while (true) {
    std::stack<std::shared_ptr<Node>> frontier;
    frontier.push(start_node);

    while (!frontier.empty()) {
      std::shared_ptr<Node> curr_node = frontier.top();
      frontier.pop();

      if (curr_node->is_goal) {
        sol.sol_depth = curr_node->g;
        sol.time = get_elapsed_time(start_time);
        std::shared_ptr<Node> temp = curr_node;
        while (temp != nullptr) {
          sol.path.push_front(temp->state);
          temp = temp->parent;
        }
        return sol;
      }
      if (curr_node->g > depth_lim) continue;

      sol.n_expanded++;

      const State* parent_state = curr_node->parent ? &curr_node->parent->state : nullptr;
      auto neighbors = get_neighbors(curr_node->state, parent_state);
      std::reverse(neighbors.begin(), neighbors.end());

      for (const auto& nb : neighbors) {
        std::shared_ptr<Node> nb_node(new Node(nb, curr_node->g + 1, curr_node));
        frontier.push(nb_node);
      }
    }
    depth_lim++;
    if (depth_lim >= MAX_DEPTH) break;
  }
  sol.time = get_elapsed_time(start_time);
  return sol;
}

Solution greedy_best_first(const State& start_state) {
  Solution sol;
  time_point start_time = timer::now();

  uint64_t node_counter = 0;
  std::shared_ptr<Node> start_node(new Node(start_state, 0, nullptr, node_counter++));
  sol.start_h = start_node->h;
  sol.avg_h += start_node->h;
  sol.root = start_node;

  if (start_node->is_goal) {
    sol.time = get_elapsed_time(start_time);
    sol.sol_depth = start_node->g;
    sol.avg_h /= (float)node_counter;
    sol.path.push_front(start_state);
    return sol;
  }
  std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, GreedyCompare> frontier;
  std::unordered_map<State, bool, StateHasher> explored;

  frontier.push(start_node);
  explored[start_state] = true;

  while (!frontier.empty()) {
    std::shared_ptr<Node> curr_node = frontier.top();
    frontier.pop();

    sol.n_expanded++;

    for (const auto& nb : get_neighbors(curr_node->state)) {
      if (explored.find(nb) == explored.end()) {
        std::shared_ptr<Node> nb_node(new Node(nb, curr_node->g + 1, curr_node, node_counter++));
        sol.avg_h += nb_node->h;
        if (nb_node->is_goal) {
          sol.time = get_elapsed_time(start_time);
          sol.sol_depth = nb_node->g;
          sol.avg_h /= (float)node_counter;
          std::shared_ptr<Node> temp = nb_node;
          while (temp != nullptr) {
            sol.path.push_front(temp->state);
            temp = temp->parent;
          }
          return sol;
        }
        frontier.push(nb_node);
        explored[nb] = true;
      }
    }
  }
  sol.time = get_elapsed_time(start_time);
  if (sol.n_expanded > 0) sol.avg_h /= (float)sol.n_expanded;
  return sol;
}

Solution astar(const State& start_state) {
  Solution sol;
  time_point start_time = timer::now();

  uint64_t node_counter = 0;
  std::shared_ptr<Node> start_node(new Node(start_state, 0, nullptr, node_counter++));
  sol.start_h = start_node->h;
  sol.avg_h = start_node->h;
  sol.root = start_node;

  if (start_node->is_goal) {
    sol.time = get_elapsed_time(start_time);
    sol.sol_depth = start_node->g;
    sol.path.push_front(start_state);
    return sol;
  }
  std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, AstarCompare> frontier;
  std::unordered_map<State, int, StateHasher> closed_set;
  frontier.push(start_node);

  while (!frontier.empty()) {
    std::shared_ptr<Node> curr_node = frontier.top();
    frontier.pop();

    if (curr_node->is_goal) {
      sol.time = get_elapsed_time(start_time);
      sol.sol_depth = curr_node->g;
      if (sol.n_expanded > 0) sol.avg_h /= (float)node_counter;
      std::shared_ptr<Node> temp = curr_node;
      while (temp != nullptr) {
        sol.path.push_front(temp->state);
        temp = temp->parent;
      }
      return sol;
    }
    auto it = closed_set.find(curr_node->state);
    if (it != closed_set.end()) {
      continue;
    }
    closed_set[curr_node->state] = curr_node->g;
    sol.n_expanded++;

    const State* parent_state = curr_node->parent ? &curr_node->parent->state : nullptr;
    auto neighbors = get_neighbors(curr_node->state, parent_state);

    for (const auto& nb : neighbors) {
      int new_g_cost = curr_node->g + 1;
      auto closed_it = closed_set.find(nb);
      if (closed_it != closed_set.end()) {
        continue;
      }
      std::shared_ptr<Node> nb_node(new Node(nb, curr_node->g + 1, curr_node, node_counter++));
      frontier.push(nb_node);
      sol.avg_h += nb_node->h;
    }
  }
  sol.time = get_elapsed_time(start_time);
  if (sol.n_expanded > 0) sol.avg_h /= (float)sol.n_expanded;
  return sol;
}

Solution idastar(const State& start_state) {
  Solution sol;
  time_point start_time = timer::now();

  uint64_t node_counter = 0;
  std::shared_ptr<Node> start_node(new Node(start_state, 0, nullptr, node_counter++));
  sol.start_h = start_node->h;
  sol.avg_h += start_node->h;

  int threshold = start_node->f;

  while (true) {
    std::stack<std::shared_ptr<Node>> frontier;
    int next_threshold = INT32_MAX;

    frontier.push(start_node);

    while (!frontier.empty()) {
      std::shared_ptr<Node> curr_node = frontier.top();
      frontier.pop();

      if (curr_node->is_goal) {
        sol.time = get_elapsed_time(start_time);
        sol.sol_depth = curr_node->g;
        sol.avg_h /= (float)sol.n_expanded;
        std::shared_ptr<Node> temp = curr_node;
        while (temp != nullptr) {
          sol.path.push_front(temp->state);
          temp = temp->parent;
        }
        return sol;
      }
      if (curr_node->f > threshold) {
        next_threshold = std::min(next_threshold, curr_node->f);
        continue;
      }
      sol.avg_h += curr_node->h;
      sol.n_expanded++;

      const State* parent_state = curr_node->parent ? &curr_node->parent->state : nullptr;
      auto neighbors = get_neighbors(curr_node->state, parent_state);
      std::reverse(neighbors.begin(), neighbors.end());

      for (const auto& nb : neighbors) {
        std::shared_ptr<Node> nb_node(new Node(nb, curr_node->g + 1, curr_node, node_counter++));
        frontier.push(nb_node);
      }
    }
    threshold = next_threshold;
    if (threshold == INT32_MAX) break;
  }
  sol.time = get_elapsed_time(start_time);
  if (sol.n_expanded > 0) sol.avg_h /= (float)sol.n_expanded;
  return sol;
}