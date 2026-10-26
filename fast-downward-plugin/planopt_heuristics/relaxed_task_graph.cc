#include "relaxed_task_graph.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

namespace planopt_heuristics {
RelaxedTaskGraph::RelaxedTaskGraph(const TaskProxy &task_proxy)
    : relaxed_task(task_proxy),
      variable_node_ids(relaxed_task.propositions.size()) {
    /*
      Lab 3, exercise 2(b). Afterwards
        - variable_node_ids[i] contains the node id of the variable node for variable i
        - initial_node_id contains the node id of the initial node
        - goal_node_id contains the node id of the goal node
        - the graph contains precondition and effect nodes for all operators
        - the graph contains all necessary edges.

      Each operator is a single AND node whose successors are its
      preconditions and whose direct cost is the operator cost (exercise 2(c));
      each of its effects is an OR node with an edge to it. Operators without
      preconditions point to one shared AND node without successors, which is
      always forced true.
    */
    for (const Proposition &prop : relaxed_task.propositions) {
        variable_node_ids[prop.id] = graph.add_node(NodeType::OR);
    }

    initial_node_id = graph.add_node(NodeType::AND);
    for (PropositionID prop_id : relaxed_task.initial_state) {
        graph.add_edge(variable_node_ids[prop_id], initial_node_id);
    }

    goal_node_id = graph.add_node(NodeType::AND);
    for (PropositionID prop_id : relaxed_task.goal) {
        graph.add_edge(goal_node_id, variable_node_ids[prop_id]);
    }

    NodeID no_precondition_id = -1;
    for (const RelaxedOperator &op : relaxed_task.operators) {
        NodeID op_node_id = graph.add_node(NodeType::AND, op.cost);
        if (op.preconditions.empty()) {
            if (no_precondition_id == -1) {
                no_precondition_id = graph.add_node(NodeType::AND);
            }
            graph.add_edge(op_node_id, no_precondition_id);
        }
        for (PropositionID prop_id : op.preconditions) {
            graph.add_edge(op_node_id, variable_node_ids[prop_id]);
        }
        for (PropositionID prop_id : op.effects) {
            graph.add_edge(variable_node_ids[prop_id], op_node_id);
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
    return graph.get_node(goal_node_id).forced_true;
}

int RelaxedTaskGraph::additive_cost_of_goal() {
    // Compute the weighted most conservative valuation of the graph and use it
    // to return the h^add value of the goal node (Lab 3, exercise 2(c)).
    graph.weighted_most_conservative_valuation();
    return graph.get_node(goal_node_id).additive_cost;
}

int RelaxedTaskGraph::ff_cost_of_goal() {
    /*
      Lab 3, exercise 2(e). Collect the best-achiever subgraph of the goal:
      all successors of every AND node reached, and the stored best achiever
      of every OR node reached. h^FF is the sum of the direct costs of the
      collected nodes, so every operator in the relaxed plan counts once.
    */
    graph.weighted_most_conservative_valuation();

    unordered_set<NodeID> collected{goal_node_id};
    vector<NodeID> stack{goal_node_id};
    int ff_cost = 0;
    while (!stack.empty()) {
        const AndOrGraphNode &node = graph.get_node(stack.back());
        stack.pop_back();
        ff_cost += node.direct_cost;

        vector<NodeID> next;
        if (node.type == NodeType::OR) {
            next.push_back(node.achiever);
        } else {
            next = node.successor_ids;
        }
        for (NodeID succ_id : next) {
            if (collected.insert(succ_id).second) {
                stack.push_back(succ_id);
            }
        }
    }
    return ff_cost;
}

}
