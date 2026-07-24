#include "factions.h"

#include <algorithm>

#include "constants.h"
#include "keywordIDs.h"
#include "keywords.h"
#include "entIDs.h"
#include "util.h"
#include "factions.h"
#include "monPassives.h"
#include "monPassiveIDs.h"
#include "sourceEnts.h"

namespace factions {

    void damage(State& state, Ent* source, Ent* target, int pips) {
        if (target->dodged || target->intangibled) {
            return;
        }
        
        if (target->redirectTarget != nullptr) {
            target = target->redirectTarget;
        }
        target->timesDamaged += 1;
        if (pips > target->shield) {
            pips += target->vulnerable;
        }
        
        if (source == nullptr && target->sourceID == DEMON) { // Mon Passive. Demons cannot take certain damage without a source (ie, a dice)
            for (int i = target->hp; i > std::max(0, target->hp - pips); i--) { // start at hp. If 7 hp and 3 pips of damage, checck 7, 6, 5. Skip 7-3=4.
                if (target->specialHP[i]) {
                    target->hp = i;
                    return;
                }
            }
            // If no special hp, then can continue as normal
        }

        if (target->onHitFunction != OnHitPassive::ON_HIT_NO_PASSIVE && target->onHitFunction != -1) {
            PassiveFuncMap[target->onHitFunction](state, source, target, pips);
        }

        // Edge Case:
        if (target->sourceID == GOLEM && target->currentSide.keywords[KeywordID::STEEL]) {
            int index = util::findEnemyIndex(state, *target); // PositionID ensures uniqueness
            Ent* hero = state.enemyTargets[index][0];
            if (hero->redirectTarget != nullptr) {
                hero = hero->redirectTarget;
            }
            hero->incomingDamage -= std::min(pips, target->shield);
        }

        if (pips > target->shield) {
            target->hp -= std::max((pips - target->shield) - target->armor, 0);
            target->shield = 0;
            if (target->sourceID < 0) { // if enemy
                state.enemyDamageD = true;
            }
        } else {
            target->shield -= pips;
        }

        if (target->undead && target->hp <= 0) {
            target->hp = 1;
        }
    }

    void poisonDamage(State& state, Ent* source, Ent* target, int pips) {
        if (target->intangibled) {
            return;
        }
        if (target->hp <= 0) {
            return; // Cant get bandit overkill damage from poison (if already dead from standard damage)
        }

        if (target->sourceID == DEMON) { // Mon Passive. Demons cannot take certain damage without a source (ie, a dice)
            for (int i = target->hp; i > std::max(0, target->hp - pips); i--) { // start at hp. If 7 hp and 3 pips of damage, checck 7, 6, 5. Skip 7-3=4.
                if (target->specialHP[i]) {
                    target->hp = i;
                    return;
                }
            }
            // If no special hp, then can continue as normal
        }

        target->timesDamaged += 1;
        if (target->onHitFunction != OnHitPassive::ON_HIT_NO_PASSIVE && target->onHitFunction != -1) {
            PassiveFuncMap[target->onHitFunction](state, source, target, pips);
        }

        target->hp -= pips;
        if (target->undead && target->hp <= 0) {
            target->hp = 1;
        }
    }

    void heal(State& state, Ent* target, int pips) {
        target->hp += pips; // At end of action, hp is reset
    }

    void resurrect(State& state, int pips) {
        // Resurrecting Hero
        for (int i = 0; i < 5; i++) {
            if (state.players[i]->dead) {
                pips -= 1;
                resHero(state, i);
                if (pips <= 0) {
                    break;
                }
            }
        }
    }

    void resHero(State& state, int index) {   
        state.replaceHero(index, SourceEnt::ALL_HEROS[state.players[index]->sourceID]); // loads character with no afflictions of any kind
        state.players[index]->usedDie = true; // Mark the new Ent as having used its die
    }

    void summon(State& state, Ent* source, int entID) {
        Ent newEnemy = SourceEnt::ALL_MONSTERS[util::monIDtoPos(entID)]; // All enemies have negative sourceIDs, so we convert to positive
        newEnemy.usedDie = true;

        if (source != nullptr) {
            newEnemy.positionID = - source->positionID;
        } else {
            newEnemy.positionID = 0;
        }

        if (util::hasRoom(state, newEnemy) && state.reinforcements.size() == 0) { // Cannot summon if reinforcements are queued
            state.addImminentSummon(newEnemy);
        } else {
            state.addReinforcement(newEnemy);
        }
    }

