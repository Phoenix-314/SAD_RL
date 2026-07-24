#pragma once

#include <vector>
#include <array>
#include <optional>

#include "state.h"
#include "ents.h"


/**
 * Alongside factions, this contains the majority of specific game logic.
 * Includes the transition function, which takes in a state and an action, and returns the next state.
 * - Transition will accept any action, even if invalid. Use validActions to determine if an action is valid.
 *
 * Also includes the useDice, endTurn, endFight, and castSpell functions
 */
void initializeLibraries();

void transition(State& state, int action);
std::vector<Ent*> findTargets(State& state, int srcNum, int targNum, bool targIsEnemy);

void castSpell(State& state, Ent* target, int splNum);
void useDice(State& state, Ent* source, const std::vector<Ent*>& targets);

void endTurn(State& state);
void continueEndTurn(State& state);
void enemyAttacks(State& state);


State initial();

State genState(std::array<int, 5> playerIDs, std::vector<int> enemIDs, std::optional<std::array<int, 5>> autoAllyRolls = std::nullopt, std::optional<std::vector<int>> autoEnemyRolls = std::nullopt);

void endFight(State& state);
void continueEndFight(State& state);

void levelHero(State& state);

void generateEnemies(State& state);

void resetState(State& state);

void rollEnemies(State& state, std::vector<int> rolls);

void rollEnemyDice(State& state);

void rollEnemyDie(State& state, int monIndex, Ent* mon, int num);
    
std::vector<Ent*> getRandomTargetForEnemy(State& state, int index, Ent* mon);

Ent* getHpRestrictTargetForEnemy(State& state, bool isHeavy);


Ent* getRandomTargetForEnemyInternal(State& state, Ent* mon, bool allowOverkill, bool allowSuboptimal);
    
void rollAllies(State& state, std::array<int, 5> rolls);
void rerollAction(State& state, std::array<bool, 5> rerollBits);

void rollAllyDie(State& state, Ent* player, int num);
    
bool isTerminal(const State& state);