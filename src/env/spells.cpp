#include "env/spells.h"

#include "env/keywordIDs.h"
#include "env/keywords.h"
#include "env/entIDs.h"
#include "env/util.h"
#include "env/factions.h"
#include "env/validActions.h"


std::vector<Ent*> findTacticCosts(State& state, SpellTargetReq spellTargetReq) {
    int xs = 0;
    int damages = 0;
    int shields = 0;
    int heals = 0;
    if (spellTargetReq == SpellTargetReq::DAMAGE_N_SHIELD_3) {
        damages = 3;
        shields = 3;
    } else if (spellTargetReq == SpellTargetReq::DAMAGE_2) {
        damages = 2;
    } else if (spellTargetReq == SpellTargetReq::ONE_OF_ALL) {
        damages = 1;
        shields = 1;
        heals = 1;
        xs = 1;
    } else if (spellTargetReq == SpellTargetReq::AN_X) {
        xs = 1;
    } else {
        throw std::runtime_error("Unhandled spellTargetReq in findTacticCosts");
    }

    std::vector<Ent*> neededHeros;
    for (int i = 0; i < state.players.size(); i++) {
        Ent* hero = state.players[i];
        if (!validActions::diceUsable(state, i)) {
            continue;
        }

        bool heroNeeded = false;
        int pips = findStaticPips(state, hero);
        if (xs > 0 && (hero->currentSide.type == SideType::BLANK || hero->petrified[hero->currentSideNum])) {
            xs -= 1;
            heroNeeded = true;
        } else { // x is incompatible with the rest. Damage, heal, and shields can be together via keywords or HEALSHIELD
            if (pips <= 0 || hero->petrified[hero->currentSideNum]) { // dont use a hero with no pips (or negative pips)
                continue;
            }

            if (damages > 0 && (hero->currentSide.type == SideType::DAMAGE || hero->currentSide.type == SideType::DAMAGE_ALL || hero->currentSide.type == SideType::DAMAGE_EVERYONE || hero->currentSide.type == SideType::SELF_DAMAGE)) {
                damages -= findStaticPips(state, hero);
                heroNeeded = true;
            }
            if (shields > 0 && (hero->currentSide.type == SideType::SHIELD || hero->currentSide.type == SideType::HEALSHIELD || hero->currentSide.type == SideType::SHIELD_ALL || hero->currentSide.keywords[KeywordID::SELFSHIELD])) {
                shields -= findStaticPips(state, hero);
                heroNeeded = true;
            }
            if (heals > 0 && (hero->currentSide.type == SideType::HEAL || hero->currentSide.type == SideType::HEALSHIELD || hero->currentSide.type == SideType::HEAL_ALL || hero->currentSide.keywords[KeywordID::SELFHEAL])) {
                heals -= findStaticPips(state, hero);
                heroNeeded = true;
            }
        }
        
        if (heroNeeded) {
            neededHeros.push_back(hero);
        }

        if (damages <= 0 && shields <= 0 && heals <= 0 && xs <= 0) {
            return neededHeros;
        }
    }
    return std::vector<Ent*>(); // Cannot use tactic
}

std::array<void (*)(State&, Ent*), NUM_SPELLS> SpellFuncMap = {nullptr};
std::array<int, NUM_SPELLS> SpellCostMap = {0};
std::array<SpellTargetReq, NUM_SPELLS> SpellReqsMap = {SpellTargetReq::UNTARGETED};
std::array<SpellCastAffect, NUM_SPELLS> SpellCastAffectMap = {SpellCastAffect::STANDARD};

