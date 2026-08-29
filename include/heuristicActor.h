#pragma once

#include "actionGenerator.h"
#include "state.h"


/*
 * HeuristicActor generates an action with some rules:
 * - End turn/Continue iff it is the only valid action available
 * - Reroll any blank dice, keep any non-blank dice as possible
 *      - With 2+ rerolls, reroll any right or rightmost sides
 *      - Reroll heal on turn 1, reroll reroll, enchant, damage_everyone, resurrect if no allies are dead, dodge if no incoming damage
 * - Use dice first:
 *      - Friendly dice target the first dying ally, then an ally which is taking damage. This does not account for poison diffs
 *      - Heal dice target the first ally with missing health if noone is dying
 *      - Enemy dice target the first enemy that can be killed, then the lowest hp enemy if noone can be killed. This does not account for mon passives
 * - Then, consider spells:
 *      - Only uses burst, since other spells depend on heros and are more situational
 *      - If enough mana to kill an enemy that is dealing enough damage to kill an ally, do so
 *      - If enough mana to kill any enemy with burst, do so
 *      - Then, if enough mana to save a dying ally with burst, do so
 *      - If cannot save or kill, use any excess (>3) mana on bursting the lowest hp enemy, and save 0-3 mana for the next turn
 */
class HeuristicActor : public ActionGenerator {
public:
    HeuristicActor() = default;
    int generateAction(State& state) override;
    std::string toString(int detail=0) override;
};