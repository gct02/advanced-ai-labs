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

#define MAX_DEPTH 1000
#define MAX_H 100

#define BUFF_SIZE 500000000
#define BUFF_SIZE_ID 500000
#define QUEUE_SIZE 5000

class AstarPQueue : public std::priority_queue<Node*, std::vector<Node*>, AstarCompare>
{
public:
  AstarPQueue(size_t reserve_size = QUEUE_SIZE)
  {
    this->c.reserve(reserve_size);
  }
};

class GreedyPQueue : public std::priority_queue<Node*, std::vector<Node*>, GreedyCompare>
{
public:
  GreedyPQueue(size_t reserve_size = QUEUE_SIZE)
  {
    this->c.reserve(reserve_size);
  }
};

Solution bfs_graph(State start_state, int size) {
  precompute_goal_positions(size);

  Solution sol;
  std::vector<Node> node_storage;
  node_storage.reserve(BUFF_SIZE);

  uint32_t node_counter = 0;
  uint8_t start_h = manhattan_dist(start_state, size);
  node_storage.emplace_back(start_state, 0, nullptr, size, start_h, node_counter++);
  Node* start_node = &node_storage.back();

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
      uint8_t h = manhattan_dist(nb, size);
      if (closed_set.find(nb) == closed_set.end()) {
        node_storage.emplace_back(nb, curr_node->g + 1, curr_node, size, h, node_counter++);
        Node* nb_node = &node_storage.back();
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
  std::vector<Node> node_storage;
  node_storage.reserve(BUFF_SIZE);

  uint32_t node_counter = 0;
  uint8_t start_h = manhattan_dist(start_state, size);
  node_storage.emplace_back(start_state, 0, nullptr, size, start_h, node_counter++);
  Node* start_node = &node_storage.back();

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
        node_storage.emplace_back(nb, curr_node->g + 1, curr_node, size, h, node_counter++);
        frontier.push(&node_storage.back());
      }
    }
    depth_lim++;
    if (depth_lim >= MAX_DEPTH) break;
  }
  return sol;
}

Solution greedy_best_first(State start_state, int size) {
  precompute_goal_positions(size);

  Solution sol;
  std::vector<Node> node_storage;
  node_storage.reserve(BUFF_SIZE);

  uint32_t node_counter = 0;
  uint8_t start_h = manhattan_dist(start_state, size);
  node_storage.emplace_back(start_state, 0, nullptr, size, start_h, node_counter++);
  Node* start_node = &node_storage.back();

  sol.start_h = (int)start_h;
  sol.root = start_node;

  int n_explored = 1;
  int h_sum = start_node->h;

  if (start_node->h == 0) {
    sol.sol_depth = start_node->g;
    sol.avg_h = (double)h_sum;
    sol.path.push_back(start_state);
    return sol;
  }
  GreedyPQueue frontier(QUEUE_SIZE);
  std::unordered_set<State> closed_set;

  std::vector<State> neighbors_buffer;
  neighbors_buffer.reserve(4);

  frontier.push(start_node);
  closed_set.insert(start_state);

  while (!frontier.empty()) {
    Node* curr_node = frontier.top();
    frontier.pop();

    sol.n_expanded++;

    State parent_state = curr_node->parent ? curr_node->parent->state : 0x0llu;
    neighbors_buffer.clear();
    get_neighbors(curr_node->state, size, parent_state, neighbors_buffer);

    for (const auto& nb : neighbors_buffer) {
      if (closed_set.find(nb) == closed_set.end()) {
        uint8_t h = manhattan_dist(nb, size);
        
        h_sum += (int)h;
        n_explored++;

        node_storage.emplace_back(nb, curr_node->g + 1, curr_node, size, h, node_counter++);
        Node* nb_node = &node_storage.back();

        if (h == 0) {
          sol.sol_depth = nb_node->g;
          sol.avg_h = h_sum / (double)n_explored;
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
  sol.avg_h /= (double)node_counter;
  return sol;
}

Solution astar(State start_state, int size) {
  precompute_goal_positions(size);

  Solution sol;
  std::vector<Node> node_storage;
  node_storage.reserve(BUFF_SIZE);

  uint32_t node_counter = 0;
  uint8_t start_h = manhattan_dist(start_state, size);
  node_storage.emplace_back(start_state, 0, nullptr, size, start_h, node_counter++);
  Node* start_node = &node_storage.back();

  sol.start_h = (int)start_h;
  sol.root = start_node;

  int n_explored = 1;
  int h_sum = start_node->h;

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
      node_storage.emplace_back(nb, curr_node->g + 1, curr_node, size, h, node_counter++);
      frontier.push(&node_storage.back());
    }
  }
  sol.avg_h = h_sum / (double)n_explored;
  return sol;
}

Solution idastar(State start_state, int size) {
  precompute_goal_positions(size);

  Solution sol;
  std::unordered_map<State, Node*> node_store;
  node_store.reserve(BUFF_SIZE_ID);

  uint32_t node_counter = 0;
  uint8_t start_h = manhattan_dist(start_state, size);
  Node* start_node = new Node(start_state, 0, nullptr, size, start_h, node_counter++);
  node_store[start_state] = start_node;

  sol.start_h = (int)start_h;

  // int h_sum = sol.start_h;
  // int n_explored = 1;
  int h_sum = 0;
  int n_explored = 0;

  std::vector<State> neighbors_buffer;
  neighbors_buffer.reserve(4);

  uint16_t threshold = start_node->f;

  std::vector<Node*> frontier;
  frontier.reserve(QUEUE_SIZE);

  while (true) {
    uint16_t next_threshold = UINT16_MAX;

    frontier.clear();
    frontier.push_back(start_node);

    // h_sum += sol.start_h;
    // n_explored += 1;

    while (!frontier.empty()) {
      Node* curr_node = frontier.back();
      frontier.pop_back();

      h_sum += curr_node->h;
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
        for (auto& it : node_store) {
          delete it.second;
        }
        node_store.clear();
        return sol;
      }
      if (curr_node->f > threshold) {
        next_threshold = std::min(next_threshold, curr_node->f);
        continue;
      }
      sol.n_expanded++;

      State parent_state = curr_node->parent ? curr_node->parent->state : 0x0llu;
      neighbors_buffer.clear();
      get_neighbors(curr_node->state, size, parent_state, neighbors_buffer);
      std::reverse(neighbors_buffer.begin(), neighbors_buffer.end());

      for (const auto& nb : neighbors_buffer) {
        // auto it = node_store.find(nb);
        // if (it != node_store.end() && it->second->g < curr_node->g + 1) {
        //   continue;
        // }
        uint8_t h = manhattan_dist(nb, size);

        // h_sum += (int)h;
        // n_explored++;

        Node* nb_node = new Node(nb, curr_node->g + 1, curr_node, size, h, node_counter++);
        node_store[nb] = nb_node;
        frontier.push_back(nb_node);
      }
    }
    threshold = next_threshold;
    if (threshold == UINT16_MAX) break;
  }
  if (sol.n_expanded > 0) sol.avg_h /= (double)sol.n_expanded;
  return sol;
}