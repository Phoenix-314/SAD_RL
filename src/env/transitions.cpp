#include "env/transitions.h"

#include <string>
#include <stdexcept>
#include <random>
#include <algorithm>

#include "env/constants.h"
#include "env/sourceEnts.h"
#include "env/keywords.h"
#include "env/spells.h"
#include "env/monPassives.h"
#include "env/actions.h"
#include "env/util.h"
#include "env/factions.h"
#include "env/validActions.h"
#include "env/entIDs.h"
#include "env/reversedIDs.h"
#include "env/fights.h"

void initializeLibraries() {
    SourceEnt::initializeSourceEnts();
    initializeKeywordMap();
    initializeSpellMaps();
    initializePassivesMap();
}


void transition(State& state, int action) {
    auto tmp = ACTION_MAP[action]; // array of all ~11*15+31+1 actions
    int actionType = tmp.first;
    std::pair<int, int> actionParams = tmp.second; // pair of ints for the action parameters

    if (actionType == REROLL_ACTION) {
        rerollAction(state, util::int2ba<5>(actionParams.first)); // actionmap[1] is bitarray for rerolls
        if (state.rerolls == 0) {
            state.stateType = StateType::EMPTY_REROLL;
        }
    }
        
    if (actionType == DICE_ALLY_ACTION) { // Must handle possesses targeting as well as shield/heal/damage_ALL
        int srcNum = actionParams.first;
        int targNum = actionParams.second;

        std::vector<Ent*> targets = findTargets(state, srcNum, targNum, false);
        
        useDice(state, state.players[srcNum], targets);

        if (state.players[srcNum]->currentSide.keywords[KeywordID::REPEL]) { // Repel cannot be on an untargeted side
            std::vector<Ent*> targets;
            for (int i=0; i < state.enemies.size(); i++) {
                Ent* mon = state.enemies[i];
                for (Ent* targetEnt : state.enemyTargets[i]) {
                    if (targetEnt == state.players[targNum]) { // if enemy's target is the hero that our repel side is targeting
                        targets.push_back(mon);
                        break;
                    }
                }
            }
            
            state.players[srcNum]->currentSide.type = SideType::DAMAGE;
            if (targets.size() > 0) {
                useDice(state, state.players[srcNum], targets);
            }
            state.players[srcNum]->currentSide.type = SideType::SHIELD;
        }
    } else if (actionType == DICE_ENEMY_ACTION) {
        int srcNum = actionParams.first;
        int targNum = actionParams.second;

        std::vector<Ent*> targets = findTargets(state, srcNum, targNum, true);
        useDice(state, state.players[srcNum], targets);
    } else if (actionType == SPELL_ALLY_ACTION) {
        int splNum = actionParams.first;
        int targNum = actionParams.second;
        if (targNum == -1) { // untargeted spells
            castSpell(state, nullptr, splNum);
        } else {
            castSpell(state, state.players[targNum], splNum);
        }
    } else if (actionType == SPELL_ENEMY_ACTION) {
        int splNum = actionParams.first;
        int targNum = actionParams.second;
        castSpell(state, state.enemies[targNum], splNum);
    } else if (actionType == END_TURN_ACTION) {
        endTurn(state);
    } else if (actionType == CONTINUE_ACTION) {
        if (state.stateType == StateType::EMPTY_TURN) {
            continueEndTurn(state);
        } else if (state.stateType == StateType::EMPTY_FIGHT) {
            continueEndFight(state);
        } else if (state.stateType == StateType::EMPTY_REROLL) {
            state.stateType = StateType::BATTLE;
        } else {
            throw std::runtime_error("CONTINUE_ACTION is only valid for EMPTY_TURN or EMPTY_FIGHT states, not stateType: " + std::to_string(static_cast<int>(state.stateType)));
        }
    }
    factions::resetHealths(state);

    factions::handleSDS(state); // Summons and deaths are handled after every action, since they can occur from inflictedPain, inflictedPoison, etc.
    if (state.enemies.size() == 0) {
        endFight(state);
    } else {
        bool lost = true;
        for (const auto& hero : state.players) {
            if (!hero->dead) {
                lost = false;
            }
        }
        if (lost) {
            state.stateType = StateType::LOST;
        }
    }
    if (state.turn > 30) { // If stuck in a fight with regen and petrified out, or similar
        state.stateType = StateType::LOST;
    }
}

