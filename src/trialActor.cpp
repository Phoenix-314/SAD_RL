#include "trialActor.h"

#include <algorithm>
#include <iostream>

#include "transitions.h"
#include "validActions.h"
#include "util.h"

#include "randomActor.h"

int TrialActor::generateAction(State& state) {
    // std::cout<< "TrialActor: Generating action for state with level: " << state.level << std::endl;

    std::vector<int> validActionsList = validActions::validActionsFast(state);

    int bestAction = -1;
    int bestScore = -1;
    for (int i=0; i < validActionsList.size(); i++) {
        int tmp = _evaluateAction(state, validActionsList[i], simNum);
        if (tmp > bestScore) {
            bestScore = tmp;
            bestAction = validActionsList[i];
        }
    }
    
    
    return bestAction;
}

int TrialActor::_evaluateAction(State& state, int action, int numSims) {
    int netSuccesses = 0;
    RandomActor randomActor;
    for (int i = 0; i < numSims; i++) {
        State simState = state;
        transition(simState, action); // Apply given action

        // Complete remaining rollout with random actions
        int antiInfiniteChecker = 0;
        while (isFightEnd(simState) == 0) {
            int act = randomActor.generateAction(simState);
            transition(simState, act);
            
            antiInfiniteChecker += 1;
            if (antiInfiniteChecker > 10000) {
                std::cout << "Error: Infinite loop detected in TrialActor::_evaluateAction" << std::endl;
                std::cout << "State: " << std::endl;
                util::printState(simState, true);
                std::cout << "EndInitialPrintState: " << std::endl << std::endl << std::endl << std::endl;
                throw std::runtime_error("Infinite loop detected in TrialActor::_evaluateAction");
            }
        }
        
        if (isFightEnd(simState) == 1) {
            netSuccesses += 1;
        }
    }
    
    return netSuccesses;
}