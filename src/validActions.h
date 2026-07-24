#pragma once

#include <vector>
#include <utility>
#include <array>

#include "ents.h"
#include "state.h"

/*
Returns a list of valid actions for the current state with validActions(state)
 - All checking for valid actions is done here. Transition will accept an invalid action without throwing an error, but may produce odd results

Also includes diceUsable
*/

namespace validActions {
    bool diceUsable(State& state, int sourceNum);
    bool checkBasicValidity(State& state, int sourceNum, int targetNum, bool targIsEnemy);
    bool canUsedPossessedDie(State& state, int sourceNum);
    bool isValidDiceAction(State& state, bool targIsEnemy, std::pair<int, int> diceAction);
    bool isValidSpellAction(State& state, bool targIsEnemy, std::pair<int, int> spellAction);
    std::vector<int> findMaxHP(State& state, bool heavy, bool isEnemy);
    std::pair<int, Ent*> findMadnessedHero(State& state);
    bool isValidReroll(State& state, std::array<bool, 5> rerollBits);
    bool isValidEndTurn(State& state);
    bool isValidAction(State& state, int action);
    std::vector<int> validActions(State& state);
} // namespace validActions