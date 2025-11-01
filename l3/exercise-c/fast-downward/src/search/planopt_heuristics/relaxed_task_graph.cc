#include "relaxed_task_graph.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <queue>

using namespace std;

namespace planopt_heuristics {
RelaxedTaskGraph::RelaxedTaskGraph(const TaskProxy &task_proxy)
    : relaxed_task(task_proxy),
      variable_node_ids(relaxed_task.propositions.size()) {
    /*
      TODO: add your code for exercise 2 (b) here. Afterwards
        - variable_node_ids[i] should contain the node id of the variable node for variable i
        - initial_node_id should contain the node id of the initial node
        - goal_node_id should contain the node id of the goal node
        - the graph should contain precondition and effect nodes for all operators
        - the graph should contain all necessary edges.
    */

    for (auto var : task_proxy.get_variables()) {
        NodeID n = this->graph.add_node(NodeType::OR);
        this->variable_node_ids.push_back(n);
        this->id_map[var.get_id()] = n;
    }

    State initial_state = task_proxy.get_initial_state();
    this->initial_node_id = this->graph.add_node(NodeType::AND);

    for (auto fact : relaxed_task.initial_state) {
        NodeID n = this->id_map[fact];
        this->graph.add_edge(n, this->initial_node_id);
    }

    for (auto op : this->relaxed_task.operators) {
        NodeID precond_node_id = this->graph.add_node(NodeType::AND);

        for (auto precond : op.preconditions) {
            NodeID n = this->id_map[precond];
            this->graph.add_edge(precond_node_id, n);
        }

        NodeID effect_node_id = this->graph.add_node(NodeType::AND, op.cost);

        this->graph.add_edge(effect_node_id, precond_node_id);

        for (auto effect : op.effects) {
            NodeID n = this->id_map[effect];
            this->graph.add_edge(n, effect_node_id);
        }
    }

    this->goal_node_id = this->graph.add_node(NodeType::AND);

    for (auto fact : relaxed_task.goal) {
        NodeID n = this->id_map[fact];
        this->graph.add_edge(this->goal_node_id, n);
    }
}

void RelaxedTaskGraph::change_initial_state(const State &state) {
    // Remove all initial edges that where introduced for relaxed_task.initial_state.
    for (PropositionID id : relaxed_task.initial_state) {
        graph.remove_edge(variable_node_ids[id], initial_node_id);
    }
    // Switch initial state of relaxed_task
    relaxed_task.initial_state = relaxed_task.translate_state(state);

    // Add all initial edges for relaxed_task.initial_state.
    for (PropositionID id : relaxed_task.initial_state) {
        graph.add_edge(variable_node_ids[id], initial_node_id);
    }
}

bool RelaxedTaskGraph::is_goal_relaxed_reachable() {
    // Compute the most conservative valuation of the graph and use it to
    // return true iff the goal is reachable in the relaxed task.

    return false;
}

int RelaxedTaskGraph::additive_cost_of_goal() {
    // Compute the weighted most conservative valuation of the graph and use it
    // to return the h^add value of the goal node.

    // TODO: add your code for exercise 2 (c) here.

    this->graph.weighted_most_conservative_valuation();

    AndOrGraphNode goal_node = this->graph.get_node(this->goal_node_id);
    return goal_node.additive_cost;
}

int RelaxedTaskGraph::ff_cost_of_goal() {
    // TODO: add your code for exercise 2 (e) here.
    unordered_set<NodeID> collected;

    AndOrGraphNode goal_node = this->graph.get_node(this->goal_node_id);

    queue<NodeID> node_queue;
    unordered_set<NodeID> visited;

    visited.emplace(this->goal_node_id);

    for (auto s : goal_node.successor_ids) {
        node_queue.push(s);
    }

    while (!node_queue.empty()) {
        NodeID n = node_queue.front();
        node_queue.pop();

        visited.emplace(n);

        AndOrGraphNode node = this->graph.get_node(n);
        if (node.direct_cost > 0) {
            collected.emplace(n);
        }

        if (node.type == NodeType::OR) {
            // Collect only the achiever
            if (visited.find(node.achiever) == visited.end()) {
                node_queue.push(node.achiever);
            }
        } else {
            for (auto s : node.successor_ids) {
                if (visited.find(s) == visited.end()) {
                    node_queue.emplace(s);
                }
            }
        }
    }

    int ff_cost = 0;
    for (auto n : collected) {
        AndOrGraphNode node = this->graph.get_node(n);
        ff_cost += node.direct_cost;
    }

    return ff_cost;
}

}
