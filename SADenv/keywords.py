import copy

from bitarray import bitarray
from SADenv.ents import *
import SADenv.util as util
import SADenv.factions as factions
from SADenv.keywordIDs import *

"""
Keyword functions. These can be called directly, or proccessed as a batch by the applyAffectKeywords, completeActionKeywords, findStaticPips, and findDynamicPips functions.
"""

KeywordFuncMap = [None] * KeywordIDs.NUM_KEYWORDS

def vulnerable(state, source, target, pips):
    target.vulnerable += pips
    return 0
KeywordFuncMap[KeywordIDs.VULNERABLE] = vulnerable
def cruel(state, source, target, pips):
    return 0 if target.hp * 2 > target.maxHP else pips
KeywordFuncMap[KeywordIDs.CRUEL] = cruel
def poison(state, source, target, pips):
    target.poison += pips
    return 0
KeywordFuncMap[KeywordIDs.POISON] = poison
def cantrip(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.CANTRIP] = cantrip
def managain(state, source, target, pips):
    state.mana += pips
    return 0
KeywordFuncMap[KeywordIDs.MANAGAIN] = managain
def engage(state, source, target, pips):
    return pips if target.hp == target.maxHP else 0
KeywordFuncMap[KeywordIDs.ENGAGE] = engage
def ranged(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.RANGED] = ranged
def sticky(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.STICKY] = sticky
def mandatory(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.MANDATORY] = mandatory
def death(state, source, target, pips):
    source.dead = True
    return 0
KeywordFuncMap[KeywordIDs.DEATH] = death
def rampage(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.RAMPAGE] = rampage
def pain(state, source, target, pips):
    factions.damage(state, None, source, pips)
    return 0
KeywordFuncMap[KeywordIDs.PAIN] = pain
def plague(state, source, target, pips):
    count = 0
    for ent in state.enemies + state.players:
        count += max(0, ent.poison)
    return count
KeywordFuncMap[KeywordIDs.PLAGUE] = plague
def cleanse(state, source, target, pips):
    target.cleansed = True
    target.poison -= pips # may be negative, representing remaining cleanse for poison

    target.weaken -= pips
    
    target.petrifyAmount -= pips
    if target.petrifyAmount <= 0:
        target.petrified = bitarray(6) # clear
    elif target.petrifyAmount > 0 and target.petrifyAmount < 6:
        petrifyClearPips = sum(target.petrified) - target.petrifyAmount # number of sides to clear 
        i = 5
        while petrifyClearPips > 0 and i >= 0:
            petrifyClearPips -= target.petrified[i] # if petrified (==1), decrease the number of sides we can clear by 1. Else, do nothing (-= 0)
            target.petrified[i] = 0
            i -= 1
    # no else since going from >6 petrify to >6 petrify doesn't change which sides are petrified
    
    target.inflictedPained = False
    target.inflictedExerted = False
    target.inflictedDeathed = False
    target.inflictedSingleUse = False
    return 0
KeywordFuncMap[KeywordIDs.CLEANSE] = cleanse
def pristine(state, source, target, pips):
    return pips if source.hp == source.maxHP else 0
KeywordFuncMap[KeywordIDs.PRISTINE] = pristine
def exert(state, source, target, pips):
    source.exerted = True
    source.toexert = True
    return 0
KeywordFuncMap[KeywordIDs.EXERT] = exert
def selfshield(state, source, target, pips):
    source.shield += pips
    return 0
KeywordFuncMap[KeywordIDs.SELFSHIELD] = selfshield
def cleave(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.CLEAVE] = cleave
def guilt(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.GUILT] = guilt
def heavy(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.HEAVY] = heavy
def singleuse(state, source, target, pips):
    source.sides[source.currentSideNum].type = SideType.BLANK
    return 0
KeywordFuncMap[KeywordIDs.SINGLEUSE] = singleuse
def deathwish(state, source, target, pips):
    return pips if util.targetIsDying(state, source) else 0
KeywordFuncMap[KeywordIDs.DEATHWISH] = deathwish
def chain(state, source, target, pips):
    return 0 if state.lastDie is None or (state.lastDie.keywords & source.currentSide.keywords) == bitarray(KeywordIDs.NUM_KEYWORDS) else pips
