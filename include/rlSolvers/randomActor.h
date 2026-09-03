#pragma once

#include "rlSolvers/actionGenerator.h"
#include "env/state.h"


/**
 * RandomActor generates an action by selecting a random valid action, with the following heuristics:
 *  - Do not end turn if any other valid action is available
 *  - Do not burst defensively (SA01-SA05) ever, since it usually wont hit a target that needs a defensive burst and offence is usually better
 *  - When rerolling, reroll any blank dice and keep any non-blank dice as possible
 */
class RandomActor : public ActionGenerator {
public:
    RandomActor() = default;
    int generateAction(State& state) override;
    std::string toString(int detail=0) override;
};