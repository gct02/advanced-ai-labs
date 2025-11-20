#include "h_pdb.h"

//#include "../option_parser.h"
#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"
using namespace std;

namespace planopt_heuristics {
PDBHeuristic::PDBHeuristic(
    const std::vector<int> &pattern,
    const shared_ptr<AbstractTask> &transform, 
    bool cache_estimates,
    const string &description, 
    utils::Verbosity verbosity
    )
    : Heuristic(transform, cache_estimates, description, verbosity),
    pdb(create_tnf_task(task_proxy), pattern) {
}

int PDBHeuristic::compute_heuristic(const State &ancestor_state) {
    State state = convert_ancestor_state(ancestor_state);
    TNFState tnf_state = state.get_unpacked_values();
    int h = pdb.lookup_distance(tnf_state);
    if (h == numeric_limits<int>::max()) {
        return DEAD_END;
    } else {
        return h;
    }
}

class PDBHeuristicFeature
    : public plugins::TypedFeature<Evaluator, PDBHeuristic> {
public:
    PDBHeuristicFeature() : TypedFeature("planopt_pdb") {
        document_title("Planopt PDB Heuristic");
        document_synopsis(
            "Heuristic based on pattern databases. Accepts a list of integers "
            "as a pattern for abstraction.");

        
        add_list_option<int>("pattern", "List of integers representing the pattern.");

        add_heuristic_options_to_feature(*this, "planopt_pdb");
        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<PDBHeuristic> create_component(
        const plugins::Options &opts,
        const utils::Context &) const override {
        return plugins::make_shared_from_arg_tuples<PDBHeuristic>(
            opts.get_list<int>("pattern"),
            get_heuristic_arguments_from_options(opts)
        );
    }
};

static plugins::FeaturePlugin<PDBHeuristicFeature> _plugin;
}