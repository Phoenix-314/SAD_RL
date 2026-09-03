#pragma once

#include <array>
#include "env/ents.h"
#include "env/state.h"

/*
 * Keyword functions. 
 * These are called directly by some monPassives or spells
 * These are processed as a batch by the applyAffectKeywords, completeActionKeywords, findStaticPips, and findDynamicPips functions
*/

void initializeKeywordMap();

extern std::array<int (*)(State&, Ent*, Ent*, int), NUM_KEYWORDS> KeywordFuncMap;


int findStaticPips(State& state, Ent* source);
int findDynamicPips(State& state, Ent* source, Ent* target, int pips);
void applyAffectKeywords(State& state, Ent* source, Ent* target, int pips);
void completeActionKeywords(State& state, Ent* source, int pips);
