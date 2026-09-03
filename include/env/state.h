#pragma once
#include <vector>
#include <array>
#include <optional>
#include <stdexcept>

#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/utility.hpp>

#include "env/ents.h"
#include "env/constants.h"

enum class StateType {
    BATTLE = 0,              // "Normal" state
    EMPTY_FIGHT = 1,          // The momentary state between battles where the next battle is not yet generated
                                // This fits well for transposition tables, since w/o enemies or playerSides selected, there are few variables
                                // Specifically, the for a particular MCTS search, the only thing that could be different is which heros are defeated
    EMPTY_TURN = 2,          // The momentary state between turns where the next turn's rolls are not completed, for transposition tables.
                                // Only available action is "Continue"
                                // Also for transposition tables, since w/o rolls, there are few variables.
    EMPTY_REROLL = 3,        // A state indicating that the player has just finished rerolling, making it more 
                                // convenient to switch to searching for an optimal turn without randomness
    HERO_SELECT = 4,         // Hero select, the next battle has been generated and is available for view to the player
    LOST = 5,                // All heros have fallen
    WON = 6                  // Lvl 20 has been completed (or tied)
};

class State {
public:
    // players, enemies, reinforcements, and imminentSummons are all pointers to Ents, and are the only pointers with responsibility for memory management.
    // enemyTargets, redirectTarget, and lastTarget are all pointers to Ents, but do not have responsibility for memory management.
    State() : State(std::array<Ent, 5>{}, std::vector<Ent>{}) {}
    State(const std::array<Ent, 5> players, const std::vector<Ent>& enemies);
    ~State();
    State(const State& other);
    State& operator=(const State& other);

    bool operator==(const State& other) const;


    void addEnemyBatch(const std::vector<Ent>& newEnemies);

    void replaceHero(int index, const Ent& hero);
    void addImminentSummon(const Ent& newEnemy);
    void addReinforcement(const Ent& newEnemy);

    void killEnemy(int index); // deletes enemy, removes it from state.enemies and state.enemyTargets

private:
    void _deleteState();
    void _copyState(const State& other);
    Ent* _findEntPointer(const State& other, const Ent* targetEnt);
    
    
public:
    // Players
    std::array<Ent*, 5> players; // List of Ents for the players, length 5
    std::array<int, 6> spellData; // 0 is burst, 1-5 are colors
    
    int mana; // current mana for team
    int fortoldMana; // mana that will be gained next turn from foretell
    int rerolls; // number of rerolls left for team. Negative values represent gaze value
    
    // Monsters
    std::vector<Ent*> enemies;
    std::vector<std::vector<Ent*>> enemyTargets;    // List of monsters: For each monster:
                                                    // a list of Ents for attacks. This list is 1 for each hero being attacked, ie, 
                                                    // 3 for cleave (typically), 2 for descend, 1 for heal, damage, 5 for damage_all, 10ish for damage_everyone
    
    std::vector<Ent*> imminentSummons; // enemies that will be summoned at the end of an action
    std::vector<Ent*> reinforcements; // queue. [0] is the next reinforcement to be added

    
    // Game:
    int turn;
    Ent* lastTarget; // for focus
    int prevLastStrikeVal; // for trio
    int lastStrikeVal; // like lastdie, but with keywords evaluated
    std::optional<Side> lastDie; // for chain, copycat, etc.
    
    // Progression
    int level;
    std::array<int, 5> playerLevels; // list of 5 levels, for each color
    
    // MonPassives
    bool enemyDamageD; // if any enemy was damaged this turn, for blind
    bool hexiaD; // if hexia is present
    int bansheeD; // number of banshees present, negative if their scream has been cast, positive if not
    bool brambleD; // if bramble is present
    bool madnessD; // if madness is present
    bool handD; // if the hand is present
    int warchiefD; // number of warchiefs present
    
    // State type
    StateType stateType; // Used to make it more convenient for RL agent to learn/search

};

std::size_t hash_value(State const& s);
    
    
namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, State & g, const unsigned int version)
{
    ar & g.players;
    ar & g.spellData;
    ar & g.mana;
    ar & g.fortoldMana;
    ar & g.rerolls;
    ar & g.enemies;
    ar & g.enemyTargets;
    ar & g.imminentSummons;
    ar & g.reinforcements;
    ar & g.turn;
    ar & g.lastTarget;
    ar & g.prevLastStrikeVal;
    ar & g.lastStrikeVal;
    ar & g.lastDie;
    ar & g.level;
    ar & g.playerLevels;
    ar & g.enemyDamageD;
    ar & g.hexiaD;
    ar & g.bansheeD;
    ar & g.brambleD;
    ar & g.madnessD;
    ar & g.handD;
    ar & g.warchiefD;
    ar & g.stateType;
}

} // namespace serialization
} // namespace boost