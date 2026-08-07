#pragma once

#include <utility>

typedef std::pair<bool, int> EntPointer; // isEnemy, index pair.

inline constexpr int MAX_NUM_SPECIAL_HP = 31;  // Max possible special HP for any monster

inline constexpr int NUM_KEYWORDS = 57;  // Number of keywords in the game
inline constexpr int START_STATICS = 8;  // first 7 are only for checking valid actions/targeting things, 8th is for copycat
inline constexpr int START_PRE_MON_EFFECTS = 20;
inline constexpr int START_DYNAMICS = 21;  // after keyword 21, save the amount for comparing with pair and trio. Engage, cruel, etc. dont count
inline constexpr int START_APPLIED_EFFECTS = 24;  // after keyword 24, the keywords are applied effects (poison, cleanse, etc.)
inline constexpr int ACT_OCCUR = 39;  // At 39, these things effect whether the dice is "used" and who is hit by the use of the dice
inline constexpr int ACT_POST_OCCUR = 47;

inline constexpr int NUM_SPELLS = 51;  // Number of spells in the game

inline constexpr int NUM_ON_HIT_PASSIVES = 33;  // Number of on-hit passives in the game

inline constexpr int HERO_TEAM_SIZE = 5;  // Number of heros in a team
inline constexpr int NUM_HEROS = 107;  // Number of heros in the game
inline constexpr int NUM_MONSTERS = 69;  // Number of monsters in the game

inline constexpr int NUM_ACTIONS = 183;  // Number of actions in the game