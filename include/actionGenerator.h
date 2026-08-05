#pragma once
#include "state.h"

class ActionGenerator {
public:
    virtual int generateAction(State& state) = 0;
    virtual ~ActionGenerator() = default;
};