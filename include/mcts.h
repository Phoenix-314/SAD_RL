#pragma once

#include <string>
#include <utility>

#include "decisionNode.h"
#include "randomNode.h"

#include "transitions.h"
#include "actionGenerator.h"
#include "randomActor.h"
#include "heuristicActor.h"


class MCTS : public ActionGenerator {
public:
    int nSims;
    HeuristicActor rolloutPolicy;
    double K;
    DecisionNode* root;
    State initialState;

    double alpha;
    double beta;

    std::unordered_map<State, std::unique_ptr<DecisionNode>, boost::hash<State>> transpositionTable; 

    std::vector<std::unordered_map<State, std::pair<double, int>, boost::hash<State>>> endTurnTranspositionTable; 
        // transpositionTable[i] represents the expected q value and number of visits for a particular EMPTY_TURN state on turn i (i+1, since turns are 1-indexed)


    int progressBar;

    MCTS(int nSims, double K, double alpha, double beta, int progressBar=0);
    std::unique_ptr<DecisionNode>& updateDecisionNode(State decisionNodeState,RandomNode& randomNode);
    void grow_tree();
    double evaluate(State state);
    State selectOutcome(State state, RandomNode& randomNode);
    DecisionNode* selectOutcomeAndUpdateDecisionNode(State state, RandomNode& randomNode);
    double UCTval(const RandomNode& randomNode);
    int select(DecisionNode& decisionNode);
    int bestAction();
    void learn(int Nsim, int progressBar=0);
    // void forward(int action, State newState);
    void save(std::string path);
    std::string toString(int maxDepth=5);

    int generateAction(State& state);



};