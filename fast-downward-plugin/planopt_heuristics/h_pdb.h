#ifndef PLANOPT_HEURISTICS_H_PDB_H
#define PLANOPT_HEURISTICS_H_PDB_H

#include "../heuristic.h"
#include "pdb.h"
#include <memory>

namespace plugins {
class Options;
}

namespace planopt_heuristics {
class PDBHeuristic : public Heuristic {
    PatternDatabase pdb;
protected:
    virtual int compute_heuristic(const State &state) override;
public:
    PDBHeuristic(
        const std::vector<int> &pattern,
        const std::shared_ptr<AbstractTask> &transform, 
        bool cache_estimates,
        const std::string &description, 
        utils::Verbosity verbosity
    );
};
}
#endif
