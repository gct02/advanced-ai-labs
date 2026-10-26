#include "h_canonical_pdbs.h"

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"
    #include <memory>
#include <string>

using namespace std;

namespace planopt_heuristics {

CanonicalPDBsHeuristic::CanonicalPDBsHeuristic(
    const std::shared_ptr<AbstractTask> &transform, 
    bool cache_estimates,
    const std::string &description, 
    utils::Verbosity verbosity,
    const std::vector<std::vector<int>> &patterns
)
    : Heuristic(transform, cache_estimates, description, verbosity),
      pdbs(create_tnf_task(task_proxy), patterns) {
}

int CanonicalPDBsHeuristic::compute_heuristic(const State &ancestor_state) {
    State state = convert_ancestor_state(ancestor_state);
    TNFState tnf_state = state.get_unpacked_values();
    int h = pdbs.compute_heuristic(tnf_state);
    if (h == numeric_limits<int>::max()) {
        return DEAD_END;
    } else {
        return h;
    }
}

class CanonicalPDBsHeuristicFeature
    : public plugins::TypedFeature<Evaluator, CanonicalPDBsHeuristic> {
public:
    using Feature::add_option;
    using Feature::add_list_option;
    using Feature::document_title;
    using Feature::document_synopsis;
    using Feature::document_language_support;
    using Feature::document_property;

    CanonicalPDBsHeuristicFeature() : TypedFeature("planopt_cpdbs") {
        document_title("Planopt Canonical PDBs Heuristic");
        document_synopsis(
            "Heuristic based on canonical pattern databases. Accepts a list of patterns, "
            "each pattern being a list of variable indices.");

        add_list_option<vector<int>>(
            "patterns", "List of patterns, each a list of integers representing the pattern.");

        add_heuristic_options_to_feature(*this, "planopt_cpdbs");
        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");
        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<CanonicalPDBsHeuristic> create_component(
        const plugins::Options &opts,
        const utils::Context &) const override {
        return plugins::make_shared_from_arg_tuples<CanonicalPDBsHeuristic>(
            get_heuristic_arguments_from_options(opts),
            opts.get_list<vector<int>>("patterns")
        );
    }
};

static plugins::FeaturePlugin<CanonicalPDBsHeuristicFeature> _plugin;

}  // namespace planopt_heuristics
