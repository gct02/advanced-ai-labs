#include "h_add.h"

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"

#include "../utils/logging.h"
#include "../utils/memory.h"

using namespace std;

namespace planopt_heuristics {
HAddHeuristic::HAddHeuristic(
    const shared_ptr<AbstractTask> &transform, 
    bool cache_estimates,
    const string &description, 
    utils::Verbosity verbosity
    )
    : Heuristic(transform, cache_estimates, description, verbosity),
    relaxed_task_graph(task_proxy){
}

int HAddHeuristic::compute_heuristic(const State &ancestor_state) {
    State state = convert_ancestor_state(ancestor_state);
    relaxed_task_graph.change_initial_state(state);
    if (relaxed_task_graph.is_goal_relaxed_reachable()) {
        return relaxed_task_graph.additive_cost_of_goal();
    } else {
        return DEAD_END;
    }
}

class HAddHeuristicFeature
    : public plugins::TypedFeature<Evaluator, HAddHeuristic> {
public:
    HAddHeuristicFeature() : TypedFeature("planopt_add") {
        document_title("Planopt ADD Heuristic");
        document_synopsis(
            "Calculates heuristic based on additive cost of goal. "
            "0 for goal states, or DEAD_END if unreachable.");

        add_heuristic_options_to_feature(*this, "planopt_add");
        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "no");
        document_property("consistent", "no");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<HAddHeuristic> create_component(
        const plugins::Options &opts,
        const utils::Context &) const override {
        return plugins::make_shared_from_arg_tuples<HAddHeuristic>(
            get_heuristic_arguments_from_options(opts)
            );
    }
};

static plugins::FeaturePlugin<HAddHeuristicFeature> _plugin;
}