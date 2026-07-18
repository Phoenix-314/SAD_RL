from dataclasses import dataclass
from enum import Enum
from bitarray import bitarray
from bitarray.util import int2ba
from typing import *
from SADenv.keywords import *
from SADenv.spells import *
from SADenv.monPassives import *
from SADenv.entIDs import *
from SADenv.ents import *

"""
Constant library of all ents in the game. Used for creating enemies/heros from scratch.
"""


def make_ent(hp, sides, sourceID, autoShield=0, autoPoison=0, autoRegen=0, autoArmor=0, onHitFunc=-1, specialHP=None, spell=-1, backrow=False, size=24):
    return Ent(hp, hp, autoShield, False, False, False, False, False, 0, 0, False,
                autoPoison, 0, 0, bitarray(6), False, False,
                autoRegen, 0, 0, 0, 0, autoArmor, backrow, size, False, False, False, False, False, None, 0, 0, 
                sides, 0, None, spell, onHitFunc, specialHP, sourceID, -1)

class SourceEnt:
    ORANGE_HEROS_LVL_1 = [
        make_ent(4, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], THIEF),
        make_ent(7, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.VULNERABLE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], SCOUNDREL),
        make_ent(3, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CRUEL])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED, KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CRUEL])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CRUEL])),
        ], LOST),
        make_ent(6, [
            Side(1, SideType.MANA, make_bitarray([])),
            Side(5, SideType.HEAL, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
        ], DABBLE),
        make_ent(6, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.SELF_DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.SELF_DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
        ], CLUMSY),
    ]
    ORANGE_HEROS_LVL_2 = [
        make_ent(8, [
            Side(2, SideType.MANA, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(4, SideType.HEAL, make_bitarray([])),
        ], DABBLER),
        make_ent(7, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], GAMBLER),
        make_ent(7, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED, KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED, KeywordIDs.ENGAGE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED, KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED, KeywordIDs.CLEAVE])),
        ], RANGER),
        make_ent(7, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CRUEL])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
        ], ROGUE),
        make_ent(8, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.VULNERABLE])),
            Side(4, SideType.KILL, make_bitarray([KeywordIDs.RANGED])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.VULNERABLE])),
        ], TRAPPER),
        make_ent(7, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.COPYCAT])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.MANAGAIN])),
        ], SPELLBLADE, spell=Spell.IMBUE),
        make_ent(6, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.COPYCAT])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.DOUBLEUSE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.COPYCAT])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.COPYCAT])),
        ], NINJA),
        make_ent(7, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.SELF_DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
        ], JUGGLER),
    ]
    ORANGE_HEROS_LVL_3 = [
        make_ent(7, [
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(6, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
        ], LUDUS),
        make_ent(9, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED, KeywordIDs.ENGAGE])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.CRUEL])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED, KeywordIDs.ENGAGE])),
        ], ASSASSIN),
        make_ent(8, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([KeywordIDs.RAMPAGE, KeywordIDs.PAIN])),
            Side(1, SideType.REROLL, make_bitarray([KeywordIDs.CANTRIP])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
        ], DANCER),
        make_ent(9, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.DOUBLEUSE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.PRISTINE])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.DOUBLEUSE])),
        ], FENCER),
        make_ent(8, [
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED])),
            Side(6, SideType.KILL, make_bitarray([KeywordIDs.RANGED])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED, KeywordIDs.COPYCAT])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED])),
        ], SHARPSHOT),
        make_ent(8, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED, KeywordIDs.POISON])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON, KeywordIDs.PLAGUE])),
            Side(3, SideType.HEAL, make_bitarray([KeywordIDs.CLEANSE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RANGED, KeywordIDs.POISON])),
        ], VENOM),
        make_ent(8, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.STICKY, KeywordIDs.DEATH, KeywordIDs.MANDATORY])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
        ], ROULETTE),
        make_ent(9, [
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.MANA, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.SHIELD, make_bitarray([])),
            Side(5, SideType.HEAL, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
        ], DABBLEST),
        make_ent(8, [
            Side(3, SideType.DAMAGE, make_bitarray([])), # SHIFTER isn't implemented, so ill just assume it averages to useless (ie, inflictNothing or similar)
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.INFLICT_PAIN, KeywordIDs.SINGLEUSE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.QUADUSE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.QUADUSE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
        ], AGENT),
    ]
    
    YELLOW_HEROS_LVL_1 = [
        make_ent(5, [
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.SHIELD, make_bitarray([])),
            Side(1, SideType.SHIELD, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
        ], FIGHTER),
        make_ent(5, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
        ], BRIGAND),
        make_ent(6, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.SHIELD, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], LAZY),
        make_ent(4, [
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
        ], RUFFIAN),
        make_ent(6, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.GUILT])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.DEATH])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SINGLEUSE])),
        ], HOARDER),
    ]
    YELLOW_HEROS_LVL_2 = [
        make_ent(8, [
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.DEATHWISH])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
        ], BERSERKER),
        make_ent(8, [
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(0, SideType.STUN, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
        ], BRUTE),
        make_ent(8, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.GROWTH])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.DEATHWISH])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.DUPLICATE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.FOCUS])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
        ], COLLECTOR),
        make_ent(7, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.ENGAGE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.ENGAGE])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
        ], GLADIATOR),
        make_ent(7, [
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
        ], SOLDIER),
        make_ent(8, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.CLEAVE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
        ], WHIRL),
        make_ent(7, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.STEEL])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.BLOODLUST])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.BLOODLUST])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.STEEL])),
        ], SCRAPPER),
        make_ent(8, [
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.CHAIN])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
        ], SINEW),
    ]
    YELLOW_HEROS_LVL_3 = [
        make_ent(10, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.BLOODLUST])),
            Side(10, SideType.DAMAGE, make_bitarray([KeywordIDs.DEATH])),
            Side(8, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.BLOODLUST])),
        ], BARBARIAN),
        make_ent(9, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RAMPAGE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.STEEL])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RAMPAGE])),
        ], BRAWLER),
        make_ent(9, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CHARGED])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.ENGAGE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.RAMPAGE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.STEEL])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.ERA])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.MANAGAIN])),
        ], CURATOR),
        make_ent(9, [
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(0, SideType.RECHARGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.DUPLICATE])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.SMITH])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
        ], LEADER),
        make_ent(11, [
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.SHIELD, make_bitarray([])),
            Side(3, SideType.SHIELD, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
        ], VETERAN),
        make_ent(10, [
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.STEEL])),
            Side(0, SideType.STUN, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
        ], BASH),
        make_ent(9, [
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], ECCENTRIC),
        make_ent(10, [
            Side(4, SideType.SHIELD, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.FOCUS])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.CHAIN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.CHAIN])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(4, SideType.SHIELD, make_bitarray([])),
        ], CAPTAIN, spell=Spell.FORMATION),
        make_ent(10, [
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.ERA])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.DEFY])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.COPYCAT])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.COPYCAT])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.ERA])),
        ], WANDERER),
    ]

    GRAY_HEROS_LVL_1 = [
        make_ent(7, [
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.SHIELD, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(1, SideType.SHIELD, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
        ], DEFENDER),
        make_ent(6, [
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.PRISTINE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([])),
        ], BUCKLE),
        make_ent(5, [
            Side(2, SideType.REDIRECTINCOMING, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.FOCUS])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.FOCUS])),
            Side(1, SideType.SHIELD, make_bitarray([])),
            Side(1, SideType.SHIELD, make_bitarray([])),
            Side(2, SideType.REDIRECTINCOMING, make_bitarray([KeywordIDs.SELFSHIELD])),
        ], SQUIRE),
        make_ent(4, [
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.COPYCAT])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.REPEL])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.CLEANSE])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.HEALSHIELD, make_bitarray([])),
        ], ALLOY),
        make_ent(5, [
            Side(0, SideType.STUN, make_bitarray([])),
            Side(0, SideType.UNDYING, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.STUN, make_bitarray([])),
        ], WALLOP),
    ]

    GRAY_HEROS_LVL_2 = [
        make_ent(7, [
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
            Side(3, SideType.SHIELD, make_bitarray([KeywordIDs.STEEL])),
            Side(3, SideType.SHIELD, make_bitarray([KeywordIDs.STEEL])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
        ], KNIGHT, spell=Spell.PARRY),
        make_ent(9, [
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.CLEAVE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(3, SideType.SHIELD, make_bitarray([])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.SMITH])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.SMITH])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.CLEAVE])),
        ], ARMORER),
        make_ent(6, [
            Side(1, SideType.SHIELD_ALL, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.REROLL, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.SHIELD_ALL, make_bitarray([])),
        ], BARD, spell=Spell.ELSE),
        make_ent(8, [
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(3, SideType.HEALSHIELD, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.CLEANSE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
        ], CLERIC, spell=Spell.LIGHT),
        make_ent(8, [
            Side(3, SideType.SHIELD, make_bitarray([KeywordIDs.ENGAGE])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.SHIELD, make_bitarray([KeywordIDs.ENGAGE])),
        ], GUARDIAN),
        make_ent(8, [
            Side(0, SideType.ENCHANT, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(0, SideType.RECHARGE, make_bitarray([])),
            Side(0, SideType.UNDYING, make_bitarray([])),
            Side(0, SideType.STUN, make_bitarray([])),
            Side(0, SideType.STUN, make_bitarray([])),
            Side(0, SideType.ENCHANT, make_bitarray([KeywordIDs.SELFSHIELD])),
        ], PILGRIM),
        make_ent(9, [
            Side(3, SideType.REDIRECTINCOMING, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.CLEANSE])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.REPEL])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.REPEL])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.REDIRECTINCOMING, make_bitarray([KeywordIDs.SELFSHIELD])),
        ], MONK),
        make_ent(10, [
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.SHIELD, make_bitarray([])),
            Side(3, SideType.SHIELD, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([])),
            Side(1, SideType.SHIELD, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
        ], WARDEN),
    ]
    GRAY_HEROS_LVL_3 = [
        make_ent(10, [
            Side(5, SideType.SHIELD, make_bitarray([KeywordIDs.STEEL])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.REPEL])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.REPEL])),
            Side(0, SideType.ENCHANT, make_bitarray([KeywordIDs.SELFSHIELD, KeywordIDs.SELFHEAL])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(5, SideType.SHIELD, make_bitarray([KeywordIDs.STEEL])),
        ], KEEPER),
        make_ent(11, [
            Side(3, SideType.HEALSHIELD, make_bitarray([])),
            Side(4, SideType.SHIELD, make_bitarray([KeywordIDs.CLEANSE])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(3, SideType.HEALSHIELD, make_bitarray([])),
        ], PALADIN),
        make_ent(9, [
            Side(3, SideType.HEALSHIELD, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.INSPIRED])),
            Side(3, SideType.SHIELD, make_bitarray([KeywordIDs.DUPLICATE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.HEALSHIELD, make_bitarray([])),
        ], PRINCE, spell=Spell.UNITE),
        make_ent(11, [
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.CLEAVE])),
            Side(3, SideType.SHIELD, make_bitarray([KeywordIDs.CLEANSE])),
            Side(3, SideType.SHIELD, make_bitarray([KeywordIDs.CLEANSE])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.DOUBLEUSE])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.CLEAVE])),
        ], STALWART),
        make_ent(8, [
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.CHARGED])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.CANTRIP])),
            Side(2, SideType.SHIELD_ALL, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.CANTRIP])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.CHARGED])),
        ], POET, spell=Spell.CLINK),
        make_ent(9, [
            Side(0, SideType.UNDYING, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.DEATHWISH])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.RESCUE])),
            Side(2, SideType.RESURRECT, make_bitarray([])),
            Side(1, SideType.RESURRECT, make_bitarray([])),
            Side(0, SideType.UNDYING, make_bitarray([])),
        ], VALKYRIE),
        make_ent(15, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(0, SideType.STUN, make_bitarray([])),
            Side(3, SideType.REDIRECTINCOMING, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(2, SideType.REDIRECTINCOMING, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(1, SideType.REDIRECTINCOMING, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
        ], STOIC),
    ]
    
    RED_HEROS_LVL_1 = [
        make_ent(5, [
            Side(4, SideType.HEAL, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(2, SideType.HEAL, make_bitarray([])),
            Side(2, SideType.HEAL, make_bitarray([])),
            Side(4, SideType.HEAL, make_bitarray([])),
        ], HEALER, spell=Spell.MEND),
        make_ent(5, [
            Side(2, SideType.HEAL, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.GROOOOWTH])),
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.HEAL, make_bitarray([])),
        ], GARDENER, spell=Spell.HEMLOCK),
        make_ent(5, [
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.CLEANSE])),
            Side(3, SideType.HEAL, make_bitarray([KeywordIDs.VITALITY])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.CLEANSE])),
        ], ACOLYTE, spell=Spell.RESTORE),
        make_ent(4, [
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.HEALSHIELD, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
        ], MYSTIC, spell=Spell.GAZE),
        make_ent(4, [
            Side(5, SideType.HEAL, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(5, SideType.HEAL, make_bitarray([KeywordIDs.SINGLEUSE])),
        ], SPLINT, spell=Spell.BANDAGE),
    ]
    RED_HEROS_LVL_2 = [
        make_ent(7, [
            Side(2, SideType.MANA, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.GROWTH])),
            Side(2, SideType.SHIELD, make_bitarray([KeywordIDs.GROWTH])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.CLEANSE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
        ], DRUID, spell=Spell.BALANCE),
        make_ent(7, [
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.REGEN])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON, KeywordIDs.SINGLEUSE])),
            Side(0, SideType.ENCHANT, make_bitarray([KeywordIDs.GROWTH])),
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.REGEN])),
        ], HERBALIST, spell=Spell.VINE),
        make_ent(7, [
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.REGEN])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.CLEAVE])),
        ], MEDIC, spell=Spell.RENEW),
        make_ent(5, [
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(4, SideType.HEAL, make_bitarray([KeywordIDs.VITALITY])),
            Side(4, SideType.HEAL, make_bitarray([KeywordIDs.VITALITY])),
            Side(1, SideType.HEAL_ALL, make_bitarray([])),
            Side(1, SideType.HEAL_ALL, make_bitarray([])),
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
        ], PRIESTESS, spell=Spell.PRAY),
        make_ent(7, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFHEAL])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(5, SideType.HEAL, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFHEAL])),
        ], VAMPIRE, spell=Spell.INFUSE),
        make_ent(7, [
            Side(0, SideType.ENCHANT, make_bitarray([KeywordIDs.SELFSHIELD, KeywordIDs.SELFHEAL])),
            Side(2, SideType.HEALSHIELD, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(0, SideType.ENCHANT, make_bitarray([KeywordIDs.ENGAGE])),
            Side(0, SideType.ENCHANT, make_bitarray([KeywordIDs.SELFHEAL])),
            Side(0, SideType.ENCHANT, make_bitarray([KeywordIDs.SELFHEAL])),
            Side(0, SideType.ENCHANT, make_bitarray([KeywordIDs.SELFSHIELD, KeywordIDs.SELFHEAL])),
        ], ENCHANTER, spell=Spell.FLIP),
        make_ent(8, [
            Side(2, SideType.MANA, make_bitarray([])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.DOUBLEUSE])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.DOUBLEUSE])),
            Side(1, SideType.RESURRECT, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
        ], DISCIPLE, spell=Spell.GLOW),
        make_ent(6, [
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.BOOST])),
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.BOOST])),
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
        ], FEY, spell=Spell.CIRCLE),
    ]
    RED_HEROS_LVL_3 = [
        make_ent(9, [
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.REGEN])),
            Side(1, SideType.DAMAGE_EVERYONE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.REGEN])),
        ], DOCTOR, spell=Spell.LIQUOR),
        make_ent(9, [
            Side(2, SideType.HEAL_ALL, make_bitarray([])),
            Side(3, SideType.RESURRECT, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFHEAL])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFHEAL])),
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
            Side(2, SideType.HEAL_ALL, make_bitarray([])),
        ], FORSAKEN, spell=Spell.BIND),
        make_ent(8, [
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(3, SideType.HEAL, make_bitarray([KeywordIDs.RESCUE])),
            Side(5, SideType.HEAL, make_bitarray([KeywordIDs.VITALITY])),
            Side(5, SideType.HEAL, make_bitarray([KeywordIDs.VITALITY])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
        ], PROPHET, spell=Spell.SOOTHE),
        make_ent(11, [
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.GROWTH])),
            Side(5, SideType.HEAL, make_bitarray([KeywordIDs.VITALITY])),
            Side(10, SideType.HEAL, make_bitarray([])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.SHIELD, make_bitarray([KeywordIDs.GROWTH])),
        ], SHAMAN, spell=Spell.RITUAL),
        make_ent(10, [
            Side(1, SideType.HEAL, make_bitarray([KeywordIDs.BOOST])),
            Side(3, SideType.MANA, make_bitarray([])),
            Side(5, SideType.HEAL, make_bitarray([KeywordIDs.CLEANSE])),
            Side(3, SideType.HEAL, make_bitarray([KeywordIDs.CLEAVE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
        ], WITCH, spell=Spell.SALVE),
        make_ent(9, [
            Side(2, SideType.MANA, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFHEAL])),
            Side(0, SideType.ENCHANT, make_bitarray([KeywordIDs.SELFHEAL, KeywordIDs.SELFSHIELD])),
            Side(0, SideType.DODGE, make_bitarray([KeywordIDs.CANTRIP])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
        ], WRAITH, spell=Spell.LEECH),
        make_ent(9, [
            Side(3, SideType.HEALSHIELD, make_bitarray([])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(2, SideType.HEAL, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(3, SideType.HEAL, make_bitarray([KeywordIDs.DOUBLEUSE])),
            Side(3, SideType.HEAL, make_bitarray([KeywordIDs.DOUBLEUSE])),
            Side(3, SideType.HEALSHIELD, make_bitarray([])),
        ], SURGEON, spell=Spell.OPERATE),
        make_ent(7, [
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(2, SideType.HEALSHIELD, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(2, SideType.HEALSHIELD, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
        ], FATE, spell=Spell.STRAND),
    ]
    
    BLUE_HEROS_LVL_1 = [
        make_ent(4, [
            Side(1, SideType.MANA, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
        ], MAGE, spell=Spell.POKE),
        make_ent(4, [
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.PAIR])),
            Side(1, SideType.RESURRECT, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(2, SideType.SELF_DAMAGE, make_bitarray([KeywordIDs.MANDATORY])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.PAIR])),
        ], PRODIGY, spell=Spell.SCORCH),
        make_ent(6, [
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
            Side(3, SideType.MANA, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.CANTRIP, KeywordIDs.BONED])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.CANTRIP, KeywordIDs.BONED])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
        ], MEDDLER, spell=Spell.SLAY),
        make_ent(5, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.RECHARGE, make_bitarray([])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], STUDENT, spell=Spell.SLICE),
        make_ent(4, [
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.GROWTH])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
        ], INITIATE, spell=Spell.GATHER),
        make_ent(5, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFHEAL, KeywordIDs.SINGLEUSE])),
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFHEAL, KeywordIDs.SINGLEUSE])),
        ], CULTIST, spell=Spell.CUT),
    ]
    BLUE_HEROS_LVL_2 = [
        make_ent(6, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CRUEL, KeywordIDs.SINGLEUSE])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.MANAGAIN])),
        ], CALDERA, spell=Spell.SCALD),
        make_ent(10, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.MANA, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], EVOKER, spell=Spell.DROP),
        make_ent(9, [
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN, KeywordIDs.SINGLEUSE])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.MANAGAIN])),
        ], GLACIA, spell=Spell.CHILL),
        make_ent(7, [
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(3, SideType.MANA, make_bitarray([])),
            Side(1, SideType.REROLL, make_bitarray([KeywordIDs.CANTRIP])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
        ], JESTER, spell=Spell.FLICK),
        make_ent(7, [
            Side(0, SideType.DAMAGE, make_bitarray([KeywordIDs.CHARGED])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.DAMAGE, make_bitarray([KeywordIDs.CHARGED])),
        ], SPARKY, spell=Spell.ZAP),
        make_ent(8, [
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.GROWTH])),
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.DECAY])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.DECAY])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.DECAY])),
            Side(1, SideType.RESURRECT, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.GROWTH])),
        ], MYCO, spell=Spell.SPORE),
        make_ent(7, [
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.GROWTH])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON, KeywordIDs.SINGLEUSE])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
        ], SEER, spell=Spell.FORETELL),
        make_ent(6, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFHEAL, KeywordIDs.SINGLEUSE])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.CANTRIP])),
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFHEAL, KeywordIDs.SINGLEUSE])),
        ], FIEND, spell=Spell.BURN),
    ]
    BLUE_HEROS_LVL_3 = [
        make_ent(8, [
            Side(4, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.SINGLEUSE, KeywordIDs.SELFHEAL])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SINGLEUSE, KeywordIDs.CHARGED])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SINGLEUSE, KeywordIDs.POISON])),
            Side(10, SideType.HEAL, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(4, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
        ], ARTIFICER, spell=Spell.BLADES),
        make_ent(10, [
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
            Side(0, SideType.MANA, make_bitarray([KeywordIDs.CHARGED])),
            Side(3, SideType.MANA, make_bitarray([])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.SINGLEUSE])),
        ], WEAVER, spell=Spell.CRUSH),
        make_ent(7, [
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.REROLL, make_bitarray([KeywordIDs.CANTRIP])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.CANTRIP])),
        ], SORCERER, spell=Spell.MIASMA),
        make_ent(7, [
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.GROWTH])),
            Side(0, SideType.RECHARGE, make_bitarray([])),
            Side(1, SideType.REROLL, make_bitarray([KeywordIDs.CANTRIP])),
            Side(0, SideType.DODGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.GROWTH])),
        ], CHRONOS, spell=Spell.TICK),
        make_ent(7, [
            Side(4, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.BLOODLUST])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.SINGLEUSE, KeywordIDs.SELFHEAL])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(4, SideType.MANA, make_bitarray([KeywordIDs.PAIN])),
        ], WARLOCK, spell=Spell.BLAZE),
        make_ent(8, [
            Side(1, SideType.MANA, make_bitarray([])),
            Side(3, SideType.MANA, make_bitarray([KeywordIDs.TRIO])),
            Side(2, SideType.MANA, make_bitarray([KeywordIDs.PAIR])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
        ], ACE, spell=Spell.DRAW),
        make_ent(8, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SINGLEUSE, KeywordIDs.WEAKEN])),
            Side(5, SideType.MANA, make_bitarray([KeywordIDs.DEATH])),
            Side(2, SideType.MANA, make_bitarray([])),
            Side(1, SideType.MANA, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SINGLEUSE, KeywordIDs.WEAKEN])),
        ], GHAST, spell=Spell.HARVEST),
        make_ent(9, [
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.SINGLEUSE, KeywordIDs.PERMABOOST])),
            Side(1, SideType.MANA, make_bitarray([KeywordIDs.CANTRIP])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.MANAGAIN])),
            Side(1, SideType.SHIELD, make_bitarray([KeywordIDs.MANAGAIN])),
        ], WIZARD, spell=Spell.INSPIRE),
    ]

    # Monsters: small / reg / big / huge
    MONSTERS_SMALL = [
        make_ent(4, [
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
        ], BONES, size=16), # Bones
        make_ent(3, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], LOG, size=16), # Log - Does not roll away, this is fine
        make_ent(2, [
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
        ], ARCHER, backrow=True, size=16), # Archer
        make_ent(3, [
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
        ], SNIPER, backrow=True, size=16), # Sniper
        make_ent(5, [
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
        ], SHADE, onHitFunc=OnHitPassive.ONHIT_SHADE, specialHP=bitarray('0 11111'), size=16), # Shade
        make_ent(3, [
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
        ], RAT, size=16), # Rat
        make_ent(5, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.INFLICT_PAIN])),
            Side(3, SideType.HEAL, make_bitarray([KeywordIDs.VITALITY])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.INFLICT_PAIN])),
            Side(1, SideType.SUMMON, int2ba(-BONES)),
            Side(1, SideType.SUMMON, int2ba(-BONES)),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.INFLICT_PAIN])),
        ], WISP, onHitFunc=OnHitPassive.ONHIT_WISP, specialHP=bitarray('0 00100 00000 00000 00000 00000 00000 0'), size=16), # Wisp
        make_ent(2, [
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
        ], SLIMELET, size=16), # Slimelet
        make_ent(4, [
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(8, SideType.DAMAGE, make_bitarray([KeywordIDs.DEATH])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
        ], IMP, onHitFunc=OnHitPassive.ONHIT_IMP, size=16), # Imp
        make_ent(4, [
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
        ], SPIDER, size=16), # Spider
        make_ent(2, [
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.DEATH])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.DEATH])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
        ], BEE, size=16), # Bee
        make_ent(3, [
            Side(1, SideType.SUMMON, int2ba(-BONES)),
            Side(1, SideType.SUMMON, int2ba(-BONES)),
            Side(1, SideType.SUMMON, int2ba(-BONES)),
            Side(1, SideType.SUMMON, int2ba(-BONES)),
            Side(1, SideType.SUMMON, int2ba(-BONES)),
            Side(1, SideType.SUMMON, int2ba(-BONES)),
        ], GRAVE, onHitFunc=OnHitPassive.ONHIT_GRAVE, specialHP=bitarray('0 111'), size=16), # Grave
        make_ent(1, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
        ], ILLUSION, size=16), # Illusion
        make_ent(5, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.SUMMON, int2ba(-SLIMELET)),
            Side(1, SideType.SUMMON, int2ba(-SLIMELET)),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
        ], CHEST, onHitFunc=OnHitPassive.ONHIT_CHEST, specialHP=bitarray('0 00001'), size=16), # Chest (summon slimelet etc.)
        make_ent(2, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
        ], THORN, onHitFunc=OnHitPassive.ONHIT_THORN, size=16), # Thorn
        make_ent(4, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.SUMMON, int2ba(-CAW)),
            Side(1, SideType.SUMMON, int2ba(-CAW)),
            Side(1, SideType.SUMMON, int2ba(-CAW)),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], CAW_EGG, size=16), # Caw egg
        make_ent(1, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.SUMMON, int2ba(-THORN)),
            Side(1, SideType.SUMMON, int2ba(-THORN)),
            Side(1, SideType.SUMMON, int2ba(-THORN)),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], SEED, size=16), # Seed
    ]

    MONSTERS_REG = [
        make_ent(5, [
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
        ], GOBLIN, size=24), # Goblin
        make_ent(3, [
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
        ], GNOLL, autoArmor=1, size=24), # Gnoll
        make_ent(10, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
        ], ZOMBIE, onHitFunc=OnHitPassive.ONHIT_ZOMBIE, size=24), # Zombie
        make_ent(6, [
            Side(3, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
        ], WOLF, size=24), # Wolf
        make_ent(5, [
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
        ], SNAKE, size=24), # Snake
        make_ent(7, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.INFLICT_SINGLEUSE])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.INFLICT_SINGLEUSE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
        ], QUARTZ, onHitFunc=OnHitPassive.ONHIT_QUARTZ, size=24), # Quartz
        make_ent(7, [
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(6, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([])),
        ], SUDUL, size=24), # Sudul
        make_ent(13, [
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(8, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(8, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.PAIN])),
        ], FANATIC, size=24), # Fanatic
        make_ent(10, [
            Side(8, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
            Side(12, SideType.DAMAGE, make_bitarray([KeywordIDs.DEATH])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(8, SideType.DAMAGE, make_bitarray([KeywordIDs.EXERT])),
        ], SABER, size=24), # Saber
        make_ent(7, [
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
        ], MILITIA, size=24), # Militia
        make_ent(10, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(1, SideType.DAMAGE_EVERYONE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
        ], CARRIER, autoPoison=2, size=24), # Carrier (last side was poisonAll)
        make_ent(8, [
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(6, SideType.DAMAGE, make_bitarray([])),
            Side(6, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
        ], BANDIT, size=24), # Bandit
        make_ent(6, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], BARREL, size=24), # Barrel
        make_ent(2, [
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(0, SideType.DAMAGE, make_bitarray([KeywordIDs.STEEL])),
            Side(0, SideType.DAMAGE, make_bitarray([KeywordIDs.STEEL])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.SELFSHIELD])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
        ], GOLEM, autoShield=8, size=24), # Golem
        make_ent(6, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], FOUNTAIN, onHitFunc=OnHitPassive.ONHIT_FOUNTAIN, specialHP=bitarray('0 11111 1'), size=24), # Fountain (blank)
        make_ent(6, [
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
            Side(0, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(0, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([])),
        ], WARCHIEF, size=24), # Warchief
        make_ent(5, [
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
        ], BLIND, size=24), # Blind
        make_ent(6, [
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(1, SideType.SUMMON, int2ba(-DRAGON)),
            Side(1, SideType.SUMMON, int2ba(-DRAGON)),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
            Side(0, SideType.BLANK, make_bitarray([])),
        ], DRAGON_EGG, size=24), # Caw egg
    ]

    MONSTERS_BIG = [
        make_ent(5, [
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(3, SideType.HEAL_ALL, make_bitarray([])),
            Side(2, SideType.SUMMON, int2ba(-BONES)),
            Side(2, SideType.SUMMON, int2ba(-BONES)),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
        ], WIZZ, onHitFunc=OnHitPassive.ONHIT_WIZZ, specialHP=bitarray('0 00010'), backrow=True, size=30), # Wizz (approx)
        make_ent(7, [
            Side(3, SideType.DAMAGE_FLANKING, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(3, SideType.DAMAGE_FLANKING, make_bitarray([])),
        ], SLIMER, onHitFunc=OnHitPassive.ONHIT_SLIMER, specialHP=bitarray('0 00001 00'), size=30), # Slimer
        make_ent(13, [
            Side(6, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.SUMMON, int2ba(-WOLF)),
            Side(1, SideType.SUMMON, int2ba(-WOLF)),
            Side(6, SideType.DAMAGE, make_bitarray([])),
        ], ALPHA, size=30), # Alpha
        make_ent(10, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
        ], OGRE, onHitFunc=OnHitPassive.ONHIT_OGRE, specialHP=bitarray('0 00100 00100'), size=30), # Ogre
        make_ent(12, [
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.INFLICT_PAIN])),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.INFLICT_PAIN])),
            Side(1, SideType.SUMMON, int2ba(-IMP)),
            Side(1, SideType.SUMMON, int2ba(-IMP)),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
        ], DEMON, onHitFunc=OnHitPassive.ONHIT_DEMON, specialHP=bitarray('0 10000 00000 10'), size=30), # Demon
        make_ent(10, [
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
        ], CHOMP, onHitFunc=OnHitPassive.ONHIT_CHOMP, specialHP=bitarray('0 11111 11111'), size=30), # Chomp
        make_ent(6, [
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND])),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.ELIMINATE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND])),
        ], GHOST, onHitFunc=OnHitPassive.ONHIT_GHOST, specialHP=bitarray('0 00001 0'), size=30), # Ghost
        make_ent(7, [
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE_FLANKING, make_bitarray([])),
            Side(2, SideType.DAMAGE_FLANKING, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([])),
        ], BOAR, onHitFunc=OnHitPassive.ONHIT_BOAR, specialHP=bitarray('0 10000 00'), size=30), # Boar
        make_ent(13, [
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
        ], SPIKER, onHitFunc=OnHitPassive.ONHIT_SPIKER, size=30), # Spiker
        make_ent(5, [
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
        ], SLATE, onHitFunc=OnHitPassive.ONHIT_SLATE, specialHP=bitarray('0 11111'), size=30), # Slate
        make_ent(15, [
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
        ], TROLL, autoRegen=1, size=30),
        make_ent(7, [
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(7, SideType.DAMAGE, make_bitarray([])),
        ], CAW, onHitFunc=OnHitPassive.ONHIT_CAW, size=30),
        make_ent(7, [
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND])),
        ], MAGRAT, onHitFunc=OnHitPassive.ONHIT_MAGRAT, specialHP=bitarray('0 00010 00'), size=30),
        make_ent(7, [
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN, KeywordIDs.CLEAVE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN, KeywordIDs.CLEAVE])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
        ], GYTHA, onHitFunc=OnHitPassive.ONHIT_GYTHA, specialHP=bitarray('0 00010 00'), size=30),
        make_ent(7, [
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(1, SideType.SUMMON, int2ba(-WOLF)),
            Side(1, SideType.SUMMON, int2ba(-WOLF)),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([])),
        ], AGNES, onHitFunc=OnHitPassive.ONHIT_AGNES, specialHP=bitarray('0 00010 00'), size=30),
        make_ent(12, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN, KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
        ], BASILISK, onHitFunc=OnHitPassive.ONHIT_BASILISK, size=30),
        make_ent(20, [
            Side(10, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN, KeywordIDs.CLEAVE])),
            Side(10, SideType.DAMAGE, make_bitarray([])),
        ], HYDRA, onHitFunc=OnHitPassive.ONHIT_HYDRA, size=30),
        make_ent(15, [
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(3, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(3, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
        ], CYCLOPS, onHitFunc=OnHitPassive.ONHIT_CYCLOPS, specialHP=bitarray('0 00000 00100 00000'), size=30),
        make_ent(11, [
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
        ], BRAMBLE, size=30),
        make_ent(10, [
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.POISON])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN])),
        ], BANSHEE, size=30),
    ]

    MONSTERS_HUGE = [
        make_ent(13, [
            Side(5, SideType.DAMAGE_FLANKING, make_bitarray([])),
            Side(9, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(5, SideType.DAMAGE_FLANKING, make_bitarray([])),
        ], SLIME_QUEEN, onHitFunc=OnHitPassive.ONHIT_SLIME_QUEEN,specialHP=bitarray('0 00001 00001 000'), size=64), # Slime Queen
        make_ent(20, [
            Side(2, SideType.SUMMON, int2ba(-BONES)),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([KeywordIDs.WEAKEN])),
            Side(1, SideType.DAMAGE_ALL, make_bitarray([KeywordIDs.WEAKEN])),
            Side(2, SideType.SUMMON, int2ba(-BONES)),
        ], LICH, onHitFunc=OnHitPassive.ONHIT_LICH, specialHP=bitarray('0 00001 00001 00001 00001'), backrow=True, size=64), # Lich
        make_ent(20, [
            Side(3, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.POISON])),
            Side(3, SideType.DAMAGE_ALL, make_bitarray([])),
        ], TROLL_KING, autoRegen=2, size=64), # Troll King
        make_ent(35, [
            Side(9, SideType.DAMAGE, make_bitarray([KeywordIDs.INFLICT_DEATH])),
            Side(4, SideType.DAMAGE_FLANKING, make_bitarray([KeywordIDs.WEAKEN])),
            Side(4, SideType.DAMAGE_FLANKING, make_bitarray([KeywordIDs.WEAKEN])),
            Side(1, SideType.SUMMON, int2ba(-SLATE)),
            Side(1, SideType.SUMMON, int2ba(-SLATE)),
            Side(9, SideType.DAMAGE, make_bitarray([KeywordIDs.INFLICT_DEATH])),
        ], BASALT, onHitFunc=OnHitPassive.ONHIT_BASALT, specialHP=int2ba(1), size=64), # Basalt
        make_ent(25, [
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN, KeywordIDs.POISON])),
            Side(12, SideType.DAMAGE, make_bitarray([])),
            Side(12, SideType.DAMAGE, make_bitarray([])),
            Side(3, SideType.SUMMON, int2ba(-SPIDER)),
            Side(2, SideType.SUMMON, int2ba(-SPIDER)),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.WEAKEN, KeywordIDs.POISON])),
        ], TARANTUS, onHitFunc=OnHitPassive.ONHIT_TARANTUS, specialHP=bitarray('0 00000 00001'), size=64), # Tarantus
        make_ent(23, [
            Side(1, SideType.SUMMON, int2ba(-IMP)),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.POISON])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.POISON])),
            Side(1, SideType.SUMMON, int2ba(-IMP)),
        ], ROTTEN, size=64), # Rotten
        make_ent(30, [
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND, KeywordIDs.INFLICT_PAIN])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.DESCEND, KeywordIDs.INFLICT_PAIN])),
            Side(1, SideType.SUMMON, int2ba(-DEMON)),
            Side(1, SideType.SUMMON, int2ba(-DEMON)),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.PETRIFY])),
        ], HEXIA, onHitFunc=OnHitPassive.ONHIT_HEXIA, size=64), # Hexia
        make_ent(40, [
            Side(15, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(5, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(5, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.POISON])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.POISON])),
            Side(15, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
        ], DRAGON, size=64), # Dragon
        make_ent(25, [
            Side(2, SideType.SUMMON, int2ba(-BONES)),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.INFLICT_EXERT])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.INFLICT_EXERT])),
            Side(2, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.INFLICT_EXERT])),
            Side(1, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.POISON])),
            Side(2, SideType.SUMMON, int2ba(-BONES)),
        ], BARON, onHitFunc=OnHitPassive.ONHIT_BARON, specialHP=bitarray('0 01010 10101 01010 10101 01010'), size=64), # Baron
        make_ent(15, [
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(2, SideType.SUMMON, int2ba(-SPIDER)),
            Side(2, SideType.SUMMON, int2ba(-BONES)),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
        ], SARCOPHAGUS, onHitFunc=OnHitPassive.ONHIT_SARCOPHAGUS, specialHP=bitarray('0 00000 00000 00111'), size=64), # Sarcophagus
        make_ent(21, [
            Side(2, SideType.DAMAGE_ALL, make_bitarray([KeywordIDs.EXERT])),
            Side(3, SideType.DAMAGE_ALL, make_bitarray([KeywordIDs.EXERT])),
            Side(3, SideType.DAMAGE_ALL, make_bitarray([KeywordIDs.EXERT])),
            Side(1, SideType.SUMMON, int2ba(-IMP)),
            Side(1, SideType.SUMMON, int2ba(-IMP)),
            Side(2, SideType.DAMAGE_ALL, make_bitarray([KeywordIDs.EXERT])),
        ], BELL, size=64), # Bell
        make_ent(14, [
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.BLOODLUST, KeywordIDs.ELIMINATE])),
            Side(5, SideType.DAMAGE, make_bitarray([KeywordIDs.BLOODLUST, KeywordIDs.ELIMINATE])),
            Side(7, SideType.DAMAGE, make_bitarray([KeywordIDs.HEAVY])),
        ], MADNESS, size=64), # Madness
        make_ent(30, [
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.ERA, KeywordIDs.CLEAVE, KeywordIDs.INFLICT_EXERT])),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.ERA, KeywordIDs.HEAVY, KeywordIDs.SELFHEAL])),
            Side(6, SideType.DAMAGE, make_bitarray([KeywordIDs.ERA, KeywordIDs.HEAVY, KeywordIDs.SELFHEAL])),
            Side(3, SideType.DAMAGE_FLANKING, make_bitarray([KeywordIDs.ERA, KeywordIDs.WEAKEN])),
            Side(3, SideType.DAMAGE_FLANKING, make_bitarray([KeywordIDs.ERA, KeywordIDs.WEAKEN])),
            Side(3, SideType.DAMAGE, make_bitarray([KeywordIDs.ERA, KeywordIDs.CLEAVE, KeywordIDs.INFLICT_EXERT])),
        ], INEVITABLE, onHitFunc=OnHitPassive.ONHIT_INEVITABLE, specialHP=bitarray('0 00100 00100 00100 00100 00100 00100'), size=64), # Inevitable
        make_ent(45, [
            Side(3, SideType.SUMMON, int2ba(-SABER)),
            Side(999, SideType.KILL, make_bitarray([])),
            Side(10, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE])),
            Side(7, SideType.DAMAGE_ALL, make_bitarray([])),
            Side(4, SideType.DAMAGE, make_bitarray([KeywordIDs.CLEAVE, KeywordIDs.POISON])),
            Side(3, SideType.SUMMON, int2ba(-SABER)),
        ], THE_HAND, size=64), # The Hand
    ]

