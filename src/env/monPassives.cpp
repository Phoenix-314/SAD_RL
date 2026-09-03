#include "env/monPassives.h"


#include "env/monPassiveIDs.h"
#include "env/entIDs.h"
#include "env/keywordIDs.h"
#include "env/keywords.h"
#include "env/factions.h"
#include "env/util.h"

bool canRetribute(State& state, Ent* source) {
    if (!source) { // if spell/bones/etc damage, do nothing
        return false;
    }
    bool hasRanged = util::hasCopycattedKeyword(state, source->currentSide, KeywordID::RANGED);
    if (hasRanged) {
        return false;
    }
    return true;
}

void intangibleHP(State& state, Ent* source, Ent* target, int pips) {
    // Intangible checked at the beginning of damage(...), so this current attack (the one triggering intangible) will still do damage
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
        if (target->specialHP[i]) {
            target->intangibled = true;
        }
    }
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
        target->specialHP[i] = false;
    }
}

void manaHP(State& state, Ent* source, Ent* target, int pips) {
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
        if (target->specialHP[i]) {
            state.mana += 1;
        }
    }
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
        target->specialHP[i] = false;
    }
}

void stoneHP(State& state, Ent* source, Ent* target, int pips) {
    if (target->specialHP[target->hp]) { // if breaking stone hp
        target->specialHP[target->hp] = false;
        target->hp += (pips - 1); // can only do one damage, that is, can only break stone hp
        return;
    }

    int v = -1;
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) { // find last special hp
        if (target->specialHP[i]) {
            v = i;
        }
    }
    if (v == -1) {
        return; // no special hp, so do nothing
    }
    target->hp += pips - (target->hp - v); // can only do enough damage to get to stone hp, but not break it
}

void stunHP(State& state, Ent* source, Ent* target, int pips) {
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
        if (target->specialHP[i]) {
            factions::modifyDamage(state, target, -999);
            target->usedDie = true;
        }
    }
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
        target->specialHP[i] = false;
    }
}

template <int spikyAmt>
void spikyHP(State& state, Ent* source, Ent* target, int pips) {
    if (!canRetribute(state, source)) { // if spell/bones/etc damage, do nothing
        return;
    }
    factions::damage(state, nullptr, source, spikyAmt);
}
template <int summonMonID>
void summonHP(State& state, Ent* source, Ent* target, int pips) {
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
        if (target->specialHP[i]) {
            factions::summon(state, target, summonMonID);
        }
    }
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
        target->specialHP[i] = false;
    }
}
template <int amt, int damageTarget>
void damageHP(State& state, Ent* source, Ent* target, int pips){
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
        if (target->specialHP[i]) {
            Ent* srcForDamage = nullptr;
            if (damageTarget == 2) {
                srcForDamage = util::findBottomPlayer(state);
            }
            if (damageTarget == 1) {
                srcForDamage = util::findMiddlePlayer(state);
            }
            if (damageTarget == 0) {
                srcForDamage = util::findTopPlayer(state);
            }

            if (srcForDamage != nullptr) {
                factions::damage(state, nullptr, srcForDamage, amt);
            }
        }
    }
    for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
        target->specialHP[i] = false;
    }
}

