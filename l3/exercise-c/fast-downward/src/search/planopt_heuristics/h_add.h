#ifndef PLANOPT_HEURISTICS_H_ADD_H
#define PLANOPT_HEURISTICS_H_ADD_H

#include "../heuristic.h"
#include "relaxed_task_graph.h"
#include <memory>

namespace plugins {
class Options;
}

namespace planopt_heuristics {
class HAddHeuristic : public Heuristic {
    RelaxedTaskGraph relaxed_task_graph;
protected:
    virtual int compute_heuristic(const State &state) override;
public:
    HAddHeuristic(
        const std::shared_ptr<AbstractTask> &transform, 
        bool cache_estimates,
        const std::string &description, 
        utils::Verbosity verbosity
    );
};
}
#endif
