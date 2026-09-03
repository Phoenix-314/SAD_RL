#include "env/ents.h"

#include <boost/functional/hash.hpp>

bool Side::operator==(const Side& other) const {
    return value == other.value && type == other.type && keywords == other.keywords;
}

std::size_t hash_value(Side const& s) {
    std::size_t seed = 0;
    boost::hash_combine(seed, s.value);
    boost::hash_combine(seed, s.type);
    for (const auto& keyword : s.keywords) {
        boost::hash_combine(seed, keyword);
    }
    return seed;
}

bool Ent::operator==(const Ent& other) const {
    return hp == other.hp &&
           maxHP == other.maxHP &&
           shield == other.shield &&
           undead == other.undead &&
           dodged == other.dodged &&
           intangibled == other.intangibled &&
           dead == other.dead &&
           usedDie == other.usedDie &&
           timesUsed == other.timesUsed &&
           timesDamaged == other.timesDamaged &&
           cleansed == other.cleansed &&
           poison == other.poison &&
           weaken == other.weaken &&
           petrifyAmount == other.petrifyAmount &&
           petrified == other.petrified &&
           toexert == other.toexert &&
           exerted == other.exerted &&
           regen == other.regen &&
           smith == other.smith &&
           boost == other.boost &&
           permaboost == other.permaboost &&
           vulnerable == other.vulnerable &&
           armor == other.armor &&
           backrow == other.backrow &&
           size == other.size &&
           inflictedPained == other.inflictedPained &&
           inflictedExerted == other.inflictedExerted &&
           inflictedDeathed == other.inflictedDeathed &&
           inflictedSingleUse == other.inflictedSingleUse &&
              ((redirectTarget == nullptr && other.redirectTarget == nullptr) || 
                (redirectTarget != nullptr && other.redirectTarget != nullptr && redirectTarget->positionID == other.redirectTarget->positionID)) &&
           incomingDamage == other.incomingDamage &&
           incomingPoison == other.incomingPoison &&
           sides == other.sides && 
           currentSideNum == other.currentSideNum && 
           currentSide.value == other.currentSide.value && 
           currentSide.type == other.currentSide.type && 
           currentSide.keywords == other.currentSide.keywords && 
           spellID == other.spellID && 
           onHitFunction == other.onHitFunction && 
           specialHP == other.specialHP && 
           sourceID == other.sourceID && 
           positionID == other.positionID;
}
bool Ent::operator!=(const Ent& other) const {
    return !(*this == other);
}

std::size_t hash_value(Ent const& p) {
    std::size_t seed = 0;
    boost::hash_combine(seed, p.hp);
    boost::hash_combine(seed, p.maxHP);
    boost::hash_combine(seed, p.shield);
    boost::hash_combine(seed, p.undead);
    boost::hash_combine(seed, p.dodged);
    boost::hash_combine(seed, p.intangibled);
    
    boost::hash_combine(seed, p.dead);
    boost::hash_combine(seed, p.usedDie);
    boost::hash_combine(seed, p.timesUsed);
    boost::hash_combine(seed, p.timesDamaged);

    boost::hash_combine(seed, p.cleansed);
    boost::hash_combine(seed, p.poison);
    boost::hash_combine(seed, p.weaken);
    boost::hash_combine(seed, p.petrifyAmount);
    for (const auto& val : p.petrified) {
        boost::hash_combine(seed, val);
    }
    
    boost::hash_combine(seed, p.toexert);
    boost::hash_combine(seed, p.exerted);

    boost::hash_combine(seed, p.regen);
    boost::hash_combine(seed, p.smith);
    boost::hash_combine(seed, p.boost);
    boost::hash_combine(seed, p.permaboost);
    boost::hash_combine(seed, p.vulnerable);
    boost::hash_combine(seed, p.armor);
    boost::hash_combine(seed, p.backrow);
    boost::hash_combine(seed, p.size);
    
    boost::hash_combine(seed, p.inflictedPained);
    boost::hash_combine(seed, p.inflictedExerted);
    boost::hash_combine(seed, p.inflictedDeathed);
    boost::hash_combine(seed, p.inflictedSingleUse);

    if (p.redirectTarget != nullptr) {
        boost::hash_combine(seed, p.redirectTarget->positionID); // Use positionID as a unique identifier for the Ent
    } else {
        boost::hash_combine(seed, 0); // Use a default value for null pointers
    }

    boost::hash_combine(seed, p.incomingDamage);
    boost::hash_combine(seed, p.incomingPoison);

    boost::hash_combine(seed, p.sides);

    boost::hash_combine(seed, p.currentSideNum);
    boost::hash_combine(seed, p.currentSide);
    boost::hash_combine(seed, p.spellID);
    boost::hash_combine(seed, p.onHitFunction);
    for (const auto& val : p.specialHP) {
        boost::hash_combine(seed, val);
    }
    boost::hash_combine(seed, p.sourceID);
    boost::hash_combine(seed, p.positionID);
    return seed;
}