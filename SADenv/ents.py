from dataclasses import dataclass
from typing import Callable, List, Tuple

from bitarray import bitarray

"""
Dataclasses for Ents and Sides
"""

class SideType:
    DAMAGE = 0
    SHIELD = 1
    HEAL = 2
    HEALSHIELD = 3
    MANA = 4
    BLANK = 5
    REROLL = 6
    REDIRECTINCOMING = 7
    SUMMON = 8
    RECHARGE = 9
    RESURRECT = 10
    ENCHANT = 11
    KILL = 12
    UNDYING = 13
    DODGE = 14
    SELF_DAMAGE = 15
    DAMAGE_ALL = 16
    DAMAGE_EVERYONE = 17
    DAMAGE_FLANKING = 18
    SHIELD_ALL = 19
    HEAL_ALL = 20
    STUN = 21
SIDES_AUTO_USABLE = [SideType.STUN, SideType.RECHARGE, SideType.ENCHANT, SideType.UNDYING, SideType.DODGE] # Do not require positive pip values


@dataclass(slots=True)
class Side:
    value: int
    type: int
    keywords: bitarray # Length NUM_KEYWORDS, where bit i indicates presence of Keyword with value i

@dataclass(slots=True)
class Ent:
    hp: int
    maxHP: int
    shield: int
    undead: bool
    dodged: bool
    intangibled: bool

    dead: bool
    usedDie: bool
    timesUsed: int
    timesDamaged: int
    
    cleansed: bool # if cleansed at all, clears all inflicted.
    poison: int # may be negative to represent remaining cleanse for poison
    weaken: int
    petrifyAmount: int # distinct, can have 10 petrify
    petrified: bitarray # length 6, for the sides. Aside from basilisk, always blanked in order
    
    toexert: bool
    exerted: bool
    
    regen: int
    smith: int
    boost: int
    permaboost: int
    vulnerable: int
    armor: int
    backrow: bool
    size: int # 16, 24, 30, and 64 for small, reg, large, and huge monsters. For reinforcements
    
    # From enemy strikes
    inflictedPained: bool
    inflictedExerted: bool
    inflictedDeathed: bool
    inflictedSingleUse: bool

    possessed: bool
    redirectTarget: 'Ent'

    # For heros only, used for rescue, deathwish, pray, etc.
    incomingDamage: int
    incomingPoison: int
    
    sides: List[Side]
    currentSideNum: int
    currentSide: Side
    spellID: int # the ID for the spell
    onHitFunction: int # the ID for the onHitFunction
    specialHP: bitarray

    sourceID: int
    positionID: int # For monsters only, used for summoning