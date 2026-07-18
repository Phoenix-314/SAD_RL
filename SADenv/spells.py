import copy

import SADenv.validActions as validActions
import SADenv.factions as factions
from SADenv.entIDs import *
from SADenv.ents import *
from SADenv.states import *
from SADenv.util import *
from SADenv.keywords import *
from SADenv.lib import *

"""
Spell functions used in castSpell.
0 is burst
1-5 are orange through blue, although typically only 4 and 5 are usable (few spells are orange, yellow, or gray)
"""

class SpellData:
    # Positive values represent additional cost for DEPLETE spells
    STANDARD = 0
    COOLDOWN_USED = -1
    SINGLE_CASTED = -2
# Spell constants
class Spell:
    MEND = 0
    HEMLOCK = 1
    RESTORE = 2
    GAZE = 3
    BANDAGE = 4
    BALANCE = 5
    VINE = 6
    RENEW = 7
    PRAY = 8
    INFUSE = 9
    FLIP = 10
    GLOW = 11
    CIRCLE = 12
    LIQUOR = 13
    BIND = 14
    SOOTHE = 15
    RITUAL = 16
    SALVE = 17
    LEECH = 18
    OPERATE = 19
    STRAND = 20
    POKE = 21
    SCORCH = 22
    SLAY = 23
    SLICE = 24
    GATHER = 25
    CUT = 26
    SCALD = 27
    DROP = 28
    CHILL = 29
    FLICK = 30
    ZAP = 31
    SPORE = 32
    FORETELL = 33
    BURN = 34
    BLADES = 35
    CRUSH = 36
    MIASMA = 37
    TICK = 38
    BLAZE = 39
    DRAW = 40
    HARVEST = 41
    INSPIRE = 42
    # Weird spells/Tactics
    IMBUE = 43
    FORMATION = 44
    PARRY = 45
    ELSE = 46
    LIGHT = 47
    UNITE = 48
    CLINK = 49
    BURST = 50 # default spell for state.spellData[0]
SPELL_NUM_SPELLS = 51

SpellFuncMap = [None] * SPELL_NUM_SPELLS
SpellCostMap = [0] * SPELL_NUM_SPELLS
SpellReqsMap = [0] * SPELL_NUM_SPELLS
SpellCastAffectMap = [0] * SPELL_NUM_SPELLS
class SpellCastAffect:
    STANDARD = 0
    COOLDOWN = 1
    SINGLE_CAST = 2
    DEPLETE = 3
class SpellTargetReq:
    UNTARGETED = 0
    ENEMY = 1
    ALLY = 2
    EITHER = 3
    RESURRECT = 4
    HP1 = 5 # Harvest
    HP2 = 6 # Zap
    HP3 = 7 # Slay
    RECHARGE = 8
    DAMAGE_N_SHIELD_3 = 9 # Formation
    DAMAGE_2 = 10 # Parry
    ONE_OF_ALL = 11 # Unite - 1x (Damage, Shield, Heal, Blank)
    AN_X = 12 # Else
    ALL_THORN_SPECIALS = [DAMAGE_N_SHIELD_3, UNTARGETED, HP1]
    ALL_TACTICS = [DAMAGE_N_SHIELD_3, DAMAGE_2, ONE_OF_ALL, AN_X]


