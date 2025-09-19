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
  Solution sol;

  std::vector<Node> node_storage;
  node_storage.reserve(BUFF_SIZE);

  uint64_t node_counter = 0;
  node_storage.emplace_back(start_state, 0, nullptr, size, node_counter++);
  Node* start_node = &node_storage.back();

  sol.start_h = (int)start_node->h;
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
        node_storage.emplace_back(nb, curr_node->g + 1, curr_node, size, node_counter++);
        Node* nb_node = &node_storage.back();
        if (nb_node->h == 0) {
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
  Solution sol;
  std::vector<Node> node_storage;
  node_storage.reserve(BUFF_SIZE);

  uint64_t node_counter = 0;
  node_storage.emplace_back(start_state, 0, nullptr, size, node_counter++);
  Node* start_node = &node_storage.back();

  sol.start_h = (int)start_node->h;
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
        node_storage.emplace_back(nb, curr_node->g + 1, curr_node, size, node_counter++);
        frontier.push(&node_storage.back());
      }
    }
    depth_lim++;
    if (depth_lim >= MAX_DEPTH) break;
  }
  return sol;
}

Solution greedy_best_first(State start_state, int size) {
  Solution sol;
  std::vector<Node> node_storage;
  node_storage.reserve(BUFF_SIZE);

  uint64_t node_counter = 0;
  node_storage.emplace_back(start_state, 0, nullptr, size, node_counter++);
  Node* start_node = &node_storage.back();

  sol.start_h = (int)start_node->h;
  sol.avg_h += (double)start_node->h;
  sol.root = start_node;

  if (start_node->h == 0) {
    sol.sol_depth = start_node->g;
    sol.avg_h /= (float)node_counter;
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
        node_storage.emplace_back(nb, curr_node->g + 1, curr_node, size, node_counter++);
        Node* nb_node = &node_storage.back();
        sol.avg_h += (double)nb_node->h;
        if (nb_node->h == 0) {
          sol.sol_depth = nb_node->g;
          sol.avg_h /= (double)node_counter;
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
  Solution sol;
  std::vector<Node> node_storage;
  node_storage.reserve(BUFF_SIZE);

  uint32_t node_counter = 0;
  node_storage.emplace_back(start_state, 0, nullptr, size, node_counter++);
  Node* start_node = &node_storage.back();

  sol.start_h = (int)start_node->h;
  sol.root = start_node;

  int n_explored = 1;
  int64_t h_sum = (int64_t)start_node->h;

  std::vector<State> neighbors_buffer;
  neighbors_buffer.reserve(4);

  AstarPQueue frontier(QUEUE_SIZE);
  std::unordered_set<State> closed_set;
  frontier.push(start_node);

  while (!frontier.empty()) {
    Node* curr_node = frontier.top();
    frontier.pop();

    h_sum += (int64_t)curr_node->h;
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
    if (closed_set.find(curr_node->state) != closed_set.end()) {
      continue;
    }
    closed_set.insert(curr_node->state);
    sol.n_expanded++;

    State parent_state = curr_node->parent ? curr_node->parent->state : 0x0llu;
    neighbors_buffer.clear();
    get_neighbors(curr_node->state, size, parent_state, neighbors_buffer);

    for (const auto& nb : neighbors_buffer) {
      if (closed_set.find(nb) != closed_set.end()) {
        continue;
      }
      node_storage.emplace_back(nb, curr_node->g + 1, curr_node, size, node_counter++);
      Node* nb_node = &node_storage.back();
      frontier.push(nb_node);
      h_sum += (int64_t)nb_node->h;
      n_explored++;
    }
  }
  sol.avg_h = h_sum / (double)n_explored;
  return sol;
}

Solution idastar(State start_state, int size) {
  Solution sol;
  std::unordered_map<State, Node*> node_store;
  node_store.reserve(BUFF_SIZE_ID);

  uint32_t node_counter = 0;
  node_store[start_state] = new Node(start_state, 0, nullptr, size, node_counter++);
  Node* start_node = node_store[start_state];

  sol.start_h = (int)start_node->h;
  sol.avg_h += (double)start_node->h;

  std::vector<State> neighbors_buffer;
  neighbors_buffer.reserve(4);

  uint16_t threshold = start_node->f;

  std::vector<Node*> frontier;
  frontier.reserve(QUEUE_SIZE);

  while (true) {
    uint16_t next_threshold = UINT16_MAX;

    frontier.clear();
    frontier.push_back(start_node);

    while (!frontier.empty()) {
      Node* curr_node = frontier.back();
      frontier.pop_back();

      if (curr_node->h == 0) {
        sol.sol_depth = (int)curr_node->g;
        if (sol.n_expanded > 0) sol.avg_h /= (double)sol.n_expanded;
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
        auto it = node_store.find(nb);
        if (it != node_store.end() && it->second->g < curr_node->g + 1) {
          continue;
        }
        node_store[nb] = new Node(nb, curr_node->g + 1, curr_node, size, node_counter++);
        Node* nb_node = node_store[nb];
        sol.avg_h += (double)nb_node->h;
        frontier.push_back(nb_node);
      }
    }
    threshold = next_threshold;
    if (threshold == UINT16_MAX) break;
  }
  if (sol.n_expanded > 0) sol.avg_h /= (double)sol.n_expanded;
  return sol;
}