#pragma once
#include "env/state.h"

/*
 * Abstract class for any RL policy
*/
class ActionGenerator {
public:
    virtual int generateAction(State& state) = 0;
    virtual ~ActionGenerator() = default;
    virtual std::string toString(int detail=0) { return "ActionGenerator"; }
};