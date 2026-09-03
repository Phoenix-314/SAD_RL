#pragma once

#include "rlSolvers/randomNode.h"

#include <array>
#include <map>
#include <memory>

#include "env/state.h"
#include "env/constants.h"

class RandomNode;

/**
 * DecisionNode class - represents a decision node in the MCTS tree
 * Primarily contains a State, a map of actions to RandomNodes, and some node data
 *  - Owns its children RandomNodes
 */

class DecisionNode {
public:
    State state;
    std::map<int, std::unique_ptr<RandomNode>> children; 
    bool isFinal;
    int visits;
    RandomNode* father;
    bool isRoot;
    bool filledActions; // For SPW, indicates whether all valid actions have been added to the children map. For base MCTS, this is always true (but unused, so variable is unset)

    DecisionNode(State state, RandomNode* father=nullptr, bool isRoot=false, bool isFinal=false);
    void addChildren(std::unique_ptr<RandomNode> randomNode);
    RandomNode& nextRandomNode(int action);

    DecisionNode(const DecisionNode& other) = delete;
    DecisionNode& operator=(const DecisionNode& other) = delete; // Nodes in tree should not be copied

    std::string toString(int depth=0, int maxDepth=5);
};


