#pragma once
#include "state.h"

class ActionGenerator {
public:
    virtual int generateAction(State& state) = 0;
    virtual ~ActionGenerator() = default;
    virtual std::string toString(int detail=5) { return "ActionGenerator"; }
};