std::vector<Ent*> findTargets(State& state, int srcNum, int targNum, bool targIsEnemy) {
    bool untargeted = (targNum == -1);
    if (untargeted) {
        std::pair<int, Ent*> possessedHeroIndexAndEnt = validActions::findMadnessedHero(state);
        int possessedHeroIndex = possessedHeroIndexAndEnt.first;
        Ent* possessedHero = possessedHeroIndexAndEnt.second;
        bool possessed = (possessedHeroIndex == srcNum);
        SideType sideType = state.players[srcNum]->currentSide.type;
        std::vector<Ent*> targets;

        std::vector<Ent*> herosAsVec = {state.players[0], state.players[1], state.players[2], state.players[3], state.players[4]};
        std::vector<Ent*> opposingUnits = possessed ? herosAsVec : state.enemies;
        std::vector<Ent*> supportingUnits = possessed ? state.enemies : herosAsVec;

        if (sideType == SideType::DAMAGE_ALL) {
            for (const auto& mon : opposingUnits) {
                if (!mon->dead) {
                    targets.push_back(mon);
                }
            }
        } else if (sideType == SideType::DAMAGE_EVERYONE) {
            for (const auto& hero : supportingUnits) {
                if (!hero->dead) {
                    targets.push_back(hero);
                }
            }
            for (const auto& mon : opposingUnits) {
                if (!mon->dead) {
                    targets.push_back(mon);
                }
            }
        } else if (sideType == SideType::HEAL_ALL || sideType == SideType::SHIELD_ALL) {
            for (const auto& hero : supportingUnits) {
                if (!hero->dead) {
                    targets.push_back(hero);
                }
            }
        } else {
            return std::vector<Ent*>(); // Other untargeted sides need not have targets, such as mana or resurrect
        }
        return targets;

    } else {
        Side& playerSide = state.players[srcNum]->currentSide;
        bool hasCleave = util::hasCopycattedKeyword(state, playerSide, KeywordID::CLEAVE);
        bool hasDescend = util::hasCopycattedKeyword(state, playerSide, KeywordID::DESCEND);
        if (!targIsEnemy) {
            std::vector<Ent*> targets = {state.players[targNum]};
            if (hasCleave) {
                targets = util::findHeroCleaveTargets(state, *state.players[targNum]);
            } else if (hasDescend) {
                targets = util::findHeroDescendTargets(state, *state.players[targNum]);
            }
            return targets;

        } else {
            std::vector<Ent*> targets = {state.enemies[targNum]};
            if ((hasCleave || hasDescend) && targNum < static_cast<int>(state.enemies.size()) - 1) {
                targets.push_back(state.enemies[targNum + 1]);
            }
            if (hasCleave && targNum > 0) {
                targets.push_back(state.enemies[targNum - 1]);
            }
            return targets;
        }
    }
}

void castSpell(State& state, Ent* target, int splNum) {
    int spellID = SpellID::BURST;
    if (splNum > 0) {
        spellID = state.players[splNum - 1]->spellID;
    }

    int spellCost = SpellCostMap[spellID] + std::max(0, state.spellData[splNum]); // positive values for splNum indicate DEPLETE costs 
    if (std::find(ALL_TACTICS.begin(), ALL_TACTICS.end(), SpellReqsMap[spellID]) != ALL_TACTICS.end()) {
        std::vector<Ent*> usedHeros = findTacticCosts(state, SpellReqsMap[spellID]);
        for (const auto& hero : usedHeros) {
            hero->usedDie = true;
        }
    } else {
        state.mana -= spellCost;
    }

    SpellCastAffect castAffectType = SpellCastAffectMap[spellID];
    if (castAffectType == SpellCastAffect::COOLDOWN) {
        state.spellData[splNum] = SpellData::COOLDOWN_USED;
    } else if (castAffectType == SpellCastAffect::DEPLETE) {
        state.spellData[splNum] += 1;
    } else if (castAffectType == SpellCastAffect::SINGLE_CAST) {
        state.spellData[splNum] = SpellData::SINGLE_CASTED;
    }

    if ((target != nullptr) && target->sourceID == THORN && !(std::find(ALL_THORN_SPECIALS.begin(), ALL_THORN_SPECIALS.end(), SpellReqsMap[spellID]) != ALL_THORN_SPECIALS.end())) {
                                                // Cannot do anything to thorns, aside from repeatedly harvest them
                                                // However, untargeted spells can still be cast and are specifically edited to avoid hitting thorns
        return;
    }

    // Actually using the spell
    SpellFuncMap[spellID](state, target);

    // Mon Passives
    if (state.bansheeD > 0) {
        for (const auto& hero : state.players) {
            factions::damage(state, nullptr, hero, state.bansheeD);
        }
        state.bansheeD *= -1; // negative bansheeD indicates that banshee has screamed
    }
    if (state.hexiaD) {
        Ent* hero = util::findBottomPlayer(state);
        if (hero != nullptr) {
            factions::damage(state, nullptr, hero, spellCost);
        }
    }
}

