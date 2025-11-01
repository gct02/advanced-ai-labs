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
#include <unordered_set>
#include <queue>
#include <stack>
#include <memory>
#include "heuristics.h"

#define BUFF_SIZE 300000000

Solution bfs_graph(State start_state, int size) {
  precompute_goal_positions(size);

  Solution sol;

  uint32_t node_counter = 0;
  uint8_t start_h = manhattan_dist(start_state, size);
  Node* start_node = new Node(start_state, 0, nullptr, size, start_h, node_counter++);

  sol.start_h = (int)start_h;
  sol.root = start_node;

  std::vector<State> neighbors_buffer;
  neighbors_buffer.reserve(4);

  if (start_node->h == 0) {
    sol.sol_depth = start_node->g;
    sol.path.push_back(start_state);
    return sol;
  }
  std::queue<Node*> frontier;
  std::unordered_set<State> closed_set;

  frontier.push(start_node);
  closed_set.insert(start_state);

  while (!frontier.empty()) {
    Node* curr_node = frontier.front();
    frontier.pop();
    sol.n_expanded++;

    State parent_state = curr_node->parent ? curr_node->parent->state : 0x0llu;
    neighbors_buffer.clear();
    get_neighbors(curr_node->state, size, parent_state, neighbors_buffer);

    for (const auto& nb : neighbors_buffer) {
      if (closed_set.find(nb) == closed_set.end()) {
        uint8_t h = manhattan_dist(nb, size);
        Node* nb_node = new Node(nb, curr_node->g + 1, curr_node, size, h, node_counter++);
        if (h == 0) {
          sol.sol_depth = nb_node->g;
          Node* temp = nb_node;
          while (temp != nullptr) {
            sol.path.push_back(temp->state);
            temp = temp->parent;
          }
          std::reverse(sol.path.begin(), sol.path.end());
          return sol;
        }
        frontier.push(nb_node);
        closed_set.insert(nb);
      }
    }
  }
  return sol;
}

Solution iter_deep(State start_state, int size) {
  precompute_goal_positions(size);

  Solution sol;

  uint32_t node_counter = 0;
  uint8_t start_h = manhattan_dist(start_state, size);
  Node* start_node = new Node(start_state, 0, nullptr, size, start_h, node_counter++);

  sol.start_h = (int)start_h;
  sol.root = start_node;

  std::vector<State> neighbors_buffer;
  neighbors_buffer.reserve(4);

  int depth_lim = 0;

  while (true) {
    std::stack<Node*> frontier;
    frontier.push(start_node);

    while (!frontier.empty()) {
      Node* curr_node = frontier.top();
      frontier.pop();

      if (curr_node->h == 0) {
        sol.sol_depth = curr_node->g;
        Node* temp = curr_node;
        while (temp != nullptr) {
          sol.path.push_back(temp->state);
          temp = temp->parent;
        }
        std::reverse(sol.path.begin(), sol.path.end());
        return sol;
      }
      if (curr_node->g > depth_lim) continue;

      sol.n_expanded++;

      State parent_state = curr_node->parent ? curr_node->parent->state : 0x0llu;
      neighbors_buffer.clear();
      get_neighbors(curr_node->state, size, parent_state, neighbors_buffer);
      std::reverse(neighbors_buffer.begin(), neighbors_buffer.end());

      for (const auto& nb : neighbors_buffer) {
        uint8_t h = manhattan_dist(nb, size);
        Node* nb_node = new Node(nb, curr_node->g + 1, curr_node, size, h, node_counter++);
        frontier.push(nb_node);
      }
    }
    if (depth_lim == UINT16_MAX) break;
    depth_lim++;
  }
  return sol;
}

Solution greedy_best_first(State start_state, int size) {
  precompute_goal_positions(size);

  Solution sol;

  uint32_t node_counter = 0;
  uint8_t start_h = manhattan_dist(start_state, size);
  Node* start_node = new Node(start_state, 0, nullptr, size, start_h, node_counter++);

  sol.start_h = (int)start_h;
  sol.root = start_node;

  int64_t n_explored = 1;
  int64_t h_sum = sol.start_h;

  if (start_node->h == 0) {
    sol.sol_depth = start_node->g;
    sol.avg_h = (double)h_sum;
    sol.path.push_back(start_state);
    return sol;
  }
  GreedyPQueue frontier(QUEUE_SIZE);
  std::unordered_map<State, uint8_t> closed_map;

  std::vector<State> neighbors_buffer;
  neighbors_buffer.reserve(4);

  frontier.push(start_node);
  closed_map[start_state] = start_h;

  while (!frontier.empty()) {
    Node* curr_node = frontier.top();
    frontier.pop();

    if (curr_node->h == 0) {
      sol.sol_depth = curr_node->g;
      sol.avg_h = h_sum / (double)n_explored;
      Node* temp = curr_node;
      while (temp != nullptr) {
        sol.path.push_back(temp->state);
        temp = temp->parent;
      }
      std::reverse(sol.path.begin(), sol.path.end());
      return sol;
    }
    sol.n_expanded++;

    State parent_state = curr_node->parent ? curr_node->parent->state : 0x0llu;
    neighbors_buffer.clear();
    get_neighbors(curr_node->state, size, parent_state, neighbors_buffer);

    for (const auto& nb : neighbors_buffer) {
      auto it = closed_map.find(nb);
      if (it != closed_map.end()) {
        h_sum += (int)it->second;
        n_explored++;
        continue;
      }
      uint8_t h = manhattan_dist(nb, size);
      
      h_sum += (int)h;
      n_explored++;

      Node* nb_node = new Node(nb, curr_node->g + 1, curr_node, size, h, node_counter++);
      frontier.push(nb_node);
      closed_map[nb] = h;
    }
  }
  sol.avg_h /= (double)node_counter;
  return sol;
}

