#pragma once

#include <vector>
#include <utility>
#include <array>

#include "env/constants.h"
#include "env/ents.h"

/**
 * Constant library of all ents in the game. Used for creating enemies/heros from scratch.
 */

namespace SourceEnt {
    void initializeSourceEnts();

    extern std::array<Ent, NUM_HEROS> ALL_HEROS; // All heros, indexed by sourceID
    extern std::array<Ent, NUM_MONSTERS> ALL_MONSTERS;
    
    extern std::vector<int> ORANGE_HEROS_LVL_1_IDS;
    extern std::vector<int> ORANGE_HEROS_LVL_2_IDS;
    extern std::vector<int> ORANGE_HEROS_LVL_3_IDS;
    extern std::vector<int> YELLOW_HEROS_LVL_1_IDS;
    extern std::vector<int> YELLOW_HEROS_LVL_2_IDS;
    extern std::vector<int> YELLOW_HEROS_LVL_3_IDS;
    extern std::vector<int> GRAY_HEROS_LVL_1_IDS;
    extern std::vector<int> GRAY_HEROS_LVL_2_IDS;
    extern std::vector<int> GRAY_HEROS_LVL_3_IDS;
    extern std::vector<int> RED_HEROS_LVL_1_IDS;
    extern std::vector<int> RED_HEROS_LVL_2_IDS;
    extern std::vector<int> RED_HEROS_LVL_3_IDS;
    extern std::vector<int> BLUE_HEROS_LVL_1_IDS;
    extern std::vector<int> BLUE_HEROS_LVL_2_IDS;
    extern std::vector<int> BLUE_HEROS_LVL_3_IDS;

    extern std::vector<std::vector<std::vector<int>>> HERO_TREE; // (color, level) -> List of hero Indexes
    extern std::vector<int> MAGIC_HEROS; // All magic heros, indexed by sourceID
}