void useDice(State& state, Ent* source, const std::vector<Ent*>& targets) {
    Side backup = source->currentSide; // backup in case of copycat
    Side& side = source->currentSide;
    if (side.type != SideType::SUMMON && side.keywords[KeywordID::COPYCAT]) {
        KeywordFuncMap[KeywordID::COPYCAT](state, source, nullptr, 0);
    }
    bool copyCattedSingleUse = (side.type == SideType::SUMMON) ? false : side.keywords[KeywordID::SINGLEUSE];
    int staticPips = findStaticPips(state, source);
    int bonusCenterDynamicPips = targets.empty() ? 0 : (findDynamicPips(state, source, targets[(targets.size() - 1) / 2], staticPips) - staticPips);
    if (std::find(SIDES_AUTO_USABLE.begin(), SIDES_AUTO_USABLE.end(), side.type) != SIDES_AUTO_USABLE.end()) {
        staticPips = 0;
    }
    state.prevLastStrikeVal = state.lastStrikeVal;
    state.lastStrikeVal = staticPips;
    bool sourceIsHero = (source->sourceID >= 0);
    state.lastDie = side; // for copycat, chain, etc. # Additional keywords appended below for inflicteds
     
    
    // Side usage (rampage and rescue in respective actions)
    bool useSide = true;
    source->timesUsed += 1;
    if (sourceIsHero && ((side.keywords[KeywordID::DOUBLEUSE] && source->timesUsed < 2) || (side.keywords[KeywordID::QUADUSE] && source->timesUsed < 4))) {
        useSide = false;
    }

    if ((sourceIsHero || source->sourceID == GOLEM) && side.keywords[KeywordID::SELFSHIELD]) {
        KeywordFuncMap[KeywordID::SELFSHIELD](state, source, nullptr, staticPips + bonusCenterDynamicPips);
    }

    // Handling inflicteds. Must occur first to prevent self-cleanse
    if (source->inflictedDeathed) {
        source->dead = true;
        if (sourceIsHero) {
            state.lastDie->keywords[KeywordID::DEATH] = true;
        }
    }
    if (source->inflictedExerted) {
        source->exerted = true;
        source->toexert = true;
        if (sourceIsHero) {
            state.lastDie->keywords[KeywordID::EXERT] = true;
        }
    }
    if (source->inflictedPained && !(sourceIsHero && side.keywords[KeywordID::PAIN])) {
        if (source->sourceID == DEMON) { // Minor bug fix. Allows inflict pain to damage demonHP. All other "sourceless" damage(...) cannot hurt demon hp
            factions::damage(state, source, source, staticPips);
        } else {
            factions::damage(state, nullptr, source, staticPips);
        }
        if (sourceIsHero) {
            state.lastDie->keywords[KeywordID::PAIN] = true;
        }
    }

    

    // The actual action
    if (targets.size() == 0) { // Untargeted sides
        if (side.type == SideType::MANA) {
            state.mana += staticPips;
            if (side.keywords[KeywordID::BONED]) {
                factions::summon(state, nullptr, BONES);
            }
        } else if (side.type == SideType::SUMMON) {
            int eneID = -1 * util::ba2int<NUM_KEYWORDS>(side.keywords); // enemy IDs are negative
            if (eneID == THORN || eneID == CAW || eneID == DRAGON) {
                source->dead = true; // These particular enemies's seeds/eggs have Death
            }

            for (int i = 0; i < staticPips; i++) {
                factions::summon(state, source, eneID);
            }
        } else if (side.type == SideType::RESURRECT) {
            int resPips = staticPips;
            factions::resurrect(state, resPips);
            
        } else if (side.type == SideType::DODGE) {
            source->dodged = true;
        } else if (side.type == SideType::SELF_DAMAGE) {
            factions::damage(state, nullptr, source, staticPips);
        } else if (side.type == SideType::REROLL) {
            state.rerolls += staticPips;
        } else {
            throw std::runtime_error("Side type needs a target: " + sidesReversedIDs.at(side.type));
        }
    }

    for (int iTarget=0;iTarget<targets.size();iTarget++) {
        Ent* target = targets.at(iTarget);
        if (source->sourceID < 0 && target->dodged) {
            continue; // Cannot apply affect keywords or anything else to a dodged target
        }
        if (source->sourceID < 0 && target->redirectTarget) { // if enemy is hitting an ally with redirect, then all effects (including applyAffectKeywords) are redirected
            target = target->redirectTarget;
        }
        int pips = findDynamicPips(state, source, target, staticPips);

        if (side.type == SideType::DAMAGE || side.type == SideType::DAMAGE_ALL || side.type == SideType::DAMAGE_EVERYONE || side.type == SideType::DAMAGE_FLANKING) {
            factions::damage(state, source, target, pips);
            if (side.keywords[KeywordID::RAMPAGE] && (target->hp <= 0 || target->dead)) {
                useSide = false;
            }
            if (side.keywords[KeywordID::GUILT] && (target->hp <= 0 || target->dead)) {
                source->dead = true;
            }
        } else if (side.type == SideType::SHIELD || side.type == SideType::SHIELD_ALL) {
            bool previouslyDying = util::targetIsDying(state, *target);
            target->shield += pips; // shielding
            if (side.keywords[KeywordID::RESCUE] && previouslyDying && !util::targetIsDying(state, *target) && !(source->inflictedPained || util::hasCopycattedKeyword(state, source->currentSide, KeywordID::PAIN))) {
                useSide = false;                               // Inflict pain prevents rescue to eliminate infinite loops (pain is applied to later, so targetIsDying doesnt account for it)
            }
        } else if (side.type == SideType::HEALSHIELD) {
            target->shield += pips;
            factions::heal(state, target, pips);
        } else if (side.type == SideType::HEAL || side.type == SideType::HEAL_ALL) {
            bool previouslyDying = util::targetIsDying(state, *target);
            factions::heal(state, target, pips);
            if (side.keywords[KeywordID::RESCUE] && previouslyDying && !util::targetIsDying(state, *target) && !(source->inflictedPained || util::hasCopycattedKeyword(state, source->currentSide, KeywordID::PAIN))) {
                useSide = false;
            }   
        } else if (side.type == SideType::RECHARGE) {
            target->usedDie = false;
            target->timesUsed = 0;
        } else if (side.type == SideType::REDIRECTINCOMING) {
            if (source != target) {
                target->redirectTarget = source;
                source->incomingDamage += target->incomingDamage;
                source->incomingPoison += target->incomingPoison;
                target->incomingDamage = 0;
                target->incomingPoison = 0;
            }
        }
    
        else if (side.type == SideType::ENCHANT) {
            for (int j=0;j<NUM_KEYWORDS;j++) {
                if (side.keywords[j]) {
                    target->currentSide.keywords[j] = true;
                }
            } // enchant sides are ENCHANT, 0 pips, keywords=[things to enchant]
        } else if (side.type == SideType::KILL) {
            target->dead = true; // checking validity done in valid_actions function
        } else if (side.type == SideType::UNDYING) {
            target->undead = true;
        } else if (side.type == SideType::STUN) {
            factions::modifyDamage(state, target, -999);
            target->usedDie = true;
        } else {
            throw std::runtime_error("Side type is not allowed for useDice() with targets: " + std::to_string((int)side.type));     // BLANK, REROLL
        }

        applyAffectKeywords(state, source, target, pips); // weaken, poison, inflict affect every target, but vulnerable goes after
    }
    // Using side. Must be after rampage and rescue checks
    if (useSide) {
        source->usedDie = true;
    }

    completeActionKeywords(state, source, staticPips + bonusCenterDynamicPips);
    source->currentSide = backup; // restore side in case of copycat

    if (targets.size() <= 0 || targets.size() >= 4) { // if no targets or 4+ targets, then focus is not applicable
        state.lastTarget = nullptr;
    } else {
        state.lastTarget = targets[(targets.size() - 1) / 2]; // If you have [EFFECT]_ALL and happen to hit 3 or fewer targets, this breaks
    }

    if (side.type != SideType::SUMMON && (source->currentSide.keywords[KeywordID::SINGLEUSE] || copyCattedSingleUse) || (source->sourceID >= 0 && state.brambleD) || source->inflictedSingleUse) { 
        source->currentSide.type = SideType::BLANK;
        source->sides[source->currentSideNum].type = SideType::BLANK;
        state.lastDie->keywords[KeywordID::SINGLEUSE] = true;
    }
}

