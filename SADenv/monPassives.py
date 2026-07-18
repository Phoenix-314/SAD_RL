from bitarray import bitarray
from bitarray.util import ba2int, int2ba
import copy

from SADenv.ents import *
from SADenv.util import *
from SADenv.keywords import *
from SADenv.lib import *
from SADenv.entIDs import *
import SADenv.factions as factions

"""
Mon passives are unique effects that occur when a monster is hit.
Does not include Bell, Rotten, chest, sarcophagus, barrel, bandit, goblin, or bones since these do not occur when the monster is hit.
Demon has additional support elsewhere as well due to complexity of its passive.
"""

class OnHitPassive:
    ONHIT_SARCOPHAGUS = 0
    ONHIT_SHADE = 1
    ONHIT_WISP = 2
    ONHIT_IMP = 3
    ONHIT_GRAVE = 4
    ONHIT_CHEST = 5
    ONHIT_ZOMBIE = 6
    ONHIT_QUARTZ = 7
    ONHIT_INEVITABLE = 8
    ONHIT_FOUNTAIN = 9
    ONHIT_WIZZ = 10
    ONHIT_SLIMER = 11
    ONHIT_OGRE = 12
    ONHIT_DEMON = 13
    ONHIT_CHOMP = 14
    ONHIT_GHOST = 15
    ONHIT_BOAR = 16
    ONHIT_SPIKER = 17
    ONHIT_SLATE = 18
    ONHIT_CAW = 19
    ONHIT_MAGRAT = 20
    ONHIT_GYTHA = 21
    ONHIT_AGNES = 22
    ONHIT_BASILISK = 23
    ONHIT_HYDRA = 24
    ONHIT_CYCLOPS = 25
    ONHIT_SLIME_QUEEN = 26
    ONHIT_LICH = 27
    ONHIT_BASALT = 28
    ONHIT_TARANTUS = 29
    ONHIT_HEXIA = 30
    ONHIT_BARON = 31
    ONHIT_THORN = 32
NUM_ONHIT_PASSIVES = 33

PassiveFuncMap = [None] * NUM_ONHIT_PASSIVES

def canRetribute(state, source):
    if not source: # if spell/bones/etc damage, do nothing
        return False
    hasRanged = hasCopycattedKeyword(state, source.currentSide, KeywordIDs.RANGED)
    if hasRanged:
        return False
    return True

def intangibleHP(state, source, target, pips):
    # Intangible checked at the beginning of damage(...), so this current attack (the one triggering intangible) will still do damage
    for i in range(max(1,target.hp - pips + 1), target.hp + 1):
        if target.specialHP[i]:
            target.intangibled = True
    target.specialHP[target.hp - pips + 1:target.hp+1] = False

def manaHP(state, source, target, pips):
    for i in range(max(1,target.hp - pips + 1), target.hp + 1):
        if target.specialHP[i]:
            state.mana += 1
    target.specialHP[target.hp - pips + 1:target.hp+1] = False
    
def spikyHP(amt):
    def onHit(state, source, target, pips):
        if not canRetribute(state, source): # if spell/bones/etc damage, do nothing
            return
        factions.damage(state, None, source, amt)
    return onHit

def stoneHP(state, source, target, pips):
    if target.specialHP[target.hp]: # if breaking stone hp
        target.specialHP[target.hp] = 0
        target.hp += (pips - 1) # can only do one damage, that is, can only break stone hp
        return
    
    v = None
    for i in range(max(1,target.hp - pips + 1), target.hp + 1): # find last special hp
        if target.specialHP[i]:
            v = i
    if not v:
        return # no special hp, so do nothing
    target.hp += pips - (target.hp - v) # can only do enough damage to get to stone hp, but not break it

def stunHP(state, source, target, pips):
    for i in range(max(1,target.hp - pips + 1), target.hp + 1):
        if target.specialHP[i]:
            factions.modifyDamage(state, target, -999)
            target.usedDie = True
    target.specialHP[target.hp - pips + 1:target.hp+1] = False

def summonHP(monID):
    def onHit(state, source, target, pips):
        for i in range(max(1,target.hp - pips + 1), target.hp + 1):
            if target.specialHP[i]:
                factions.summon(state, target, monID)
        target.specialHP[target.hp - pips + 1:target.hp+1] = False
    return onHit
    
def damageHP(amt, damageTarget):
    def onHit(state, source, target, pips):
        for i in range(max(1,target.hp - pips + 1), target.hp + 1):
            if target.specialHP[i]:
                srcForDamage = None
                if damageTarget == 2:
                    srcForDamage = findBottomPlayer(state)
                if damageTarget == 1:
                    srcForDamage = findMiddlePlayer(state)
                if damageTarget == 0:
                    srcForDamage = findTopPlayer(state)
                
                if srcForDamage:
                    factions.damage(state, None, srcForDamage, amt)
        target.specialHP[target.hp - pips + 1:target.hp+1] = False
    return onHit


#PassiveFuncMap[OnHitPassive.ONHIT_BONE] = boneDamage(1)
#PassiveFuncMap[OnHitPassive.ONHIT_BARREL] = boneDamage(5)

