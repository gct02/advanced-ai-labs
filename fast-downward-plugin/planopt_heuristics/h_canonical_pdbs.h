#ifndef PLANOPT_HEURISTICS_H_CANONICAL_PDBS_H
#define PLANOPT_HEURISTICS_H_CANONICAL_PDBS_H

#include "canonical_pdbs.h"
#include "../heuristic.h"
#include <memory>
#include <string>

namespace plugins {
class Options;
}

namespace planopt_heuristics {
class CanonicalPDBsHeuristic : public Heuristic {
    CanonicalPatternDatabases pdbs;
protected:
    virtual int compute_heuristic(const State &ancestor_state) override;
public:
    CanonicalPDBsHeuristic(    
        const std::shared_ptr<AbstractTask> &transform, 
        bool cache_estimates,
        const std::string &description, 
        utils::Verbosity verbosity,  // Corrected typo and added parameter name
        const std::vector<std::vector<int>> &patterns
    );
};
}

#endif  // PLANOPT_HEURISTICS_H_CANONICAL_PDBS_H