void endTurn(State& state) {
    // Step 0: Mana, can gain mana btwn turns which exceeds 3
    if (state.mana > 3) {
        state.mana = 3;
    }

    // Step 0.75: A few affects must be reset so enemies might reapply them. Enemies, on the other hand, have these affects cleared after attacking
    for (const auto& hero : state.players) {
        hero->weaken = std::min(0, hero->weaken);
        hero->inflictedDeathed = false;
        hero->inflictedExerted = false;
        hero->inflictedPained = false;
        hero->inflictedSingleUse = false;
    }
    // Step 1: Enemy Turns including passives (bell, ...)
    enemyAttacks(state);
    // Step 1.25:
    for (const auto& mon : state.enemies) {
        mon->weaken = 0;
        mon->inflictedDeathed = false;
        mon->inflictedExerted = false;
        mon->inflictedPained = false;
        mon->inflictedSingleUse = false;
    }

    // Step 2: Applying poison/regen + bell/rotten
    factions::resetHealths(state);
    for (const auto& mon : state.players) {
        int delta = std::max(mon->poison, 0) - mon->regen;
        if (delta > 0) {
            factions::poisonDamage(state, nullptr, mon, delta);
        } else {
            factions::heal(state, mon, -delta);
        }
    }
    for (const auto& mon : state.enemies) {
        int delta = std::max(mon->poison, 0) - mon->regen;
        if (delta > 0) {
            factions::poisonDamage(state, nullptr, mon, delta);
        } else {
            factions::heal(state, mon, -delta);
        }
    }
    // Mon passives
    Ent* bell = nullptr;
    Ent* rotten = nullptr;
    for (const auto& mon : state.enemies) {
        if (mon->sourceID == BELL) {
            bell = mon;
        }
        if (mon->sourceID == ROTTEN) {
            rotten = mon;
        }
    }
    if (bell != nullptr) {
        for (const auto& hero : state.players) {
            factions::damage(state, nullptr, hero, 1);
        }
        factions::damage(state, nullptr, bell, 5); // Bell self-damage
    }
    if (rotten != nullptr) {
        for (const auto& hero : state.players) {
            hero->maxHP = std::max(1, hero->maxHP - 1);
            hero->hp = std::min(hero->hp, hero->maxHP);
        }
    }

    // Step 3: Deaths and summons round
    factions::handleSDS(state);
    if (state.enemies.size() == 0) {
        endFight(state);
        return; // endFight resets state properly
    }

    // Step 4: Resetting all usedDie, exerted, dodged, intangibled, redirectTarget. Inflicted/weaken was handled above
    // 4.Ents
    std::vector<Ent*> allEnts = state.enemies;
    allEnts.insert(allEnts.end(), state.players.begin(), state.players.end());
    for (const auto& ent : allEnts) {
        ent->shield = (ent->sourceID != GOLEM) ? 0 : ent->shield;
        ent->undead = false;
        ent->dodged = false;
        ent->intangibled = false;
        
        if (!ent->dead) {
            ent->usedDie = false;
        } else {
            ent->usedDie = true;
        }
        ent->timesUsed = 0;
        ent->timesDamaged = 0;
        
        ent->cleansed = false;
        ent->poison = std::max(ent->poison, 0); // poison might have been negative to represent remaining cleanse for poison
        ent->weaken = std::max(ent->weaken, 0);
        ent->petrifyAmount = std::max(ent->petrifyAmount, 0);

        if (ent->toexert) {
            ent->exerted = true;
        } else {
            ent->exerted = false;
        }
        ent->toexert = false;

        ent->smith = 0;
        ent->boost = 0;
        ent->vulnerable = 0;
        if (ent->sourceID == CAW) {
            ent->backrow = false;
        }

        ent->redirectTarget = nullptr;

        ent->incomingDamage = 0;
        ent->incomingPoison = 0;
        
        ent->currentSideNum = -1;
        ent->currentSide = Side(0, SideType::ERROR, {false});
    }

    // 4.State
    state.enemyTargets = std::vector<std::vector<Ent*>>(state.enemies.size(), std::vector<Ent*>()); // 2D vector of enemy targets, reset to empty for each enemy
    state.mana += state.fortoldMana;
    state.fortoldMana = 0;
    state.rerolls = 3 - state.rerolls; // state.rerolls becomes negative for gaze spell. Else, we force player to roll down.
    for (int v = 0; v < 6; v++) {
        if (state.spellData[v] == SpellData::COOLDOWN_USED) {
            state.spellData[v] = SpellData::STANDARD;
        }
    }
    state.turn += 1;
    state.lastTarget = nullptr;
    state.prevLastStrikeVal = -1;
    state.lastStrikeVal = -1;
    state.lastDie.reset();
    // Mon Passives
    state.enemyDamageD = false;
    if (state.bansheeD < 0) {
        state.bansheeD *= -1; // Negative bansheeD indicates that banshee has screamed, so its reset for the next turn
    }

    state.stateType = StateType::EMPTY_TURN;
    // Steps 5 and 6 are done after intermediate empty state
}

