#include "env/util.h"

#include "env/reversedIDs.h"

namespace util {

std::vector<Ent*> findHeroCleaveTargets(const State& state, const Ent& ent) {
    
    int index = ent.positionID;
    int center = index;
    std::vector<Ent*> l;
    index = center + 1;
    while (index < state.players.size()) {
        if (!state.players.at(index)->dead) {
            l.push_back(state.players.at(index));
            break;
        }
        index += 1;
    }
    l.push_back(state.players.at(center));
    index = center - 1;
    while (index >= 0) {
        if (!state.players.at(index)->dead) {
            l.push_back(state.players.at(index));
            break;
        }
        index -= 1;
    }

    return l;
}

std::vector<Ent*> findHeroDescendTargets(const State& state, const Ent& ent) {
    int index = ent.positionID;
    int center = index;
    std::vector<Ent*> l;
    index = center + 1;
    while (index < state.players.size()) {
        if (!state.players.at(index)->dead) {
            l.push_back(state.players.at(index));
            break;
        }
        index += 1;
    }
    l.push_back(state.players.at(center));

    return l;
}

int findEnemyIndex(const State& state, const Ent& target) {
    int index = (int) (std::find(state.enemies.begin(), state.enemies.end(), &target) - state.enemies.begin()); // PositionID ensures uniqueness
    return index;
}

std::vector<Ent*> findEnemyCleaveTargets(const State& state, const Ent& target) {
    int index = findEnemyIndex(state, target); // PositionID ensures uniqueness
    std::vector<Ent*> l;
    if (index + 1 < state.enemies.size()) {
        l.push_back(state.enemies.at(index + 1));
    }
    l.push_back(state.enemies.at(index));
    if (index - 1 >= 0) {
        l.push_back(state.enemies.at(index - 1));
    }
    return l;
}

Ent* findTopPlayer(const State& state) {
    int index = 0;
    while (index < 5 && state.players.at(index)->dead) {
        index += 1;
    }
    if (index < 5) {
        return state.players.at(index);
    }
    return nullptr;
}
Ent* findBottomPlayer(const State& state) {
    int index = 4;
    while (index >= 0 && state.players.at(index)->dead) {
        index -= 1;
    }
    if (index >= 0) {
        return state.players.at(index);
    }
    return nullptr;
}
Ent* findMiddlePlayer(const State& state) { // Technically inaccurate to finding the middle player, but close enough
    if (!state.players[2]->dead) {
        return state.players[2];
    } else if (!state.players[1]->dead) {
        return state.players[1];
    } else if (!state.players[3]->dead) {
        return state.players[3];
    } else if (!state.players[0]->dead) {
        return state.players[0];
    } else if (!state.players[4]->dead) {
        return state.players[4];
    }
    return nullptr;
}

bool hasRoom(const State& state, const Ent& reinforceEnt) {
    int count = reinforceEnt.size;
    for (const auto& ene : state.enemies) {
        count += ene->size;
    }
    for (const auto& summon : state.imminentSummons) {
        count += summon->size;
    }
    return count <= 165;
}

bool targetIsDying(const State& state, const Ent& ent) {
    int netPoison = std::max((ent.poison + ent.incomingPoison) - (ent.regen), 0); // ent.poison may be negative
    int effectiveHP = std::min(ent.hp, ent.maxHP);
    bool dyingToPoison = netPoison >= effectiveHP;
    bool dyingToDamage = (netPoison + ent.incomingDamage >= effectiveHP + ent.shield);

    if (ent.undead || ent.intangibled || ent.dead) {
        return false;
    }
    if (ent.dodged) {
        return dyingToPoison;
    }
    return dyingToPoison || dyingToDamage;
}
bool hasCopycattedKeyword(const State& state, const Side& side, int keywordID) {
    if (side.keywords[keywordID]) {
        return true;
    }
    if (side.keywords[KeywordID::COPYCAT] && state.lastDie.has_value() && state.lastDie->keywords[keywordID]) {
        return true;
    }
    return false;
}

template <int N>
std::array<bool, N> makeBoolArray(const std::vector<int>& keywords){
    std::array<bool, N> b;
    for (int i = 0; i < N; i++) {
        b[i] = false;
    }
    for (int keyword : keywords) {
        b[keyword] = true;
    }
    return b;
}

int monIDtoPos(int monID) {
    return (monID + 1) * -1;
}
int heroIDtoPos(int heroID) {
    return heroID;
}

void printState(const State& state, bool printEnts) {
    std::cout << "/STATE=====================================" << std::endl;

    std::cout << "Players: " << std::endl;
    for (int i=0;i<state.players.size();i++) {
        std::cout << "\tPlayer " << i << ": " << state.players[i]->sourceID << "-" << entsReversedIDs.at(state.players[i]->sourceID) << std::endl;
    }
    std::cout << "Enemies: " << std::endl;
    for (int i=0;i<state.enemies.size();i++) {
        std::cout << "\tEnemy " << i << ": " << state.enemies[i]->sourceID << "-" << entsReversedIDs.at(state.enemies[i]->sourceID) << std::endl;
    }

    std::cout << "Enemy Targets: " << std::endl;
    for (int i=0;i<state.enemyTargets.size();i++) {
        std::cout << "\tEnemy " << i << ": " << state.enemies[i]->sourceID << "-" << entsReversedIDs.at(state.enemies[i]->sourceID) << " targets: " << std::endl;
        for (int j=0;j<state.enemyTargets[i].size();j++) {
            std::cout << "\t\tEnemytarget " << j << ": " << state.enemyTargets[i][j]->sourceID << "-" << entsReversedIDs.at(state.enemyTargets[i][j]->sourceID) << std::endl;
        }
    }

    if (state.imminentSummons.size() > 0) {
        std::cout << "Imminent Summons: " << std::endl;
        for (int i=0;i<state.imminentSummons.size();i++) {
            std::cout << "\tImminent Summon " << i << ": " << state.imminentSummons[i]->sourceID << "-" << entsReversedIDs.at(state.imminentSummons[i]->sourceID) << std::endl;
        }
    }
    if (state.reinforcements.size() > 0) {
        std::cout << "Reinforcements: " << std::endl;
        for (int i=0;i<state.reinforcements.size();i++) {
            std::cout << "\tReinforcement " << i << ": " << state.reinforcements[i]->sourceID << "-" << entsReversedIDs.at(state.reinforcements[i]->sourceID) << std::endl;
        }
    }


    std::cout << "Spell Data: ";
    for (int i=0;i<state.spellData.size();i++) {
        std::cout << state.spellData[i] << ",";
    }
    std::cout << std::endl;


    std::cout << "Mana: " << state.mana << std::endl;
    std::cout << "Foretell Mana: " << state.fortoldMana << std::endl;
    std::cout << "Rerolls: " << state.rerolls << std::endl;
    
    // Game:
    std::cout << "Turn: " << state.turn << std::endl;
    if (state.lastTarget != nullptr) {
        std::cout << "Last Target: " << state.lastTarget->sourceID << "-" << entsReversedIDs.at(state.lastTarget->sourceID) << std::endl;
    } else {
        std::cout << "Last Target: None" << std::endl;
    }
    std::cout << "Prev Last Strike Val: " << state.prevLastStrikeVal << std::endl;
    std::cout << "Last Strike Val: " << state.lastStrikeVal << std::endl;
    if (state.lastDie.has_value()) {
        std::cout << "Last Die: " << state.lastDie->value << "-" << sidesReversedIDs.at(state.lastDie->type) << std::endl;
    } else {
        std::cout << "Last Die: None" << std::endl;
    }
    std::cout << "Level: " << state.level << std::endl;
    std::cout << "Player Levels: ";
    for (int i=0;i<state.playerLevels.size();i++) {
        std::cout << state.playerLevels[i] << ",";
    }
    std::cout << std::endl;
    
    std::cout << "MonPassives: " << std::endl;
    std::cout << "\tEnemy Damage D: " << state.enemyDamageD << std::endl;
    std::cout << "\tHexia D: " << state.hexiaD << std::endl;
    std::cout << "\tBanshee D: " << state.bansheeD << std::endl;
    std::cout << "\tBramble D: " << state.brambleD << std::endl;
    std::cout << "\tMadness D: " << state.madnessD << std::endl;
    std::cout << "\tHand D: " << state.handD << std::endl;
    std::cout << "\tWarchief D: " << state.warchiefD << std::endl;

    std::cout << "State Type: " << stateTypesReversedIDs.at(state.stateType) << std::endl;

    if (printEnts) {
        std::cout << "===========================================" << std::endl;
        std::cout << "Printing Ents..." << std::endl;
        for (int i=0;i<state.players.size();i++) {
            printEnt(state, *state.players[i]);
        }
        for (int i=0;i<state.enemies.size();i++) {
            printEnt(state, *state.enemies[i]);
        }
    }
    std::cout << "===========================================" << std::endl;
}

void printEnt(const State& state, const Ent& ent) {
    std::cout << "/ENT=====================================" << std::endl;
    std::cout << "Entity " << ent.sourceID << "-" << entsReversedIDs.at(ent.sourceID) << " @ " << &ent << std::endl;
    
    std::cout << "hp: " << ent.hp << std::endl;
    std::cout << "maxHP: " << ent.maxHP << std::endl;
    std::cout << "shield: " << ent.shield << std::endl;
    std::cout << "undead: " << ent.undead << std::endl;
    std::cout << "dodged: " << ent.dodged << std::endl;
    std::cout << "intangibled: " << ent.intangibled << std::endl;
    
    std::cout << "dead: " << ent.dead << std::endl;
    std::cout << "usedDie: " << ent.usedDie << std::endl;
    std::cout << "timesUsed: " << ent.timesUsed << std::endl;
    std::cout << "timesDamaged: " << ent.timesDamaged << std::endl;

    std::cout << "cleansed: " << ent.cleansed << std::endl;
    std::cout << "poison: " << ent.poison << std::endl;
    std::cout << "weaken: " << ent.weaken << std::endl;
    std::cout << "petrifyAmount: " << ent.petrifyAmount << std::endl;

    std::cout << "petrified: ";
    for (int i=0;i<ent.petrified.size();i++) {
        std::cout << ent.petrified[i] << ",";
    }
    std::cout << std::endl;
    
    std::cout << "toexert: " << ent.toexert << std::endl;
    std::cout << "exerted: " << ent.exerted << std::endl;
    
    std::cout << "regen: " << ent.regen << std::endl;
    std::cout << "smith: " << ent.smith << std::endl;
    std::cout << "boost: " << ent.boost << std::endl;
    std::cout << "permaboost: " << ent.permaboost << std::endl;
    std::cout << "vulnerable: " << ent.vulnerable << std::endl;
    std::cout << "armor: " << ent.armor << std::endl;
    std::cout << "backrow: " << ent.backrow << std::endl;
    std::cout << "size: " << ent.size << std::endl;
    
    std::cout << "inflictedPained: " << ent.inflictedPained << std::endl;
    std::cout << "inflictedExerted: " << ent.inflictedExerted << std::endl;
    std::cout << "inflictedDeathed: " << ent.inflictedDeathed << std::endl;
    std::cout << "inflictedSingleUse: " << ent.inflictedSingleUse << std::endl;
    
    if (ent.redirectTarget != nullptr) {
        std::cout << "redirectTarget: " << ent.redirectTarget->sourceID << "-" << entsReversedIDs.at(ent.redirectTarget->sourceID) << " @ " << ent.redirectTarget << std::endl;
    } else {
        std::cout << "redirectTarget: None" << std::endl;
    }
    std::cout << "incomingDamage: " << ent.incomingDamage << std::endl;
    std::cout << "incomingPoison: " << ent.incomingPoison << std::endl;
    
    std::cout << "currentSideNum: " << ent.currentSideNum << std::endl;
    std::cout << "currentSide: " << ent.currentSide.value << "-" << sidesReversedIDs.at(ent.currentSide.type) << std::endl;
    for (int i=0;i<NUM_KEYWORDS;i++) {
        if (ent.currentSide.keywords[i]) {
            std::cout << "\tKeyword " << i << ": " << keywordsReversedIDs.at(i) << std::endl;
        }
    }

    std::cout << "Sides\n";
    for (int i=0;i<ent.sides.size();i++) {
        std::cout << "\tSide " << i << ": " << ent.sides[i].value << "-" << sidesReversedIDs.at(ent.sides[i].type) << std::endl;
        for (int j=0;j<NUM_KEYWORDS;j++) {
            if (ent.sides[i].keywords[j]) {
                std::cout << "\t\tKeyword " << j << ": " << keywordsReversedIDs.at(j) << std::endl;
            }
        }
    }

    std::cout << "SpellID: " << ent.spellID << std::endl;
    std::cout << "OnHitFunction: " << ent.onHitFunction << std::endl;
    std::cout << "SpecialHP: ";
    for (int i=0;i<ent.specialHP.size();i++) {
        if (ent.specialHP[i]) {
            std::cout << 1;
        } else {
            std::cout << 0;
        }
    }
    std::cout << std::endl;

    std::cout << "SourceID: " << ent.sourceID << "-" << entsReversedIDs.at(ent.sourceID) << std::endl;
    std::cout << "PositionID: " << ent.positionID << std::endl;

    std::cout << "===========================================" << std::endl;
}


template <int N>
std::array<bool, N> int2ba(const int value) {
    std::array<bool, N> b;

    int mask = 1;
    for (int i = 0; i < N; ++i) {
        if (value & mask) {
            b[i] = true;
        } else {
            b[i] = false;
        }
        mask <<= 1;
    }
    return b;
}

template <int N>
int ba2int(const std::array<bool, N>& b) {
    int value = 0;
    int mask = 1;
    for (int i = 0; i < N; ++i) {
        if (b[i]) {
            value |= mask;
        }
        mask <<= 1;
    }
    return value;
}
template std::array<bool, NUM_KEYWORDS> int2ba<NUM_KEYWORDS>(const int value); // summon sides
template std::array<bool, 5> int2ba<5>(const int value); // actions to dice array
template std::array<bool, MAX_NUM_SPECIAL_HP> int2ba<MAX_NUM_SPECIAL_HP>(const int value); // basalt hp
template int ba2int<MAX_NUM_SPECIAL_HP>(const std::array<bool, MAX_NUM_SPECIAL_HP>& b); // basalt hp
template int ba2int<NUM_KEYWORDS>(const std::array<bool, NUM_KEYWORDS>& b); // summon sides
template int ba2int<5>(const std::array<bool, 5>& b); // user input to actionID, then back to array later
template std::array<bool, NUM_KEYWORDS> makeBoolArray<NUM_KEYWORDS>(const std::vector<int>& keywords); // sides with keywords generation
template std::array<bool, MAX_NUM_SPECIAL_HP> makeBoolArray<MAX_NUM_SPECIAL_HP>(const std::vector<int>& keywords); // specialHP generation

std::string getActionStr(int act) {
    int action = ACTION_MAP[act].first;
    std::pair<int, int> data = ACTION_MAP[act].second;
    if (action == DICE_ALLY_ACTION) {
        return "DA" + std::to_string(data.first) + std::to_string(data.second);
    } else if (action == DICE_ENEMY_ACTION) {
        return "DE" + std::to_string(data.first) + std::to_string(data.second);
    } else if (action == SPELL_ALLY_ACTION) {
        return "SA" + std::to_string(data.first) + std::to_string(data.second);
    } else if (action == SPELL_ENEMY_ACTION) {
        return "SE" + std::to_string(data.first) + std::to_string(data.second);
    } else if (action == REROLL_ACTION) {
        std::array<bool, 5> boolarray = int2ba<5>(data.first);
        std::string dataStr = "R";
        for (int i=0;i<5;i++) {
            if (boolarray[i]) {
                dataStr += "1";
            } else {
                dataStr += "0";
            }
        }
        return dataStr;
    } else if (action == END_TURN_ACTION) {
        return "E";
    }
    else if (action == CONTINUE_ACTION) {
        return "C";
    }
    return std::to_string(act);
}
}