def findTacticCosts(state, spellTargetReq):
    xs = 0
    damages = 0
    shields = 0
    heals = 0
    if spellTargetReq == SpellTargetReq.DAMAGE_N_SHIELD_3:
        damages = 3
        shields = 3
    elif spellTargetReq == SpellTargetReq.DAMAGE_2:
        damages = 2
    elif spellTargetReq == SpellTargetReq.ONE_OF_ALL:
        damages = 1
        shields = 1
        heals = 1
        xs = 1
    elif spellTargetReq == SpellTargetReq.AN_X:
        xs = 1
        
    neededHeros = []
    for i, hero in enumerate(state.players):
        if not validActions.diceUsable(state, i):
            continue

        heroNeeded = False
        pips = findStaticPips(state, hero)
        if xs > 0 and (hero.currentSide.type == SideType.BLANK or hero.petrified[hero.currentSideNum]):
            xs -= 1
            heroNeeded = True
            
        else: # x is incompatible with the rest. Damage, heal, and shields can be together via keywords or HEALSHIELD
            if pips <= 0 or hero.petrified[hero.currentSideNum]: # dont use a hero with no pips (or negative pips)
                continue

            if damages > 0 and hero.currentSide.type in [SideType.DAMAGE, SideType.DAMAGE_ALL, SideType.DAMAGE_EVERYONE, SideType.SELF_DAMAGE]:
                damages -= findStaticPips(state, hero)
                heroNeeded = True
            if shields > 0 and (hero.currentSide.type in [SideType.SHIELD, SideType.HEALSHIELD, SideType.SHIELD_ALL] or hero.currentSide.keywords[KeywordIDs.SELFSHIELD]):
                shields -= findStaticPips(state, hero)
                heroNeeded = True
            if heals > 0 and (hero.currentSide.type in [SideType.HEAL, SideType.HEALSHIELD, SideType.HEAL_ALL] or hero.currentSide.keywords[KeywordIDs.SELFHEAL]):
                heals -= findStaticPips(state, hero)
                heroNeeded = True
        
        if heroNeeded:
            neededHeros.append(hero)

        if damages <= 0 and shields <= 0 and heals <= 0 and xs <= 0:
            return neededHeros
    return None # Cannot use tactic
    
def mend(state : State, target: Ent):
    target.hp = 5
    if target.maxHP < 5:
        target.maxHP = 5
SpellFuncMap[Spell.MEND] = mend
SpellCostMap[Spell.MEND] = 2
SpellReqsMap[Spell.MEND] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.MEND] = SpellCastAffect.STANDARD

def hemlock(state : State, target: Ent):
    factions.damage(state, None, target, 1)
    KeywordFuncMap[KeywordIDs.POISON](state, None, target, 1)
SpellFuncMap[Spell.HEMLOCK] = hemlock
SpellCostMap[Spell.HEMLOCK] = 2
SpellReqsMap[Spell.HEMLOCK] = SpellTargetReq.ENEMY
SpellCastAffectMap[Spell.HEMLOCK] = SpellCastAffect.SINGLE_CAST

def restore(state : State, target: Ent):
    for hero in state.players:
        factions.heal(state, hero, 1)
SpellFuncMap[Spell.RESTORE] = restore
SpellCostMap[Spell.RESTORE] = 2
SpellReqsMap[Spell.RESTORE] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.RESTORE] = SpellCastAffect.STANDARD

def gaze(state : State, target: Ent):
    state.rerolls -= 1 # must be at 0 rerolls in order to use spells
SpellFuncMap[Spell.GAZE] = gaze
SpellCostMap[Spell.GAZE] = 1
SpellReqsMap[Spell.GAZE] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.GAZE] = SpellCastAffect.STANDARD

def bandage(state : State, target: Ent):
    for hero in findHeroCleaveTargets(state, target):
        hero.shield += 1
        factions.heal(state, hero, 1)
SpellFuncMap[Spell.BANDAGE] = bandage
SpellCostMap[Spell.BANDAGE] = 2
SpellReqsMap[Spell.BANDAGE] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.BANDAGE] = SpellCastAffect.SINGLE_CAST

def balance(state : State, target: Ent):
    for i, mon in enumerate(state.enemies):
        if mon.sourceID == THORN:
            continue # Cannot damage thorns
        factions.damage(state, None, mon, 1)
    for hero in state.players:
        factions.heal(state, hero, 1)
SpellFuncMap[Spell.BALANCE] = balance
SpellCostMap[Spell.BALANCE] = 3
SpellReqsMap[Spell.BALANCE] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.BALANCE] = SpellCastAffect.STANDARD