KeywordFuncMap[KeywordIDs.CHAIN] = chain
def bloodlust(state, source, target, pips):
    count = 0
    targs = state.enemies if source.sourceID >= 0 else state.players
    for ent in targs:
        if ent.hp < ent.maxHP:
            count += 1
    return count
KeywordFuncMap[KeywordIDs.BLOODLUST] = bloodlust
def steel(state, source, target, pips):
    return source.shield
KeywordFuncMap[KeywordIDs.STEEL] = steel
def duplicate(state, source, target, pips):
    for ent in state.players:
        ent.currentSide = copy.deepcopy(source.currentSide)
        ent.currentSide.value = findStaticPips(state, source)
        ent.currentSide.keywords[KeywordIDs.STASIS] = True
        ent.currentSide.keywords[KeywordIDs.DUPLICATE] = False

        ent.exerted = False
        ent.inflictedPained =  source.inflictedPained
        ent.inflictedExerted = source.inflictedExerted
        ent.inflictedDeathed = source.inflictedDeathed
        ent.inflictedSingleUse = source.inflictedSingleUse
    return 0
KeywordFuncMap[KeywordIDs.DUPLICATE] = duplicate
def eliminate(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.ELIMINATE] = eliminate
def focus(state, source, target, pips):
    return pips if (state.lastTarget == target) else 0
KeywordFuncMap[KeywordIDs.FOCUS] = focus
def descend(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.DESCEND] = descend
def charged(state, source, target, pips):
    return state.mana
KeywordFuncMap[KeywordIDs.CHARGED] = charged
def era(state, source, target, pips):
    return state.turn - 1
KeywordFuncMap[KeywordIDs.ERA] = era
def smith(state, source, target, pips):
    target.smith += pips
    return 0
KeywordFuncMap[KeywordIDs.SMITH] = smith
def defy(state, source, target, pips):
    return max(source.incomingDamage - source.shield, 0)
KeywordFuncMap[KeywordIDs.DEFY] = defy
def repel(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.REPEL] = repel
def rescue(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.RESCUE] = rescue
def inspired(state, source, target, pips):
    return pips if state.lastStrikeVal is not None and state.lastStrikeVal > pips else 0
KeywordFuncMap[KeywordIDs.INSPIRED] = inspired
def groooowth(state, source, target, pips):
    source.permaboost += 1
    return 0
KeywordFuncMap[KeywordIDs.GROOOOWTH] = groooowth
def decay(state, source, target, pips):
    source.sides[source.currentSideNum].value -= 1
    source.currentSide.value -= 1
    return 0
KeywordFuncMap[KeywordIDs.DECAY] = decay
def vitality(state, source, target, pips):
    target.maxHP += pips
    return 0
KeywordFuncMap[KeywordIDs.VITALITY] = vitality
def selfheal(state, source, target, pips):
    factions.heal(state, source, pips)
    return 0
KeywordFuncMap[KeywordIDs.SELFHEAL] = selfheal
def regen(state, source, target, pips):
    target.regen += pips
    return 0
KeywordFuncMap[KeywordIDs.REGEN] = regen
def growth(state, source, target, pips):
    source.sides[source.currentSideNum].value += 1
    source.currentSide.value += 1
    return 0
KeywordFuncMap[KeywordIDs.GROWTH] = growth
def weaken(state, source, target, pips):
    factions.modifyDamage(state, target, -pips)
    target.weaken += pips
    return 0
KeywordFuncMap[KeywordIDs.WEAKEN] = weaken
def boost(state, source, target, pips):
    target.boost += pips
    return 0
KeywordFuncMap[KeywordIDs.BOOST] = boost
def pair(state, source, target, pips):
    return pips if state.lastStrikeVal is not None and state.lastStrikeVal == pips else 0
KeywordFuncMap[KeywordIDs.PAIR] = pair
def trio(state, source, target, pips):
    return 2 * pips if state.prevLastStrikeVal is not None and state.prevLastStrikeVal == state.lastStrikeVal and state.lastStrikeVal == pips else 0
KeywordFuncMap[KeywordIDs.TRIO] = trio
def boned(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.BONED] = boned
def permaboost(state, source, target, pips):
    target.permaboost += pips
    return 0