    void repositionEnemies(State& state) {
        for (int i = 0; i < state.enemies.size(); i++) {
            state.enemies[i]->positionID = i + 1;
        }
    }

    void modifyDamage(State& state, Ent* target, int amt) {
        if (target->sourceID >= 0) {
            return;
        }
        if (target->usedDie) {
            return;
        }

        if (amt < 0) {
            amt = std::max(amt, -findStaticPips(state, target));
        }

        int index = util::findEnemyIndex(state, *target);

        for (int i = 0; i < state.enemyTargets[index].size(); i++) {
            Ent* hero = state.enemyTargets[index][i];
            if (hero->sourceID < 0) {
                continue;
            }
            if (hero->redirectTarget) {
                hero = hero->redirectTarget;
            }
            hero->incomingDamage += amt;
            if (target->currentSide.keywords[KeywordID::POISON]) {
                hero->incomingPoison += amt;
            }
        }
    }

    void resetHealths(State& state) {
        for (const auto& hero : state.players) {
            hero->hp = std::min(hero->maxHP, hero->hp);
        }
        for (const auto& enemy : state.enemies) {
            enemy->hp = std::min(enemy->maxHP, enemy->hp);
        }
    }

    void handleSDS(State& state) {
        handleSummons(state);
        handleDeaths(state); // handleDeaths includes a summonsRound (ex: a bones breaking a summonHP)
    }

    void handleSummons(State& state) {
        std::sort(state.imminentSummons.begin(), state.imminentSummons.end(), [](const Ent* a, const Ent* b) {
            return a->positionID < b->positionID;
        });

        for (const auto& newEnemy : state.imminentSummons) {
            int posID = -newEnemy->positionID;
            state.enemies.insert(state.enemies.begin() + posID, newEnemy);
            std::vector<Ent*> newTargets = std::vector<Ent*>(); // overloaded operator for vector insert requires an explicit typing
            state.enemyTargets.insert(state.enemyTargets.begin() + posID, newTargets);
        }
        state.imminentSummons = std::vector<Ent*>(); // Clear summons. However, since they have been appended to enemies, they are not lost memory
        repositionEnemies(state);
    }