def vine(state : State, target: Ent):
    if target.sourceID >= 0: # if target is hero
        factions.heal(state, target, 1)
    else:
        factions.damage(state, None, target, 1)
SpellFuncMap[Spell.VINE] = vine
SpellCostMap[Spell.VINE] = 1
SpellReqsMap[Spell.VINE] = SpellTargetReq.EITHER
SpellCastAffectMap[Spell.VINE] = SpellCastAffect.STANDARD

def renew(state : State, target: Ent):
    target.hp = 4
    if target.maxHP < 4:
        target.maxHP = 4
SpellFuncMap[Spell.RENEW] = renew
SpellCostMap[Spell.RENEW] = 1
SpellReqsMap[Spell.RENEW] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.RENEW] = SpellCastAffect.STANDARD

def pray(state : State, target: Ent):
    for i, hero in enumerate(state.players):
        if targetIsDying(state, hero):
            factions.heal(state, hero, 1)
            hero.shield += 1
SpellFuncMap[Spell.PRAY] = pray
SpellCostMap[Spell.PRAY] = 1
SpellReqsMap[Spell.PRAY] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.PRAY] = SpellCastAffect.DEPLETE

def infuse(state : State, target: Ent):
    for hero in state.players:
        delta = 2 if hero.hp == 1 else 1
        factions.heal(state, hero, delta)
SpellFuncMap[Spell.INFUSE] = infuse
SpellCostMap[Spell.INFUSE] = 2
SpellReqsMap[Spell.INFUSE] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.INFUSE] = SpellCastAffect.STANDARD

def flip(state : State, target: Ent):
    mp = [5, 3, 4, 1, 2, 0] # top maps to bottom, left to right, middle to rightmost
    for hero in state.players:
        hero.currentSide = copy.deepcopy(hero.sides[mp[hero.currentSideNum]])
SpellFuncMap[Spell.FLIP] = flip
SpellCostMap[Spell.FLIP] = 2
SpellReqsMap[Spell.FLIP] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.FLIP] = SpellCastAffect.STANDARD

def glow(state : State, target: Ent):
    for hero in state.players:
        if hero.currentSide.type == SideType.BLANK or hero.petrified[hero.currentSideNum] or hero.exerted:
            hero.currentSide = Side(2, SideType.HEALSHIELD, bitarray(KeywordIDs.NUM_KEYWORDS))
            hero.currentSide.keywords[KeywordIDs.STASIS] = True
            hero.exerted = False
            hero.inflictedPained = False
            hero.inflictedExerted = False
            hero.inflictedDeathed = False
            hero.inflictedSingleUse = False
SpellFuncMap[Spell.GLOW] = glow
SpellCostMap[Spell.GLOW] = 1
SpellReqsMap[Spell.GLOW] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.GLOW] = SpellCastAffect.STANDARD

def circle(state : State, target: Ent):
    for hero in state.players:
        hero.currentSide.keywords = hero.currentSide.keywords | make_bitarray(KeywordIDs.SELFHEAL)
SpellFuncMap[Spell.CIRCLE] = circle
SpellCostMap[Spell.CIRCLE] = 2
SpellReqsMap[Spell.CIRCLE] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.CIRCLE] = SpellCastAffect.SINGLE_CAST

def liquor(state : State, target: Ent):
    factions.heal(state, target, 10)
    KeywordFuncMap[KeywordIDs.CLEANSE](state, None, target, 10)
SpellFuncMap[Spell.LIQUOR] = liquor
SpellCostMap[Spell.LIQUOR] = 3
SpellReqsMap[Spell.LIQUOR] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.LIQUOR] = SpellCastAffect.STANDARD

def bind(state : State, target: Ent):
    target.intangibled = True
SpellFuncMap[Spell.BIND] = bind
SpellCostMap[Spell.BIND] = 3
SpellReqsMap[Spell.BIND] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.BIND] = SpellCastAffect.DEPLETE

def soothe(state : State, target: Ent):
    for hero in state.players:
        factions.heal(state, hero, 1)
        hero.regen += 1
