#include "heuristicActor.h"

#include <random>
#include <array>

#include "actions.h"
#include "validActions.h"
#include "reversedIDs.h"
#include "keywordIDs.h"
#include "util.h"
#include "transitions.h"
#include "keywords.h"



int HeuristicActor::generateAction(State& state) {
    std::vector<int> validActionsList = validActions::validActionsFast(state);

    int act = validActionsList[0];
    int type = ACTION_MAP[act].first;
    std::pair<int, int> params = ACTION_MAP[act].second;
    
    if (type == DICE_ALLY_ACTION) {
        if (params.second == -1) { // if the action is untargeted, use it. One has to use it sooner or later
            return act;
        }

        Ent* hero = state.players[params.first];
        int tempIdx = 0;
        int tempAct = validActionsList[tempIdx];
        while (ACTION_MAP[tempAct].second.first == params.first && ACTION_MAP[tempAct].first == DICE_ALLY_ACTION) {
            if (util::targetIsDying(state, *state.players[ACTION_MAP[tempAct].second.second])) { // if can cast friendly thing to dying ally, do so
                return tempAct;
            }
            tempIdx++;
            if (tempIdx >= validActionsList.size()) { break; }
            tempAct = validActionsList[tempIdx];
        }
        tempIdx = 0;
        tempAct = validActionsList[tempIdx];
        while (ACTION_MAP[tempAct].second.first == params.first && ACTION_MAP[tempAct].first == DICE_ALLY_ACTION) {
            Ent* target = state.players[ACTION_MAP[tempAct].second.second];
            if ((target->incomingDamage > target->shield) && (hero->currentSide.type == SideType::SHIELD || hero->currentSide.type == SideType::HEALSHIELD)) { // if can cast friendly thing to dying ally, do so
                return tempAct;
            }
            if ((target->hp < target->maxHP) && (hero->currentSide.type == SideType::HEAL || hero->currentSide.type == SideType::HEALSHIELD)) { // if can cast friendly thing to dying ally, do so
                return tempAct;
            }
            tempIdx++;
            if (tempIdx >= validActionsList.size()) { break; }
            tempAct = validActionsList[tempIdx];
        }
        return act; // if no better action is found, return the original action


    } else if (type == DICE_ENEMY_ACTION) {
        Ent* hero = state.players[params.first];
        int tempIdx = 0;
        int tempAct = validActionsList[tempIdx];
        int pips = findStaticPips(state, hero);
        int minEnemHp = 999;
        while (ACTION_MAP[tempAct].second.first == params.first && ACTION_MAP[tempAct].first == DICE_ENEMY_ACTION) {
            Ent* target = state.enemies[ACTION_MAP[tempAct].second.second];
            if (pips > target->hp) { // if able to kill enemy, do so
                return tempAct;
            }
            if (target->hp < minEnemHp) {
                minEnemHp = target->hp;
            }
            tempIdx++;
            if (tempIdx >= validActionsList.size()) { break; }
            tempAct = validActionsList[tempIdx];
        }
        tempIdx = 0;
        tempAct = validActionsList[tempIdx];
        while (ACTION_MAP[tempAct].second.first == params.first && ACTION_MAP[tempAct].first == DICE_ENEMY_ACTION) {
            Ent* target = state.enemies[ACTION_MAP[tempAct].second.second];
            if (target->hp == minEnemHp) { // if able to damage lowest hp enemy, do so
                return tempAct;
            }
            tempIdx++;
            if (tempIdx >= validActionsList.size()) { break; }
            tempAct = validActionsList[tempIdx];
        }
        return act; // if no better action is found, return the original action - This cannot occur given the current heuristics

    } else if (type == SPELL_ALLY_ACTION || type == SPELL_ENEMY_ACTION) {
        if (params.first == 0) { // if enough mana for burst, check to see if we can kill or save someone. Else, dont use mana
            int tempIdx = 0;
            int tempAct = validActionsList[tempIdx];

            while (ACTION_MAP[tempAct].second.first == params.first && ACTION_MAP[tempAct].first == SPELL_ALLY_ACTION) { // check for possible offensive kill before defensive saves
                tempIdx++;
                if (tempIdx >= validActionsList.size()) { break; }
                tempAct = validActionsList[tempIdx];
            }

            while (ACTION_MAP[tempAct].second.first == params.first && ACTION_MAP[tempAct].first == SPELL_ENEMY_ACTION) {
                Ent* target = state.enemies[ACTION_MAP[tempAct].second.second];
                int maxPossibleBurstDamage = 2 * (state.mana / 2);
                bool canKill = (target->hp + target->shield) <= maxPossibleBurstDamage;

                if (canKill) {
                    return tempAct;
                }

                tempIdx++;
                if (tempIdx >= validActionsList.size()) { break; }
                tempAct = validActionsList[tempIdx];
            }

            tempIdx = 0; // Resetting for defensive burst saves
            tempAct = validActionsList[tempIdx];

            while (ACTION_MAP[tempAct].second.first == params.first && ACTION_MAP[tempAct].first == SPELL_ALLY_ACTION) {
                Ent* target = state.players[ACTION_MAP[tempAct].second.second];
                if (util::targetIsDying(state, *target)) { // if can cast friendly thing to dying ally, do so
                    int maxPossibleBurstShield = 2 * (state.mana / 2);
                    target->shield += maxPossibleBurstShield;
                    bool canSave = !util::targetIsDying(state, *target);
                    target->shield -= maxPossibleBurstShield; // reset shield to original value
                    
                    if (canSave) {
                        return tempAct;
                    }
                }
                tempIdx++;
                if (tempIdx >= validActionsList.size()) { break; }
                tempAct = validActionsList[tempIdx];
            }
            

        }

        if (state.mana > 3) { // if we would lose mana from turn, just use it to damage lowest hp enemy
            int tempIdx = 0;
            int tempAct = validActionsList[tempIdx];
            while (ACTION_MAP[tempAct].second.first == params.first && ACTION_MAP[tempAct].first == SPELL_ALLY_ACTION) { // move idx to offensive bursts
                tempIdx++;
                if (tempIdx >= validActionsList.size()) { break; }
                tempAct = validActionsList[tempIdx];
            }
            int minEnemHp = 999;
            int resetOffensiveBurstIdx = tempIdx;
            while (ACTION_MAP[tempAct].second.first == params.first && ACTION_MAP[tempAct].first == SPELL_ENEMY_ACTION) {
                Ent* target = state.enemies[ACTION_MAP[tempAct].second.second];
                if (target->hp < minEnemHp) {
                    minEnemHp = target->hp;
                }
                tempIdx++;
                if (tempIdx >= validActionsList.size()) { break; }
                tempAct = validActionsList[tempIdx];
            }
            tempIdx = resetOffensiveBurstIdx;
            tempAct = validActionsList[tempIdx];
            while (ACTION_MAP[tempAct].second.first == params.first && ACTION_MAP[tempAct].first == SPELL_ENEMY_ACTION) {
                Ent* target = state.enemies[ACTION_MAP[tempAct].second.second];
                if (target->hp == minEnemHp) { // if able to damage lowest hp enemy, do so
                    return tempAct;
                }
                tempIdx++;
                if (tempIdx >= validActionsList.size()) { break; }
                tempAct = validActionsList[tempIdx];
            }
        }
        return 181; // If no save or kill, and can save mana for next turn without wasting mana, just end turn
    } else if (type == REROLL_ACTION) { // Heuristic: Reroll any blank dice, keep any non-blank dice as possible
        auto rr = std::array<bool, 5>{false};

        for (int i = 0; i < 5; i++) {
            if (!validActions::diceUsable(state, i) && !(state.players[i]->dead || state.players[i]->currentSide.keywords[KeywordID::STICKY])) {
                rr[i] = true;
            } else {
                rr[i] = false;
            }
        }
        int targAct = actionsReversedIDs.at(std::make_pair(REROLL_ACTION, std::make_pair(util::ba2int<5>(rr), 0)));
        if (std::find(validActionsList.begin(), validActionsList.end(), targAct) != validActionsList.end()) {
            act = targAct;
        }
        
    } else if (type == END_TURN_ACTION || type == CONTINUE_ACTION) { // Heuristic: Do not end turn if any other valid action is available. 
                                                                     // validActions[0] will be END/CONTINUE iff it is the only valid action available 
        return act;
    }
    
    return act;
}

std::string HeuristicActor::toString(int detail) {
    return "HeuristicActor";
}