Lib.ALL_MONSTERS = [None for _ in range(len(SourceEnt.MONSTERS_SMALL) + len(SourceEnt.MONSTERS_REG) + len(SourceEnt.MONSTERS_BIG) + len(SourceEnt.MONSTERS_HUGE))]
for mon in SourceEnt.MONSTERS_SMALL:
    Lib.ALL_MONSTERS[monIDtoPos(mon.sourceID)] = mon
for mon in SourceEnt.MONSTERS_REG:
    Lib.ALL_MONSTERS[monIDtoPos(mon.sourceID)] = mon
for mon in SourceEnt.MONSTERS_BIG:
    Lib.ALL_MONSTERS[monIDtoPos(mon.sourceID)] = mon
for mon in SourceEnt.MONSTERS_HUGE:
    Lib.ALL_MONSTERS[monIDtoPos(mon.sourceID)] = mon

Lib.ALL_HEROS = [None for _ in range(len(SourceEnt.ORANGE_HEROS_LVL_1) + len(SourceEnt.ORANGE_HEROS_LVL_2) + len(SourceEnt.ORANGE_HEROS_LVL_3) + len(SourceEnt.YELLOW_HEROS_LVL_1) + len(SourceEnt.YELLOW_HEROS_LVL_2) + len(SourceEnt.YELLOW_HEROS_LVL_3) + len(SourceEnt.GRAY_HEROS_LVL_1) + len(SourceEnt.GRAY_HEROS_LVL_2) + len(SourceEnt.GRAY_HEROS_LVL_3) + len(SourceEnt.RED_HEROS_LVL_1) + len(SourceEnt.RED_HEROS_LVL_2) + len(SourceEnt.RED_HEROS_LVL_3) + len(SourceEnt.BLUE_HEROS_LVL_1) + len(SourceEnt.BLUE_HEROS_LVL_2) + len(SourceEnt.BLUE_HEROS_LVL_3))]
for hero in SourceEnt.ORANGE_HEROS_LVL_1:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.ORANGE_HEROS_LVL_2:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.ORANGE_HEROS_LVL_3:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.YELLOW_HEROS_LVL_1:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.YELLOW_HEROS_LVL_2:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.YELLOW_HEROS_LVL_3:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.GRAY_HEROS_LVL_1:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.GRAY_HEROS_LVL_2:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.GRAY_HEROS_LVL_3:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.RED_HEROS_LVL_1:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.RED_HEROS_LVL_2:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.RED_HEROS_LVL_3:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.BLUE_HEROS_LVL_1:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.BLUE_HEROS_LVL_2:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero
for hero in SourceEnt.BLUE_HEROS_LVL_3:
    Lib.ALL_HEROS[heroIDtoPos(hero.sourceID)] = hero