std::array<void (*)(State&, Ent*, Ent*, int), NUM_ON_HIT_PASSIVES> PassiveFuncMap = {nullptr};
void initializePassivesMap() {
    PassiveFuncMap[OnHitPassive::ON_HIT_SHADE] = intangibleHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_WISP] = manaHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_IMP] = spikyHP<1>;
    PassiveFuncMap[OnHitPassive::ON_HIT_GRAVE] = stoneHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_CHEST] = stoneHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_ZOMBIE] = [](State& state, Ent* source, Ent* target, int pips) {
        if (pips > 4) {
            target->dead = true;
        }
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_QUARTZ] = [](State& state, Ent* source, Ent* target, int pips) {
        if (target->hp - pips == 2) { // technically, this could break if a wizz heals a quartz back above 3hp.
            target->dead = true;
        }
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_FOUNTAIN] = manaHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_WIZZ] = stunHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_SLIMER] = summonHP<SLIMELET>;
    PassiveFuncMap[OnHitPassive::ON_HIT_OGRE] = [](State& state, Ent* source, Ent* target, int pips) {
        for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
            if (target->specialHP[i]) {
                target->permaboost += 1;
                factions::modifyDamage(state, target, 1);
            }
        }
        for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
            target->specialHP[i] = false;
        }
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_DEMON] = [](State& state, Ent* source, Ent* target, int pips) {
        for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
            if (target->specialHP[i]) {
                target->specialHP[i] = false;
            }
        }
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_CHOMP] = [](State& state, Ent* source, Ent* target, int pips) {
        for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
            if (target->specialHP[i]) {
                Ent* srcForDamage = nullptr;
                if (i <= 5) {
                    srcForDamage = util::findBottomPlayer(state);
                }
                if (i >= 6) {
                    srcForDamage = util::findTopPlayer(state);
                }

                if (srcForDamage != nullptr) {
                    factions::damage(state, nullptr, srcForDamage, 1);
                }
            }
        }
        for (int i = std::max(1, target->hp - pips + 1); i <= target->hp; i++) {
            target->specialHP[i] = false;
        }
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_GHOST] = intangibleHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_BOAR] = stoneHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_SPIKER] = spikyHP<2>;
    PassiveFuncMap[OnHitPassive::ON_HIT_SLATE] = stoneHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_CAW] = [](State& state, Ent* source, Ent* target, int pips) {
        target->backrow = true;
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_MAGRAT] = damageHP<2, 2>; // targets bottom
    PassiveFuncMap[OnHitPassive::ON_HIT_GYTHA] = damageHP<2, 1>; // targets middle
    PassiveFuncMap[OnHitPassive::ON_HIT_AGNES] = damageHP<2, 0>; // targets top
    PassiveFuncMap[OnHitPassive::ON_HIT_BASILISK] = [](State& state, Ent* source, Ent* target, int pips) {
        if (!canRetribute(state, source)) { // cant petrify a spell
            return;
        }
        if (source->petrified[source->currentSideNum]) { // if already petrified, do nothing
            return;
        }
        source->petrifyAmount += 1;
        if (source->petrifyAmount > 0) { // in case of cleanse
            source->petrified[source->currentSideNum] = true;
        }
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_HYDRA] = [](State& state, Ent* source, Ent* target, int pips) {
        if (target->timesDamaged >= 5) {
            target->dead = true;
        }
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_CYCLOPS] = stunHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_SLIME_QUEEN] = summonHP<SLIMER>;
    PassiveFuncMap[OnHitPassive::ON_HIT_LICH] = summonHP<BONES>;
    PassiveFuncMap[OnHitPassive::ON_HIT_BASALT] = [](State& state, Ent* source, Ent* target, int pips) {
        int amt = util::ba2int<MAX_NUM_SPECIAL_HP>(target->specialHP);
        if (pips == amt) {
            target->hp -= pips; // Damage - This works since Basalt's passive doubles damage more or less directly, having vulnerable applied first
            target->specialHP = util::int2ba<MAX_NUM_SPECIAL_HP>(amt + 1);
        }
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_TARANTUS] = [](State& state, Ent* source, Ent* target, int pips) {
        if (target->specialHP[10] && target->hp - pips < 10) {
            target->specialHP[10] = false;
            Ent* targ = util::findTopPlayer(state);
            if (targ == nullptr) { // ie, all players are dead and poison triggered this hp
                return;
            }
            if (targ->dodged) {
                return;
            }

            if (targ->redirectTarget) {
                targ = targ->redirectTarget;
            }
            if (targ->undead) {
                return;
            }

            targ->dead = true;
        }
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_HEXIA] = [](State& state, Ent* source, Ent* target, int pips) {
        if (!canRetribute(state, source)) { // if spell/bones/etc damage, do nothing
            return;
        }
        factions::damage(state, nullptr, source, pips);
    };
    PassiveFuncMap[OnHitPassive::ON_HIT_BARON] = manaHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_SARCOPHAGUS] = stoneHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_INEVITABLE] = intangibleHP;
    PassiveFuncMap[OnHitPassive::ON_HIT_THORN] = spikyHP<5>;
}