PassiveFuncMap[OnHitPassive.ONHIT_SHADE] = intangibleHP
PassiveFuncMap[OnHitPassive.ONHIT_WISP] = manaHP
PassiveFuncMap[OnHitPassive.ONHIT_IMP] = spikyHP(1)
PassiveFuncMap[OnHitPassive.ONHIT_GRAVE] = stoneHP
PassiveFuncMap[OnHitPassive.ONHIT_CHEST] = stoneHP
def onHitZombie(state, source, target, pips):
    if pips > 4:
        target.dead = True
PassiveFuncMap[OnHitPassive.ONHIT_ZOMBIE] = onHitZombie
def onHitQuartz(state, source, target, pips):
    if target.hp - pips == 2:
        target.dead = True
PassiveFuncMap[OnHitPassive.ONHIT_QUARTZ] = onHitQuartz
PassiveFuncMap[OnHitPassive.ONHIT_FOUNTAIN] = manaHP
PassiveFuncMap[OnHitPassive.ONHIT_WIZZ] = stunHP
PassiveFuncMap[OnHitPassive.ONHIT_SLIMER] = summonHP(SLIMELET)
def onHitOgre(state, source, target, pips):
    for i in range(max(1,target.hp - pips + 1), target.hp + 1):
        if target.specialHP[i]:
            target.permaboost += 1
            factions.modifyDamage(state, target, 1)
    target.specialHP[target.hp - pips + 1:target.hp+1] = False
PassiveFuncMap[OnHitPassive.ONHIT_OGRE] = onHitOgre
def onHitDemon(state, source, target, pips):
    target.specialHP[target.hp - pips + 1:target.hp+1] = False
PassiveFuncMap[OnHitPassive.ONHIT_DEMON] = onHitDemon
def onHitChomp(state, source, target, pips):
    for i in range(max(1,target.hp - pips + 1), target.hp + 1):
        if target.specialHP[i]:
            srcForDamage = None
            if i <= 5:
                srcForDamage = findBottomPlayer(state)
            if i >= 6:
                srcForDamage = findTopPlayer(state)
            
            if srcForDamage:
                factions.damage(state, None, srcForDamage, 1)
    target.specialHP[target.hp - pips + 1:target.hp] = False
PassiveFuncMap[OnHitPassive.ONHIT_CHOMP] = onHitChomp
PassiveFuncMap[OnHitPassive.ONHIT_GHOST] = intangibleHP
PassiveFuncMap[OnHitPassive.ONHIT_BOAR] = stoneHP
PassiveFuncMap[OnHitPassive.ONHIT_SPIKER] = spikyHP(2)
PassiveFuncMap[OnHitPassive.ONHIT_SLATE] = stoneHP
def onHitCaw(state, source, target, pips):
    target.backrow = True
PassiveFuncMap[OnHitPassive.ONHIT_CAW] = onHitCaw
PassiveFuncMap[OnHitPassive.ONHIT_MAGRAT] = damageHP(2, 2) # targets bottom
PassiveFuncMap[OnHitPassive.ONHIT_GYTHA] = damageHP(2, 1) # targets middle
PassiveFuncMap[OnHitPassive.ONHIT_AGNES] = damageHP(2, 0) # targets top
def onHitBasilisk(state, source, target, pips):
    if not canRetribute(state, source): # cant petrify a spell
        return
    if source.petrified[source.currentSideNum]: # if already petrified, do nothing
        return
    source.petrifyAmount += 1
    if source.petrifyAmount > 0:
        source.petrified[source.currentSideNum] = 1

PassiveFuncMap[OnHitPassive.ONHIT_BASILISK] = onHitBasilisk
def onHitHydra(state, source, target, pips):
    if target.timesDamaged >= 5:
        target.dead = True
PassiveFuncMap[OnHitPassive.ONHIT_HYDRA] = onHitHydra
PassiveFuncMap[OnHitPassive.ONHIT_CYCLOPS] = stunHP
PassiveFuncMap[OnHitPassive.ONHIT_SLIME_QUEEN] = summonHP(SLIMER)
PassiveFuncMap[OnHitPassive.ONHIT_LICH] = summonHP(BONES)
def onHitBasalt(state, source, target, pips):
    amt = ba2int(target.specialHP)
    if (pips == amt):
        target.hp -= pips # Damage - This works since Basalt's passive doubles damage more or less directly, having vulnerable applied first
        target.specialHP = int2ba(amt + 1)
PassiveFuncMap[OnHitPassive.ONHIT_BASALT] = onHitBasalt
def onHitTarantus(state, source, target, pips):
    if target.specialHP[10] and target.hp - pips < 10:
        target.specialHP[10] = False
        targ = findTopPlayer(state)
        if not targ: # ie, all players are dead and poison triggered this hp
            return
        if targ.dodged:
            return
        targ = targ.redirectTarget if targ.redirectTarget else targ
        if targ.undead:
            return
        targ.dead = True
PassiveFuncMap[OnHitPassive.ONHIT_TARANTUS] = onHitTarantus
def onHitHexia(state, source, target, pips):
    if not canRetribute(state, source): # if spell/bones/etc damage, do nothing
        return
    factions.damage(state, None, source, pips)
PassiveFuncMap[OnHitPassive.ONHIT_HEXIA] = onHitHexia
PassiveFuncMap[OnHitPassive.ONHIT_BARON] = manaHP
PassiveFuncMap[OnHitPassive.ONHIT_SARCOPHAGUS] = stoneHP
PassiveFuncMap[OnHitPassive.ONHIT_INEVITABLE] = intangibleHP
PassiveFuncMap[OnHitPassive.ONHIT_THORN] = spikyHP(5)