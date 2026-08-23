#include "mcts.h"

#include "validActions.h"

#include <iostream>

MCTS::MCTS(int nSims, double K, double alpha, double beta, int progressBar) : nSims(nSims), K(K), root(nullptr), initialState(State()), alpha(alpha), beta(beta), progressBar(progressBar) {}

// std::unique_ptr<DecisionNode>& MCTS::updateDecisionNode(State decisionNodeState, RandomNode& randomNode) {
//     if (randomNode.children.count(decisionNodeState) == 0) {
//         auto decisionNode = std::make_unique<DecisionNode>(decisionNodeState, &randomNode, false, isFightEnd(decisionNodeState) != 0);
//         return randomNode.addChildren(std::move(decisionNode));
//     } else {
//         return randomNode.children[decisionNodeState];
//     }
// }

DecisionNode* MCTS::selectOutcomeAndUpdateDecisionNode(State state, RandomNode& randomNode) {
    // DPW SelectOutcome algorithm
    if (std::pow(randomNode.visits, beta) >= randomNode.children.size()) {
        transition(state, randomNode.action);
        auto it = transpositionTable.find(state);
        if (it == transpositionTable.end()) {
            auto decisionNode = std::make_unique<DecisionNode>(state, &randomNode, false, isFightEnd(state) != 0);
            it = transpositionTable.emplace(state, std::move(decisionNode)).first;
        }
        randomNode.addChildren(it->second.get());
        return it->second.get();
    } else {
        int randIndex = std::rand() % randomNode.children.size(); // Randomly select pre-existing state
        auto x = randomNode.children[randIndex];
        return x;
    }
    // Base MCTS selectOutcome algorithm
    // transition(state, randomNode.action);
    // return state;
    
}

void MCTS::grow_tree() {
    DecisionNode* decisionNodePtr = root;
    State state = initialState;
    
    std::vector<DecisionNode*> decisionNodePath;
    std::vector<RandomNode*> randomNodePath;
    while (!decisionNodePtr->isFinal && decisionNodePtr->visits > 1) {

        int a = select(*decisionNodePtr);

        RandomNode& newRandomNode = decisionNodePtr->nextRandomNode(a);

        // state = selectOutcome(state, newRandomNode);
        // decisionNodePtr = updateDecisionNode(state, newRandomNode).get();

        randomNodePath.push_back(&newRandomNode);
        decisionNodePath.push_back(decisionNodePtr);

        DecisionNode* prevDecisionNodePtr = decisionNodePtr; // for updating visit count
        decisionNodePtr = selectOutcomeAndUpdateDecisionNode(state, newRandomNode);
        state = decisionNodePtr->state;

        // std::exit(500); // TODO - Fix the infinite loop potential if a random node has only 1 outcome and that outcome is its own father
        // TODO - Fix the infinite loop potential if a random node has only 1 outcome and that outcome is its own father

        newRandomNode.visits += 1; // Updates are done online to prevent infinite loops if a random node has only 1 outcome and that outcome is its own father
        prevDecisionNodePtr->visits += 1; // However, updates are done after decisions are made to avoid interrupting standard behavior
    }

    decisionNodePtr->visits += 1; // last decisionNodePtr is not updated as a "prevDecisionNodePtr"
    double reward = evaluate(decisionNodePtr->state);

    for (const auto& randomNodePtr : randomNodePath) {
        randomNodePtr->cumulativeReward += reward; // cumulativeReward;
        // randomNodePtr->visits += 1;
    }
    for (const auto& decisionNodePtr : decisionNodePath) {
        // decisionNodePtr->visits += 1;
    }
}

double MCTS::evaluate(State state) {
    int maxItr = 400;
    int itr = 0;

    bool hasFoundTranspositionTableEntry = false;
    State transpositionTableState;
    while (isFightEnd(state) == 0) {
        if (state.stateType == StateType::EMPTY_TURN && !hasFoundTranspositionTableEntry) {
            hasFoundTranspositionTableEntry = true;
            transpositionTableState = state;
        }
        itr += 1;

        int a = rolloutPolicy.generateAction(state);
        transition(state, a);

        if (itr > maxItr) {
            throw std::runtime_error("MCTS::evaluate: maxItr exceeded. The fight is not ending after 400 iterations. This is likely a rare occurence that indicates maxItr is unneeded.");
            // could probably just assume a fight is lost if it takes more than 400 actions.
        }
    }

    int reward = isFightEnd(state) == 1 ? 1 : 0;
    if (hasFoundTranspositionTableEntry) {
        int turnIdx = transpositionTableState.turn - 2; // turnIdx is 0-indexed, not 1-indexed, so -1. Also, EMPTY_TURN has already incremented turn by 1, so -1 again.
        std::size_t hashValue = boost::hash<State>()(transpositionTableState);
        if (turnIdx >= endTurnTranspositionTable.size()) {
            endTurnTranspositionTable.resize(turnIdx + 1);
        }
        auto& tmp = endTurnTranspositionTable[turnIdx][transpositionTableState];
        tmp.first += reward;
        tmp.second += 1;
        return tmp.first / tmp.second; // return more accurate q value
    }

    return isFightEnd(state) == 1; // if no entry, just use the current rollout. This happens near the end of the game, so variance should be small anyway.
}

