#include "validActions.h"

#include <stdexcept>
#include <string>

#include "sourceEnts.h"
#include "constants.h"
#include "entIDs.h"
#include "keywordIDs.h"
#include "spells.h"
#include "spellIDs.h"
#include "keywords.h"
#include "actions.h"
#include "util.h"

namespace validActions {

bool diceUsable(State& state, int sourceNum) {
    // Doesn't check side types
    if (state.players[sourceNum]->dead || state.players[sourceNum]->usedDie || state.players[sourceNum]->exerted) {
        return false;
    }
    if (state.players[sourceNum]->petrified[state.players[sourceNum]->currentSideNum] && !state.players[sourceNum]->currentSide.keywords[KeywordID::STASIS]) { // Stasis overrides petrify
        return false;
    }
    if (state.players[sourceNum]->currentSide.type == SideType::BLANK) {
        return false;
    }

    bool autoUsableSide = std::find(SIDES_AUTO_USABLE.begin(), SIDES_AUTO_USABLE.end(), state.players[sourceNum]->currentSide.type) != SIDES_AUTO_USABLE.end();
    if (!autoUsableSide) {
        int staticPips = findStaticPips(state, state.players[sourceNum]);
        if (staticPips <= 0) {
            return false;
        }
    }
    
    return true;
}

bool checkBasicValidity(State& state, int sourceNum, int targetNum, bool targIsEnemy) {
    // Checks that are irrelevant to side type
    if (!diceUsable(state, sourceNum)) {
        return false;
    }
    const Side& curSide = state.players[sourceNum]->currentSide;
    const std::array<bool, NUM_KEYWORDS>& currSideKeywords = curSide.keywords;
    if (targIsEnemy) {
        if (targetNum >= state.enemies.size()) {
            return false;
        }

        if (state.enemies[targetNum]->backrow) {
            bool allAreBackrow = true;
            for (const auto& enemy : state.enemies) {
                if (!enemy->backrow) {
                    allAreBackrow = false;
                    break;
                }
            }
            bool hasRanged = util::hasCopycattedKeyword(state, curSide, KeywordID::RANGED);
            if (! (hasRanged || allAreBackrow)) {
                return false;
            }
        }

        if (currSideKeywords[KeywordID::HEAVY]) {
            auto maxHPs = findMaxHP(state, true, true);
            if (std::find(maxHPs.begin(), maxHPs.end(), targetNum) == maxHPs.end()) {
                return false;
            }
        }
        if (currSideKeywords[KeywordID::ELIMINATE]) {
            auto minHPs = findMaxHP(state, false, true);
            if (std::find(minHPs.begin(), minHPs.end(), targetNum) == minHPs.end()) {
                return false;
            }
        }
    } else {
        if (targetNum != -1 && state.players[targetNum]->dead) {
            return false;
        }
        constexpr std::array<SideType, 8> untargetedSideTypes = {SideType::MANA, SideType::RESURRECT, SideType::DODGE, SideType::DAMAGE_ALL, SideType::DAMAGE_EVERYONE, SideType::HEAL_ALL, SideType::SHIELD_ALL, SideType::SELF_DAMAGE};
        if (targetNum == -1) {
            return std::find(untargetedSideTypes.begin(), untargetedSideTypes.end(), curSide.type) != untargetedSideTypes.end();
        } else if (targetNum != -1 && std::find(untargetedSideTypes.begin(), untargetedSideTypes.end(), curSide.type) != untargetedSideTypes.end()) {
            return false;
        }

        if (currSideKeywords[KeywordID::HEAVY]) { // Technically only relevant if the source is possessed
            auto maxHPs = findMaxHP(state, true, false);
            return std::find(maxHPs.begin(), maxHPs.end(), targetNum) != maxHPs.end();
        }
        if (currSideKeywords[KeywordID::ELIMINATE]) {
            auto minHPs = findMaxHP(state, false, false);
            return std::find(minHPs.begin(), minHPs.end(), targetNum) != minHPs.end();
        }
    }

    return true;
}

bool canUsedPossessedDie(State& state, int sourceNum) { // Used to see if mandatory should be applied
    if (!diceUsable(state, sourceNum)) {
        return false;
    }
    const Side& side = state.players[sourceNum]->currentSide;
    if (side.type == SideType::BLANK || side.type == SideType::REROLL || side.type == SideType::RECHARGE) {
        return false;
    }
    if (side.type == SideType::RESURRECT) {
        for (const auto& hero : state.players) {
            if (hero->dead) {
                return true;
            }
        }
        return false;
    }
    if (side.type == SideType::KILL) {
        for (const auto& hero : state.players) {
            if (hero->dead) {
                continue;
            }
            if (hero->hp <= findStaticPips(state, state.players[sourceNum])) {
                return true;
            }
        }
        return false;
    }
    if (side.type == SideType::STUN) {
        return true; // Can always self-stun
    }

    return true; // Most sides can be used
}

bool isValidDiceAction(State& state, bool targIsEnemy, std::pair<int, int> diceAction) {
    int sourceNum = diceAction.first;
    int targetNum = diceAction.second;

    if (!checkBasicValidity(state, sourceNum, targetNum, targIsEnemy)) {
        return false;
    }

    bool possessed = (findMadnessedHero(state).first == sourceNum);
    if (possessed) {
        targIsEnemy = !targIsEnemy; // Pretending to be an enemy handles most logic:
            // Untargeted sides still just check targetNum, the different targets will be handled in transition
            // Sides that typically target enemies will only accept initial inputs that were verified to target a valid ally
            // Similar for target allies.
            // RECHARGE cannot be used, RESURRECT still targets allies, and KILL & STUN have new requirements
    }
    
    
    // Side Types
    int staticPips = findStaticPips(state, state.players[sourceNum]);
    SideType sideType = state.players[sourceNum]->currentSide.type;
    
    if (sideType == SideType::DAMAGE) {
        return targIsEnemy;
    }
    if (sideType == SideType::SHIELD) {
        return !targIsEnemy;
    }
    if (sideType == SideType::HEAL) {
        return !targIsEnemy;
    }
    if (sideType == SideType::HEALSHIELD) {
        return !targIsEnemy;
    }
    if (sideType == SideType::MANA) {
        return targetNum == -1; // untargeted
    }
    if (sideType == SideType::BLANK) {
        return false;
    }
    if (sideType == SideType::REROLL) {
        return false;
    }
    if (sideType == SideType::REDIRECTINCOMING) {
        return !targIsEnemy;
    }
    if (sideType == SideType::SUMMON) {
        throw std::invalid_argument("Invalid input in isValidDiceAction for sourceNum " + std::to_string(sourceNum) + " and targetNum " + std::to_string(targetNum));
    }
    if (sideType == SideType::RECHARGE) {
        if (possessed) {
            return false;
        }
        return (!targIsEnemy) && state.players[targetNum]->usedDie && state.players[targetNum]->currentSide.type != SideType::RECHARGE;
    }
    if (sideType == SideType::RESURRECT) {
        if (targetNum != -1) {
            return false;
        }
        for (const auto& hero : state.players) {
            if (hero->dead) {
                return true;
            }
        }
        return false;
    }
    if (sideType == SideType::ENCHANT) {
        return !targIsEnemy;
    }
    if (sideType == SideType::KILL) {
        if (possessed) {
            return targIsEnemy && state.players[targetNum]->hp <= staticPips;
        }
        return targIsEnemy && state.enemies[targetNum]->hp <= staticPips;
    }
    if (sideType == SideType::UNDYING) {
        return !targIsEnemy;
    }
    if (sideType == SideType::DODGE) {
        return targetNum == -1; // untargeted
    }
    if (sideType == SideType::SELF_DAMAGE) {
        return targetNum == -1; // untargeted
    }
    if (sideType == SideType::DAMAGE_ALL) {
        return targetNum == -1; // untargeted
    }
    if (sideType == SideType::DAMAGE_EVERYONE) {
        return targetNum == -1; // untargeted
    }
    if (sideType == SideType::DAMAGE_FLANKING) {
        throw std::runtime_error("Invalid input in isValidDiceAction for sourceNum " + std::to_string(sourceNum) + " and targetNum " + std::to_string(targetNum));
    }
    if (sideType == SideType::SHIELD_ALL) {
        return targetNum == -1; // untargeted
    }
    if (sideType == SideType::HEAL_ALL) {
        return targetNum == -1; // untargeted
    }
    if (sideType == SideType::STUN) {
        if (possessed) {
            return targIsEnemy && state.players[targetNum]->hp <= state.players[sourceNum]->hp;
        }
        return targIsEnemy && state.enemies[targetNum]->hp <= state.players[sourceNum]->hp;
    }
    throw std::runtime_error("Unhandled case in isValidDiceAction for sourceNum " + std::to_string(sourceNum) + " and targetNum " + std::to_string(targetNum));
    return true || false;
}
    

bool isValidSpellAction(State& state, bool targIsEnemy, std::pair<int, int> spellAction) {
    int spellColor = spellAction.first;
    int targetNum = spellAction.second;
    
    if (state.spellData[spellColor] < 0) { // Spell not available // SpellData.COOLDOWN_USED=-1, SpellData.SINGLE_CASTED=-2
        return false;
    }
    if (spellColor != 0) {
        if (state.players[spellColor - 1]->spellID == -1) { // Character has no spell
            return false;
        }
        if (state.players[spellColor - 1]->dead) { // Dead characters cannot cast spells
            return false;
        }
    }


    if (targIsEnemy && targetNum >= state.enemies.size()) {
        return false;
    }
    if (targIsEnemy && state.enemies[targetNum]->backrow) {
        bool allAreBackrow = true;
        for (const auto& enemy : state.enemies) {
            if (!enemy->backrow) {
                allAreBackrow = false;
                break;
            }
        }
        if (!allAreBackrow) {
            return false; // Cannot target backrow enemies with spells (except for untargeted/cleave spells, which do not have targIsEnemy=True/target a backrow)
        }
    }
    if (!targIsEnemy && (targetNum != -1 && state.players[targetNum]->dead)) {
        return false;
    }
    
    int spellID = (spellColor == 0) ? SpellID::BURST : state.players[spellColor - 1]->spellID;
    SpellTargetReq spellReq = SpellReqsMap[spellID];
    int spellCost = SpellCostMap[spellID] + std::max(0, state.spellData[spellColor]); // positive values for spellColor indicate DEPLETE costs
    bool canCastCost = (state.mana >= spellCost);
    if (!canCastCost && std::find(ALL_TACTICS.begin(), ALL_TACTICS.end(), spellReq) == ALL_TACTICS.end()) { // Tactics can be cast without mana, but still need to meet targeting requirements
        return false;
    }

    if (targetNum == -1) {
        if (spellReq != SpellTargetReq::UNTARGETED && spellReq != SpellTargetReq::RESURRECT && spellReq != SpellTargetReq::DAMAGE_N_SHIELD_3) {
            return false;
        }
    }

    if (spellReq == SpellTargetReq::UNTARGETED) {
        return targetNum == -1;
    } else if (spellReq == SpellTargetReq::ENEMY) {
        return targIsEnemy;
    } else if (spellReq == SpellTargetReq::ALLY) {
        return !targIsEnemy;
    } else if (spellReq == SpellTargetReq::EITHER) {
        return true; // General checks done above for targeting alive/existant enemies/allies
    } else if (spellReq == SpellTargetReq::RESURRECT) {
        if (targetNum != -1) {
            return false;
        }

        for (const auto& hero : state.players) {
            if (hero->dead) {
                return true;
            }
        }
        return false;
    } else if (spellReq == SpellTargetReq::HP1) {
        if (targIsEnemy) {
            return state.enemies[targetNum]->hp == 1;
        }
        return false;
    } else if (spellReq == SpellTargetReq::HP2) {
        if (targIsEnemy) {
            return state.enemies[targetNum]->hp == 2;
        }
        return false;
    } else if (spellReq == SpellTargetReq::HP3) {
        if (targIsEnemy) {
            return state.enemies[targetNum]->hp == 3;
        }
        return false;
    } else if (spellReq == SpellTargetReq::RECHARGE) {
        if (targIsEnemy) {
            return false;
        }
        return state.players[targetNum]->usedDie;
    } else if (spellReq == SpellTargetReq::DAMAGE_N_SHIELD_3 || spellReq == SpellTargetReq::DAMAGE_2 || spellReq == SpellTargetReq::ONE_OF_ALL || spellReq == SpellTargetReq::AN_X) {
        if (findTacticCosts(state, spellReq).size() == 0) {
            return false;
        }

        if (spellReq == SpellTargetReq::DAMAGE_N_SHIELD_3) { // Must be UNTARGETED
            return targetNum == -1;
        } else if (spellReq == SpellTargetReq::DAMAGE_2) { // ALLY
            return !targIsEnemy;
        } else if (spellReq == SpellTargetReq::ONE_OF_ALL) { // ENEMY
            return targIsEnemy;
        } else if (spellReq == SpellTargetReq::AN_X) { // ALLY
            return !targIsEnemy;
        }
    } else {
        throw std::runtime_error("Unknown spell requirement");
    }
    
    throw std::runtime_error("Unhandled case in isValidSpellAction for spellColor " + std::to_string(spellColor) + " and targetNum " + std::to_string(targetNum));
    return false || true;
}
    
std::vector<int> findMaxHP(State& state, bool heavy, bool isEnemy) {
    int maxHP = (heavy) ? -999 : 999;
    std::vector<int> maxIndices;

    if (isEnemy) {
        bool allAreBackrow = true;
        for (const auto& enemy : state.enemies) {
            if (!enemy->backrow) {
                allAreBackrow = false;
                break;
            }
        }

        for (int i = 0; i < state.enemies.size(); i++) {
            Ent* enemy = state.enemies[i];
            if (enemy->backrow && !allAreBackrow) {
                continue;
            }
            if ((enemy->hp >= maxHP && heavy) || (enemy->hp <= maxHP && !heavy)) {
                if (enemy->hp != maxHP) {
                    maxIndices.clear();
                    maxHP = enemy->hp;
                }
                maxIndices.push_back(i);
            }
        }
    } else {
        for (int i = 0; i < state.players.size(); i++) {
            Ent* hero = state.players[i];
            if (hero->dead) {
                continue;
            }
            if ((hero->hp >= maxHP && heavy) || (hero->hp <= maxHP && !heavy)) {
                if (hero->hp != maxHP) {
                    maxIndices.clear();
                    maxHP = hero->hp;
                }
                maxIndices.push_back(i);
            }
        }
    }
    return maxIndices;
}

std::pair<int, Ent*> findMadnessedHero(State& state) {
    if (!state.madnessD) {
        return std::make_pair(-1, nullptr);
    }

    for (int i = 0; i < state.players.size(); i++) {
        Ent* hero = state.players[i];
        if (!hero->dead && std::find(SourceEnt::MAGIC_HEROS.begin(), SourceEnt::MAGIC_HEROS.end(), hero->sourceID) == SourceEnt::MAGIC_HEROS.end()) {
            return std::make_pair(i, hero);
        }
    }
    return std::make_pair(-1, nullptr);
}

bool isValidReroll(State& state, std::array<bool, 5> rerollBits) {
    // Check if the rerollBits is a valid reroll action based on the current state
    // For example, check if the number of rerolls left is sufficient for the number of bits set in rerollBits
    if (state.rerolls <= 0) {
        return false;
    }
    for (int i = 0; i < 5; i++) {
        if (rerollBits[i] && (state.players[i]->dead || state.players[i]->currentSide.keywords[KeywordID::STICKY])) {
            return false;
        }
    }
    return true;
}

bool isValidEndTurn(State& state) {
    std::pair<int, Ent*> possessedHero = findMadnessedHero(state);
    for (int i = 0; i < state.players.size(); i++) {
        if (i == possessedHero.first) {
            return state.players[i]->usedDie || !canUsedPossessedDie(state, i);
        }
        if (state.players[i]->currentSide.keywords[KeywordID::MANDATORY] && !state.players[i]->usedDie && diceUsable(state, i)) {
            return false;
        }
    }
    
    return true;
}

bool isValidAction(State& state, int action) {
    int actionType = ACTION_MAP[action].first;
    std::pair<int, int> actionData = ACTION_MAP[action].second;
    if (actionType == CONTINUE_ACTION) {
        return state.stateType == StateType::EMPTY_FIGHT || state.stateType == StateType::EMPTY_TURN || state.stateType == StateType::EMPTY_REROLL;
    } else {
        if (state.stateType == StateType::EMPTY_FIGHT || state.stateType == StateType::EMPTY_TURN || state.stateType == StateType::EMPTY_REROLL || state.stateType == StateType::WON || state.stateType == StateType::LOST) {
            return false;  // Only CONTINUE_ACTION is valid in these states
        }

        if (actionType == REROLL_ACTION) {
            std::array<bool, 5> rerollBits = util::int2ba<5>(actionData.first);
            return isValidReroll(state, rerollBits);
        } else {
            if (state.rerolls > 0) {
                return false; // If rerolls are present, force AI to use them for training speed. It has the option of '00000' for a reroll
            }

            if (actionType == DICE_ALLY_ACTION || actionType == DICE_ENEMY_ACTION) {
                return isValidDiceAction(state, actionType == DICE_ENEMY_ACTION, actionData);
            } else if (actionType == SPELL_ALLY_ACTION || actionType == SPELL_ENEMY_ACTION) {
                return isValidSpellAction(state, actionType == SPELL_ENEMY_ACTION, actionData);
            } else if (actionType == END_TURN_ACTION) {
                return isValidEndTurn(state);
            }
        }
    }
    throw std::runtime_error("Unknown action type");
}

std::vector<int> validActions(State& state) { // TODO - profile to see if this is a bottleneck. Optimize by not repeating checks for the same characters, and maybe caching actions
    std::vector<int> validActions;
    for (int i = 0; i < ACTION_MAP.size(); i++) {
        // if (i == 100) {
        //     continue; // TODO Remove
        // }
        if (isValidAction(state, i)) {
            validActions.push_back(i);
            
        }
    }

    if (validActions.size() == 0) {
        throw std::runtime_error("No valid actions found for the current state. This should not happen.");
    }

    return validActions;
}


std::vector<int> validActionsFast(State& state) {
    std::vector<bool> validActions(183, false); // Actions will be updated to true if valid, then converted to a vector of ints at the end

    /*
     * Dice actions. Evaluates constant facts about each of the 5 heros, then iterates through them and sets validActions to true as needed
    */
    if (state.stateType == StateType::WON || state.stateType == StateType::LOST) {
        throw std::runtime_error("validActionsFast called on a terminal state");
        return std::vector<int>{};
    }
    if (state.stateType == StateType::EMPTY_FIGHT || state.stateType == StateType::EMPTY_TURN || state.stateType == StateType::EMPTY_REROLL) {
        return std::vector<int>{182};
    }
    if (state.rerolls <= 0) { // can only use dice/cast spells if rerolling is done
    std::pair<int, Ent*> possessedHero = findMadnessedHero(state);


    std::array<bool, 5> canUseDice{false, false, false, false, false};
    std::array<bool, 5> hasRanged{false, false, false, false, false};
    std::array<int, 5> staticPipsArray{0, 0, 0, 0, 0};
    for (int i = 0; i < 5; i++) {
        Ent& hero = *state.players[i];

        if (hero.dead || hero.usedDie || hero.exerted) {
            continue;
        }
        if (hero.petrified[hero.currentSideNum] && !hero.currentSide.keywords[KeywordID::STASIS]) { // Stasis overrides petrify
            continue;
        }

        bool autoUsableSide = std::find(SIDES_AUTO_USABLE.begin(), SIDES_AUTO_USABLE.end(), hero.currentSide.type) != SIDES_AUTO_USABLE.end();
        if (!autoUsableSide) {
            int staticPips = findStaticPips(state, state.players[i]);
            staticPipsArray[i] = staticPips;
            if (staticPips <= 0) {
                continue;
            }
        }

        hasRanged[i] = util::hasCopycattedKeyword(state, hero.currentSide, KeywordID::RANGED);
        canUseDice[i] = true;
    }

    bool allAreBackrow = true;
    for (const auto& enemy : state.enemies) {
        if (!enemy->backrow) {
            allAreBackrow = false;
            break;
        }
    }

    // Scans through the first 80 actions, which are all dice actions, and sets the validActions to true as needed. Avoids repeated calculations
    for (int i=0;i<5;i++) {
        Ent& hero = *state.players[i];

        if (!canUseDice[i]) {
            continue;
        }

        bool possessed = (i == possessedHero.first);
        SideType sideType = hero.currentSide.type;

        bool untargeted = (sideType == SideType::MANA || sideType == SideType::DODGE || sideType == SideType::SELF_DAMAGE || sideType == SideType::DAMAGE_ALL || sideType == SideType::DAMAGE_EVERYONE || sideType == SideType::SHIELD_ALL || sideType == SideType::HEAL_ALL);
        bool isAllyType = (sideType == SideType::SHIELD || sideType == SideType::HEAL || sideType == SideType::HEALSHIELD || sideType == SideType::REDIRECTINCOMING || sideType == SideType::ENCHANT || sideType == SideType::UNDYING);
        bool isEnemyType = (sideType == SideType::DAMAGE);
        if (untargeted) {
            // Untargeted
            validActions[i*16 + 0] = true;
        } else if ((isAllyType && !possessed) || (isEnemyType && possessed)) {
            // Target ally            
            if (hero.currentSide.keywords[KeywordID::HEAVY]) { // Possession, shortcircuits
                for (const auto& target : findMaxHP(state, true, false)) {
                    validActions[i*16 + target + 1] = true;
                }
                continue;
            }
            for (int j=0;j<5;j++) {
                Ent& ally = *state.players[j];
                if (ally.dead) {
                    continue;
                }

                validActions[i*16 + j + 1] = true;
            }

        } else if ((isAllyType && possessed) || (isEnemyType && !possessed)) {
            // Target enemy
            if (hero.currentSide.keywords[KeywordID::HEAVY]) { // Possession, shortcircuits
                for (const auto& target : findMaxHP(state, true, true)) {
                    validActions[i*16 + target + 6] = true;
                }
                continue;
            }
            for (int j=0;j<state.enemies.size();j++) {
                Ent& enemy = *state.enemies[j];
                if (enemy.backrow && !hasRanged[i] && !allAreBackrow) {
                    continue;
                }

                validActions[i*16 + j + 6] = true;
            }
        } else if (sideType == SideType::KILL) {
            if (possessed) {
                for (int j=0;j<5;j++) {
                    Ent& ally = *state.players[j];
                    if (ally.hp <= staticPipsArray[i] && (!ally.dead)) {
                        validActions[i*16 + j + 1] = true;
                    }
                }
            } else {
                for (int j=0;j<state.enemies.size();j++) {
                    Ent& enemy = *state.enemies[j];
                    if (enemy.hp <= staticPipsArray[i]) {
                        validActions[i*16 + j + 6] = true;
                    }
                }
            }
        } else if (sideType == SideType::STUN) {
            if (possessed) {
                for (int j=0;j<5;j++) {
                    Ent& ally = *state.players[j];
                    if (ally.hp <= hero.hp && (!ally.dead)) {
                        validActions[i*16 + j + 1] = true;
                    }
                }
            } else {
                for (int j=0;j<state.enemies.size();j++) {
                    Ent& enemy = *state.enemies[j];
                    if (enemy.hp <= hero.hp && (!enemy.backrow || hasRanged[i] || allAreBackrow)) {
                        validActions[i*16 + j + 6] = true;
                    }
                }
            }
        } else if (sideType == SideType::RECHARGE) {
            if (possessed) {
                continue; // Possessed cannot use RECHARGE
            }
            for (int j=0;j<5;j++) {
                Ent& ally = *state.players[j];
                if (ally.usedDie && ally.currentSide.type != SideType::RECHARGE && !ally.dead) {
                    validActions[i*16 + j + 1] = true;
                }
            }
        } else if (sideType == SideType::RESURRECT) {
            bool heroIsDead = false;
            for (int j=0;j<5;j++) {
                Ent& ally = *state.players[j];
                if (ally.dead) {
                    heroIsDead = true;
                    break;
                }
            }
            if (heroIsDead) {
                validActions[i*16 + 0] = true;
            }
        } else if (sideType == SideType::REROLL || sideType == SideType::BLANK) {
            // cannot use these sides
        } else {
            // heros should not have these sides (SUMMON, ERROR, DAMAGE_FLANKING)
            throw std::runtime_error("Unhandled case in validActionsFast for sourceNum " + std::to_string(i));
        }


    }
        
    /*
     * Spell actions.
    */

    // Burst
    if (state.mana >= 2) {
        for (int i=0;i<5;i++) {
            validActions[80 + i] = !state.players[i]->dead;
        }
        for (int i=0;i<state.enemies.size();i++) {
            Ent& enemy = *state.enemies[i];
            if (enemy.backrow && !allAreBackrow) {
                continue;
            }
            validActions[80 + 5 + i] = true;
        }
    }
    // Player spells - Handling orange and yellow seperately since they do not have many spells
    // Imbue
    if (state.players[0]->sourceID == SPELLBLADE && state.spellData[1] >= 0 && !state.players[0]->dead && state.mana >= 1) {
        for (int i=0;i<5;i++) {
            Ent& ally = *state.players[i];
            if (!ally.dead) {
                validActions[95 + i] = true;
            }
        }
    }
    if (state.players[1]->sourceID == CAPTAIN && !state.players[1]->dead) {
        if (findTacticCosts(state, SpellTargetReq::DAMAGE_N_SHIELD_3).size() > 0) {
            validActions[100] = true; // Tactics can be cast without mana, but still need to meet targeting requirements
        }
    }
    for (int i=3;i<6;i++) { // All remaining spells for gray, red, and blue
        Ent& hero = *state.players[i-1];
        int spellID = hero.spellID;
        if (hero.dead || state.spellData[i] < 0 || spellID == -1) {
            continue;
        }

        SpellTargetReq spellReq = SpellReqsMap[spellID];
        int spellCost = SpellCostMap[spellID] + std::max(0, state.spellData[i]); // positive values for spellColor indicate DEPLETE costs
        bool canCastCost = (state.mana >= spellCost);
        if (!canCastCost && std::find(ALL_TACTICS.begin(), ALL_TACTICS.end(), spellReq) == ALL_TACTICS.end()) { // Tactics can be cast without mana, but still need to meet targeting requirements
            continue;
        }

        int index = 101 + (i-3)*16; // 101 is the first spell action, then each hero has 16 actions

        if (spellReq == SpellTargetReq::UNTARGETED) {
            validActions[index + 0] = true;
        }
        if (spellReq == SpellTargetReq::ENEMY || spellReq == SpellTargetReq::EITHER) {
            for (int j=0;j<state.enemies.size();j++) {
                Ent& enemy = *state.enemies[j];
                if (enemy.backrow && !allAreBackrow) {
                    continue;
                }
                validActions[index + j + 6] = true;
            }
        }
        if (spellReq == SpellTargetReq::ALLY || spellReq == SpellTargetReq::EITHER) {
            for (int j=0;j<5;j++) {
                Ent& ally = *state.players[j];
                if (!ally.dead) {
                    validActions[index + j + 1] = true;
                }
            }
        }
        if (spellReq == SpellTargetReq::RESURRECT) {
            bool heroIsDead = false;
            for (int j=0;j<5;j++) {
                Ent& ally = *state.players[j];
                if (ally.dead) {
                    heroIsDead = true;
                    break;
                }
            }
            if (heroIsDead) {
                validActions[index + 0] = true;
            }
        }
        if (spellReq == SpellTargetReq::HP1 || spellReq == SpellTargetReq::HP2 || spellReq == SpellTargetReq::HP3) {
            int amount = 1;
            if (spellReq == SpellTargetReq::HP2) {
                amount = 2;
            } else if (spellReq == SpellTargetReq::HP3) {
                amount = 3;
            }
            for (int j=0;j<state.enemies.size();j++) {
                Ent& enemy = *state.enemies[j];
                if (enemy.hp == amount && (!enemy.backrow || allAreBackrow)) {
                    validActions[index + j + 6] = true;
                }
            }
        }
        if (spellReq == SpellTargetReq::RECHARGE) {
            for (int j=0;j<5;j++) {
                Ent& ally = *state.players[j];
                if (ally.usedDie && !ally.dead) {
                    validActions[index + j + 1] = true;
                }
            }
        }
        if (spellReq == SpellTargetReq::DAMAGE_2) {
            if (findTacticCosts(state, SpellTargetReq::DAMAGE_2).size() > 0) {
                for (int j=0;j<5;j++) {
                    Ent& ally = *state.players[j];
                    if (!ally.dead) {
                        validActions[index + j + 1] = true;
                    }
                }
            }
        }
        if (spellReq == SpellTargetReq::ONE_OF_ALL) {
            if (findTacticCosts(state, SpellTargetReq::ONE_OF_ALL).size() > 0) {
                for (int j=0;j<state.enemies.size();j++) {
                    Ent& enemy = *state.enemies[j];
                    if (!enemy.backrow || allAreBackrow) {
                        validActions[index + j + 6] = true;
                    }
                }
            }
        }
        if (spellReq == SpellTargetReq::AN_X) {
            if (findTacticCosts(state, SpellTargetReq::AN_X).size() > 0) {
                for (int j=0;j<5;j++) {
                    Ent& ally = *state.players[j];
                    if (!ally.dead) {
                        validActions[index + j + 1] = true;
                    }
                }
            }
        }
    }

    // Ent turn
    if (isValidEndTurn(state)) {
        validActions[181] = true; // END_TURN_ACTION
    }

    } // End of (state.rerolls == 0) check
    if (state.rerolls > 0) {
        std::array<bool, 5> allowReroll{false, false, false, false, false};
        for (int i = 0; i < 5; i++) {
            if (state.players[i]->dead || state.players[i]->currentSide.keywords[KeywordID::STICKY]) {
                allowReroll[i] = false;
            } else {
                allowReroll[i] = true;
            }
        }
        int index = 149;
        for (int i=0;i<32;i++) {
            if (i & 1 && !allowReroll[0]) continue;
            if (i & 2 && !allowReroll[1]) continue;
            if (i & 4 && !allowReroll[2]) continue;
            if (i & 8 && !allowReroll[3]) continue;
            if (i & 16 && !allowReroll[4]) continue;

            validActions[index + i] = true;
        }   
    }


    std::vector<int> ret;
    for (int i=0;i<validActions.size();i++) {
        if (validActions[i]) {
            ret.push_back(i);
        }
    }

    return ret;
}

} // namespace validActions