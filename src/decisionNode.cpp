#include "decisionNode.h"

#include "util.h"

DecisionNode::DecisionNode(State state, RandomNode* father, bool isRoot, bool isFinal) : state(state), isFinal(isFinal), visits(0), isRoot(isRoot), filledActions(false) {}

void DecisionNode::addChildren(std::unique_ptr<RandomNode> randomNode) {
    children[randomNode->action] = std::move(randomNode);
}

RandomNode& DecisionNode::nextRandomNode(int action) {
    if (children.count(action) == 0) { // Only triggered with SPW select because all the valid actions are added immediately in base MCTS
        addChildren(std::make_unique<RandomNode>(action, this));
        // throw std::runtime_error("DecisionNode::nextRandomNode: action not found in children. This should not happen, as all valid actions should be added to children before calling nextRandomNode with MCTS select");
    }
    return *children[action];
}

std::string DecisionNode::toString(int depth, int maxDepth) {
    double calcedCumulativeReward = 0;
    for (const auto& [action, randomNode] : children) {
        calcedCumulativeReward += randomNode->cumulativeReward;
    }
    double qVal = (visits > 0) ? calcedCumulativeReward / visits : 0;
    std::string ret;
    ret += "DecisionNode: visits=" + std::to_string(visits) + ", isFinal=" + std::to_string(isFinal) + ", Calced CR: " + std::to_string(calcedCumulativeReward) + ", Q: " + std::to_string(qVal);
    if (depth > maxDepth) {
        return ret + "\n";
    }
    ret += ", unvisitedActions=[";
    for (const auto& [action, randomNode] : children) {
        if (randomNode->visits == 0) { ret += " " + util::getActionStr(action); } // Don't print unvisited nodes
    }

    

    ret += "]\n";
    for (const auto& [action, randomNode] : children) {
        if (randomNode->visits == 0) { continue; } // Don't print unvisited nodes

        for (int i=0;i<depth;i++) { ret += "  |"; }
        ret += "  RandNode with key Action: " + std::to_string(action) + "  -> ";
        ret += randomNode->toString(depth + 1, maxDepth);
    }
    return ret;
}