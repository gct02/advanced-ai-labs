#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include "heuristics.h"

State split(std::string s) {
    size_t pos_start = 0, pos_end;
    std::string token;
    State res;
    while ((pos_end = s.find(" ", pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + 1;
        res.push_back((uint32_t)std::stoi(token));
    }
    res.push_back((uint32_t)std::stoi(s.substr(pos_start)));
    return res;
}

std::vector<State> parse_instances(const std::string& filename) {
    std::ifstream in(filename);
    std::vector<State> states;
    std::string line;

    while (getline(in, line)) {
        if (line.size() > 1)
            states.push_back(split(line));
    }
    return states;
}

void print_graph(const std::string& filename, std::shared_ptr<Node> root) {
    std::ofstream out(filename);
    std::queue<std::shared_ptr<Node>> queue;
    std::shared_ptr<Node> pred = nullptr;
    std::shared_ptr<Node> curr = nullptr;
    queue.push(root);

    while(!queue.empty()) {
        std::shared_ptr<Node> curr = queue.front();
        queue.pop();
        if (pred) {
            if (pred->g < curr->g) out << "\n";
            else out << " ";
        }
        out << state_to_string(curr->state);
        for (auto child : curr->children) {
            queue.push(child);
        }
        pred = curr;
    }
}

int main(int argc, char** argv) {
    State state = {5, 0, 2, 6, 4, 8, 1, 7, 3};

    std::cout << "\nBFS:" << "\n";
    Solution sol_bfs = bfs_graph(state);
    std::cout << sol_bfs.n_expanded << "\n";
    std::cout << sol_bfs.sol_depth << "\n";
    std::cout << sol_bfs.time << "\n";
    std::cout << sol_bfs.start_h << "\n";

    std::cout << "\nGSFS:" << "\n";
    Solution sol_gbfs = greedy_best_first(state);
    std::cout << sol_gbfs.n_expanded << "\n";
    std::cout << sol_gbfs.sol_depth << "\n";
    std::cout << sol_gbfs.time << "\n";

    std::cout << "\nID:" << "\n";
    Solution sol_iter_deep = iter_deep(state);
    std::cout << sol_iter_deep.n_expanded << "\n";
    std::cout << sol_iter_deep.sol_depth << "\n";
    std::cout << sol_iter_deep.time << "\n";

    std::cout << "\nA*:" << "\n";
    Solution sol_astar = astar(state);
    std::cout << sol_astar.n_expanded << "\n";
    std::cout << sol_astar.sol_depth << "\n";
    std::cout << sol_astar.avg_h << "\n";
    std::cout << sol_astar.time << "\n";

    std::shared_ptr<Node> root_astar = sol_astar.root;
    print_graph("graph_astar.txt", root_astar);

    std::cout << "\nIDA*:" << "\n";
    Solution sol_idastar = idastar(state);
    std::cout << sol_idastar.n_expanded << "\n";
    std::cout << sol_idastar.sol_depth << "\n";
    std::cout << sol_idastar.avg_h << "\n";
    std::cout << sol_idastar.time << "\n";

    // std::string filename;
    // if (argc > 1) {
    //     filename = argv[1];
    // } else {
    //     filename = "8puzzle_instances.txt";
    // }
    // std::vector<State> states = parse_instances(filename);
    // std::cout << "Running on " << filename << "\n\n";

    // for (State state : states) {
    //     std::cout << "----------------------------" << "\n";
    //     std::cout << "State (" << state_to_string(state) << "):" << "\n";
    //     std::cout << "----------------------------" << "\n";

    //     std::cout << "\nBFS:" << "\n";
    //     Solution sol_bfs = bfs_graph(state);
    //     std::cout << sol_bfs.n_expanded << "\n";
    //     std::cout << sol_bfs.sol_depth << "\n";
    //     std::cout << sol_bfs.time << "\n";

    //     std::cout << "\nID:" << "\n";
    //     Solution sol_iter_deep = iter_deep(state);
    //     std::cout << sol_iter_deep.n_expanded << "\n";
    //     std::cout << sol_iter_deep.sol_depth << "\n";
    //     std::cout << sol_iter_deep.time << "\n";

    //     std::cout << "\nA*:" << "\n";
    //     Solution sol_astar = astar(state);
    //     std::cout << sol_astar.n_expanded << "\n";
    //     std::cout << sol_astar.sol_depth << "\n";
    //     std::cout << sol_astar.avg_h << "\n";
    //     std::cout << sol_astar.time << "\n";

    //     std::cout << "\nIDA*:" << "\n";
    //     Solution sol_idastar = idastar(state);
    //     std::cout << sol_idastar.n_expanded << "\n";
    //     std::cout << sol_idastar.sol_depth << "\n";
    //     std::cout << sol_idastar.avg_h << "\n";
    //     std::cout << sol_idastar.time << "\n";

    //     std::cout << "\n";
    // }
    return 0;
}