Lib.HERO_TREE = [ # (color, level) -> List of hero Indexes
    [
        ORANGE_HEROS_LVL_1_IDS,
        ORANGE_HEROS_LVL_2_IDS,
        ORANGE_HEROS_LVL_3_IDS,
    ],
    [
        YELLOW_HEROS_LVL_1_IDS,
        YELLOW_HEROS_LVL_2_IDS,
        YELLOW_HEROS_LVL_3_IDS,
    ],
    [
        GRAY_HEROS_LVL_1_IDS,
        GRAY_HEROS_LVL_2_IDS,
        GRAY_HEROS_LVL_3_IDS,
    ],
    [
        RED_HEROS_LVL_1_IDS,
        RED_HEROS_LVL_2_IDS,
        RED_HEROS_LVL_3_IDS,
    ],
    [
        BLUE_HEROS_LVL_1_IDS,
        BLUE_HEROS_LVL_2_IDS,
        BLUE_HEROS_LVL_3_IDS,
    ]
]

Lib.MAGIC_HEROS = []
Lib.MAGIC_HEROS.extend(RED_HEROS_LVL_1_IDS) # All red, blue heros are magical
Lib.MAGIC_HEROS.extend(RED_HEROS_LVL_2_IDS)
Lib.MAGIC_HEROS.extend(RED_HEROS_LVL_3_IDS)
Lib.MAGIC_HEROS.extend(BLUE_HEROS_LVL_1_IDS)
Lib.MAGIC_HEROS.extend(BLUE_HEROS_LVL_2_IDS)
Lib.MAGIC_HEROS.extend(BLUE_HEROS_LVL_3_IDS)
# Additional magical heros:
nonRedBlueMagics = [
    DABBLE, # Orange
    SPELLBLADE, # Orange
    DABBLER, # Orange
    DABBLEST, # Orange
    CURATOR, # Yellow
    CLERIC, # Gray
    POET, # Gray
]
Lib.MAGIC_HEROS.extend(nonRedBlueMagics)

