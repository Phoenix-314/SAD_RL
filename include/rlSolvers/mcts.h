#pragma once

#include <string>
#include <utility>

#include "rlSolvers/decisionNode.h"
#include "rlSolvers/randomNode.h"

#include "env/transitions.h"
#include "rlSolvers/actionGenerator.h"
#include "rlSolvers/randomActor.h"
#include "rlSolvers/heuristicActor.h"

/*
 * Monte Carlo Tree Search implementation with double progressive widening (Couetoux et al., 2011))
 *  Includes minor optimizations with a transposition table specific to SnD
*/
class MCTS : public ActionGenerator {
public:
    int nSims;
    ActionGenerator* rolloutPolicy;
    double K;
    std::unique_ptr<DecisionNode> root;
    State initialState;

    double alpha;
    double beta;
    std::vector<std::unordered_map<State, std::pair<double, int>, boost::hash<State>>> transpositionTable; 
        // transpositionTable[i] represents the expected q value and number of visits for a particular EMPTY_TURN state on turn i (i+1, since turns are 1-indexed)
        // EMPTY_TURN states are much more likely to be repeatedly reached from different paths, so storing expected values for these states is efficient

    int progressBar;

    MCTS(ActionGenerator* rolloutPolicy, int nSims, double K, double alpha, double beta, int progressBar=0);
    std::unique_ptr<DecisionNode>& updateRandomNode(State decisionNodeState,RandomNode& randomNode);
    void grow_tree();
    double evaluate(State state);
    State selectOutcome(State state, RandomNode& randomNode);
    double UCTval(const RandomNode& randomNode);
    int select(DecisionNode& decisionNode);
    int bestAction();
    void learn(int Nsim, int progressBar=0);
    // void forward(int action, State newState);
    std::string toString(int maxDepth=5);

    int generateAction(State& state);
};