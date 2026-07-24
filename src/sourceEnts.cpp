#include "sourceEnts.h"

#include "spellIDs.h"
#include "keywordIDs.h"
#include "monPassiveIDs.h"
#include "entIDs.h"
#include "util.h"

namespace SourceEnt {

std::array<Ent, NUM_HEROS> ALL_HEROS;
std::array<Ent, NUM_MONSTERS> ALL_MONSTERS;
std::vector<int> ORANGE_HEROS_LVL_1_IDS;
std::vector<int> ORANGE_HEROS_LVL_2_IDS;
std::vector<int> ORANGE_HEROS_LVL_3_IDS;
std::vector<int> YELLOW_HEROS_LVL_1_IDS;
std::vector<int> YELLOW_HEROS_LVL_2_IDS;
std::vector<int> YELLOW_HEROS_LVL_3_IDS;
std::vector<int> GRAY_HEROS_LVL_1_IDS;
std::vector<int> GRAY_HEROS_LVL_2_IDS;
std::vector<int> GRAY_HEROS_LVL_3_IDS;
std::vector<int> RED_HEROS_LVL_1_IDS;
std::vector<int> RED_HEROS_LVL_2_IDS;
std::vector<int> RED_HEROS_LVL_3_IDS;
std::vector<int> BLUE_HEROS_LVL_1_IDS;
std::vector<int> BLUE_HEROS_LVL_2_IDS;
std::vector<int> BLUE_HEROS_LVL_3_IDS;
std::vector<std::vector<std::vector<int>>> HERO_TREE; // (color, level) -> List of hero Indexes
std::vector<int> MAGIC_HEROS; // All magic heros, indexed by source
void initializeSourceEnts() {

    ORANGE_HEROS_LVL_1_IDS = {THIEF, SCOUNDREL, LOST, DABBLE, CLUMSY};
    ALL_HEROS[util::heroIDtoPos(THIEF)] = Ent(4, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, THIEF, 0);
    ALL_HEROS[util::heroIDtoPos(SCOUNDREL)] = Ent(7, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::VULNERABLE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SCOUNDREL, 0);
    ALL_HEROS[util::heroIDtoPos(LOST)] = Ent(3, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CRUEL})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED, KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CRUEL})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CRUEL})),
        }, LOST, 0);
    ALL_HEROS[util::heroIDtoPos(DABBLE)] = Ent(6, std::array<Side, 6>{
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, DABBLE, 0);
    ALL_HEROS[util::heroIDtoPos(CLUMSY)] = Ent(6, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::SELF_DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::SELF_DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
        }, CLUMSY, 0),
    
    ORANGE_HEROS_LVL_2_IDS = {DABBLER, GAMBLER, RANGER, ROGUE, TRAPPER, SPELLBLADE, NINJA, JUGGLER};
    ALL_HEROS[util::heroIDtoPos(DABBLER)] = Ent(8, std::array<Side, 6>{
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, DABBLER, 0);
    ALL_HEROS[util::heroIDtoPos(GAMBLER)] = Ent(7, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, GAMBLER, 0);
    ALL_HEROS[util::heroIDtoPos(RANGER)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED, KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED, KeywordID::ENGAGE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED, KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED, KeywordID::CLEAVE})),
        }, RANGER, 0);
    ALL_HEROS[util::heroIDtoPos(ROGUE)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CRUEL})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
        }, ROGUE, 0);
    ALL_HEROS[util::heroIDtoPos(TRAPPER)] = Ent(8, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::VULNERABLE})),
            Side(4, SideType::KILL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::VULNERABLE})),
        }, TRAPPER, 0);
    ALL_HEROS[util::heroIDtoPos(SPELLBLADE)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::COPYCAT})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
        }, SPELLBLADE, 0, SpellID::IMBUE);
    ALL_HEROS[util::heroIDtoPos(NINJA)] = Ent(6, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::COPYCAT})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DOUBLEUSE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::COPYCAT})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::COPYCAT})),
        }, NINJA, 0);
    ALL_HEROS[util::heroIDtoPos(JUGGLER)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::SELF_DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
        }, JUGGLER, 0),
    
    ORANGE_HEROS_LVL_3_IDS = {LUDUS, ASSASSIN, DANCER, FENCER, SHARPSHOT, VENOM, ROULETTE, DABBLEST, AGENT};
    ALL_HEROS[util::heroIDtoPos(LUDUS)] = Ent(7, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, LUDUS, 0);
    ALL_HEROS[util::heroIDtoPos(ASSASSIN)] = Ent(8, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED, KeywordID::ENGAGE})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CRUEL})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED, KeywordID::ENGAGE})),
        }, ASSASSIN, 0);
    ALL_HEROS[util::heroIDtoPos(DANCER)] = Ent(8, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RAMPAGE, KeywordID::PAIN})),
            Side(1, SideType::REROLL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
        }, DANCER, 0);
    ALL_HEROS[util::heroIDtoPos(FENCER)] = Ent(9, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DOUBLEUSE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PRISTINE})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DOUBLEUSE})),
        }, FENCER, 0);
    ALL_HEROS[util::heroIDtoPos(SHARPSHOT)] = Ent(8, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED})),
            Side(6, SideType::KILL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED, KeywordID::COPYCAT})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED})),
        }, SHARPSHOT, 0);
    ALL_HEROS[util::heroIDtoPos(VENOM)] = Ent(8, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED, KeywordID::POISON})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON, KeywordID::PLAGUE})),
            Side(3, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RANGED, KeywordID::POISON})),
        }, VENOM, 0);
    ALL_HEROS[util::heroIDtoPos(ROULETTE)] = Ent(8, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STICKY, KeywordID::DEATH, KeywordID::MANDATORY})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
        }, ROULETTE, 0);
    ALL_HEROS[util::heroIDtoPos(DABBLEST)] = Ent(9, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, DABBLEST, 0);
    ALL_HEROS[util::heroIDtoPos(AGENT)] = Ent(8, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})), // SHIFTER isn't implemented, so ill just assume it averages to useless (ie, inflictNothing or similar)
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INFLICT_PAIN, KeywordID::SINGLEUSE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::QUADUSE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::QUADUSE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, AGENT, 0);

    YELLOW_HEROS_LVL_1_IDS = {FIGHTER, BRIGAND, LAZY, RUFFIAN, HOARDER};
    ALL_HEROS[util::heroIDtoPos(FIGHTER)] = Ent(5, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, FIGHTER, 1);
    ALL_HEROS[util::heroIDtoPos(BRIGAND)] = Ent(5, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
        }, BRIGAND, 1);
    ALL_HEROS[util::heroIDtoPos(LAZY)] = Ent(5, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, LAZY, 1);
    ALL_HEROS[util::heroIDtoPos(RUFFIAN)] = Ent(4, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, RUFFIAN, 1);
    ALL_HEROS[util::heroIDtoPos(HOARDER)] = Ent(6, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GUILT})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEATH})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
        }, HOARDER, 1);
    
    YELLOW_HEROS_LVL_2_IDS = {BERSERKER, BRUTE, COLLECTOR, GLADIATOR, SOLDIER, WHIRL, SCRAPPER, SINEW};
    ALL_HEROS[util::heroIDtoPos(BERSERKER)] = Ent(8, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEATHWISH})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
        }, BERSERKER, 1);
    ALL_HEROS[util::heroIDtoPos(BRUTE)] = Ent(8, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(0, SideType::STUN, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
        }, BRUTE, 1);
    ALL_HEROS[util::heroIDtoPos(COLLECTOR)] = Ent(8, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEATHWISH})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DUPLICATE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::FOCUS})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
        }, COLLECTOR, 1);
    ALL_HEROS[util::heroIDtoPos(GLADIATOR)] = Ent(7, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ENGAGE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ENGAGE})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
        }, GLADIATOR, 1);
    ALL_HEROS[util::heroIDtoPos(SOLDIER)] = Ent(7, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SOLDIER, 1);
    ALL_HEROS[util::heroIDtoPos(WHIRL)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
        }, WHIRL, 1);
    ALL_HEROS[util::heroIDtoPos(SCRAPPER)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::BLOODLUST})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::BLOODLUST})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
        }, SCRAPPER, 1);
    ALL_HEROS[util::heroIDtoPos(SINEW)] = Ent(8, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::CHAIN})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
        }, SINEW, 1);
    
    YELLOW_HEROS_LVL_3_IDS = {BARBARIAN, BRAWLER, CURATOR, LEADER, VETERAN, BASH, ECCENTRIC, CAPTAIN, WANDERER};
    ALL_HEROS[util::heroIDtoPos(BARBARIAN)] = Ent(10, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::BLOODLUST})),
            Side(10, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEATH})),
            Side(8, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::BLOODLUST})),
        }, BARBARIAN, 1);
    ALL_HEROS[util::heroIDtoPos(BRAWLER)] = Ent(9, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RAMPAGE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RAMPAGE})),
        }, BRAWLER, 1);
    ALL_HEROS[util::heroIDtoPos(CURATOR)] = Ent(9, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CHARGED})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ENGAGE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RAMPAGE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ERA})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
        }, CURATOR, 1);
    ALL_HEROS[util::heroIDtoPos(LEADER)] = Ent(9, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::RECHARGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DUPLICATE})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SMITH})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, LEADER, 1);
    ALL_HEROS[util::heroIDtoPos(VETERAN)] = Ent(11, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, VETERAN, 1);
    ALL_HEROS[util::heroIDtoPos(BASH)] = Ent(10, std::array<Side, 6>{
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
            Side(0, SideType::STUN, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
        }, BASH, 1);
    ALL_HEROS[util::heroIDtoPos(ECCENTRIC)] = Ent(9, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, ECCENTRIC, 1);
    ALL_HEROS[util::heroIDtoPos(CAPTAIN)] = Ent(10, std::array<Side, 6>{
            Side(4, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::FOCUS})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::CHAIN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::CHAIN})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, CAPTAIN, 1, SpellID::FORMATION);
    ALL_HEROS[util::heroIDtoPos(WANDERER)] = Ent(10, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ERA})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEFY})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::COPYCAT})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::COPYCAT})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ERA})),
        }, WANDERER, 1);

    GRAY_HEROS_LVL_1_IDS = {DEFENDER, BUCKLE, SQUIRE, ALLOY, WALLOP};
    ALL_HEROS[util::heroIDtoPos(DEFENDER)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, DEFENDER, 2);
    ALL_HEROS[util::heroIDtoPos(BUCKLE)] = Ent(6, std::array<Side, 6>{
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PRISTINE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, BUCKLE, 2);
    ALL_HEROS[util::heroIDtoPos(SQUIRE)] = Ent(5, std::array<Side, 6>{
            Side(2, SideType::REDIRECTINCOMING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::FOCUS})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::FOCUS})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::REDIRECTINCOMING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
        }, SQUIRE, 2);
    ALL_HEROS[util::heroIDtoPos(ALLOY)] = Ent(4, std::array<Side, 6>{
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::COPYCAT})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::REPEL})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, ALLOY, 2);
    ALL_HEROS[util::heroIDtoPos(WALLOP)] = Ent(5, std::array<Side, 6>{
            Side(0, SideType::STUN, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::UNDYING, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::STUN, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, WALLOP, 2);

    GRAY_HEROS_LVL_2_IDS = {KNIGHT, ARMORER, BARD, CLERIC, GUARDIAN, PILGRIM, MONK, WARDEN};
    ALL_HEROS[util::heroIDtoPos(KNIGHT)] = Ent(7, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
        }, KNIGHT, 2, SpellID::PARRY);
    ALL_HEROS[util::heroIDtoPos(ARMORER)] = Ent(9, std::array<Side, 6>{
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SMITH})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SMITH})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
        }, ARMORER, 2);
    ALL_HEROS[util::heroIDtoPos(BARD)] = Ent(6, std::array<Side, 6>{
            Side(1, SideType::SHIELD_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::REROLL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SHIELD_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, BARD, 2, SpellID::ELSE);
    ALL_HEROS[util::heroIDtoPos(CLERIC)] = Ent(8, std::array<Side, 6>{
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(3, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
        }, CLERIC, 2, SpellID::LIGHT);
    ALL_HEROS[util::heroIDtoPos(GUARDIAN)] = Ent(8, std::array<Side, 6>{
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ENGAGE})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ENGAGE})),
        }, GUARDIAN, 2);
    ALL_HEROS[util::heroIDtoPos(PILGRIM)] = Ent(8, std::array<Side, 6>{
            Side(0, SideType::ENCHANT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(0, SideType::RECHARGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::UNDYING, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::STUN, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::STUN, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::ENCHANT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
        }, PILGRIM, 2);
    ALL_HEROS[util::heroIDtoPos(MONK)] = Ent(9, std::array<Side, 6>{
            Side(3, SideType::REDIRECTINCOMING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::REPEL})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::REPEL})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::REDIRECTINCOMING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
        }, MONK, 2);
    ALL_HEROS[util::heroIDtoPos(WARDEN)] = Ent(10, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, WARDEN, 2);
    
    GRAY_HEROS_LVL_3_IDS = {KEEPER, PALADIN, PRINCE, STALWART, POET, VALKYRIE, STOIC};
    ALL_HEROS[util::heroIDtoPos(KEEPER)] = Ent(10, std::array<Side, 6>{
            Side(5, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::REPEL})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::REPEL})),
            Side(0, SideType::ENCHANT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD, KeywordID::SELFHEAL})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
        }, KEEPER, 2);
    ALL_HEROS[util::heroIDtoPos(PALADIN)] = Ent(11, std::array<Side, 6>{
            Side(3, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(3, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, PALADIN, 2);
    ALL_HEROS[util::heroIDtoPos(PRINCE)] = Ent(9, std::array<Side, 6>{
            Side(3, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INSPIRED})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DUPLICATE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, PRINCE, 2, SpellID::UNITE);
    ALL_HEROS[util::heroIDtoPos(STALWART)] = Ent(11, std::array<Side, 6>{
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DOUBLEUSE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
        }, STALWART, 2);
    ALL_HEROS[util::heroIDtoPos(POET)] = Ent(8, std::array<Side, 6>{
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CHARGED})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(2, SideType::SHIELD_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CHARGED})),
        }, POET, 2, SpellID::CLINK);
    ALL_HEROS[util::heroIDtoPos(VALKYRIE)] = Ent(9, std::array<Side, 6>{
            Side(0, SideType::UNDYING, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEATHWISH})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RESCUE})),
            Side(2, SideType::RESURRECT, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::RESURRECT, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::UNDYING, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, VALKYRIE, 2);
    ALL_HEROS[util::heroIDtoPos(STOIC)] = Ent(15, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(0, SideType::STUN, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::REDIRECTINCOMING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(2, SideType::REDIRECTINCOMING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(1, SideType::REDIRECTINCOMING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
        }, STOIC, 2);
    
    RED_HEROS_LVL_1_IDS = {HEALER, GARDENER, ACOLYTE, MYSTIC, SPLINT};
    ALL_HEROS[util::heroIDtoPos(HEALER)] = Ent(5, std::array<Side, 6>{
            Side(4, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, HEALER, 3, SpellID::MEND);
    ALL_HEROS[util::heroIDtoPos(GARDENER)] = Ent(5, std::array<Side, 6>{
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROOOOWTH})),
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, GARDENER, 3, SpellID::HEMLOCK);
    ALL_HEROS[util::heroIDtoPos(ACOLYTE)] = Ent(5, std::array<Side, 6>{
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
            Side(3, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::VITALITY})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
        }, ACOLYTE, 3, SpellID::RESTORE);
    ALL_HEROS[util::heroIDtoPos(MYSTIC)] = Ent(4, std::array<Side, 6>{
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
        }, MYSTIC, 3, SpellID::GAZE);
    ALL_HEROS[util::heroIDtoPos(SPLINT)] = Ent(4, std::array<Side, 6>{
            Side(5, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
        }, SPLINT, 3, SpellID::BANDAGE);
    
    RED_HEROS_LVL_2_IDS = {DRUID, HERBALIST, MEDIC, PRIESTESS, VAMPIRE, ENCHANTER, DISCIPLE, FEY};
    ALL_HEROS[util::heroIDtoPos(DRUID)] = Ent(7, std::array<Side, 6>{
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
            Side(2, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, DRUID, 3, SpellID::BALANCE),
    ALL_HEROS[util::heroIDtoPos(HERBALIST)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::REGEN})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON, KeywordID::SINGLEUSE})),
            Side(0, SideType::ENCHANT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::REGEN})),
        }, HERBALIST, 3, SpellID::VINE),
    ALL_HEROS[util::heroIDtoPos(MEDIC)] = Ent(7, std::array<Side, 6>{
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::REGEN})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
        }, MEDIC, 3, SpellID::RENEW),
    ALL_HEROS[util::heroIDtoPos(PRIESTESS)] = Ent(5, std::array<Side, 6>{
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(4, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::VITALITY})),
            Side(4, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::VITALITY})),
            Side(1, SideType::HEAL_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::HEAL_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
        }, PRIESTESS, 3, SpellID::PRAY),
    ALL_HEROS[util::heroIDtoPos(VAMPIRE)] = Ent(7, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL})),
        }, VAMPIRE, 3, SpellID::INFUSE),
    ALL_HEROS[util::heroIDtoPos(ENCHANTER)] = Ent(7, std::array<Side, 6>{
            Side(0, SideType::ENCHANT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD, KeywordID::SELFHEAL})),
            Side(2, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(0, SideType::ENCHANT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ENGAGE})),
            Side(0, SideType::ENCHANT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL})),
            Side(0, SideType::ENCHANT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL})),
            Side(0, SideType::ENCHANT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD, KeywordID::SELFHEAL})),
        }, ENCHANTER, 3, SpellID::FLIP),
    ALL_HEROS[util::heroIDtoPos(DISCIPLE)] = Ent(8, std::array<Side, 6>{
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DOUBLEUSE})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DOUBLEUSE})),
            Side(1, SideType::RESURRECT, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, DISCIPLE, 3, SpellID::GLOW),
    ALL_HEROS[util::heroIDtoPos(FEY)] = Ent(6, std::array<Side, 6>{
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::BOOST})),
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::BOOST})),
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
        }, FEY, 3, SpellID::CIRCLE);

    RED_HEROS_LVL_3_IDS = {DOCTOR, FORSAKEN, PROPHET, SHAMAN, WITCH, WRAITH, SURGEON, FATE};
    ALL_HEROS[util::heroIDtoPos(DOCTOR)] = Ent(9, std::array<Side, 6>{
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::REGEN})),
            Side(1, SideType::DAMAGE_EVERYONE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::REGEN})),
        }, DOCTOR, 3, SpellID::LIQUOR),
    ALL_HEROS[util::heroIDtoPos(FORSAKEN)] = Ent(9, std::array<Side, 6>{
            Side(2, SideType::HEAL_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::RESURRECT, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(2, SideType::HEAL_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, FORSAKEN, 3, SpellID::BIND),
    ALL_HEROS[util::heroIDtoPos(PROPHET)] = Ent(8, std::array<Side, 6>{
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(3, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::RESCUE})),
            Side(5, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::VITALITY})),
            Side(5, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::VITALITY})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
        }, PROPHET, 3, SpellID::SOOTHE),
    ALL_HEROS[util::heroIDtoPos(SHAMAN)] = Ent(11, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
            Side(5, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::VITALITY})),
            Side(10, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
        }, SHAMAN, 3, SpellID::RITUAL),
    ALL_HEROS[util::heroIDtoPos(WITCH)] = Ent(10, std::array<Side, 6>{
            Side(1, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::BOOST})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEANSE})),
            Side(3, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
        }, WITCH, 3, SpellID::SALVE),
    ALL_HEROS[util::heroIDtoPos(WRAITH)] = Ent(9, std::array<Side, 6>{
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL})),
            Side(0, SideType::ENCHANT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL, KeywordID::SELFSHIELD})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, WRAITH, 3, SpellID::LEECH),
    ALL_HEROS[util::heroIDtoPos(SURGEON)] = Ent(9, std::array<Side, 6>{
            Side(3, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(2, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(3, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DOUBLEUSE})),
            Side(3, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DOUBLEUSE})),
            Side(3, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SURGEON, 3, SpellID::OPERATE);
    ALL_HEROS[util::heroIDtoPos(FATE)] = Ent(7, std::array<Side, 6>{
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(2, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(2, SideType::HEALSHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
        }, FATE, 3, SpellID::STRAND);
    
    BLUE_HEROS_LVL_1_IDS = {MAGE, PRODIGY, MEDDLER, STUDENT, INITIATE, CULTIST};
    ALL_HEROS[util::heroIDtoPos(MAGE)] = Ent(4, std::array<Side, 6>{
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, MAGE, 4, SpellID::POKE),
    ALL_HEROS[util::heroIDtoPos(PRODIGY)] = Ent(4, std::array<Side, 6>{
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIR})),
            Side(1, SideType::RESURRECT, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(2, SideType::SELF_DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANDATORY})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIR})),
        }, PRODIGY, 4, SpellID::SCORCH),
    ALL_HEROS[util::heroIDtoPos(MEDDLER)] = Ent(6, std::array<Side, 6>{
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP, KeywordID::BONED})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP, KeywordID::BONED})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
        }, MEDDLER, 4, SpellID::SLAY),
    ALL_HEROS[util::heroIDtoPos(STUDENT)] = Ent(5, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::RECHARGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, STUDENT, 4, SpellID::SLICE),
    ALL_HEROS[util::heroIDtoPos(INITIATE)] = Ent(4, std::array<Side, 6>{
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
        }, INITIATE, 4, SpellID::GATHER),
    ALL_HEROS[util::heroIDtoPos(CULTIST)] = Ent(5, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL, KeywordID::SINGLEUSE})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL, KeywordID::SINGLEUSE})),
        }, CULTIST, 4, SpellID::CUT),
    
    BLUE_HEROS_LVL_2_IDS = {CALDERA, EVOKER, GLACIA, JESTER, SPARKY, MYCO, SEER, FIEND};
    ALL_HEROS[util::heroIDtoPos(CALDERA)] = Ent(6, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CRUEL, KeywordID::SINGLEUSE})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
        }, CALDERA, 4, SpellID::SCALD),
    ALL_HEROS[util::heroIDtoPos(EVOKER)] = Ent(10, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, EVOKER, 4, SpellID::DROP),
    ALL_HEROS[util::heroIDtoPos(GLACIA)] = Ent(9, std::array<Side, 6>{
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN, KeywordID::SINGLEUSE})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
        }, GLACIA, 4, SpellID::CHILL),
    ALL_HEROS[util::heroIDtoPos(JESTER)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::REROLL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
        }, JESTER, 4, SpellID::FLICK),
    ALL_HEROS[util::heroIDtoPos(SPARKY)] = Ent(7, std::array<Side, 6>{
            Side(0, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CHARGED})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CHARGED})),
        }, SPARKY, 4, SpellID::ZAP),
    ALL_HEROS[util::heroIDtoPos(MYCO)] = Ent(8, std::array<Side, 6>{
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DECAY})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DECAY})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DECAY})),
            Side(1, SideType::RESURRECT, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
        }, MYCO, 4, SpellID::SPORE),
    ALL_HEROS[util::heroIDtoPos(SEER)] = Ent(7, std::array<Side, 6>{
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON, KeywordID::SINGLEUSE})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
        }, SEER, 4, SpellID::FORETELL),
    ALL_HEROS[util::heroIDtoPos(FIEND)] = Ent(6, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL, KeywordID::SINGLEUSE})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFHEAL, KeywordID::SINGLEUSE})),
        }, FIEND, 4, SpellID::BURN),
    
    BLUE_HEROS_LVL_3_IDS = {ARTIFICER, WEAVER, SORCERER, CHRONOS, WARLOCK, ACE, GHAST, WIZARD};
    ALL_HEROS[util::heroIDtoPos(ARTIFICER)] = Ent(8, std::array<Side, 6>{
            Side(4, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE, KeywordID::SELFHEAL})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE, KeywordID::CHARGED})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE, KeywordID::POISON})),
            Side(10, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(4, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
        }, ARTIFICER, 4, SpellID::BLADES),
    ALL_HEROS[util::heroIDtoPos(WEAVER)] = Ent(10, std::array<Side, 6>{
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
            Side(0, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CHARGED})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE})),
        }, WEAVER, 4, SpellID::CRUSH),
    ALL_HEROS[util::heroIDtoPos(SORCERER)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::REROLL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
        }, SORCERER, 4, SpellID::MIASMA),
    ALL_HEROS[util::heroIDtoPos(CHRONOS)] = Ent(7, std::array<Side, 6>{
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
            Side(0, SideType::RECHARGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::REROLL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(0, SideType::DODGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::GROWTH})),
        }, CHRONOS, 4, SpellID::TICK),
    ALL_HEROS[util::heroIDtoPos(WARLOCK)] = Ent(7, std::array<Side, 6>{
            Side(4, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::BLOODLUST})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE, KeywordID::SELFHEAL})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
        }, WARLOCK, 4, SpellID::BLAZE),
    ALL_HEROS[util::heroIDtoPos(ACE)] = Ent(8, std::array<Side, 6>{
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::TRIO})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIR})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, ACE, 4, SpellID::DRAW),
    ALL_HEROS[util::heroIDtoPos(GHAST)] = Ent(8, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE, KeywordID::WEAKEN})),
            Side(5, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEATH})),
            Side(2, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE, KeywordID::WEAKEN})),
        }, GHAST, 4, SpellID::HARVEST),
    ALL_HEROS[util::heroIDtoPos(WIZARD)] = Ent(9, std::array<Side, 6>{
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SINGLEUSE, KeywordID::PERMABOOST})),
            Side(1, SideType::MANA, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CANTRIP})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
            Side(1, SideType::SHIELD, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::MANAGAIN})),
        }, WIZARD, 4, SpellID::INSPIRE);

    // Monsters: small / reg / big / huge
    
    // Small monsters
    ALL_MONSTERS[util::monIDtoPos(BONES)] = Ent(4, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, BONES, OnHitPassive::ON_HIT_NO_PASSIVE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Bones
    ALL_MONSTERS[util::monIDtoPos(LOG)] = Ent(3, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, LOG, OnHitPassive::ON_HIT_NO_PASSIVE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Log - Does not roll away, this is fine
    ALL_MONSTERS[util::monIDtoPos(ARCHER)] = Ent(2, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, ARCHER, 0, 0, 0, 0, OnHitPassive::ON_HIT_NO_PASSIVE, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}), true, 16); // Archer
    ALL_MONSTERS[util::monIDtoPos(SNIPER)] = Ent(3, std::array<Side, 6>{
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SNIPER, 0, 0, 0, 0, OnHitPassive::ON_HIT_NO_PASSIVE, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}), true, 16); // Sniper
    ALL_MONSTERS[util::monIDtoPos(SHADE)] = Ent(5, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
        }, SHADE, OnHitPassive::ON_HIT_SHADE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({1,2,3,4,5})); // Shade
    ALL_MONSTERS[util::monIDtoPos(RAT)] = Ent(3, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, RAT, OnHitPassive::ON_HIT_NO_PASSIVE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Rat
    ALL_MONSTERS[util::monIDtoPos(WISP)] = Ent(5, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INFLICT_PAIN})),
            Side(3, SideType::HEAL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::VITALITY})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INFLICT_PAIN})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INFLICT_PAIN})),
        }, WISP, OnHitPassive::ON_HIT_WISP, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({3})); // Wisp
    ALL_MONSTERS[util::monIDtoPos(SLIMELET)] = Ent(2, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SLIMELET, OnHitPassive::ON_HIT_NO_PASSIVE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Slimelet
    ALL_MONSTERS[util::monIDtoPos(IMP)] = Ent(4, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(8, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEATH})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, IMP, OnHitPassive::ON_HIT_IMP, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Imp
    ALL_MONSTERS[util::monIDtoPos(SPIDER)] = Ent(4, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SPIDER, OnHitPassive::ON_HIT_NO_PASSIVE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Spider
    ALL_MONSTERS[util::monIDtoPos(BEE)] = Ent(2, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEATH})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEATH})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, BEE, OnHitPassive::ON_HIT_NO_PASSIVE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Bee
    ALL_MONSTERS[util::monIDtoPos(GRAVE)] = Ent(3, std::array<Side, 6>{
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
        }, GRAVE, OnHitPassive::ON_HIT_GRAVE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({1,2,3})); // Grave
    ALL_MONSTERS[util::monIDtoPos(ILLUSION)] = Ent(1, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
        }, ILLUSION, OnHitPassive::ON_HIT_NO_PASSIVE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Illusion
    ALL_MONSTERS[util::monIDtoPos(CHEST)] = Ent(5, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-SLIMELET)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-SLIMELET)),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
        }, CHEST, OnHitPassive::ON_HIT_CHEST, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({5})); // Chest (summon slimelet etc.)
    ALL_MONSTERS[util::monIDtoPos(THORN)] = Ent(2, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
        }, THORN, OnHitPassive::ON_HIT_THORN, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Thorn
    ALL_MONSTERS[util::monIDtoPos(CAW_EGG)] = Ent(4, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-CAW)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-CAW)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-CAW)),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, CAW_EGG, OnHitPassive::ON_HIT_NO_PASSIVE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Caw egg
    ALL_MONSTERS[util::monIDtoPos(SEED)] = Ent(1, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-THORN)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-THORN)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-THORN)),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SEED, OnHitPassive::ON_HIT_NO_PASSIVE, 16, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Seed
    // Reg Monsters
    ALL_MONSTERS[util::monIDtoPos(GOBLIN)] = Ent(5, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
        }, GOBLIN, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Goblin
    ALL_MONSTERS[util::monIDtoPos(GNOLL)] = Ent(3, std::array<Side, 6>{
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
        }, GNOLL, 0, 0, 0, 1, OnHitPassive::ON_HIT_NO_PASSIVE, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}), false, 24); // Gnoll
    ALL_MONSTERS[util::monIDtoPos(ZOMBIE)] = Ent(10, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
        }, ZOMBIE, OnHitPassive::ON_HIT_ZOMBIE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Zombie
    ALL_MONSTERS[util::monIDtoPos(WOLF)] = Ent(6, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, WOLF, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Wolf
    ALL_MONSTERS[util::monIDtoPos(SNAKE)] = Ent(5, std::array<Side, 6>{
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
        }, SNAKE, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Snake
    ALL_MONSTERS[util::monIDtoPos(QUARTZ)] = Ent(7, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INFLICT_SINGLEUSE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INFLICT_SINGLEUSE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
        }, QUARTZ, OnHitPassive::ON_HIT_QUARTZ, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Quartz
    ALL_MONSTERS[util::monIDtoPos(SUDUL)] = Ent(7, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SUDUL, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Sudul
    ALL_MONSTERS[util::monIDtoPos(FANATIC)] = Ent(13, std::array<Side, 6>{
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(8, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(8, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PAIN})),
        }, FANATIC, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Fanatic
    ALL_MONSTERS[util::monIDtoPos(SABER)] = Ent(10, std::array<Side, 6>{
            Side(8, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(12, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DEATH})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(8, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
        }, SABER, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Saber
    ALL_MONSTERS[util::monIDtoPos(MILITIA)] = Ent(7, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, MILITIA, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Militia
    ALL_MONSTERS[util::monIDtoPos(CARRIER)] = Ent(10, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE_EVERYONE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
        }, CARRIER, 0, 2, 0, 0, OnHitPassive::ON_HIT_NO_PASSIVE, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}), false, 24); // Carrier (last side was poisonAll)
    ALL_MONSTERS[util::monIDtoPos(BANDIT)] = Ent(8, std::array<Side, 6>{
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, BANDIT, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Bandit
    ALL_MONSTERS[util::monIDtoPos(BARREL)] = Ent(6, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, BARREL, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Barrel
    ALL_MONSTERS[util::monIDtoPos(GOLEM)] = Ent(2, std::array<Side, 6>{
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
            Side(0, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::STEEL})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::SELFSHIELD})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, GOLEM, 8, 0, 0, 0, OnHitPassive::ON_HIT_NO_PASSIVE, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}), false, 24); // Golem
    ALL_MONSTERS[util::monIDtoPos(FOUNTAIN)] = Ent(6, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, FOUNTAIN, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({1,2,3,4,5,6})); // Fountain (blank)
    ALL_MONSTERS[util::monIDtoPos(WARCHIEF)] = Ent(6, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(0, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, WARCHIEF, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Warchief
    ALL_MONSTERS[util::monIDtoPos(BLIND)] = Ent(5, std::array<Side, 6>{
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, BLIND, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Blind
    ALL_MONSTERS[util::monIDtoPos(DRAGON_EGG)] = Ent(6, std::array<Side, 6>{
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-DRAGON)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-DRAGON)),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(0, SideType::BLANK, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, DRAGON_EGG, OnHitPassive::ON_HIT_NO_PASSIVE, 24, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Caw egg

    // Big Monsters
    ALL_MONSTERS[util::monIDtoPos(WIZZ)] = Ent(5, std::array<Side, 6>{
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(3, SideType::HEAL_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(2, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, WIZZ, 0, 0, 0, 0, OnHitPassive::ON_HIT_WIZZ, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({4}), true, 30); // Wizz
    ALL_MONSTERS[util::monIDtoPos(SLIMER)] = Ent(7, std::array<Side, 6>{
            Side(3, SideType::DAMAGE_FLANKING, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(3, SideType::DAMAGE_FLANKING, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SLIMER, OnHitPassive::ON_HIT_SLIMER, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({5})); // Slimer
    ALL_MONSTERS[util::monIDtoPos(ALPHA)] = Ent(13, std::array<Side, 6>{
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-WOLF)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-WOLF)),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, ALPHA, OnHitPassive::ON_HIT_NO_PASSIVE, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Alpha
    ALL_MONSTERS[util::monIDtoPos(OGRE)] = Ent(10, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
        }, OGRE, OnHitPassive::ON_HIT_OGRE, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({3, 8})); // Ogre
    ALL_MONSTERS[util::monIDtoPos(DEMON)] = Ent(12, std::array<Side, 6>{
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INFLICT_PAIN})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INFLICT_PAIN})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-IMP)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-IMP)),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, DEMON, OnHitPassive::ON_HIT_DEMON, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({1, 11})); // Demon
    ALL_MONSTERS[util::monIDtoPos(CHOMP)] = Ent(10, std::array<Side, 6>{
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, CHOMP, OnHitPassive::ON_HIT_CHOMP, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({1,2,3,4,5,6,7,8,9,10})); // Chomp
    ALL_MONSTERS[util::monIDtoPos(GHOST)] = Ent(6, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ELIMINATE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND})),
        }, GHOST, OnHitPassive::ON_HIT_GHOST, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({5})); // Ghost
    ALL_MONSTERS[util::monIDtoPos(BOAR)] = Ent(7, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE_FLANKING, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE_FLANKING, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, BOAR, OnHitPassive::ON_HIT_BOAR, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({1})); // Boar
    ALL_MONSTERS[util::monIDtoPos(SPIKER)] = Ent(13, std::array<Side, 6>{
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SPIKER, OnHitPassive::ON_HIT_SPIKER, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Spiker
    ALL_MONSTERS[util::monIDtoPos(SLATE)] = Ent(5, std::array<Side, 6>{
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SLATE, OnHitPassive::ON_HIT_SLATE, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({1,2,3,4,5})); // Slate
    ALL_MONSTERS[util::monIDtoPos(TROLL)] = Ent(15, std::array<Side, 6>{
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::POISON})),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, TROLL, 0, 0, 1, 0, OnHitPassive::ON_HIT_NO_PASSIVE, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}), false, 30); // Troll
    ALL_MONSTERS[util::monIDtoPos(CAW)] = Ent(7, std::array<Side, 6>{
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, CAW, OnHitPassive::ON_HIT_CAW, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}));
    ALL_MONSTERS[util::monIDtoPos(MAGRAT)] = Ent(7, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND})),
        }, MAGRAT, OnHitPassive::ON_HIT_MAGRAT, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({4}));
    ALL_MONSTERS[util::monIDtoPos(GYTHA)] = Ent(7, std::array<Side, 6>{
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN, KeywordID::CLEAVE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN, KeywordID::CLEAVE})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
        }, GYTHA, OnHitPassive::ON_HIT_GYTHA, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({4}));
    ALL_MONSTERS[util::monIDtoPos(AGNES)] = Ent(7, std::array<Side, 6>{
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-WOLF)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-WOLF)),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, AGNES, OnHitPassive::ON_HIT_AGNES, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({4}));
    ALL_MONSTERS[util::monIDtoPos(BASILISK)] = Ent(12, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN, KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
        }, BASILISK, OnHitPassive::ON_HIT_BASILISK, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}));
    ALL_MONSTERS[util::monIDtoPos(HYDRA)] = Ent(20, std::array<Side, 6>{
            Side(10, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN, KeywordID::CLEAVE})),
            Side(10, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, HYDRA, OnHitPassive::ON_HIT_HYDRA, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}));
    ALL_MONSTERS[util::monIDtoPos(CYCLOPS)] = Ent(15, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(3, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
        }, CYCLOPS, OnHitPassive::ON_HIT_CYCLOPS, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({8}));
    ALL_MONSTERS[util::monIDtoPos(BRAMBLE)] = Ent(11, std::array<Side, 6>{
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
        }, BRAMBLE, OnHitPassive::ON_HIT_NO_PASSIVE, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}));
    ALL_MONSTERS[util::monIDtoPos(BANSHEE)] = Ent(10, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::POISON})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
        }, BANSHEE, OnHitPassive::ON_HIT_NO_PASSIVE, 30, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}));
    // Huge Monsters
    ALL_MONSTERS[util::monIDtoPos(SLIME_QUEEN)] = Ent(13, std::array<Side, 6>{
            Side(5, SideType::DAMAGE_FLANKING, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(9, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(5, SideType::DAMAGE_FLANKING, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, SLIME_QUEEN, OnHitPassive::ON_HIT_SLIME_QUEEN, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({5,10})); // Slime Queen
    ALL_MONSTERS[util::monIDtoPos(LICH)] = Ent(20, std::array<Side, 6>{
            Side(2, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(1, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(2, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
        }, LICH, OnHitPassive::ON_HIT_LICH, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({5, 10, 15, 20})); // Lich
    ALL_MONSTERS[util::monIDtoPos(TROLL_KING)] = Ent(20, std::array<Side, 6>{
            Side(3, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::POISON})),
            Side(3, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
        }, TROLL_KING, 0, 0, 2, 0, OnHitPassive::ON_HIT_NO_PASSIVE, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({}), false, 64); // Troll King
    ALL_MONSTERS[util::monIDtoPos(BASALT)] = Ent(35, std::array<Side, 6>{
            Side(9, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INFLICT_DEATH})),
            Side(4, SideType::DAMAGE_FLANKING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(4, SideType::DAMAGE_FLANKING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-SLATE)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-SLATE)),
            Side(9, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::INFLICT_DEATH})),
        }, BASALT, OnHitPassive::ON_HIT_BASALT, 64, util::int2ba<MAX_NUM_SPECIAL_HP>(1)); // Basalt
    ALL_MONSTERS[util::monIDtoPos(TARANTUS)] = Ent(25, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN, KeywordID::POISON})),
            Side(12, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(12, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-SPIDER)),
            Side(2, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-SPIDER)),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::WEAKEN, KeywordID::POISON})),
        }, TARANTUS, OnHitPassive::ON_HIT_TARANTUS, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({10})); // Tarantus
    ALL_MONSTERS[util::monIDtoPos(ROTTEN)] = Ent(23, std::array<Side, 6>{
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-IMP)),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::POISON})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::POISON})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-IMP)),
        }, ROTTEN, OnHitPassive::ON_HIT_NO_PASSIVE, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Rotten
    ALL_MONSTERS[util::monIDtoPos(HEXIA)] = Ent(30, std::array<Side, 6>{
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND, KeywordID::INFLICT_PAIN})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::DESCEND, KeywordID::INFLICT_PAIN})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-DEMON)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-DEMON)),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::PETRIFY})),
        }, HEXIA, OnHitPassive::ON_HIT_HEXIA, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Hexia
    ALL_MONSTERS[util::monIDtoPos(DRAGON)] = Ent(40, std::array<Side, 6>{
            Side(15, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(5, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(5, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::POISON})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::POISON})),
            Side(15, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
        }, DRAGON, OnHitPassive::ON_HIT_NO_PASSIVE, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Dragon
    ALL_MONSTERS[util::monIDtoPos(BARON)] = Ent(25, std::array<Side, 6>{
            Side(2, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::INFLICT_EXERT})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::INFLICT_EXERT})),
            Side(2, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::INFLICT_EXERT})),
            Side(1, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::POISON})),
            Side(2, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
        }, BARON, OnHitPassive::ON_HIT_BARON, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({2,4,6,8,10,12,14,16,18,20,22,24})); // Baron
    ALL_MONSTERS[util::monIDtoPos(SARCOPHAGUS)] = Ent(15, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(2, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-SPIDER)),
            Side(2, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-BONES)),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
        }, SARCOPHAGUS, OnHitPassive::ON_HIT_SARCOPHAGUS, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({13, 14, 15})); // Sarcophagus
    ALL_MONSTERS[util::monIDtoPos(BELL)] = Ent(21, std::array<Side, 6>{
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(3, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(3, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-IMP)),
            Side(1, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-IMP)),
            Side(2, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::EXERT})),
        }, BELL, OnHitPassive::ON_HIT_NO_PASSIVE, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Bell
    ALL_MONSTERS[util::monIDtoPos(MADNESS)] = Ent(14, std::array<Side, 6>{
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::BLOODLUST, KeywordID::ELIMINATE})),
            Side(5, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::BLOODLUST, KeywordID::ELIMINATE})),
            Side(7, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::HEAVY})),
        }, MADNESS, OnHitPassive::ON_HIT_NO_PASSIVE, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // Madness
    ALL_MONSTERS[util::monIDtoPos(INEVITABLE)] = Ent(30, std::array<Side, 6>{
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ERA, KeywordID::CLEAVE, KeywordID::INFLICT_EXERT})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ERA, KeywordID::HEAVY, KeywordID::SELFHEAL})),
            Side(6, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ERA, KeywordID::HEAVY, KeywordID::SELFHEAL})),
            Side(3, SideType::DAMAGE_FLANKING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ERA, KeywordID::WEAKEN})),
            Side(3, SideType::DAMAGE_FLANKING, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ERA, KeywordID::WEAKEN})),
            Side(3, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::ERA, KeywordID::CLEAVE, KeywordID::INFLICT_EXERT})),
        }, INEVITABLE, OnHitPassive::ON_HIT_INEVITABLE, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({3, 8, 13, 18, 23, 28})); // Inevitable
    ALL_MONSTERS[util::monIDtoPos(THE_HAND)] = Ent(45, std::array<Side, 6>{
            Side(3, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-SABER)),
            Side(999, SideType::KILL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(10, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE})),
            Side(7, SideType::DAMAGE_ALL, util::makeBoolArray<NUM_KEYWORDS>({})),
            Side(4, SideType::DAMAGE, util::makeBoolArray<NUM_KEYWORDS>({KeywordID::CLEAVE, KeywordID::POISON})),
            Side(3, SideType::SUMMON, util::int2ba<NUM_KEYWORDS>(-SABER)),
        }, THE_HAND, OnHitPassive::ON_HIT_NO_PASSIVE, 64, util::makeBoolArray<MAX_NUM_SPECIAL_HP>({})); // The Hand

    HERO_TREE = { // (color, level) -> List of hero Indexes
        {
            ORANGE_HEROS_LVL_1_IDS,
            ORANGE_HEROS_LVL_2_IDS,
            ORANGE_HEROS_LVL_3_IDS,
        },
        {
            YELLOW_HEROS_LVL_1_IDS,
            YELLOW_HEROS_LVL_2_IDS,
            YELLOW_HEROS_LVL_3_IDS,
        },
        {
            GRAY_HEROS_LVL_1_IDS,
            GRAY_HEROS_LVL_2_IDS,
            GRAY_HEROS_LVL_3_IDS,
        },
        {
            RED_HEROS_LVL_1_IDS,
            RED_HEROS_LVL_2_IDS,
            RED_HEROS_LVL_3_IDS,
        },
        {
            BLUE_HEROS_LVL_1_IDS,
            BLUE_HEROS_LVL_2_IDS,
            BLUE_HEROS_LVL_3_IDS,
        }
    };

    MAGIC_HEROS = std::vector<int>(RED_HEROS_LVL_1_IDS.size() + RED_HEROS_LVL_2_IDS.size() + RED_HEROS_LVL_3_IDS.size() +
                       BLUE_HEROS_LVL_1_IDS.size() + BLUE_HEROS_LVL_2_IDS.size() + BLUE_HEROS_LVL_3_IDS.size() +
                       7);
    MAGIC_HEROS.insert(MAGIC_HEROS.end(), RED_HEROS_LVL_1_IDS.begin(), RED_HEROS_LVL_1_IDS.end());
    MAGIC_HEROS.insert(MAGIC_HEROS.end(), RED_HEROS_LVL_2_IDS.begin(), RED_HEROS_LVL_2_IDS.end());
    MAGIC_HEROS.insert(MAGIC_HEROS.end(), RED_HEROS_LVL_3_IDS.begin(), RED_HEROS_LVL_3_IDS.end());
    MAGIC_HEROS.insert(MAGIC_HEROS.end(), BLUE_HEROS_LVL_1_IDS.begin(), BLUE_HEROS_LVL_1_IDS.end());
    MAGIC_HEROS.insert(MAGIC_HEROS.end(), BLUE_HEROS_LVL_2_IDS.begin(), BLUE_HEROS_LVL_2_IDS.end());
    MAGIC_HEROS.insert(MAGIC_HEROS.end(), BLUE_HEROS_LVL_3_IDS.begin(), BLUE_HEROS_LVL_3_IDS.end());
    
    std::vector<int> nonRedBlueMagics = {
        DABBLE, // Orange
        SPELLBLADE, // Orange
        DABBLER, // Orange
        DABBLEST, // Orange
        CURATOR, // Yellow
        CLERIC, // Gray
        POET, // Gray
    };
    MAGIC_HEROS.insert(MAGIC_HEROS.end(), nonRedBlueMagics.begin(), nonRedBlueMagics.end());
}

}