void initializeSpellMaps() {
    
    SpellFuncMap[SpellID::MEND] = [](State& state, Ent* target) -> void {
        target->hp = 5;
        if (target->maxHP < 5) {
            target->maxHP = 5;
        }
    };
    SpellCostMap[SpellID::MEND] = 2;
    SpellReqsMap[SpellID::MEND] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::MEND] = SpellCastAffect::STANDARD;

    SpellFuncMap[SpellID::HEMLOCK] = [](State& state, Ent* target) -> void {
        factions::damage(state, nullptr, target, 1);
        KeywordFuncMap[KeywordID::POISON](state, nullptr, target, 1);
    };
    SpellCostMap[SpellID::HEMLOCK] = 2;
    SpellReqsMap[SpellID::HEMLOCK] = SpellTargetReq::ENEMY;
    SpellCastAffectMap[SpellID::HEMLOCK] = SpellCastAffect::SINGLE_CAST;
    
    
    SpellFuncMap[SpellID::RESTORE] = [](State& state, Ent* target) -> void {
        for (Ent* hero : state.players) {
            factions::heal(state, hero, 1);
        }
    };
    SpellCostMap[SpellID::RESTORE] = 2;
    SpellReqsMap[SpellID::RESTORE] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::RESTORE] = SpellCastAffect::STANDARD;

    SpellFuncMap[SpellID::GAZE] = [](State& state, Ent* target) -> void {
        state.rerolls -= 1; // must be at 0 rerolls in order to use spells
    };
    SpellCostMap[SpellID::GAZE] = 1;
    SpellReqsMap[SpellID::GAZE] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::GAZE] = SpellCastAffect::STANDARD;

    SpellFuncMap[SpellID::BANDAGE] = [](State& state, Ent* target) -> void {
        for (Ent* hero : util::findHeroCleaveTargets(state, *target)) {
            hero->shield += 1;
            factions::heal(state, hero, 1);
        }
    };
    SpellCostMap[SpellID::BANDAGE] = 2;
    SpellReqsMap[SpellID::BANDAGE] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::BANDAGE] = SpellCastAffect::SINGLE_CAST;

    SpellFuncMap[SpellID::BALANCE] = [](State& state, Ent* target) -> void {
        for (Ent* mon : state.enemies) {
            if (mon->sourceID == THORN) {
                continue; // Cannot damage thorns
            }
            factions::damage(state, nullptr, mon, 1);
        }
        for (Ent* hero : state.players) {
            factions::heal(state, hero, 1);
        }
    };
    SpellCostMap[SpellID::BALANCE] = 3;
    SpellReqsMap[SpellID::BALANCE] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::BALANCE] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::VINE] = [](State& state, Ent* target) -> void {
        if (target->sourceID >= 0) { // if target is hero
            factions::heal(state, target, 1);
        } else {
            factions::damage(state, nullptr, target, 1);
        }
    };
    SpellCostMap[SpellID::VINE] = 1;
    SpellReqsMap[SpellID::VINE] = SpellTargetReq::EITHER;
    SpellCastAffectMap[SpellID::VINE] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::RENEW] = [](State& state, Ent* target) -> void {
        target->hp = 4;
        if (target->maxHP < 4) {
            target->maxHP = 4;
        }
    };
    SpellCostMap[SpellID::RENEW] = 1;
    SpellReqsMap[SpellID::RENEW] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::RENEW] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::PRAY] = [](State& state, Ent* target) -> void {
        for (Ent* hero : state.players) {
            if (util::targetIsDying(state, *hero)) {
                factions::heal(state, hero, 1);
                hero->shield += 1;
            }
        }
    };
    SpellCostMap[SpellID::PRAY] = 1;
    SpellReqsMap[SpellID::PRAY] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::PRAY] = SpellCastAffect::DEPLETE;
    
    SpellFuncMap[SpellID::INFUSE] = [](State& state, Ent* target) -> void {
        for (Ent* hero : state.players) {
            int delta = (hero->hp == 1) ? 2 : 1;
            factions::heal(state, hero, delta);
        }
    };
    SpellCostMap[SpellID::INFUSE] = 2;
    SpellReqsMap[SpellID::INFUSE] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::INFUSE] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::FLIP] = [](State& state, Ent* target) -> void {
        std::array<int, 6> mp = {5, 3, 4, 1, 2, 0}; // top maps to bottom, left to right, middle to rightmost
        for (Ent* hero : state.players) {
            hero->currentSide = hero->sides[mp[hero->currentSideNum]];
        }
    };
    SpellCostMap[SpellID::FLIP] = 2;
    SpellReqsMap[SpellID::FLIP] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::FLIP] = SpellCastAffect::STANDARD;

    SpellFuncMap[SpellID::GLOW] = [](State& state, Ent* target) -> void {
        for (Ent* hero : state.players) {
            if (hero->currentSide.type == SideType::BLANK || hero->petrified[hero->currentSideNum] || hero->exerted) {
                hero->currentSide = Side(2, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STASIS}));
                hero->currentSide.keywords[KeywordID::STASIS] = true;
                hero->exerted = false;
                hero->inflictedPained = false;
                hero->inflictedExerted = false;
                hero->inflictedDeathed = false;
                hero->inflictedSingleUse = false;
            }
        }
    };
    SpellCostMap[SpellID::GLOW] = 1;
    SpellReqsMap[SpellID::GLOW] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::GLOW] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::CIRCLE] = [](State& state, Ent* target) -> void {
        for (Ent* hero : state.players) {
            hero->currentSide.keywords[KeywordID::SELFHEAL] = true;
        }
    };
    SpellCostMap[SpellID::CIRCLE] = 2;
    SpellReqsMap[SpellID::CIRCLE] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::CIRCLE] = SpellCastAffect::SINGLE_CAST;
    
    SpellFuncMap[SpellID::LIQUOR] = [](State& state, Ent* target) -> void {
        factions::heal(state, target, 10);
        KeywordFuncMap[KeywordID::CLEANSE](state, nullptr, target, 10);
    };
    SpellCostMap[SpellID::LIQUOR] = 3;
    SpellReqsMap[SpellID::LIQUOR] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::LIQUOR] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::BIND] = [](State& state, Ent* target) -> void {
        target->intangibled = true;
    };
    SpellCostMap[SpellID::BIND] = 3;
    SpellReqsMap[SpellID::BIND] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::BIND] = SpellCastAffect::DEPLETE;

    SpellFuncMap[SpellID::SOOTHE] = [](State& state, Ent* target) -> void {
        for (Ent* hero : state.players) {
            factions::heal(state, hero, 1);
            hero->regen += 1;
        }
    };
    SpellCostMap[SpellID::SOOTHE] = 4;
    SpellReqsMap[SpellID::SOOTHE] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::SOOTHE] = SpellCastAffect::STANDARD;

    SpellFuncMap[SpellID::RITUAL] = [](State& state, Ent* target) -> void {
        for (Ent* hero : util::findHeroCleaveTargets(state, *target)) {
            factions::heal(state, hero, 2);
            KeywordFuncMap[KeywordID::CLEANSE](state, nullptr, hero, 2);
        }
    };
    SpellCostMap[SpellID::RITUAL] = 3;
    SpellReqsMap[SpellID::RITUAL] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::RITUAL] = SpellCastAffect::STANDARD;

    
    SpellFuncMap[SpellID::SALVE] = [](State& state, Ent* target) -> void {
        factions::heal(state, target, 2);
    };
    SpellCostMap[SpellID::SALVE] = 1;
    SpellReqsMap[SpellID::SALVE] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::SALVE] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::LEECH] = [](State& state, Ent* target) -> void {
        for (Ent* hero : state.players) {
            factions::heal(state, hero, 5);
        }
        target->dead = true;
    };
    SpellCostMap[SpellID::LEECH] = 1;
    SpellReqsMap[SpellID::LEECH] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::LEECH] = SpellCastAffect::COOLDOWN;
    
    SpellFuncMap[SpellID::OPERATE] = [](State& state, Ent* target) -> void {
        factions::resurrect(state, 1);
    };
    SpellCostMap[SpellID::OPERATE] = 3;
    SpellReqsMap[SpellID::OPERATE] = SpellTargetReq::RESURRECT;
    SpellCastAffectMap[SpellID::OPERATE] = SpellCastAffect::DEPLETE;
    
    SpellFuncMap[SpellID::STRAND] = [](State& state, Ent* target) -> void {
        bool dying = util::targetIsDying(state, *target);
        factions::heal(state, target, 2);
        if (dying && !util::targetIsDying(state, *target)) {
            state.mana += 4;
        }
    };
    SpellCostMap[SpellID::STRAND] = 4;
    SpellReqsMap[SpellID::STRAND] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::STRAND] = SpellCastAffect::STANDARD;
    
    // START OF BLUE SPELLS
    SpellFuncMap[SpellID::POKE] = [](State& state, Ent* target) -> void {
        factions::damage(state, nullptr, target, 1);
    };
    SpellCostMap[SpellID::POKE] = 1;
    SpellReqsMap[SpellID::POKE] = SpellTargetReq::ENEMY;
    SpellCastAffectMap[SpellID::POKE] = SpellCastAffect::COOLDOWN;
    
    SpellFuncMap[SpellID::SCORCH] = [](State& state, Ent* target) -> void {
        for (Ent* mon : util::findEnemyCleaveTargets(state, *target)) {
            if (mon->sourceID == THORN) {
                continue; // Cannot damage thorns
            }
            factions::damage(state, nullptr, mon, 1);
        }
    };
    SpellCostMap[SpellID::SCORCH] = 2;
    SpellReqsMap[SpellID::SCORCH] = SpellTargetReq::ENEMY;
    SpellCastAffectMap[SpellID::SCORCH] = SpellCastAffect::SINGLE_CAST;
    
    SpellFuncMap[SpellID::SLAY] = [](State& state, Ent* target) -> void {
        target->dead = true; // checking for valid targets already done
    };
    SpellCostMap[SpellID::SLAY] = 3;
    SpellReqsMap[SpellID::SLAY] = SpellTargetReq::HP3;
    SpellCastAffectMap[SpellID::SLAY] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::SLICE] = [](State& state, Ent* target) -> void {
        for (Ent* mon : state.enemies) {
            if (mon->sourceID == THORN) {
                continue; // Cannot damage thorns
            }
            factions::damage(state, nullptr, mon, 1);
        }
    };
    SpellCostMap[SpellID::SLICE] = 3;
    SpellReqsMap[SpellID::SLICE] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::SLICE] = SpellCastAffect::STANDARD;


    SpellFuncMap[SpellID::GATHER] = [](State& state, Ent* target) -> void {
        for (Ent* hero : state.players) {
            if (hero->currentSide.type == SideType::BLANK || hero->petrified[hero->currentSideNum] || hero->exerted) {
                hero->currentSide = Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STASIS}));
                hero->currentSide.keywords[KeywordID::STASIS] = true;
                hero->exerted = false;
                hero->inflictedPained = false;
                hero->inflictedExerted = false;
                hero->inflictedDeathed = false;
                hero->inflictedSingleUse = false;
            }
        }
    };
    SpellCostMap[SpellID::GATHER] = 2;
    SpellReqsMap[SpellID::GATHER] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::GATHER] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::CUT] = [](State& state, Ent* target) -> void {
        factions::damage(state, nullptr, target, 3);
        if (state.enemies[0]->sourceID == THORN) {
            return; // Cannot damage thorns
        }
        factions::damage(state, nullptr, state.enemies[0], 2);
    };
    SpellCostMap[SpellID::CUT] = 1;
    SpellReqsMap[SpellID::CUT] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::CUT] = SpellCastAffect::COOLDOWN;
    
    SpellFuncMap[SpellID::SCALD] = [](State& state, Ent* target) -> void {
        for (Ent* mon : state.enemies) {
            if (mon->hp < mon->maxHP && mon->sourceID != THORN) {
                factions::damage(state, nullptr, mon, 2);
            }
        }
    };
    SpellCostMap[SpellID::SCALD] = 3;
    SpellReqsMap[SpellID::SCALD] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::SCALD] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::DROP] = [](State& state, Ent* target) -> void {
        if (state.enemies[0]->sourceID == THORN) {
            return; // Cannot damage thorns
        }
        factions::damage(state, nullptr, state.enemies[0], 4);
    };
    SpellCostMap[SpellID::DROP] = 3;
    SpellReqsMap[SpellID::DROP] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::DROP] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::CHILL] = [](State& state, Ent* target) -> void {
        factions::damage(state, nullptr, target, 2);
        KeywordFuncMap[KeywordID::WEAKEN](state, nullptr, target, 2);
    };
    SpellCostMap[SpellID::CHILL] = 2;
    SpellReqsMap[SpellID::CHILL] = SpellTargetReq::ENEMY;
    SpellCastAffectMap[SpellID::CHILL] = SpellCastAffect::SINGLE_CAST;
    
    SpellFuncMap[SpellID::FLICK] = [](State& state, Ent* target) -> void {
        int damage = (target->hp == target->maxHP) ? 2 : 1;
        factions::damage(state, nullptr, target, damage);
    };
    SpellCostMap[SpellID::FLICK] = 1;
    SpellReqsMap[SpellID::FLICK] = SpellTargetReq::ENEMY;
    SpellCastAffectMap[SpellID::FLICK] = SpellCastAffect::COOLDOWN;
    
    SpellFuncMap[SpellID::ZAP] = [](State& state, Ent* target) -> void {
        target->dead = true; // zap op.
    };
    SpellCostMap[SpellID::ZAP] = 1;
    SpellReqsMap[SpellID::ZAP] = SpellTargetReq::HP2;
    SpellCastAffectMap[SpellID::ZAP] = SpellCastAffect::COOLDOWN;
    
    SpellFuncMap[SpellID::SPORE] = [](State& state, Ent* target) -> void {
        target->currentSide.keywords[KeywordID::DECAY] = true;
        target->currentSide.value += 1; // Current side is copy, so these dont change underlying side. However, DECAY keyword does affect underlying side
    };
    SpellCostMap[SpellID::SPORE] = 1;
    SpellReqsMap[SpellID::SPORE] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::SPORE] = SpellCastAffect::COOLDOWN;
    
    SpellFuncMap[SpellID::FORETELL] = [](State& state, Ent* target) -> void {
        state.fortoldMana += 4;
    };
    SpellCostMap[SpellID::FORETELL] = 3;
    SpellReqsMap[SpellID::FORETELL] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::FORETELL] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::BURN] = [](State& state, Ent* target) -> void {
        for (Ent* hero : state.players) {
            factions::damage(state, nullptr, hero, 1);
        }
        for (Ent* mon : state.enemies) {
            if (mon->sourceID == THORN) {
                continue; // Cannot damage thorns
            }
            factions::damage(state, nullptr, mon, 1);
        }
    };
    SpellCostMap[SpellID::BURN] = 1;
    SpellReqsMap[SpellID::BURN] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::BURN] = SpellCastAffect::COOLDOWN;
    
    SpellFuncMap[SpellID::BLADES] = [](State& state, Ent* target) -> void {
        for (Ent* mon : state.enemies) {
            if (mon->sourceID == THORN) {
                continue; // Cannot damage thorns
            }
            factions::damage(state, nullptr, mon, 2);
        }
    };
    SpellCostMap[SpellID::BLADES] = 4;
    SpellReqsMap[SpellID::BLADES] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::BLADES] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::CRUSH] = [](State& state, Ent* target) -> void {
        if (state.enemies[0]->sourceID != THORN) {
            factions::damage(state, nullptr, state.enemies[0], 3);
        }
        if (state.enemies.size() > 1 && state.enemies.back()->sourceID != THORN) {
            factions::damage(state, nullptr, state.enemies.back(), 3);
        }
    };
    SpellCostMap[SpellID::CRUSH] = 3;
    SpellReqsMap[SpellID::CRUSH] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::CRUSH] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::MIASMA] = [](State& state, Ent* target) -> void {
        for (Ent* mon : util::findEnemyCleaveTargets(state, *target)) {
            if (mon->sourceID == THORN) {
                continue; // Cannot damage thorns
            }
            factions::damage(state, nullptr, mon, 1);
            KeywordFuncMap[KeywordID::POISON](state, nullptr, mon, 1);
        }
    };
    SpellCostMap[SpellID::MIASMA] = 3;
    SpellReqsMap[SpellID::MIASMA] = SpellTargetReq::ENEMY;
    SpellCastAffectMap[SpellID::MIASMA] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::TICK] = [](State& state, Ent* target) -> void {
        for (Ent* mon : util::findEnemyCleaveTargets(state, *target)) {
            if (mon->sourceID == THORN) {
                continue; // Cannot damage thorns
            }
            factions::damage(state, nullptr, mon, 1);
            KeywordFuncMap[KeywordID::WEAKEN](state, nullptr, mon, 1);
        }
    };
    SpellCostMap[SpellID::TICK] = 4;
    SpellReqsMap[SpellID::TICK] = SpellTargetReq::ENEMY;
    SpellCastAffectMap[SpellID::TICK] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::BLAZE] = [](State& state, Ent* target) -> void {
        factions::damage(state, nullptr, target, 13);
    };
    SpellCostMap[SpellID::BLAZE] = 6;
    SpellReqsMap[SpellID::BLAZE] = SpellTargetReq::ENEMY;
    SpellCastAffectMap[SpellID::BLAZE] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::DRAW] = [](State& state, Ent* target) -> void {
        target->boost += 1;
    };
    SpellCostMap[SpellID::DRAW] = 1;
    SpellReqsMap[SpellID::DRAW] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::DRAW] = SpellCastAffect::DEPLETE;
    
    SpellFuncMap[SpellID::HARVEST] = [](State& state, Ent* target) -> void {
        if (target->sourceID != THORN) {
            target->dead = true;
        }
        state.mana += 3;
    };
    SpellCostMap[SpellID::HARVEST] = 1;
    SpellReqsMap[SpellID::HARVEST] = SpellTargetReq::HP1;
    SpellCastAffectMap[SpellID::HARVEST] = SpellCastAffect::COOLDOWN;
    
    SpellFuncMap[SpellID::INSPIRE] = [](State& state, Ent* target) -> void {
        target->usedDie = false;
        target->timesUsed = 0;
    };
    SpellCostMap[SpellID::INSPIRE] = 4;
    SpellReqsMap[SpellID::INSPIRE] = SpellTargetReq::RECHARGE;
    SpellCastAffectMap[SpellID::INSPIRE] = SpellCastAffect::COOLDOWN;
    
    // Wierd spells / Tactics
    SpellFuncMap[SpellID::IMBUE] = [](State& state, Ent* target) -> void {
        if (target->currentSide.type == SideType::DAMAGE || target->currentSide.type == SideType::DAMAGE_ALL || target->currentSide.type == SideType::SELF_DAMAGE) {
            target->currentSide.value += 1;
        }
    };
    SpellCostMap[SpellID::IMBUE] = 1;
    SpellReqsMap[SpellID::IMBUE] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::IMBUE] = SpellCastAffect::SINGLE_CAST;
    
    SpellFuncMap[SpellID::FORMATION] = [](State& state, Ent* target) -> void {
        for (Ent* mon : state.enemies) {
            if (mon->sourceID == THORN) {
                continue; // Cannot damage thorns
            }
            factions::damage(state, nullptr, mon, 2);
        }
        for (Ent* hero : state.players) {
            hero->shield += 2;
        }
    };
    SpellCostMap[SpellID::FORMATION] = 6;
    SpellReqsMap[SpellID::FORMATION] = SpellTargetReq::DAMAGE_N_SHIELD_3;
    SpellCastAffectMap[SpellID::FORMATION] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::PARRY] = [](State& state, Ent* target) -> void {
        target->shield += 3;
    };
    SpellCostMap[SpellID::PARRY] = 2;
    SpellReqsMap[SpellID::PARRY] = SpellTargetReq::DAMAGE_2;
    SpellCastAffectMap[SpellID::PARRY] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::ELSE] = [](State& state, Ent* target) -> void {
        target->shield += 1;
        KeywordFuncMap[KeywordID::CLEANSE](state, nullptr, target, 1);
    };
    SpellCostMap[SpellID::ELSE] = 1;
    SpellReqsMap[SpellID::ELSE] = SpellTargetReq::AN_X;
    SpellCastAffectMap[SpellID::ELSE] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::LIGHT] = [](State& state, Ent* target) -> void {
        for (Ent* hero : util::findHeroCleaveTargets(state, *target)) {
            hero->shield += 1;
            KeywordFuncMap[KeywordID::CLEANSE](state, nullptr, hero, 1);
        }
    };
    SpellCostMap[SpellID::LIGHT] = 1;
    SpellReqsMap[SpellID::LIGHT] = SpellTargetReq::ALLY;
    SpellCastAffectMap[SpellID::LIGHT] = SpellCastAffect::SINGLE_CAST;
    
    SpellFuncMap[SpellID::UNITE] = [](State& state, Ent* target) -> void {
        factions::damage(state, nullptr, target, 15);
    };
    SpellCostMap[SpellID::UNITE] = 4;
    SpellReqsMap[SpellID::UNITE] = SpellTargetReq::ONE_OF_ALL;
    SpellCastAffectMap[SpellID::UNITE] = SpellCastAffect::STANDARD;
    
    SpellFuncMap[SpellID::CLINK] = [](State& state, Ent* target) -> void {
        for (Ent* hero : state.players) {
            hero->shield += 1;
            hero->boost += 1;
        }
    };
    SpellCostMap[SpellID::CLINK] = 4;
    SpellReqsMap[SpellID::CLINK] = SpellTargetReq::UNTARGETED;
    SpellCastAffectMap[SpellID::CLINK] = SpellCastAffect::SINGLE_CAST;
    
    SpellFuncMap[SpellID::BURST] = [](State& state, Ent* target) -> void {
        if (target->sourceID >= 0) { // if target is hero
            target->shield += 2;
        } else {
            factions::damage(state, nullptr, target, 2);
        }
    };
    SpellCostMap[SpellID::BURST] = 2;
    SpellReqsMap[SpellID::BURST] = SpellTargetReq::EITHER;
    SpellCastAffectMap[SpellID::BURST] = SpellCastAffect::STANDARD;
    
}
