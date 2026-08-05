#pragma once

#include "actionGenerator.h"
#include "state.h"

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
private:
    int simNum;
    int _evaluateAction(State& state, int action, int numSims);
};