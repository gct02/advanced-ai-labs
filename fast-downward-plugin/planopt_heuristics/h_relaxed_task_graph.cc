
#include "h_relaxed_task_graph.h"

//#include "../option_parser.h"

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

#include "../utils/logging.h"
#include "../utils/memory.h"


using namespace std;

namespace planopt_heuristics {
RelaxedTaskGraphHeuristic::RelaxedTaskGraphHeuristic(
        const shared_ptr<AbstractTask> &transform, 
        bool cache_estimates,
        const string &description, 
        utils::Verbosity verbosity
    )
    : Heuristic(transform, cache_estimates, description, verbosity),
    relaxed_task_graph(task_proxy),
    min_operator_cost(task_properties::get_min_operator_cost(task_proxy)){
}

int RelaxedTaskGraphHeuristic::compute_heuristic(const State &ancestor_state) {
    State state = convert_ancestor_state(ancestor_state);
    relaxed_task_graph.change_initial_state(state);
    if (relaxed_task_graph.is_goal_relaxed_reachable()) {
        if (task_properties::is_goal_state(task_proxy, state)) {
            return 0;
        } else {
            return min_operator_cost;
        }
    } else {
        return DEAD_END;
    }
}

class RelaxedTaskGraphHeuristicFeature
    : public plugins::TypedFeature<Evaluator, RelaxedTaskGraphHeuristic> {
public:
    RelaxedTaskGraphHeuristicFeature() : TypedFeature("planopt_relaxed_task_graph") {
        document_title("Relaxed Task Graph Heuristic");
        document_synopsis(
            "Calculates heuristic based on relaxed task graph reachability. "
            "0 for goal states, or DEAD_END if unreachable.");

        add_heuristic_options_to_feature(*this, "planopt_relaxed_task_graph");
        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "no");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<RelaxedTaskGraphHeuristic> create_component(
        const plugins::Options &opts,
        const utils::Context &) const override {
        return plugins::make_shared_from_arg_tuples<RelaxedTaskGraphHeuristic>(
            get_heuristic_arguments_from_options(opts)
            );
    }
};

static plugins::FeaturePlugin<RelaxedTaskGraphHeuristicFeature> _plugin;
}

