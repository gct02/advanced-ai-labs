#include "pdb.h"

#include "../utils/logging.h"

#include <queue>
#include <limits>

namespace planopt_heuristics {

/*
  An entry in the queue is a tuple (h, i) where h is the goal distance of state i.
  See comments below for details.
*/
using QueueEntry = std::pair<int, int>;

PatternDatabase::PatternDatabase(const TNFTask &task, const Pattern &pattern)
    : projection(task, pattern) {
    /*
      We want to compute goal distances for all abstract states in the
      projected task. To do so, we start by assuming every abstract state has
      an infinite goal distance and then do a backwards uniform cost search
      updating the goal distances of all encountered states.

      Instead of searching on the actual states, we use perfect hashing to
      run the search on the hash indices of states. To go from a state s to its
      index use rank(s) and to go from an index i to its state use unrank(i).
    */
    const TNFTask &projected_task = projection.get_projected_task();
    distances.resize(projected_task.get_num_states(), std::numeric_limits<int>::max());

    /*
      Priority queues usually order entries so the largest entry is the first.
      By using the comparator greater<T> instead of the default less<T>, we
      change the ordering to sort the smallest element first.
    */
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> queue;
    /*
      Note that we start with the goal state to turn the search into a regression.
      We also have to switch the role of precondition and effect in operators
      later on. This is sufficient to turn the search into a regression since
      the task is in TNF.
    */
    int goal_index = projection.rank_state(projected_task.goal_state);
    distances[goal_index] = 0;
    queue.push({0, goal_index});

    // Lab 4, exercise 2(b): Dijkstra's algorithm on the regression. In TNF, an
    // operator leads into state s iff s agrees with all of its effect values,
    // and the predecessor replaces those values by the precondition values.
    while (!queue.empty()) {
        auto [distance, index] = queue.top();
        queue.pop();
        if (distance > distances[index]) {
            continue;  // outdated entry
        }

        const TNFState state = projection.unrank_state(index);
        for (const TNFOperator &op : projected_task.operators) {
            bool leads_into_state = true;
            for (const TNFOperatorEntry &entry : op.entries) {
                if (state[entry.variable_id] != entry.effect_value) {
                    leads_into_state = false;
                    break;
                }
            }
            if (!leads_into_state) {
                continue;
            }

            TNFState predecessor = state;
            for (const TNFOperatorEntry &entry : op.entries) {
                predecessor[entry.variable_id] = entry.precondition_value;
            }
            int predecessor_index = projection.rank_state(predecessor);
            int new_distance = distance + op.cost;
            if (new_distance < distances[predecessor_index]) {
                distances[predecessor_index] = new_distance;
                queue.push({new_distance, predecessor_index});
            }
        }
    }
}

int PatternDatabase::lookup_distance(const TNFState &original_state) const {
    TNFState abstract_state = projection.project_state(original_state);
    int index = projection.rank_state(abstract_state);
    return distances[index];
}

}
