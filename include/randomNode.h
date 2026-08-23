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
    std::vector<DecisionNode*> children; // Non-owning pointers to the DecisionNodes stored in mcts.transpositionTable
    double cumulativeReward;
    int visits;
    DecisionNode* father; // Non-owning pointer to the DecisionNode that owns this RandomNode

    RandomNode(int action, DecisionNode* father);
    void addChildren(DecisionNode* decisionNode); // void since the DecisionNode is owned by mcts.transpositionTable

    RandomNode(const RandomNode& other) = delete;
    RandomNode& operator=(const RandomNode& other) = delete; // Nodes in tree should not be copied

    std::string toString(int depth=0, int maxDepth=5);
};