SpellFuncMap[Spell.SOOTHE] = soothe
SpellCostMap[Spell.SOOTHE] = 4
SpellReqsMap[Spell.SOOTHE] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.SOOTHE] = SpellCastAffect.STANDARD

def ritual(state : State, target: Ent):
    for hero in findHeroCleaveTargets(state, target):
        factions.heal(state, hero, 2)
        KeywordFuncMap[KeywordIDs.CLEANSE](state, None, hero, 2)
SpellFuncMap[Spell.RITUAL] = ritual
SpellCostMap[Spell.RITUAL] = 3
SpellReqsMap[Spell.RITUAL] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.RITUAL] = SpellCastAffect.SINGLE_CAST

def salve(state : State, target: Ent):
    factions.heal(state, target, 2)
SpellFuncMap[Spell.SALVE] = salve
SpellCostMap[Spell.SALVE] = 1
SpellReqsMap[Spell.SALVE] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.SALVE] = SpellCastAffect.STANDARD

def leech(state : State, target: Ent):
    for hero in state.players:
        factions.heal(state, hero, 5)
    target.dead = True
SpellFuncMap[Spell.LEECH] = leech
SpellCostMap[Spell.LEECH] = 1
SpellReqsMap[Spell.LEECH] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.LEECH] = SpellCastAffect.COOLDOWN

def operate(state : State, target: Ent):
    factions.resurrect(state, 1)
SpellFuncMap[Spell.OPERATE] = operate
SpellCostMap[Spell.OPERATE] = 3
SpellReqsMap[Spell.OPERATE] = SpellTargetReq.RESURRECT
SpellCastAffectMap[Spell.OPERATE] = SpellCastAffect.DEPLETE

def strand(state : State, target: Ent):
    dying = targetIsDying(state, target)
    factions.heal(state, target, 2)
    if dying and not targetIsDying(state, target):
        state.mana += 4
SpellFuncMap[Spell.STRAND] = strand
SpellCostMap[Spell.STRAND] = 4
SpellReqsMap[Spell.STRAND] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.STRAND] = SpellCastAffect.STANDARD

# START OF BLUE SPELLS
def poke(state : State, target: Ent):
    factions.damage(state, None, target, 1)
SpellFuncMap[Spell.POKE] = poke
SpellCostMap[Spell.POKE] = 1
SpellReqsMap[Spell.POKE] = SpellTargetReq.ENEMY
SpellCastAffectMap[Spell.POKE] = SpellCastAffect.COOLDOWN

def scorch(state : State, target: Ent):
    for mon in findEnemyCleaveTargets(state, target):
        if mon.sourceID == THORN:
            continue # Cannot damage thorns
        factions.damage(state, None, mon, 1)
SpellFuncMap[Spell.SCORCH] = scorch
SpellCostMap[Spell.SCORCH] = 2
SpellReqsMap[Spell.SCORCH] = SpellTargetReq.ENEMY
SpellCastAffectMap[Spell.SCORCH] = SpellCastAffect.SINGLE_CAST

def slay(state : State, target: Ent):
    target.dead = True # checking for valid targets already done
SpellFuncMap[Spell.SLAY] = slay
SpellCostMap[Spell.SLAY] = 3
SpellReqsMap[Spell.SLAY] = SpellTargetReq.HP3
SpellCastAffectMap[Spell.SLAY] = SpellCastAffect.STANDARD

def slice(state : State, target: Ent):
    for i, mon in enumerate(state.enemies):
        if mon.sourceID == THORN:
            continue # Cannot damage thorns
        factions.damage(state, None, mon, 1)
SpellFuncMap[Spell.SLICE] = slice
SpellCostMap[Spell.SLICE] = 3
SpellReqsMap[Spell.SLICE] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.SLICE] = SpellCastAffect.STANDARD