void continueEndTurn(State& state) {
    // Step 5: Rolling all enemy dice, assigning inflictedDamage, etc.
    rollEnemyDice(state);
    // Step 6: Rolling all player dice
    rerollAction(state, {true, true, true, true, true}); // reroll all players

    state.stateType = StateType::BATTLE;
}

void enemyAttacks(State& state) {
    for (int i=0; i < state.enemies.size(); i++) {
        Ent* mon = state.enemies[i];
        if (!mon->usedDie && !mon->exerted && findStaticPips(state, mon) > 0 && mon->currentSide.type != SideType::BLANK) { // if the enemy has strength to use a die
            useDice(state, mon, state.enemyTargets[i]);
        }
        factions::resetHealths(state);
        factions::handleSDS(state); // Theoretically, a pained bones or a bandit could have death below them, stopping other enemies from attacking
    }
    
    // More passives for fleeing
    bool logged = true;
    for (const auto& mon : state.enemies) {
        if (!state.enemyDamageD && mon->sourceID == BLIND) { // Blind flees (dies, equivalently) if no enemy was damaged this turn, prior to handleDeaths
            mon->dead = true;
        }
        if (state.turn == 1 && mon->sourceID == CHEST) { // Autoflee
            mon->dead = true;
        }
        if (state.turn == 3 && mon->sourceID == SARCOPHAGUS) {
            mon->dead = true;
        }
        if (logged && mon->sourceID != LOG) {
            logged = false;
        }
    }

    if (logged) { // Log flees if no other enemy types are present
        for (const auto& mon : state.enemies) {
            mon->dead = true;
        }
    }
}


State initial() {
    // Heros
    std::array<Ent, HERO_TEAM_SIZE> newPlayers;
    for (int i = 0; i < HERO_TEAM_SIZE; i++) {
        std::vector<int> options = SourceEnt::HERO_TREE[i][0];
        int randVal = rand() % options.size();
        
        int newHeroId = options[randVal];
        newPlayers[i] = SourceEnt::ALL_HEROS[newHeroId];
    }
    
    State state(newPlayers, {});


    state.level = 0;
    state.playerLevels = {1, 1, 1, 1, 1};

    // Resets state, rolls enem dice, rolls player dice
    endFight(state); // resets all state values, including enemies, mana, etc. and rolls all dice
    continueEndFight(state); // rolls all player dice, levels up a hero if applicable, and rolls all enemy dice
    return state;
}

