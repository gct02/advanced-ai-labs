#ifndef PLANOPT_HEURISTICS_H_RELAXED_TASK_GRAPH_H
#define PLANOPT_HEURISTICS_H_RELAXED_TASK_GRAPH_H

#include "../heuristic.h"
#include "relaxed_task_graph.h"

#include <memory>

namespace plugins {
class Options;
}
/*
    rm -rf build
    python3 build.py
    python3 fast-downward.py misc/tests/benchmarks/gripper/prob01.pddl --search "astar(planopt_relaxed_task_graph())"
*/
namespace planopt_heuristics {
class RelaxedTaskGraphHeuristic : public Heuristic {
    RelaxedTaskGraph relaxed_task_graph;
    int min_operator_cost;
protected:
    virtual int compute_heuristic(const State &ancestor_state) override;
public:
    //explicit RelaxedTaskGraphHeuristic(const options::Options &options);
    RelaxedTaskGraphHeuristic(
        const std::shared_ptr<AbstractTask> &transform, 
        bool cache_estimates,
        const std::string &description, 
        utils::Verbosity verbosity
    );
};
}

#endif
