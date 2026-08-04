#pragma once

#include <vector>
#include <utility>
#include <iostream>

#include "ents.h"
#include "state.h"
#include "keywordIDs.h"

/*
A few utility functions that depend only on the dataclass/ID files
Includes printState and printEnt, which are used for debugging
*/

namespace util {

    std::vector<Ent*> findHeroCleaveTargets(const State& state, const Ent& ent);
    
    std::vector<Ent*> findHeroDescendTargets(const State& state, const Ent& ent);
    
    int findEnemyIndex(const State& state, const Ent& target);
    std::vector<Ent*> findEnemyCleaveTargets(const State& state, const Ent& target);
    
    Ent* findTopPlayer(const State& state);
    Ent* findBottomPlayer(const State& state);
    Ent* findMiddlePlayer(const State& state);
    
    bool hasRoom(const State& state, const Ent& reinforceEnt);
    
    template <int N>
    std::array<bool, N> makeBoolArray(const std::vector<int>& keywords);
    
    bool targetIsDying(const State& state, const Ent& ent);
    bool hasCopycattedKeyword(const State& state, const Side& side, int keywordID);
    int monIDtoPos(int monID);
    int heroIDtoPos(int heroID);
    
    void printState(const State& state, bool printEnts=false);
    void printEnt(const State& state, const Ent& ent);
    std::string getActionStr(int act);

    template <int N>
    std::array<bool, N> int2ba(const int value);
    
    template <int N>
    int ba2int(const std::array<bool, N>& b);
    
}