State genState(std::array<int, 5> playerIDs, std::vector<int> enemIDs, std::optional<std::array<int, 5>> autoAllyRolls, std::optional<std::vector<int>> autoEnemyRolls) {

    std::array<Ent, HERO_TEAM_SIZE> newPlayers;
    for (int i = 0; i < HERO_TEAM_SIZE; i++) {
        newPlayers[i] = SourceEnt::ALL_HEROS[playerIDs[i]];
    }
    State state(newPlayers, {});
    resetState(state);
    
    std::vector<Ent> newEnemies;
    for (int monID : enemIDs) {
        newEnemies.push_back(SourceEnt::ALL_MONSTERS[util::monIDtoPos(monID)]);
        if (monID == HEXIA) {
            state.hexiaD = true;
        }
        if (monID == BANSHEE) {
            state.bansheeD += 1;
        }
        if (monID == BRAMBLE) {
            state.brambleD = true;
        }
        if (monID == MADNESS) {
            state.madnessD = true;
        }
        if (monID == THE_HAND) {
            state.handD = true;
        }
        if (monID == WARCHIEF) {
            state.warchiefD += 1;
        }
    }
    state.addEnemyBatch(newEnemies);
    factions::repositionEnemies(state);
    
    if (autoEnemyRolls.has_value()) {
        rollEnemies(state, autoEnemyRolls.value());
    } else {
        rollEnemyDice(state);
    }
    
    if (autoAllyRolls.has_value()) {
        rollAllies(state, autoAllyRolls.value());
    } else {
        rerollAction(state, {true, true, true, true, true}); // reroll all players
    }

    factions::handleSDS(state);
    state.stateType = StateType::BATTLE;
    return state;
}

void endFight(State& state) {
    // Step 1: Reset state and load enemies
    resetState(state);

    // Steps 1.5-4: After empty state
    state.stateType = StateType::EMPTY_FIGHT;
    if (state.level == 20) {
        state.stateType = StateType::WON;
    }
}    

void continueEndFight(State& state) {
    // Step 1.5: Generate enemies
    generateEnemies(state);

    // Step 2: (Maybe) level-up a hero
    state.level += 1;
    if (state.level % 2 == 0) { // Every 2 levels, level up a hero (Randomly for now, TODO)
        levelHero(state);
    }

    // Step 3: Roll Enemies
    rollEnemyDice(state);

    // Step 4: Roll Players
    rerollAction(state, {true, true, true, true, true}); // reroll all players
    
    state.stateType = StateType::BATTLE;
}

void levelHero(State& state) {
    int tier = (state.level <= 10) ? 2 : 3;
    std::vector<int> levelableHeroCols;
    for (int i=0;i<5;i++) {
        if (state.playerLevels[i] < tier) {
            levelableHeroCols.push_back(i);
        }
    }
    int randVal = rand() % levelableHeroCols.size();
    int levelUpColor = levelableHeroCols[randVal];
    state.playerLevels[levelUpColor] += 1;
    int specificRandVal = rand() % SourceEnt::HERO_TREE[levelUpColor][tier - 1].size();
    int newHeroId = SourceEnt::HERO_TREE[levelUpColor][tier - 1][specificRandVal];
    state.replaceHero(levelUpColor, SourceEnt::ALL_HEROS[newHeroId]); // loads character with no afflictions of any kind
}

void generateEnemies(State& state) {
    std::vector<int> enemIDs = Fights::generateFight(state.level); // Tuple of monster sourceIDs # Level hasnt increased yet, but also generateFight is 0-indexed, so +1-1=0
    std::vector<Ent> newEnemies;
    for (int monID : enemIDs) {
        newEnemies.push_back(SourceEnt::ALL_MONSTERS[util::monIDtoPos(monID)]);

        if (monID == HEXIA) {
            state.hexiaD = true;
        }
        if (monID == BANSHEE) {
            state.bansheeD += 1;
        }
        if (monID == BRAMBLE) {
            state.brambleD = true;
        }
        if (monID == MADNESS) {
            state.madnessD = true;
        }
        if (monID == THE_HAND) {
            state.handD = true;
        }
        if (monID == WARCHIEF) {
            state.warchiefD += 1;
        }
    }
    state.addEnemyBatch(newEnemies);
    factions::repositionEnemies(state);
}

void resetState(State& state) {
    for (int i = 0; i < 5; i++) {
        state.replaceHero(i, SourceEnt::ALL_HEROS[state.players[i]->sourceID]); // loads character with no afflictions of any kind
        state.players[i]->currentSideNum = 0;
        state.players[i]->currentSide = state.players[i]->sides[0];
    }
    state.spellData = std::array<int, 6>({SpellData::STANDARD, SpellData::STANDARD, SpellData::STANDARD, SpellData::STANDARD, SpellData::STANDARD, SpellData::STANDARD});
    
    state.mana = 0;
    state.fortoldMana = 0;
    state.rerolls = 3;

    // MonPassives:
    state.enemyDamageD = false;
    state.hexiaD=false;
    state.bansheeD=0;
    state.brambleD=false;
    state.madnessD=false;
    state.handD=false;
    state.warchiefD=0;
    // ENEMIES
    if (state.enemies.size() > 0) {
        throw std::runtime_error("Enemies exist in state while resetting");
    }
    state.enemies.clear();
    if (state.imminentSummons.size() > 0 || state.reinforcements.size() > 0 || state.enemyTargets.size() > 0) {
        throw std::runtime_error("Imminent summons exist in state while resetting");
    }
    state.enemyTargets.clear(); // will be set once enemy dice are rolled
    
    state.imminentSummons.clear();
    state.reinforcements.clear();

    // General state data
    state.turn = 1;
    state.lastTarget = nullptr;
    state.prevLastStrikeVal = -1; // for trio
    state.lastStrikeVal = -1; // like lastdie, but with keywords evaluated
    state.lastDie = std::nullopt; // for chain, copycat, etc.

    // MonPassives above enemy generation
}

