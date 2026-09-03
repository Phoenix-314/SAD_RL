#pragma once
#include "env/constants.h"
#include <array>

#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/utility.hpp>

/*
 * Ent dataclass - stores the data of a single entity, such as a hero or monster
 * Side dataclass - stores the data of a single side of a die, including its value, type, and keywords
 * SideType enum - list of possible sidetypes (includes all sides on base hero/monster types, but not all possible sides in og SAD)
*/

enum class SideType {
    DAMAGE = 0,
    SHIELD = 1,
    HEAL = 2,
    HEALSHIELD = 3,
    MANA = 4,
    BLANK = 5,
    REROLL = 6,
    REDIRECTINCOMING = 7,
    SUMMON = 8,
    RECHARGE = 9,
    RESURRECT = 10,
    ENCHANT = 11,
    KILL = 12,
    UNDYING = 13,
    DODGE = 14,
    SELF_DAMAGE = 15,
    DAMAGE_ALL = 16,
    DAMAGE_EVERYONE = 17,
    DAMAGE_FLANKING = 18,
    SHIELD_ALL = 19,
    HEAL_ALL = 20,
    STUN = 21,
    ERROR = 22
};
inline constexpr std::array<SideType, 5> SIDES_AUTO_USABLE = {SideType::STUN, SideType::RECHARGE, SideType::ENCHANT, SideType::UNDYING, SideType::DODGE};  // Do not require positive pip values


class Side {
public:
    Side() : value(-1), type(SideType::BLANK), keywords({false}) {}
    Side(int value, SideType type, const std::array<bool, NUM_KEYWORDS>& keywords)
        : value(value), type(type), keywords(keywords) {}
    bool operator==(const Side& other) const;
    int value;
    SideType type;
    std::array<bool, NUM_KEYWORDS> keywords; // Length NUM_KEYWORDS, where bit i indicates presence of Keyword with value i
};

std::size_t hash_value(Side const& s);

