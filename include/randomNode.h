#pragma once

#include <unordered_map>
#include <memory>
#include <boost/functional/hash.hpp>

#include "decisionNode.h"

#include "state.h"
#include "constants.h"

class DecisionNode;

class RandomNode {
public:
    int action;
    std::unordered_map<State, std::unique_ptr<DecisionNode>, boost::hash<State>> children;
    double cumulativeReward;
    int visits;
    // double reward;
    DecisionNode* father;

    RandomNode(int action, DecisionNode* father);
    std::unique_ptr<DecisionNode>& addChildren(std::unique_ptr<DecisionNode> decisionNode);

    RandomNode(const RandomNode& other) = delete;
    RandomNode& operator=(const RandomNode& other) = delete; // Nodes in tree should not be copied

    std::string toString(int depth=0, int maxDepth=5);
};