    void handleDeaths(State& state) {
        // enemies, reinforcements, imminentSummons, and players are all memory responsible
        for (const auto& hero : state.players) {
            if (hero->hp <= 0 && !hero->dead) {
                hero->dead = true; // nothing needs to be reset, since the hero will be reloaded from Lib.ALL_HEROS if resurrected
            }
            if (hero->undead) {
                hero->dead = false;
                if (hero->hp <= 0) {
                    hero->hp = 1;
                }
            }
        }

        for (int i = 0; i < state.enemies.size(); i++) {
            Ent* mon = state.enemies[i];
            if (mon->hp <= 0) { // This check is uncessary, but helps with clarity
                mon->dead = true;
            }
            if (mon->sourceID == MILITIA) {
                if (state.enemyTargets[i].size() > 0) {
                    Ent* targ = state.enemyTargets[i][0];
                    if (targ->redirectTarget != nullptr) {
                        targ = targ->redirectTarget;
                    }
                    if (targ->shield >= 5) { // if the target has 5 or more shield, then militia flees (dies, is equivalent)
                        mon->dead = true;     // Im going to just check shields at the end of every action, instead of in the middle of a shielding action for convenience. Technically,
                                            // militias still flee on stuff like selfshield pain or selfshield spikymon, but this is one of the few things for which I am not being accurate to
                    }
                }
            }
        }

        int index = ((int) state.enemies.size()) - 1; // Must go in reverse for bone/barrel damage to apply properly
        std::vector<int> processedIndices;
        while (index >= 0) {
            Ent* dyingMon = state.enemies[index];
            if ((dyingMon->dead || dyingMon->hp <= 0) && !(std::find(processedIndices.begin(), processedIndices.end(), index) != processedIndices.end())) {
                processedIndices.push_back(index);
                dyingMon->dead = true;
                int initIndex = index;
                int overkillHP = dyingMon->hp;
                int sourceID = dyingMon->sourceID;

                if (sourceID == BONES || sourceID == BARREL) { // Bone damage applies to nearby enemies
                    int dmgAmt = (sourceID == BONES) ? 1 : 5;
                    if (initIndex > 0) {
                        damage(state, nullptr, state.enemies[initIndex - 1], dmgAmt);
                    }
                    int bonusIndex = initIndex + 1;
                    while (bonusIndex < state.enemies.size() && state.enemies[bonusIndex]->dead) {
                        bonusIndex += 1;
                    }
                    if (bonusIndex < state.enemies.size()) {
                        damage(state, nullptr, state.enemies[bonusIndex], dmgAmt);
                        index = bonusIndex + 1; // ensures reprocessing of dead bone/barrel
                    }
                }

                if (overkillHP <= -2) {
                    if (initIndex > 0 && state.enemies[initIndex - 1]->sourceID == BANDIT) {
                        state.enemies[initIndex - 1]->dead = true;
                    }
                    int bonusIndex = initIndex + 1;
                    while (bonusIndex < state.enemies.size() && state.enemies[bonusIndex]->dead) {
                        bonusIndex += 1;
                    }
                    if (bonusIndex < state.enemies.size() && state.enemies[bonusIndex]->sourceID == BANDIT) {
                        state.enemies[bonusIndex]->dead = true;
                        index = bonusIndex + 1; // ensures reprocessing of dead bandit
                    }
                }

                // Mon state passives
                if (sourceID == HEXIA) {
                    state.hexiaD = false;
                } else if (sourceID == BANSHEE) {
                    if (state.bansheeD > 0) {
                        state.bansheeD -= 1;
                    } else { // Cannot be 0, since a banshee was present to be killed
                        state.bansheeD += 1;
                    }
                } else if (sourceID == BRAMBLE) {
                    state.brambleD = false;
                } else if (sourceID == MADNESS) {
                    state.madnessD = false;
                } else if (sourceID == THE_HAND) {
                    state.handD = false;
                } else if (sourceID == WARCHIEF) {
                    state.warchiefD -= 1;
                }


                //state.enemies.pop(initIndex) // Done after a summons round, so that summons are added in the correct places
                if (state.lastTarget == dyingMon) {
                    state.lastTarget = nullptr;
                }

                modifyDamage(state, dyingMon, -999); // Remove all damage from the dead mon, so that it doesnt affect other heroes. This is done after the mon's onHitFunction is called, so that the damage can still be applied to other heroes if needed


                for (int i = 0; i < state.enemyTargets.size(); i++) {
                    std::vector<Ent*>& monTargetData = state.enemyTargets[i];
                    for (int j=0; j < monTargetData.size(); j++) {
                        Ent* target = monTargetData[j];
                        if (target == dyingMon) {
                            monTargetData.erase(monTargetData.begin() + j);
                        }
                    }
                }
            }

            index -= 1;
            if (index >= (int) state.enemies.size()) {
                index = ((int) state.enemies.size()) - 1;
            }
        }

        handleSummons(state);
        // Remove dead enemies -- must clear memory
        index = ((int) state.enemies.size()) - 1;
        while (index >= 0) {
            if (state.enemies[index]->dead) {
                state.killEnemy(index);
            }
            index -= 1;
        }

        // Handle Reinforcements - No need for memory management since pointers are being transfered from state.reinforcements to state.enemies
        while (state.reinforcements.size() > 0 && util::hasRoom(state, *state.reinforcements[0])) {
            std::vector<Ent*> newTargets = std::vector<Ent*>(); // overloaded operator for vector insert requires an explicit typing
            state.enemyTargets.insert(state.enemyTargets.begin(), newTargets);
            state.enemies.insert(state.enemies.begin(), state.reinforcements[0]);
            state.reinforcements.erase(state.reinforcements.begin());
        }
        repositionEnemies(state);
        

        if (state.enemies.size() == 1 && state.enemies[0]->sourceID == GOBLIN) {
            state.killEnemy(0); // Goblin dies if it is the only enemy left
        }
    }

}