Solution astar(State start_state, int size) {
  precompute_goal_positions(size);

  Solution sol;

  uint32_t node_counter = 0;
  uint8_t start_h = manhattan_dist(start_state, size);
  Node* start_node = new Node(start_state, 0, nullptr, size, start_h, node_counter++);

  sol.start_h = (int)start_h;
  sol.root = start_node;

  int64_t n_explored = 1;
  int64_t h_sum = start_node->h;

  std::vector<State> neighbors_buffer;
  neighbors_buffer.reserve(4);

  AstarPQueue frontier(QUEUE_SIZE);
  std::unordered_map<State, uint8_t> closed_map;
  frontier.push(start_node);

  while (!frontier.empty()) {
    Node* curr_node = frontier.top();
    frontier.pop();

    if (curr_node->h == 0) {
      sol.sol_depth = (int)curr_node->g;
      sol.avg_h = h_sum / (double)n_explored;
      Node* temp = curr_node;
      while (temp != nullptr) {
        sol.path.push_back(temp->state);
        temp = temp->parent;
      }
      std::reverse(sol.path.begin(), sol.path.end());
      return sol;
    }
    if (closed_map.find(curr_node->state) != closed_map.end()) {
      continue;
    }
    closed_map[curr_node->state] = curr_node->h;
    sol.n_expanded++;

    State parent_state = curr_node->parent ? curr_node->parent->state : 0x0llu;
    neighbors_buffer.clear();
    get_neighbors(curr_node->state, size, parent_state, neighbors_buffer);

    for (const auto& nb : neighbors_buffer) {
      auto it = closed_map.find(nb);
      if (it != closed_map.end()) {
        h_sum += (int)it->second;
        n_explored++;
        continue;
      }
      uint8_t h = manhattan_dist(nb, size);

      h_sum += (int)h;
      n_explored++;

      Node* nb_node = new Node(nb, curr_node->g + 1, curr_node, size, h, node_counter++);
      frontier.push(nb_node);
    }
  }
  sol.avg_h = h_sum / (double)n_explored;
  return sol;
}

Solution idastar(State start_state, int size) {
  precompute_goal_positions(size);

  Solution sol;

  uint32_t node_counter = 0;
  uint8_t start_h = manhattan_dist(start_state, size);
  Node* start_node = new Node(start_state, 0, nullptr, size, start_h, node_counter++);
  sol.start_h = (int)start_h;

  int64_t h_sum = 0;
  int64_t n_explored = 0;

  std::vector<State> neighbors_buffer;
  neighbors_buffer.reserve(4);

  uint16_t threshold = (uint16_t)start_node->g + start_node->h;

  std::vector<Node*> frontier;
  frontier.reserve(QUEUE_SIZE);

  while (true) {
    uint16_t next_threshold = UINT16_MAX;

    frontier.clear();
    frontier.push_back(start_node);

    while (!frontier.empty()) {
      Node* curr_node = frontier.back();
      frontier.pop_back();

      h_sum += (int)curr_node->h;
      n_explored++;

      if (curr_node->h == 0) {
        sol.sol_depth = (int)curr_node->g;
        sol.avg_h = h_sum / (double)n_explored;
        Node* temp = curr_node;
        while (temp != nullptr) {
          sol.path.push_back(temp->state);
          temp = temp->parent;
        }
        std::reverse(sol.path.begin(), sol.path.end());
        return sol;
      }
      uint16_t f = (uint16_t)curr_node->g + curr_node->h;
      if (f > threshold) {
        next_threshold = std::min(next_threshold, f);
        continue;
      }
      sol.n_expanded++;

      State parent_state = curr_node->parent ? curr_node->parent->state : 0x0llu;
      neighbors_buffer.clear();
      get_neighbors(curr_node->state, size, parent_state, neighbors_buffer);
      std::reverse(neighbors_buffer.begin(), neighbors_buffer.end());

      for (const auto& nb : neighbors_buffer) {
        uint8_t h = manhattan_dist(nb, size);
        Node* nb_node = new Node(nb, curr_node->g + 1, curr_node, size, h, node_counter++);
        frontier.push_back(nb_node);
      }
    }
    threshold = next_threshold;
    if (threshold == UINT16_MAX) break;
  }
  if (sol.n_expanded > 0) sol.avg_h /= (double)sol.n_expanded;
  return sol;
}