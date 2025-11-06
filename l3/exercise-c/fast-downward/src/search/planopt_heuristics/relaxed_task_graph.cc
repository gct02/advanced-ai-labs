#include "relaxed_task_graph.h"

#include <iostream>
#include <vector>
#include <unordered_map>
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

    for (Proposition prop : relaxed_task.propositions) {
        variable_node_ids[prop.id] = graph.add_node(NodeType::OR);
    }

    initial_node_id = graph.add_node(NodeType::AND);

    for (PropositionID prop_id : relaxed_task.initial_state) {
        graph.add_edge(variable_node_ids[prop_id], initial_node_id);
    }

    goal_node_id = graph.add_node(NodeType::AND);

    for (PropositionID id : relaxed_task.goal) {
        graph.add_edge(goal_node_id, variable_node_ids[id]);
    }

    NodeID tautology_id = -1;

    for (RelaxedOperator op : relaxed_task.operators) {
        NodeID op_node_id = graph.add_node(NodeType::AND, op.cost);

        if (op.preconditions.size() == 0) { // Tautology
            if (tautology_id == -1) {
                tautology_id = graph.add_node(NodeType::AND);
            }
            graph.add_edge(op_node_id, tautology_id);
        } else {
            for (PropositionID precond_id : op.preconditions) {
                graph.add_edge(op_node_id, variable_node_ids[precond_id]);
            }
        }

        for (PropositionID eff_id : op.effects) {
            graph.add_edge(variable_node_ids[eff_id], op_node_id);
        }
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

    graph.most_conservative_valuation();
    const AndOrGraphNode& goal_node = graph.get_node(goal_node_id);

    return goal_node.forced_true;
}

int RelaxedTaskGraph::additive_cost_of_goal() {
    // Compute the weighted most conservative valuation of the graph and use it
    // to return the h^add value of the goal node.

    // TODO: add your code for exercise 2 (c) here.

    graph.weighted_most_conservative_valuation();
    const AndOrGraphNode& goal_node = graph.get_node(goal_node_id);

    return goal_node.additive_cost;
}

int RelaxedTaskGraph::ff_cost_of_goal() {
    // TODO: add your code for exercise 2 (e) here.

    graph.weighted_most_conservative_valuation();

    unordered_set<NodeID> collected;
    unordered_set<NodeID> visited;
    queue<NodeID> node_queue;

    AndOrGraphNode goal_node = graph.get_node(goal_node_id);

    visited.emplace(goal_node_id);

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
