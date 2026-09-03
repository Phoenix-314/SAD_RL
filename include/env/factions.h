#pragma once

#include <vector>
#include <utility>
#include <array>

#include "env/ents.h"
#include "env/state.h"

/**
 * Fundamental actions, such as damage, heal, summon, res, handleDeaths, etc.
 */

namespace factions {
    void damage(State& state, Ent* source, Ent* target, int pips);
    void poisonDamage(State& state, Ent* source, Ent* target, int pips);
    void heal(State& state, Ent* target, int pips);
    void resurrect(State& state, int pips);
    void resHero(State& state, int index);
    void summon(State& state, Ent* source, int entID);

    void repositionEnemies(State& state);
    void modifyDamage(State& state, Ent* target, int amt);
    void resetHealths(State& state);

    void handleSDS(State& state);
    void handleSummons(State& state);
    void handleDeaths(State& state);
}