KeywordFuncMap[KeywordIDs.PERMABOOST] = permaboost
def copycat(state, source, target, pips):
    if state.lastDie:
        source.currentSide.keywords = source.currentSide.keywords | state.lastDie.keywords # self.lastDie is appended keywords like inflictedDeath, etc.
    return 0
KeywordFuncMap[KeywordIDs.COPYCAT] = copycat
def doubleuse(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.DOUBLEUSE] = doubleuse
def quaduse(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.QUADUSE] = quaduse

def inflictpain(state, source, target, pips):
    if not target.cleansed:
        target.inflictedPained = True
    return 0
KeywordFuncMap[KeywordIDs.INFLICT_PAIN] = inflictpain
def inflictexert(state, source, target, pips):
    if not target.cleansed:
        target.inflictedExerted = True
    return 0
KeywordFuncMap[KeywordIDs.INFLICT_EXERT] = inflictexert
def inflictdeath(state, source, target, pips):
    if not target.cleansed:
        target.inflictedDeathed = True
    return 0
KeywordFuncMap[KeywordIDs.INFLICT_DEATH] = inflictdeath
def inflictsingleuse(state, source, target, pips):
    if not target.cleansed:
        target.inflictedSingleUse = True
    return 0
KeywordFuncMap[KeywordIDs.INFLICT_SINGLEUSE] = inflictsingleuse
def stasis(state, source, target, pips):
    return 0
KeywordFuncMap[KeywordIDs.STASIS] = stasis
def petrify(state, source, target, pips):
    newPetrifyAmount = pips if target.petrifyAmount >= 0 else target.petrifyAmount + pips
    target.petrifyAmount += pips

    if target.petrifyAmount >= 6:
        target.petrified = bitarray(6)
        target.petrified.setall(1)
        return 0
    
    i = 0
    while newPetrifyAmount > 0 and i < 6:
        newPetrifyAmount -= (1 - target.petrified[i])
        target.petrified[i] = 1
        i += 1
    return 0
KeywordFuncMap[KeywordIDs.PETRIFY] = petrify


def findStaticPips(state, source):
    side = source.currentSide
    pips = side.value
    if side.type != SideType.SUMMON and side.keywords[KeywordIDs.STASIS]:
        return pips
    
    pips += source.boost + source.permaboost - max(source.weaken, 0)
    if source.sourceID < 0: # if enemy
        pips += state.warchiefD # Mon Passive. Warchief increases enemy strength by 1 per warchief present
    else: # if hero
        pips += state.handD # Mon Passive. The Hand increases hero strength by 1 per hand present

    if side.type == SideType.SUMMON:
        return pips # No summon side has keywords, so that space is used to encode the summonID
    
    if (side.type in [SideType.DAMAGE, SideType.SHIELD, SideType.HEALSHIELD] or side.keywords[KeywordIDs.SELFSHIELD]):
        pips += source.smith
    
    
    for i in range(KeywordIDs.START_STATICS, KeywordIDs.START_PRE_MON_EFFECTS):
        if side.keywords[i]:
            pips += KeywordFuncMap[i](state, source, None, pips) # Target is irrelevant to static keywords
    return pips

def findDynamicPips(state, source, target, pips):
    side = source.currentSide
    if side.keywords[KeywordIDs.STASIS]:
        return pips
    
    for i in range(KeywordIDs.START_DYNAMICS, KeywordIDs.START_APPLIED_EFFECTS):
        if side.keywords[i]:
            pips += KeywordFuncMap[i](state, source, target, pips)
    return pips

def applyAffectKeywords(state, source, target, pips):
    side = source.currentSide
    for i in range(KeywordIDs.START_APPLIED_EFFECTS, KeywordIDs.ACT_OCCUR):
        if side.keywords[i]:
            pips += KeywordFuncMap[i](state, source, target, pips)

def completeActionKeywords(state, source, pips):
    side = source.currentSide
    if side.type == SideType.SUMMON:
        return
    
    for i in range(KeywordIDs.ACT_POST_OCCUR, KeywordIDs.NUM_KEYWORDS):
        if side.keywords[i]:
            pips += KeywordFuncMap[i](state, source, None, pips) # These also do not affect the target