class Ent {
public:
    Ent() : hp(0), maxHP(0), shield(0), undead(false), dodged(false), intangibled(false),
        dead(false), usedDie(false), timesUsed(0), timesDamaged(0),
        cleansed(false), poison(0), weaken(0), petrifyAmount(0),
        petrified({false}),
        toexert(false), exerted(false),
        regen(0), smith(0), boost(0), permaboost(0), vulnerable(0),
        armor(0), backrow(false), size(24),
        inflictedPained(false), inflictedExerted(false),
        inflictedDeathed(false), inflictedSingleUse(false),
        redirectTarget(nullptr),
        incomingDamage(0), incomingPoison(0),
        sides({Side(-1, SideType::BLANK, {false}), Side(-1, SideType::BLANK, {false}), Side(-1, SideType::BLANK, {false}), Side(-1, SideType::BLANK, {false}), Side(-1, SideType::BLANK, {false}), Side(-1, SideType::BLANK, {false})}),
        currentSideNum(-1), currentSide(Side(-1, SideType::BLANK, {false})),
        spellID(-1), onHitFunction(-1),
        specialHP({false}), sourceID(-1), positionID(-1) {}
    Ent(int hp, const std::array<Side, 6>& sides, int sourceID, int positionID) : hp(hp), maxHP(hp), shield(0), undead(false), dodged(false), intangibled(false),
        dead(false), usedDie(false), timesUsed(0), timesDamaged(0),
        cleansed(false), poison(0), weaken(0), petrifyAmount(0),
        petrified({false}),
        toexert(false), exerted(false),
        regen(0), smith(0), boost(0), permaboost(0), vulnerable(0),
        armor(0), backrow(false), size(24),
        inflictedPained(false), inflictedExerted(false),
        inflictedDeathed(false), inflictedSingleUse(false),
        redirectTarget(nullptr),
        incomingDamage(0), incomingPoison(0),
        sides(sides), currentSideNum(-1), currentSide(Side(-1, SideType::BLANK, {false})),
        spellID(-1), onHitFunction(-1),
        specialHP({false}), sourceID(sourceID), positionID(positionID) {}
    Ent(int hp, const std::array<Side, 6>& sides, int sourceID, int positionID, int spellID) : hp(hp), maxHP(hp), shield(0), undead(false), dodged(false), intangibled(false),
        dead(false), usedDie(false), timesUsed(0), timesDamaged(0),
        cleansed(false), poison(0), weaken(0), petrifyAmount(0),
        petrified({false}),
        toexert(false), exerted(false),
        regen(0), smith(0), boost(0), permaboost(0), vulnerable(0),
        armor(0), backrow(false), size(24),
        inflictedPained(false), inflictedExerted(false),
        inflictedDeathed(false), inflictedSingleUse(false),
        redirectTarget(nullptr),
        incomingDamage(0), incomingPoison(0),
        sides(sides), currentSideNum(-1), currentSide(Side(-1, SideType::BLANK, {false})),
        spellID(spellID), onHitFunction(-1),
        specialHP({false}), sourceID(sourceID), positionID(positionID) {}
    Ent(int hp, const std::array<Side, 6>& sides, int sourceID, int onHitFunc, int size, const std::array<bool, MAX_NUM_SPECIAL_HP>& specialHP) : hp(hp), maxHP(hp), 
        shield(0), undead(false), dodged(false), intangibled(false),
        dead(false), usedDie(false), timesUsed(0), timesDamaged(0),
        cleansed(false), poison(0), weaken(0), petrifyAmount(0),
        petrified({false}),
        toexert(false), exerted(false),
        regen(0), smith(0), boost(0), permaboost(0), vulnerable(0),
        armor(0), backrow(false), size(size),
        inflictedPained(false), inflictedExerted(false),
        inflictedDeathed(false), inflictedSingleUse(false),
        redirectTarget(nullptr),
        incomingDamage(0), incomingPoison(0),
        sides(sides), currentSideNum(-1), currentSide(Side(-1, SideType::BLANK, {false})),
        spellID(-1), onHitFunction(onHitFunc),
        specialHP(specialHP), sourceID(sourceID), positionID(-1) {}
    Ent(int hp, const std::array<Side, 6>& sides, int sourceID, int autoShield, int autoPoison, int autoRegen, int autoArmor, int onHitFunc, const std::array<bool, MAX_NUM_SPECIAL_HP>& specialHP, bool backrow, int size) : hp(hp),
        maxHP(hp), shield(autoShield), undead(false), dodged(false), intangibled(false),
        dead(false), usedDie(false), timesUsed(0), timesDamaged(0),
        cleansed(false), poison(autoPoison), weaken(0), petrifyAmount(0),
        petrified({false}),
        toexert(false), exerted(false),
        regen(autoRegen), smith(0), boost(0), permaboost(0), vulnerable(0),
        armor(autoArmor), backrow(backrow), size(size),
        inflictedPained(false), inflictedExerted(false),
        inflictedDeathed(false), inflictedSingleUse(false),
        redirectTarget(nullptr),
        incomingDamage(0), incomingPoison(0),
        sides(sides), currentSideNum(-1), currentSide(Side(-1, SideType::BLANK, {false})),
        spellID(-1), onHitFunction(onHitFunc),
        specialHP(specialHP), sourceID(sourceID), positionID(-1) {}
    Ent(int hp, int maxHP, int shield, bool undead, bool dodged, bool intangibled,
        bool dead, bool usedDie, int timesUsed, int timesDamaged,
        bool cleansed, int poison, int weaken, int petrifyAmount,
        const std::array<bool, 6>& petrified,
        bool toexert, bool exerted,
        int regen, int smith, int boost, int permaboost, int vulnerable,
        int armor, bool backrow, int size,
        bool inflictedPained, bool inflictedExerted,
        bool inflictedDeathed, bool inflictedSingleUse,
        Ent* redirectTarget,
        int incomingDamage, int incomingPoison,
        const std::array<Side, 6>& sides,
        int currentSideNum, Side currentSide,
        int spellID, int onHitFunction,
        const std::array<bool, MAX_NUM_SPECIAL_HP>& specialHP,
        int sourceID, int positionID) :
    hp(hp), maxHP(maxHP), shield(shield), undead(undead), dodged(dodged), intangibled(intangibled),
    dead(dead), usedDie(usedDie), timesUsed(timesUsed), timesDamaged(timesDamaged),
    cleansed(cleansed), poison(poison), weaken(weaken), petrifyAmount(petrifyAmount),
    petrified(petrified), toexert(toexert), exerted(exerted),
    regen(regen), smith(smith), boost(boost), permaboost(permaboost), vulnerable(vulnerable),
    armor(armor), backrow(backrow), size(size),
    inflictedPained(inflictedPained), inflictedExerted(inflictedExerted),
    inflictedDeathed(inflictedDeathed), inflictedSingleUse(inflictedSingleUse),
    redirectTarget(redirectTarget),
    incomingDamage(incomingDamage), incomingPoison(incomingPoison),
    sides(sides), currentSideNum(currentSideNum), currentSide(currentSide),
    spellID(spellID), onHitFunction(onHitFunction),
    specialHP(specialHP), sourceID(sourceID), positionID(positionID) {}

