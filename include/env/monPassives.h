#pragma once

#include <array>
#include "env/ents.h"
#include "env/state.h"
#include "env/constants.h"

/*
 * Mon passives are unique effects that occur when a monster is hit.
 * Does not include Bell, Rotten, chest, sarcophagus, barrel, bandit, goblin, or bones since these do not occur when the monster is hit.
 * Demon has additional code in damage(...) function.
*/

void initializePassivesMap();

extern std::array<void (*)(State&, Ent*, Ent*, int), NUM_ON_HIT_PASSIVES> PassiveFuncMap;

bool canRetribute(State& state, Ent* source);


void intangibleHP(State& state, Ent* source, Ent* target, int pips);
void manaHP(State& state, Ent* source, Ent* target, int pips);
void stoneHP(State& state, Ent* source, Ent* target, int pips);
void stunHP(State& state, Ent* source, Ent* target, int pips);

template <int spikyAmt>
void spikyHP(State& state, Ent* source, Ent* target, int pips);

template <int summonMonID>
void summonHP(State& state, Ent* source, Ent* target, int pips);

template <int amt, int damageTarget>
void damageHP(State& state, Ent* source, Ent* target, int pips);
