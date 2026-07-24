#include "state.h"

#include <stdexcept>
#include <random>

State::State(const std::array<Ent, 5> players, const std::vector<Ent>& enemies) {
    for (int i = 0; i < 5; i++) {
        this->players[i] = new Ent(players[i]);
    }
    for (const auto& enemy : enemies) {
        this->enemies.push_back(new Ent(enemy));
    }
    spellData = {0, 0, 0, 0, 0, 0};
    
    mana = 0;
    fortoldMana = 0;
    rerolls = 0;
    
    enemyTargets = std::vector<std::vector<Ent*>>();
    
    imminentSummons = std::vector<Ent*>();
    reinforcements = std::vector<Ent*>();

    turn = 0;
    lastTarget = nullptr;
    prevLastStrikeVal = -1;
    lastStrikeVal = -1;
    lastDie = std::nullopt;
    
    level = 0;
    playerLevels = {1, 1, 1, 1, 1};
    
    enemyDamageD = false;
    hexiaD = false;
    bansheeD = 0;
    brambleD = false;
    madnessD = false;
    handD = false;
    warchiefD = 0;
    stateType = StateType::EMPTY_FIGHT;
}
State::~State() {
    _deleteState();
}
State::State(const State& other) {
    _copyState(other);
}
State& State::operator=(const State& other) {
    if (this != &other) {
        _deleteState();
        _copyState(other);
    }
    return *this;
}

void State::_deleteState() {
    for (auto player : players) {
        delete player;
    }
    for (auto enemy : enemies) {
        delete enemy;
    }
    for (auto enemy : imminentSummons) {
        delete enemy;
    }
    for (auto enemy : reinforcements) {
        delete enemy;
    }
    players.fill(nullptr);
    enemies.clear();
    imminentSummons.clear();
    reinforcements.clear();
}
Ent* State::_findEntPointer(const State& other, const Ent* targetEnt) {
    for (int i = 0; i < 5; i++) {
        if (other.players[i] == targetEnt) {
            return players[i];
        }
    }
    for (int i = 0; i < other.enemies.size(); i++) {
        if (other.enemies[i] == targetEnt) {
            return enemies[i];
        }
    }
    throw std::runtime_error("Ent pointer not found in other state");
    return nullptr;
}

void State::_copyState(const State& other) {
    players.fill(nullptr);
    enemies.clear();
    imminentSummons.clear();
    reinforcements.clear();
    for (int i = 0; i < 5; i++) {
        players[i] = new Ent(*other.players[i]);
    }
    for (const auto& enemy : other.enemies) {
        enemies.push_back(new Ent(*enemy));
    }
    for (const auto& enemy : other.imminentSummons) {
        imminentSummons.push_back(new Ent(*enemy));
    }
    for (const auto& enemy : other.reinforcements) {
        reinforcements.push_back(new Ent(*enemy));
    }

    // Pointer management
    enemyTargets.clear();
    for (const auto& enemyTargetList : other.enemyTargets) {
        std::vector<Ent*> newEnemyTargetList;
        for (const auto& enemyTarget : enemyTargetList) {
            newEnemyTargetList.push_back(_findEntPointer(other, enemyTarget));
        }
        enemyTargets.push_back(newEnemyTargetList);
    }
    if (other.lastTarget != nullptr) {
        lastTarget = _findEntPointer(other, other.lastTarget);
    } else {
        lastTarget = nullptr;
    }

    for (unsigned i=0; i<5; i++) {
        if (other.players[i]->redirectTarget != nullptr) {
            players[i]->redirectTarget = _findEntPointer(other, other.players[i]->redirectTarget);
        } else {
            players[i]->redirectTarget = nullptr;
        }
    }

    // Non memory members
    spellData = other.spellData;
    mana = other.mana;
    fortoldMana = other.fortoldMana;
    rerolls = other.rerolls;

    turn = other.turn;
    prevLastStrikeVal = other.prevLastStrikeVal;
    lastStrikeVal = other.lastStrikeVal;
    lastDie = other.lastDie;
    
    level = other.level;
    playerLevels = other.playerLevels;

    enemyDamageD = other.enemyDamageD;
    hexiaD = other.hexiaD;
    bansheeD = other.bansheeD;
    brambleD = other.brambleD;
    madnessD = other.madnessD;
    handD = other.handD;
    warchiefD = other.warchiefD;
    stateType = other.stateType;
}

void State::addEnemyBatch(const std::vector<Ent>& newEnemies) {
    if (enemies.size() != 0) {
        throw std::runtime_error("addEnemyBatch can only be called on an empty enemies vector.");
    }
    for (const auto& enemy : newEnemies) {
        enemies.push_back(new Ent(enemy));
    }
}

void State::replaceHero(int index, const Ent& hero) {
    std::vector<std::pair<int, int>> retargeting;
    for (int i=0;i<enemyTargets.size();i++) {
        std::vector<Ent*>& monTargetData = enemyTargets[i];
        for (int j=0;j<monTargetData.size();j++) {
            if (monTargetData[j] == players[index]) {
                retargeting.push_back(std::make_pair(i, j));
            }
        }
    }
    std::vector<std::pair<int, int>> redirectRetargeting;
    for (int i=0;i<5;i++) {
        if (players[i]->redirectTarget == players[index]) {
            redirectRetargeting.push_back(std::make_pair(i, index));
        }
    }


    delete players[index]; // Delete the old Ent to prevent memory leak
    players[index] = new Ent(hero); // Create a new Ent and assign it to the players array

    int D6 = rand() % 6; // Generate a random number between 0 and 5
    players[index]->currentSideNum = D6;
    players[index]->currentSide = players[index]->sides[D6];

    
    
    for (const auto& pair : retargeting) {
        enemyTargets[pair.first][pair.second] = players[index];
    }
    for (const auto& pair : redirectRetargeting) {
        players[pair.first]->redirectTarget = players[index];
    }
}

void State::addImminentSummon(const Ent& newEnemy) {
    imminentSummons.push_back(new Ent(newEnemy));
}
void State::addReinforcement(const Ent& newEnemy) {
    reinforcements.push_back(new Ent(newEnemy));
}
void State::killEnemy(int index) {
    Ent* dyingMon = enemies[index];
    delete dyingMon; // Free the memory for the dying monster
    enemies.erase(enemies.begin() + index); // Remove the pointer from the vector
    enemyTargets.erase(enemyTargets.begin() + index); // Remove the corresponding targets
}
