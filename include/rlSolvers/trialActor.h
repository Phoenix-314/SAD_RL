#pragma once

#include "rlSolvers/actionGenerator.h"
#include "env/state.h"

#include <string>

/*
 * TrialActor class - generates an action by taking each action numSims=100 times, and simulating the rest of the fight with the rollout policy 
 * Returns the action that reaches the next fight the highest number of times.
 *  - Does not account for any data that transfers between fights, assumes all methods of reaching the next fight are equally good
 * 
*/

class TrialActor : public ActionGenerator {
public:
    TrialActor(ActionGenerator* policy) : rolloutPolicy(policy), simNum(100) {}
    TrialActor(ActionGenerator* policy, int numSims) : rolloutPolicy(policy), simNum(numSims) {} 
    int generateAction(State& state) override;
    std::string toString(int detail=0) override;
private:
    ActionGenerator* rolloutPolicy;
    std::vector<std::pair<int, int>> lastTrials;
    int simNum;
    int _evaluateAction(State& state, int action, int numSims);
};