void rollEnemies(State& state, std::vector<int> rolls) {
    state.enemyTargets = std::vector<std::vector<Ent*>>(state.enemies.size(), std::vector<Ent*>());
    for (int i = 0; i < state.enemies.size(); i++) {
        rollEnemyDie(state, i, state.enemies[i], rolls[i]);
    }
}

void rollEnemyDice(State& state) {


    state.enemyTargets = std::vector<std::vector<Ent*>>(state.enemies.size(), std::vector<Ent*>());
    for (int i = 0; i < state.enemies.size(); i++) {
        int num = rand() % 6;
        rollEnemyDie(state, i, state.enemies[i], num);
    }
}

void rollEnemyDie(State& state, int monIndex, Ent* mon, int num) {
    mon->currentSideNum = num;
    mon->currentSide = mon->sides[mon->currentSideNum];
    if (mon->exerted) {
        state.enemyTargets[monIndex] = {}; // Exerted enemies cannot attack, so they have no targets
        return;
    }

    std::vector<Ent*> targets = getRandomTargetForEnemy(state, monIndex, mon); // List[Ent] of targets
    state.enemyTargets[monIndex] = targets;

    factions::modifyDamage(state, mon, findStaticPips(state, mon)); // Mon passives that modify damage (like banshee) are applied before the attack
}

std::vector<Ent*> getRandomTargetForEnemy(State& state, int index, Ent* mon) {
    Side& side = mon->currentSide;
    if (side.type == SideType::HEAL || side.type == SideType::HEAL_ALL || side.type == SideType::BLANK || side.type == SideType::SUMMON || side.type == SideType::KILL || side.type == SideType::DAMAGE_ALL || side.type == SideType::DAMAGE_EVERYONE || side.type == SideType::DAMAGE_FLANKING) { // All of these have very specific required targets
        if (side.type == SideType::HEAL) {
            return {state.enemies[index]}; // heal self
        } else if (side.type == SideType::HEAL_ALL) {
            return state.enemies; // heal all allies
        } else if (side.type == SideType::BLANK || side.type == SideType::SUMMON) {
            return {}; // blank has no target
        } else if (side.type == SideType::KILL) {
            return {util::findTopPlayer(state)}; // kill top player
        } else if (side.type == SideType::DAMAGE_ALL) {
            std::vector<Ent*> validTargets;
            for (const auto& hero : state.players) {
                if (!hero->dead) {
                    validTargets.push_back(hero);
                }
            }
            return validTargets;
        } else if (side.type == SideType::DAMAGE_EVERYONE) {
            std::vector<Ent*> validTargets = state.enemies;
            for (const auto& hero : state.players) {
                if (!hero->dead) {
                    validTargets.push_back(hero);
                }
            }
            return validTargets;
        } else if (side.type == SideType::DAMAGE_FLANKING) {
            Ent* a = util::findTopPlayer(state);
            Ent* b = util::findBottomPlayer(state);
            if (a == b) {
                return {a};
            }
            return {a, b};
        }
        throw std::runtime_error("Side type is not allowed for getRandomTargetForEnemy(): " + std::to_string(static_cast<int>(side.type)));
    }

    // If not above, only remaining sidetypes are damage (damage cleave, damage descend, eliminate/heavy)
    if (side.keywords[KeywordID::ELIMINATE]) {
        return {getHpRestrictTargetForEnemy(state, false)};
    }
    if (side.keywords[KeywordID::HEAVY]) {
        return {getHpRestrictTargetForEnemy(state, true)};
    }

    for (int i = 0; i < 4; i++) {
        Ent* potentialTarget = getRandomTargetForEnemyInternal(state, mon, (i & 1) > 0, (i & 2) > 0);
        if (potentialTarget == nullptr) {
            continue;
        }
        if (side.keywords[KeywordID::CLEAVE]) {
            return util::findHeroCleaveTargets(state, *potentialTarget);
        }
        if (side.keywords[KeywordID::DESCEND]) {
            return util::findHeroDescendTargets(state, *potentialTarget);
        }
        return {potentialTarget};
    }

    throw std::runtime_error("Could not find a valid target for enemy: " + std::to_string(mon->sourceID) + " with side type: " + std::to_string(static_cast<int>(side.type)));
    return std::vector<Ent*>(); // If no valid targets were found, return empty vector
}

Ent* getHpRestrictTargetForEnemy(State& state, bool isHeavy) {
    std::array<Ent*, 5> potentials = state.players; // Shallow copy is necessary to keep references properly
    std::shuffle(potentials.begin(), potentials.end(), std::default_random_engine());
    Ent* target = nullptr;
    int bestHP = isHeavy ? -999 : 999;
    for (const auto& hero : potentials) {
        if (!hero->dead) {
            int futureHP = hero->hp - hero->incomingDamage - std::max((hero->poison + hero->incomingPoison), 0); // ignoring regen, since that appears to be what SAD does

            if ((futureHP > bestHP) != isHeavy) {
                continue;
            }
            bestHP = futureHP;
            target = hero;
        }
    }
    return target;
}