    bool operator==(const Ent& other) const;
    bool operator!=(const Ent& other) const;

    int hp;
    int maxHP;
    int shield;
    bool undead;
    bool dodged;
    bool intangibled;

    bool dead;
    bool usedDie;
    int timesUsed;
    int timesDamaged;
    
    bool cleansed; // if cleansed at all, clears all inflicted.
    int poison; // may be negative to represent remaining cleanse for poison
    int weaken;
    int petrifyAmount; // distinct, can have 10 petrify
    std::array<bool, 6> petrified; // length 6, for the sides. Aside from basilisk, always blanked in order
    
    bool toexert;
    bool exerted;
    
    int regen;
    int smith;
    int boost;
    int permaboost;
    int vulnerable;
    int armor;
    bool backrow;
    int size; // 16, 24, 30, and 64 for small, reg, large, and huge monsters. For reinforcements

    // From enemy strikes
    bool inflictedPained;
    bool inflictedExerted;
    bool inflictedDeathed;
    bool inflictedSingleUse;

    Ent* redirectTarget;

    // For heros only, used for rescue, deathwish, pray, etc.
    int incomingDamage;
    int incomingPoison;

    std::array<Side, 6> sides;
    int currentSideNum;
    Side currentSide;
    int spellID; // the ID for the spell
    int onHitFunction; // the ID for the onHitFunction
    std::array<bool, MAX_NUM_SPECIAL_HP> specialHP;

    int sourceID;
    int positionID; // For monsters only, used for summoning
};

std::size_t hash_value(Ent const& p);
        

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, Side & g, const unsigned int version)
{
    ar & g.value;
    ar & g.type;
    ar & g.keywords;
}

template<class Archive>
void serialize(Archive & ar, Ent & g, const unsigned int version)
{
    ar & g.hp;
    ar & g.maxHP;
    ar & g.shield;
    ar & g.undead;
    ar & g.dodged;
    ar & g.intangibled;

    ar & g.dead;
    ar & g.usedDie;
    ar & g.timesUsed;
    ar & g.timesDamaged;
    
    ar & g.cleansed; // if cleansed at all, clears all inflicted.
    ar & g.poison; // may be negative to represent remaining cleanse for poison
    ar & g.weaken;
    ar & g.petrifyAmount; // distinct, can have 10 petrify
    ar & g.petrified; // length 6, for the sides. Aside from basilisk, always blanked in order
    
    ar & g.toexert;
    ar & g.exerted;
    
    ar & g.regen;
    ar & g.smith;
    ar & g.boost;
    ar & g.permaboost;
    ar & g.vulnerable;
    ar & g.armor;
    ar & g.backrow;
    ar & g.size; // 16, 24, 30, and 64 for small, reg, large, and huge monsters. For reinforcements

    // From enemy strikes
    ar & g.inflictedPained;
    ar & g.inflictedExerted;
    ar & g.inflictedDeathed;
    ar & g.inflictedSingleUse;

    ar & g.redirectTarget;
    ar & g.incomingDamage;
    ar & g.incomingPoison;

    ar & g.sides;
    ar & g.currentSideNum;
    ar & g.currentSide;
    ar & g.spellID; // the ID for the spell
    ar & g.onHitFunction; // the ID for the onHitFunction
    ar & g.specialHP;

    ar & g.sourceID;
    ar & g.positionID; // For monsters only, used for summoning
}

} // namespace serialization
} // namespace boost