def gather(state : State, target: Ent):
    for hero in state.players:
        if hero.currentSide.type == SideType.BLANK or hero.petrified[hero.currentSideNum] or hero.exerted:
            hero.currentSide = Side(2, SideType.MANA, bitarray(KeywordIDs.NUM_KEYWORDS))
            hero.currentSide.keywords[KeywordIDs.STASIS] = True
            hero.exerted = False
            hero.inflictedPained = False
            hero.inflictedExerted = False
            hero.inflictedDeathed = False
            hero.inflictedSingleUse = False
SpellFuncMap[Spell.GATHER] = gather
SpellCostMap[Spell.GATHER] = 2
SpellReqsMap[Spell.GATHER] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.GATHER] = SpellCastAffect.STANDARD

def cut(state : State, target: Ent):
    factions.damage(state, None, target, 3)
    if state.enemies[0].sourceID == THORN:
        return # Cannot damage thorns
    factions.damage(state, None, state.enemies[0], 2)
SpellFuncMap[Spell.CUT] = cut
SpellCostMap[Spell.CUT] = 1
SpellReqsMap[Spell.CUT] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.CUT] = SpellCastAffect.COOLDOWN

def scald(state : State, target: Ent):
    for i, mon in enumerate(state.enemies):
        if mon.hp < mon.maxHP and mon.sourceID != THORN:
            factions.damage(state, None, mon, 2)
SpellFuncMap[Spell.SCALD] = scald
SpellCostMap[Spell.SCALD] = 3
SpellReqsMap[Spell.SCALD] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.SCALD] = SpellCastAffect.STANDARD

def drop(state : State, target: Ent):
    if state.enemies[0].sourceID == THORN:
        return # Cannot damage thorns
    factions.damage(state, None, state.enemies[0], 4)
SpellFuncMap[Spell.DROP] = drop
SpellCostMap[Spell.DROP] = 3
SpellReqsMap[Spell.DROP] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.DROP] = SpellCastAffect.STANDARD

def chill(state : State, target: Ent):
    factions.damage(state, None, target, 2)
    KeywordFuncMap[KeywordIDs.WEAKEN](state, None, target, 2)
SpellFuncMap[Spell.CHILL] = chill
SpellCostMap[Spell.CHILL] = 2
SpellReqsMap[Spell.CHILL] = SpellTargetReq.ENEMY
SpellCastAffectMap[Spell.CHILL] = SpellCastAffect.SINGLE_CAST

def flick(state : State, target: Ent):
    damage = 2 if target.hp == target.maxHP else 1
    factions.damage(state, None, target, damage)
SpellFuncMap[Spell.FLICK] = flick
SpellCostMap[Spell.FLICK] = 1
SpellReqsMap[Spell.FLICK] = SpellTargetReq.ENEMY
SpellCastAffectMap[Spell.FLICK] = SpellCastAffect.COOLDOWN

def zap(state : State, target: Ent):
    target.dead = True # zap op.
SpellFuncMap[Spell.ZAP] = zap
SpellCostMap[Spell.ZAP] = 1
SpellReqsMap[Spell.ZAP] = SpellTargetReq.HP2
SpellCastAffectMap[Spell.ZAP] = SpellCastAffect.COOLDOWN

def spore(state : State, target: Ent):
    target.currentSide.keywords |= make_bitarray(KeywordIDs.DECAY)
    target.currentSide.value += 1 # Current side is copy, so these dont change underlying side. However, DECAY keyword does affect underlying side
SpellFuncMap[Spell.SPORE] = spore
SpellCostMap[Spell.SPORE] = 1
SpellReqsMap[Spell.SPORE] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.SPORE] = SpellCastAffect.COOLDOWN

def foretell(state : State, target: Ent):
    state.fortoldMana += 4
SpellFuncMap[Spell.FORETELL] = foretell
SpellCostMap[Spell.FORETELL] = 3
SpellReqsMap[Spell.FORETELL] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.FORETELL] = SpellCastAffect.STANDARD

