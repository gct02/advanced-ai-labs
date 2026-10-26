#include "canonical_pdbs.h"

#include "../algorithms/max_cliques.h"
#include <algorithm>
#include <limits>

using namespace std;

namespace planopt_heuristics {

// True iff `op` changes the value of some variable in `pattern`.
bool affects_pattern(const TNFOperator &op, const Pattern &pattern) {
    for (const TNFOperatorEntry &entry : op.entries) {
        if (entry.precondition_value == entry.effect_value) {
            continue;
        }
        for (int var_id : pattern) {
            if (entry.variable_id == var_id) {
                return true;
            }
        }
    }
    return false;
}

vector<vector<int>> build_compatibility_graph(const vector<Pattern> &patterns, const TNFTask &task) {
    /*
      Build the compatibility graph of the given pattern collection in the form
      of adjacency lists: the outer vector has one entry for each pattern
      representing the vertices of the graph. Each such entry is a vector of
      ints that represents the outgoing edges of that vertex, i.e., an edge
      to each other vertex that represents an additive pattern.
    */
    vector<vector<int>> graph(patterns.size());

    // Lab 4, exercise 2(d). Two patterns are additive iff no operator affects
    // both of them.
    const int num_patterns = patterns.size();
    const int num_operators = task.operators.size();
    vector<vector<bool>> affects(num_patterns, vector<bool>(num_operators));
    for (int i = 0; i < num_patterns; ++i) {
        for (int op = 0; op < num_operators; ++op) {
            affects[i][op] = affects_pattern(task.operators[op], patterns[i]);
        }
    }

    for (int i = 0; i < num_patterns; ++i) {
        for (int j = i + 1; j < num_patterns; ++j) {
            bool additive = true;
            for (int op = 0; op < num_operators && additive; ++op) {
                additive = !(affects[i][op] && affects[j][op]);
            }
            if (additive) {
                graph[i].push_back(j);
                graph[j].push_back(i);
            }
        }
    }
    return graph;
}

CanonicalPatternDatabases::CanonicalPatternDatabases(
    const TNFTask &task, const vector<Pattern> &patterns) {
    for (const Pattern &pattern : patterns) {
        pdbs.emplace_back(task, pattern);
    }

    vector<vector<int>> compatibility_graph = build_compatibility_graph(patterns, task);
    max_cliques::compute_max_cliques(compatibility_graph, maximal_additive_sets);
}

int CanonicalPatternDatabases::compute_heuristic(const TNFState &original_state) {
    /*
      To avoid the overhead of looking up the heuristic value of a PDB multiple
      times (if that PDB occurs in multiple cliques), we pre-compute all
      heuristic values. Use heuristic_values[i] for the heuristic value of
      pdbs[i] in your code below.
    */
    vector<int> heuristic_values;
    heuristic_values.reserve(pdbs.size());
    for (const PatternDatabase &pdb : pdbs) {
        heuristic_values.push_back(pdb.lookup_distance(original_state));
        /*
          special case: if one of the PDBs detects unsolvability, we can
          return infinity right away. Otherwise, we would have to deal with
          integer overflows when adding numbers below.
        */
        if (heuristic_values.back() == numeric_limits<int>::max()) {
            return numeric_limits<int>::max();
        }
    }

    // Lab 4, exercise 2(d): the canonical heuristic is the maximum, over all
    // maximal additive sets (cliques), of the sum of their PDB values.
    int h = 0;
    for (const vector<int> &clique : maximal_additive_sets) {
        int clique_sum = 0;
        for (int pdb_index : clique) {
            clique_sum += heuristic_values[pdb_index];
        }
        h = max(h, clique_sum);
    }
    return h;
}
}
