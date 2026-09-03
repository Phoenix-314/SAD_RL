#include "rlSolvers/randomNode.h"

#include "env/util.h"

RandomNode::RandomNode(int action, DecisionNode* father) : action(action), cumulativeReward(0), visits(0), father(father) {}

std::unique_ptr<DecisionNode>& RandomNode::addChildren(std::unique_ptr<DecisionNode> decisionNode) {
	State s = decisionNode->state;
    children[s] = std::move(decisionNode);
    return children[s];
}

std::string stateToString(const State& state) { // Modifiable to fit the situation
    std::string ret;
    for (const auto& player : state.players) {
        ret += std::to_string(player->hp) + "(" + std::to_string(player->currentSideNum) + ")" + (player->currentSide.type == SideType::BLANK ? "B" : "G") + ", ";
    }
    return ret;
}

std::string RandomNode::toString(int depth, int maxDepth) {
    if (depth > maxDepth || visits == 0) {
        return "RandomNode: action=" + util::getActionStr(action) + " (" + std::to_string(action) + "), visits=" + std::to_string(visits) + ", cumulativeReward=" + std::to_string(cumulativeReward) + ", qValue=" + std::to_string(cumulativeReward / visits) + "\n";
    }
    std::string ret;
    ret += "RandomNode: action=" + util::getActionStr(action) + " (" + std::to_string(action) + "), visits=" + std::to_string(visits) + ", cumulativeReward=" + std::to_string(cumulativeReward) + ", qValue=" + std::to_string(cumulativeReward / visits) + "\n";
    for (const auto& [state, decisionNode] : children) {
        for (int i=0;i<depth;i++) { ret += "  |"; }
        ret += "  DecNode with key State: " + stateToString(state) + "  -> ";
        ret += decisionNode->toString(depth + 1, maxDepth);
    }
    return ret;
}