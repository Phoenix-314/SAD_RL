#include "keywords.h"



#include "constants.h"
#include "keywordIDs.h"
#include "entIDs.h"
#include "util.h"
#include "factions.h"


std::array<int (*)(State&, Ent*, Ent*, int), NUM_KEYWORDS> KeywordFuncMap = {nullptr};

void initializeKeywordMap() {
    
    KeywordFuncMap[KeywordID::VULNERABLE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        target->vulnerable += pips;
        return 0;
    };
    KeywordFuncMap[KeywordID::CRUEL] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return (target->hp * 2 > target->maxHP) ? 0 : pips;
    };
    KeywordFuncMap[KeywordID::POISON] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        target->poison += pips;
        return 0;
    };
    KeywordFuncMap[KeywordID::CANTRIP] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::MANAGAIN] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        state.mana += pips;
        return 0;
    };
    KeywordFuncMap[KeywordID::ENGAGE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return (target->hp == target->maxHP) ? pips : 0;
    };
    KeywordFuncMap[KeywordID::RANGED] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::STICKY] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::MANDATORY] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::DEATH] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        source->dead = true;
        return 0;
    };
    KeywordFuncMap[KeywordID::RAMPAGE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::PAIN] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        factions::damage(state, nullptr, source, pips);
        return 0;
    };
    KeywordFuncMap[KeywordID::PLAGUE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        int count = 0;
        for (const Ent* ent : state.enemies) {
            count += std::max(0, ent->poison);
        }
        for (const Ent* ent : state.players) {
            count += std::max(0, ent->poison);
        }
        return count;
    };
    KeywordFuncMap[KeywordID::CLEANSE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        target->cleansed = true;
        target->poison -= pips; // may be negative, representing remaining cleanse for poison

        target->weaken -= pips;
        
        target->petrifyAmount -= pips;
        if (target->petrifyAmount <= 0) {
            target->petrified = {false}; // clear
        } else if (target->petrifyAmount > 0 && target->petrifyAmount < 6) {
            int sum = 0;
            for (int i=0;i<target->petrified.size();i++) { if (target->petrified[i]) { sum += 1; } }
            int petrifyClearPips = sum - target->petrifyAmount; // number of sides to clear
            int i = 5;
            while (petrifyClearPips > 0 && i >= 0) {
                if (target->petrified[i]) {
                    petrifyClearPips -= 1; // if petrified (==1), decrease the number of sides we can clear by 1. Else, do nothing (-= 0)
                    target->petrified[i] = false;
                }
                i -= 1;
            }
        } // no else since going from >6 petrify to >6 petrify doesn't change which sides are petrified
        
        target->inflictedPained = false;
        target->inflictedExerted = false;
        target->inflictedDeathed = false;
        target->inflictedSingleUse = false;
        return 0;
    };
    KeywordFuncMap[KeywordID::PRISTINE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return (source->hp == source->maxHP) ? pips : 0;
    };
    KeywordFuncMap[KeywordID::EXERT] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        source->exerted = true;
        source->toexert = true;
        return 0;
    };
    KeywordFuncMap[KeywordID::SELFSHIELD] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        source->shield += pips;
        return 0;
    };
    KeywordFuncMap[KeywordID::CLEAVE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::GUILT] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::HEAVY] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::SINGLEUSE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        source->sides[source->currentSideNum].type = SideType::BLANK;
        return 0;
    };
    KeywordFuncMap[KeywordID::DEATHWISH] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return (util::targetIsDying(state, *source)) ? pips : 0;
    };
    KeywordFuncMap[KeywordID::CHAIN] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        if (!state.lastDie.has_value()) {
            return 0;
        }
        for (int i=0;i<NUM_KEYWORDS;i++) {
            if (state.lastDie->keywords[i] && source->currentSide.keywords[i]) {
                return pips;
            }
        }
        return 0;
    };
    KeywordFuncMap[KeywordID::BLOODLUST] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        int count = 0;
        
        if (source->sourceID >= 0) {
            for (const Ent* ent : state.enemies) {
                if (ent->hp < ent->maxHP) {
                    count += 1;
                }
            }
        } else {
            for (const Ent* ent : state.players) {
                if (ent->hp < ent->maxHP) {
                    count += 1;
                }
            }
        }
        return count;
    };
    KeywordFuncMap[KeywordID::STEEL] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return source->shield;
    };
    KeywordFuncMap[KeywordID::DUPLICATE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        for (Ent* ent : state.players) {
            ent->currentSide = source->currentSide; // will make a deepcopy
            ent->currentSide.value = findStaticPips(state, source);
            ent->currentSide.keywords[KeywordID::STASIS] = true;
            ent->currentSide.keywords[KeywordID::DUPLICATE] = false;
    
            ent->exerted = false;
            ent->inflictedPained =  source->inflictedPained;
            ent->inflictedExerted = source->inflictedExerted;
            ent->inflictedDeathed = source->inflictedDeathed;
            ent->inflictedSingleUse = source->inflictedSingleUse;
        }
        return 0;
    };
    KeywordFuncMap[KeywordID::ELIMINATE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::FOCUS] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return (state.lastTarget == target) ? pips : 0;
    };
    KeywordFuncMap[KeywordID::DESCEND] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::CHARGED] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return state.mana;
    };
    KeywordFuncMap[KeywordID::ERA] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return state.turn - 1;
    };
    KeywordFuncMap[KeywordID::SMITH] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        target->smith += pips;
        return 0;
    };
    KeywordFuncMap[KeywordID::DEFY] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return std::max(source->incomingDamage - source->shield, 0);
    };
    KeywordFuncMap[KeywordID::REPEL] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::RESCUE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::INSPIRED] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return (state.lastStrikeVal > pips) ? pips : 0;
    };
    KeywordFuncMap[KeywordID::GROOOOWTH] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        source->permaboost += 1;
        return 0;
    };
    KeywordFuncMap[KeywordID::DECAY] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        source->sides[source->currentSideNum].value -= 1;
        source->currentSide.value -= 1;
        return 0;
    };
    KeywordFuncMap[KeywordID::VITALITY] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        target->maxHP += pips;
        return 0;
    };
    KeywordFuncMap[KeywordID::SELFHEAL] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        factions::heal(state, source, pips);
        return 0;
    };
    KeywordFuncMap[KeywordID::REGEN] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        target->regen += pips;
        return 0;
    };
    KeywordFuncMap[KeywordID::GROWTH] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        source->sides[source->currentSideNum].value += 1;
        source->currentSide.value += 1;
        return 0;
    };
    KeywordFuncMap[KeywordID::WEAKEN] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        factions::modifyDamage(state, target, -pips);
        target->weaken += pips;
        return 0;
    };
    KeywordFuncMap[KeywordID::BOOST] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        target->boost += pips;
        return 0;
    };
    KeywordFuncMap[KeywordID::PAIR] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return (state.lastStrikeVal == pips) ? pips : 0;
    };
    KeywordFuncMap[KeywordID::TRIO] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return (state.prevLastStrikeVal == state.lastStrikeVal && state.lastStrikeVal == pips) ? 2 * pips : 0;
    };
    KeywordFuncMap[KeywordID::BONED] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::PERMABOOST] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        target->permaboost += pips;
        return 0;
    };
    KeywordFuncMap[KeywordID::COPYCAT] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        if (state.lastDie.has_value()) {
            for (int i=0;i<NUM_KEYWORDS;i++) {
                if (state.lastDie->keywords[i]) {
                    source->currentSide.keywords[i] = true;
                }
            }
        }
        return 0;
    };
    KeywordFuncMap[KeywordID::DOUBLEUSE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::QUADUSE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::INFLICT_PAIN] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        if (!target->cleansed) {
            target->inflictedPained = true;
        }
        return 0;
    };
    KeywordFuncMap[KeywordID::INFLICT_EXERT] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        if (!target->cleansed) {
            target->inflictedExerted = true;
        }
        return 0;
    };
    KeywordFuncMap[KeywordID::INFLICT_DEATH] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        if (!target->cleansed) {
            target->inflictedDeathed = true;
        }
        return 0;
    };
    KeywordFuncMap[KeywordID::INFLICT_SINGLEUSE] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        if (!target->cleansed) {
            target->inflictedSingleUse = true;
        }
        return 0;
    };
    KeywordFuncMap[KeywordID::STASIS] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        return 0;
    };
    KeywordFuncMap[KeywordID::PETRIFY] = [](State& state, Ent* source, Ent* target, int pips) -> int {
        int newPetrifyAmount = (target->petrifyAmount >= 0) ? pips : target->petrifyAmount + pips;
        target->petrifyAmount += pips;
    
        if (target->petrifyAmount >= 6) {
            target->petrified = {true, true, true, true, true, true};
            return 0;
        }
        
        int i = 0;
        while (newPetrifyAmount > 0 && i < 6) {
            if (!target->petrified[i]) {
                newPetrifyAmount -= 1;
                target->petrified[i] = true;
            }
            i += 1;
        }
        return 0;
    };
};
    
    
int findStaticPips(State& state, Ent* source) {
    Side& side = source->currentSide;
    int pips = side.value;
    if (side.type != SideType::SUMMON && side.keywords[KeywordID::STASIS]) {
        return pips;
    }

    pips += source->boost + source->permaboost - std::max(source->weaken, 0);
    if (source->sourceID < 0) { // if enemy
        pips += state.warchiefD; // Mon Passive. Warchief increases enemy strength by 1 per warchief present
    } else { // if hero
        pips += state.handD; // Mon Passive. The Hand increases hero strength by 1 per hand present
    }

    if (side.type == SideType::SUMMON) {
        return pips; // No summon side has keywords, so that space is used to encode the summonID
    }

    if (side.type == SideType::DAMAGE || side.type == SideType::SHIELD || side.type == SideType::HEALSHIELD || side.keywords[KeywordID::SELFSHIELD]) {
        pips += source->smith;
    }

    for (int i = START_STATICS; i < START_PRE_MON_EFFECTS; i++) {
        if (side.keywords[i]) {
            pips += KeywordFuncMap[i](state, source, nullptr, pips); // Target is irrelevant to static keywords
        }
    }
    return pips;
}

int findDynamicPips(State& state, Ent* source, Ent* target, int pips) {
    Side& side = source->currentSide;
    if (side.keywords[KeywordID::STASIS]) {
        return pips;
    }
    
    for (int i = START_DYNAMICS; i < START_APPLIED_EFFECTS; i++) {
        if (side.keywords[i]) {
            pips += KeywordFuncMap[i](state, source, target, pips);
        }
    }
    return pips;
}
    
void applyAffectKeywords(State& state, Ent* source, Ent* target, int pips) {
    Side& side = source->currentSide;
    for (int i = START_APPLIED_EFFECTS; i < ACT_OCCUR; i++) {
        if (side.keywords[i]) {
            pips += KeywordFuncMap[i](state, source, target, pips);
        }
    }
}

void completeActionKeywords(State& state, Ent* source, int pips) {
    Side& side = source->currentSide;
    if (side.type == SideType::SUMMON) {
        return;
    }

    for (int i = ACT_POST_OCCUR; i < NUM_KEYWORDS; i++) {
        if (side.keywords[i]) {
            pips += KeywordFuncMap[i](state, source, nullptr, pips); // These also do not affect the target
        }
    }
}