def burn(state : State, target: Ent):
    for ent in state.players:
        factions.damage(state, None, ent, 1)
    for i, mon in enumerate(state.enemies):
        if mon.sourceID == THORN:
            continue # Cannot damage thorns
        factions.damage(state, None, mon, 1)
SpellFuncMap[Spell.BURN] = burn
SpellCostMap[Spell.BURN] = 1
SpellReqsMap[Spell.BURN] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.BURN] = SpellCastAffect.COOLDOWN

def blades(state : State, target: Ent):
    for i, mon in enumerate(state.enemies):
        if mon.sourceID == THORN:
            continue # Cannot damage thorns
        factions.damage(state, None, mon, 2)
SpellFuncMap[Spell.BLADES] = blades
SpellCostMap[Spell.BLADES] = 4
SpellReqsMap[Spell.BLADES] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.BLADES] = SpellCastAffect.STANDARD

def crush(state : State, target: Ent):
    if state.enemies[0].sourceID != THORN:
        factions.damage(state, None, state.enemies[0], 3)
    if len(state.enemies) > 1 and state.enemies[len(state.enemies) - 1].sourceID != THORN:
        factions.damage(state, None, state.enemies[len(state.enemies) - 1], 3)
SpellFuncMap[Spell.CRUSH] = crush
SpellCostMap[Spell.CRUSH] = 3
SpellReqsMap[Spell.CRUSH] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.CRUSH] = SpellCastAffect.STANDARD

def miasma(state : State, target: Ent):
    for mon in findEnemyCleaveTargets(state, target):
        if mon.sourceID == THORN:
            continue # Cannot damage thorns
        factions.damage(state, None, mon, 1)
        KeywordFuncMap[KeywordIDs.POISON](state, None, mon, 1)
SpellFuncMap[Spell.MIASMA] = miasma
SpellCostMap[Spell.MIASMA] = 3
SpellReqsMap[Spell.MIASMA] = SpellTargetReq.ENEMY
SpellCastAffectMap[Spell.MIASMA] = SpellCastAffect.STANDARD

def tick(state : State, target: Ent):
    for mon in findEnemyCleaveTargets(state, target):
        if mon.sourceID == THORN:
            continue # Cannot damage thorns
        factions.damage(state, None, mon, 1)
        KeywordFuncMap[KeywordIDs.WEAKEN](state, None, mon, 1)
SpellFuncMap[Spell.TICK] = tick
SpellCostMap[Spell.TICK] = 4
SpellReqsMap[Spell.TICK] = SpellTargetReq.ENEMY
SpellCastAffectMap[Spell.TICK] = SpellCastAffect.STANDARD

def blaze(state : State, target: Ent):
    factions.damage(state, None, target, 13)
SpellFuncMap[Spell.BLAZE] = blaze
SpellCostMap[Spell.BLAZE] = 6 # Kaboom
SpellReqsMap[Spell.BLAZE] = SpellTargetReq.ENEMY
SpellCastAffectMap[Spell.BLAZE] = SpellCastAffect.STANDARD

def draw(state : State, target: Ent):
    target.boost += 1
SpellFuncMap[Spell.DRAW] = draw
SpellCostMap[Spell.DRAW] = 1
SpellReqsMap[Spell.DRAW] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.DRAW] = SpellCastAffect.DEPLETE

def harvest(state : State, target: Ent):
    if target.sourceID != THORN:
        target.dead = True
    state.mana += 3
SpellFuncMap[Spell.HARVEST] = harvest
SpellCostMap[Spell.HARVEST] = 1
SpellReqsMap[Spell.HARVEST] = SpellTargetReq.HP1
SpellCastAffectMap[Spell.HARVEST] = SpellCastAffect.COOLDOWN

def inspire(state : State, target: Ent):
    target.usedDie = False
    target.timesUsed = 0
SpellFuncMap[Spell.INSPIRE] = inspire
SpellCostMap[Spell.INSPIRE] = 4
SpellReqsMap[Spell.INSPIRE] = SpellTargetReq.RECHARGE
SpellCastAffectMap[Spell.INSPIRE] = SpellCastAffect.COOLDOWN