Ent* getRandomTargetForEnemyInternal(State& state, Ent* mon, bool allowOverkill, bool allowSuboptimal) {
    Side& side = mon->currentSide;
    std::vector<Ent*> validTargets;
    for (const auto& hero : state.players) {
        if (!hero->dead) {
            validTargets.push_back(hero);
        }
    }
    
    if (!allowOverkill) {
        for (int i = (int)(validTargets.size()) - 1; i >= 0; i--) {
            Ent* hero = validTargets[i];
            if (!util::targetIsDying(state, *hero)) {
                continue;
            }
            validTargets.erase(validTargets.begin() + i);
        }
    }
    
    if (!allowSuboptimal) {
        if (side.keywords[KeywordID::CLEAVE]) {
            if (validTargets.size() < 3) {
                return nullptr;
            }
            
            validTargets.pop_back(); // remove bottom hero
            validTargets.erase(validTargets.begin()); // remove top hero
        }
        
        if (side.keywords[KeywordID::DESCEND]) {
            if (validTargets.size() < 2) {
                return nullptr;
            }
            validTargets.pop_back();
        }
    }

    if (validTargets.size() > 0) {
        int idx = rand() % validTargets.size();
        return validTargets[idx];
    }
    return nullptr;
}

void rollAllies(State& state, std::array<int, HERO_TEAM_SIZE> rolls) {
    for (int i = 0; i < HERO_TEAM_SIZE; i++) {
        rollAllyDie(state, state.players[i], rolls[i]);
    }
    state.rerolls -= 1;
}

void rerollAction(State& state, std::array<bool, HERO_TEAM_SIZE> rerollBits) {
    if (state.rerolls <= 0) {
        throw std::runtime_error("Cannot reroll when rerolls is 0 or less");
    }

    for (int i = 0; i < HERO_TEAM_SIZE; i++) {
        if (rerollBits[i]) {
            int num = rand() % 6;
            rollAllyDie(state, state.players[i], num);
        }
    }

    state.rerolls -= 1;
}

void rollAllyDie(State& state, Ent* player, int num) {
    player->currentSideNum = num;
    Side newSide = player->sides[num];
    player->currentSide = newSide;

    bool cantripped = util::hasCopycattedKeyword(state, newSide, KeywordID::CANTRIP);
    if (!player->dead && cantripped && validActions::diceUsable(state, player->positionID)) { // cantrip is only usable if the die is usable (not exerted, not singleuse, etc.)
        // First 2 ifs require targets
        bool possessed = (validActions::findMadnessedHero(state).second == player);
        
        if (newSide.type == SideType::DAMAGE) { // select random enemy
            Ent* target = state.enemies[rand() % state.enemies.size()];
            if (possessed) { // if possessed, then target ally instead of enemy
                std::vector<Ent*> v = {};
                for (const auto& ent : state.players) {
                    if (!ent->dead) {
                        v.push_back(ent);
                    }
                }
                target = v[rand() % v.size()];
            }

            useDice(state, player, {target});
        } else if (newSide.type == SideType::SHIELD) { // select random ally
            std::vector<Ent*> v = {};
            for (const auto& ent : state.players) {
                if (!ent->dead && ent->shield < ent->incomingDamage) {
                    v.push_back(ent);
                }
            }
            if (v.size() == 0) { // if no allies are alive and not fully shielded, then target self
                for (const auto& ent : state.players) {
                    if (!ent->dead) {
                        v.push_back(ent);
                    }
                }
            }
            if (possessed) {
                v = state.enemies;
            }

            Ent* target = v[rand() % v.size()];
            useDice(state, player, {target});

        } else if (newSide.type == SideType::MANA || newSide.type == SideType::REROLL || newSide.type == SideType::DODGE || newSide.type == SideType::SELF_DAMAGE) {
            useDice(state, player, {});
        } else if (newSide.type == SideType::BLANK) {
            // do nothing
        } else {
            throw std::runtime_error("Cantrip side that is not damage, shield, rerolls, or mana or DODGE. Instead, it is: " + std::to_string(static_cast<int>(newSide.type)));
        }
        // only certain types can have cantrip. Heal, for example, cannot have cantrip

        player->usedDie = false; // cantrip does not use the die, so reset usedDie to false
        player->timesUsed = 0; // useDice(...) uses the dice, but cantrip shouldnt do this when triggered via reroll
    }
}

bool isTerminal(const State& state) {
    if (state.stateType == StateType::WON || state.stateType == StateType::LOST) {
        return true;
    }
    return false;
}

int isFightEnd(const State& state) {
    if (state.stateType == StateType::WON || state.stateType == StateType::EMPTY_FIGHT) {
        return 1; // fight won
    }
    if (state.stateType == StateType::LOST) {
        return -1; // fight lost
    }
    return 0; // fight continues
}