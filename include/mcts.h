#pragma once

#include <string>

#include "decisionNode.h"
#include "randomNode.h"

#include "transitions.h"
#include "actionGenerator.h"
#include "randomActor.h"


class MCTS : public ActionGenerator {
public:
    int nSims;
    RandomActor rolloutPolicy;
    double K;
    std::unique_ptr<DecisionNode> root;
    State initialState;

    double alpha;
    double beta;

    int progressBar;

    MCTS(int nSims, double K, double alpha, double beta, int progressBar=0);
    std::unique_ptr<DecisionNode>& updateDecisionNode(State decisionNodeState,RandomNode& randomNode);
    void grow_tree();
    double evaluate(State state);
    State selectOutcome(State state, RandomNode& randomNode);
    double UCTval(const RandomNode& randomNode);
    int select(DecisionNode& decisionNode);
    int bestAction();
    void learn(int Nsim, int progressBar=0);
    // void forward(int action, State newState);
    void save(std::string path);
    std::string toString(int maxDepth=5);

    int generateAction(State& state);
};