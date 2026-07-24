#pragma once

#include <vector>
#include <utility>
#include <array>

#include "spellIDs.h"
#include "constants.h"
#include "ents.h"
#include "state.h"

/**
 * Spell functions used in castSpell.
 * 0 is burst
 * 1-5 are orange through blue, although typically only 4 and 5 are usable (few spells are orange, yellow, or gray)
 */

void initializeSpellMaps();

extern std::array<void (*)(State&, Ent*), NUM_SPELLS> SpellFuncMap;
extern std::array<int, NUM_SPELLS> SpellCostMap;
extern std::array<SpellTargetReq, NUM_SPELLS> SpellReqsMap;
extern std::array<SpellCastAffect, NUM_SPELLS> SpellCastAffectMap;

inline constexpr std::array<SpellTargetReq, 4> ALL_THORN_SPECIALS = {SpellTargetReq::DAMAGE_N_SHIELD_3, SpellTargetReq::UNTARGETED, SpellTargetReq::HP1};
inline constexpr std::array<SpellTargetReq, 4> ALL_TACTICS = {SpellTargetReq::DAMAGE_N_SHIELD_3, SpellTargetReq::DAMAGE_2, SpellTargetReq::ONE_OF_ALL, SpellTargetReq::AN_X};



std::vector<Ent*> findTacticCosts(State& state, SpellTargetReq spellTargetReq);