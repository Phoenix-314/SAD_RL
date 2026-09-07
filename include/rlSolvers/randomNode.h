#pragma once

#include <unordered_map>
#include <memory>
#include <boost/functional/hash.hpp>

#include "rlSolvers/decisionNode.h"

#include "env/state.h"
#include "env/constants.h"

class DecisionNode;

/**
 * RandomNode class - represents a random node in the MCTS tree
 * Primarily contains an action, a map of states to DecisionNodes, and some node data
 *  - Owns its children DecisionNodes
 */

class RandomNode {
public:
    int action;
    std::vector<std::unique_ptr<DecisionNode>> children;
    double cumulativeReward;
    int visits;
    DecisionNode* father;

    RandomNode(int action, DecisionNode* father);
    std::unique_ptr<DecisionNode>& addChildren(std::unique_ptr<DecisionNode> decisionNode);

    RandomNode(const RandomNode& other) = delete;
    RandomNode& operator=(const RandomNode& other) = delete; // Nodes in tree should not be copied

    std::string toString(int depth=0, int maxDepth=5);
};