# Wierd spells / Tactics
def imbue(state : State, target: Ent):
    if target.currentSide.type in [SideType.DAMAGE, SideType.DAMAGE_ALL, SideType.SELF_DAMAGE]:
        target.currentSide.value += 1
SpellFuncMap[Spell.IMBUE] = imbue
SpellCostMap[Spell.IMBUE] = 1
SpellReqsMap[Spell.IMBUE] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.IMBUE] = SpellCastAffect.SINGLE_CAST

def formation(state : State, target: Ent):
    for i, mon in enumerate(state.enemies):
        if mon.sourceID == THORN:
            continue # Cannot damage thorns
        factions.damage(state, None, mon, 2)
    for hero in state.players:
        hero.shield += 2
SpellFuncMap[Spell.FORMATION] = formation
SpellCostMap[Spell.FORMATION] = 6
SpellReqsMap[Spell.FORMATION] = SpellTargetReq.DAMAGE_N_SHIELD_3
SpellCastAffectMap[Spell.FORMATION] = SpellCastAffect.STANDARD

def parry(state : State, target: Ent):
    target.shield += 3
SpellFuncMap[Spell.PARRY] = parry
SpellCostMap[Spell.PARRY] = 2
SpellReqsMap[Spell.PARRY] = SpellTargetReq.DAMAGE_2
SpellCastAffectMap[Spell.PARRY] = SpellCastAffect.STANDARD

def else_spell(state : State, target: Ent):
    target.shield += 1
    KeywordFuncMap[KeywordIDs.CLEANSE](state, None, target, 1)
SpellFuncMap[Spell.ELSE] = else_spell
SpellCostMap[Spell.ELSE] = 1
SpellReqsMap[Spell.ELSE] = SpellTargetReq.AN_X
SpellCastAffectMap[Spell.ELSE] = SpellCastAffect.STANDARD

def light(state : State, target: Ent):
    for hero in findHeroCleaveTargets(state, target):
        hero.shield += 1
        KeywordFuncMap[KeywordIDs.CLEANSE](state, None, hero, 1)
SpellFuncMap[Spell.LIGHT] = light
SpellCostMap[Spell.LIGHT] = 1
SpellReqsMap[Spell.LIGHT] = SpellTargetReq.ALLY
SpellCastAffectMap[Spell.LIGHT] = SpellCastAffect.SINGLE_CAST

def unite(state : State, target: Ent):
    factions.damage(state, None, target, 15)
SpellFuncMap[Spell.UNITE] = unite
SpellCostMap[Spell.UNITE] = 4
SpellReqsMap[Spell.UNITE] = SpellTargetReq.ONE_OF_ALL
SpellCastAffectMap[Spell.UNITE] = SpellCastAffect.STANDARD

def clink(state : State, target: Ent):
    for hero in state.players:
        hero.shield += 1
        hero.boost += 1
SpellFuncMap[Spell.CLINK] = clink
SpellCostMap[Spell.CLINK] = 4
SpellReqsMap[Spell.CLINK] = SpellTargetReq.UNTARGETED
SpellCastAffectMap[Spell.CLINK] = SpellCastAffect.SINGLE_CAST

def burst(state : State, target: Ent):
    if target.sourceID >= 0: # if target is hero
        target.shield += 2
    else:
        factions.damage(state, None, target, 2)
SpellFuncMap[Spell.BURST] = burst
SpellCostMap[Spell.BURST] = 2
SpellReqsMap[Spell.BURST] = SpellTargetReq.EITHER
SpellCastAffectMap[Spell.BURST] = SpellCastAffect.STANDARD


Lib.SPELL_FUNC_MAP = SpellFuncMap
Lib.SPELL_COST_MAP = SpellCostMap
Lib.SPELL_REQS_MAP = SpellReqsMap
Lib.SPELL_CAST_AFFECT_MAP = SpellCastAffectMap