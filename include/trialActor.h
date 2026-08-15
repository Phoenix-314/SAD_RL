#pragma once

#include "actionGenerator.h"
#include "state.h"

#include <string>

/*
 * TrialActor generates an action by taking each action numSims=100 times, and simulating the rest of the fight with heuristic random actions. 
 * It then returns the action that reaches the next fight the highest number of times.
 * (Built before implementing information btwn fights, such dead heros starting with half health and hero choice)
*/

class TrialActor : public ActionGenerator {
public:
    TrialActor() : simNum(100) {}
    TrialActor(int numSims) : simNum(numSims) {} 
    int generateAction(State& state) override;
    std::string toString(int detail=0) override;
private:
    std::vector<std::pair<int, int>> lastTrials;
    int simNum;
    int _evaluateAction(State& state, int action, int numSims);
};