// State MCTS::selectOutcome(State state, RandomNode& randomNode) {
//     // DPW SelectOutcome algorithm


//     if (std::pow(randomNode.visits, beta) >= randomNode.children.size()) {
//         transition(state, randomNode.action);
//         return state;
//     } else {
//         int randIndex = std::rand() % randomNode.children.size(); // Randomly select pre-existing state
//         auto x = randomNode.children.begin();
//         std::advance(x, randIndex);
//         return x->first;
//     }

//     // Base MCTS selectOutcome algorithm
//     // transition(state, randomNode.action);
//     // return state;
// }

double MCTS::UCTval(const RandomNode& randomNode) {
    if (randomNode.visits == 0) {
        return std::numeric_limits<double>::infinity();
    }
    return (randomNode.cumulativeReward / randomNode.visits) + K * std::sqrt(std::log(randomNode.father->visits) / randomNode.visits); // todo
}

int MCTS::select(DecisionNode& decisionNode) {
    // SPW Select algorithm
    int a=0;
    if (!decisionNode.filledActions && std::pow(decisionNode.visits, alpha) >= decisionNode.children.size()) {
        auto vactions = validActions::validActionsFast(decisionNode.state);
        
        if (vactions.size() == decisionNode.children.size()) { // As soon as all valid actions actions have been added to children map, only use UCT from then on
            decisionNode.filledActions = true;
        }
        a = vactions[std::rand() % vactions.size()];
    } else {
        a = std::max_element(decisionNode.children.begin(), decisionNode.children.end(), 
            [this](const auto& a, const auto& b) { return this->UCTval(*a.second) < this->UCTval(*b.second); })->first;
    }
    return a;

    // Base MCTS select algorithm
    // if (decisionNode.visits <= 2) {
    //     std::vector<int> validActionsList = validActions::validActionsFast(decisionNode.state);
    //     if (decisionNode.children.size() != 0) {
    //         throw std::runtime_error("MCTS::select: decisionNode already has children when it should not have yet generated the children (since visists <= 2)");
    //     }
    //     for (int a : validActionsList) {
    //         decisionNode.addChildren(std::make_unique<RandomNode>(a, &decisionNode));
    //     }
    // }
    // int a = std::max_element(decisionNode.children.begin(), decisionNode.children.end(), [this](const auto& a, const auto& b) { return this->UCTval(*a.second) < this->UCTval(*b.second); })->first;
    // return a;
}


int MCTS::bestAction() {
    int bestAction = -1;
    int bestVisits = -1;
    for (const auto& [action, node] : root->children) {
        if (node->visits > bestVisits) {
            bestVisits = node->visits;
            bestAction = action;
        }
    }
    return bestAction;
}

void MCTS::learn(int Nsim, int progressBar) {
    endTurnTranspositionTable.clear();
    for (int i = 0; i < Nsim; i++) {
        if (progressBar >= 2 && i % (std::max(Nsim / 345, 1)) == 0) {
            std::cout << "MCTS Learning: " << i << "/" << Nsim << "                                \r";
        }
        grow_tree();
		//std::cout << toString(15) << std::endl;
    }
    if (progressBar >= 1) {
        if (progressBar >= 2) {
            std::cout << "MCTS Learning: " << Nsim << "/" << Nsim << "                                " << std::endl;
        } else {
            std::cout << "MCTS Learning: Learned " << Nsim << " simulations                                " << std::endl;
        }
    }
}

// def forward(self, action, new_state):
//     """
//     If the env is determonostic we can salvage most of the tree structure.
//     Advances the tree in the action taken if found in the tree nodes.

//     :param action: (tuple)
//     :param new_state: (tuple)
//     """
//     if self._hash_action(action) in self.root.children.keys():
//         rnd_node = self.root.children[self._hash_action(action)]
//         if len(rnd_node.children) > 1:
//             self.root = DecisionNode(state=new_state, is_root=True)
//         else:
//             next_decision_node = np.random.choice(list(rnd_node.children.values()))
//             if np.linalg.norm(next_decision_node.state-new_state) > 1e-3:
//                 raise RuntimeWarning("The env is probably stochastic")
//             else:
//                 next_decision_node.father = None
//                 self.root.children.pop(self._hash_action(action))
//                 self.root = next_decision_node
//                 self.root.is_root = True
//     else:
//         raise RuntimeWarning("Action taken: {} is not in the children of the root node.".format(action))

void MCTS::save(std::string path) {
    throw std::runtime_error("MCTS::save is not implemented yet.");
}

int MCTS::generateAction(State& state) {
    initialState = state;
    transpositionTable.clear();
    transpositionTable.emplace(state, std::make_unique<DecisionNode>(state, nullptr, true));
    root = transpositionTable[state].get();
    learn(nSims, progressBar);
    // std::cout << toString() << std::endl;
    return bestAction();
}

std::string MCTS::toString(int maxDepth) {
    std::string ret;
    ret += "MCTS: nSims=" + std::to_string(nSims) + ", K=" + std::to_string(K) + ", alpha=" + std::to_string(alpha) + ", beta=" + std::to_string(beta) + "\n";
    ret += "Tree:\n";
    ret += root->toString(0, maxDepth);
    return ret;
}