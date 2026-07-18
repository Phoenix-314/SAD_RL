import copy
from typing import Tuple
import numpy as np

from bitarray import bitarray
from bitarray.util import ba2int

from SADenv.states import *
from SADenv.entIDs import *
from SADenv.ents import *
from SADenv.sourceEnts import *
from SADenv.keywordIDs import *
from SADenv.keywords import *
from SADenv.actions import *
from SADenv.spells import *
from SADenv.factions import *
from SADenv.util import *
from SADenv.validActions import *
from SADenv.fights import *

"""
Alongside factions, this contains the majority of specific game logic.
Includes the transition function, which takes in a state and an action, and returns the next state.
 - Transition will accept any action, even if invalid. Use validActions to determine if an action is valid.
 
Also includes the useDice, endTurn, endFight, and castSpell functions
"""

NUM_HEROS = 5
def transition(state : State, action : int):
    tmp = ACTION_MAP[action] # array of all 11*15+31+1 actions
    actionType = tmp[0]

    if actionType == REROLL_ACTION:
        rerollAction(state, tmp[1]) # actionmap[1] is bitarray for rerolls
        if state.rerolls == 0:
            state.stateType = StateType.EMPTY_REROLL
    
        
    if actionType == DICE_ALLY_ACTION: # Must handle possesses targeting as well as shield/heal/damage_ALL
        srcNum, targNum = tmp[1]

        targets = findTargets(state, srcNum, targNum, False)
        
        useDice(state, state.players[srcNum], targets)

        if (state.players[srcNum].currentSide.keywords[KeywordIDs.REPEL]): # Repel cannot be on an untargeted side
            targets = []
            for i, mon in enumerate(state.enemies):
                for targetEnt in state.enemyTargets[i]:
                    if targetEnt == state.players[targNum]: # if enemy's target is the hero that our repel side is targeting
                        targets.append(mon)
                        break # Only repel against an enemy once regardless of redirectincoming effects
            state.players[srcNum].currentSide.type = SideType.DAMAGE
            if len(targets) > 0:
                useDice(state, state.players[srcNum], targets)
            state.players[srcNum].currentSide.type = SideType.SHIELD


    elif actionType == DICE_ENEMY_ACTION:
        srcNum, targNum = tmp[1]

        targets = findTargets(state, srcNum, targNum, True)
        useDice(state, state.players[srcNum], targets)

    elif actionType == SPELL_ALLY_ACTION:
        splNum, targNum = tmp[1]
        if targNum == -1: # untargeted spells
            castSpell(state, None, splNum)
        else:
            castSpell(state, state.players[targNum], splNum)

    elif actionType == SPELL_ENEMY_ACTION:
        splNum, targNum = tmp[1]
        castSpell(state, state.enemies[targNum], splNum)
    
        
    elif actionType == END_TURN_ACTION:
        endTurn(state)

    elif actionType == CONTINUE_ACTION:
        if state.stateType == StateType.EMPTY_TURN:
            continueEndTurn(state)
        elif state.stateType == StateType.EMPTY_FIGHT:
            continueEndFight(state)
        elif state.stateType == StateType.EMPTY_REROLL:
            state.stateType = StateType.BATTLE
        else:
            raise Exception("CONTINUE_ACTION is only valid for EMPTY_TURN or EMPTY_FIGHT states, not stateType: " + str(state.stateType))
        
    resetHealths(state)

    handleSDS(state) # Summons and deaths are handled after every action, since they can occur from inflictedPain, inflictedPoison, etc.
    if len(state.enemies) == 0:
        endFight(state)
    else:
        lost = True
        for hero in state.players:
            if not hero.dead:
                lost = False
        if lost:
            state.stateType = StateType.LOST
    if state.turn > 30: # If stuck in a fight with regen and petrified out, or similar
        state.stateType = StateType.LOST

def findTargets(state: State, srcNum: int, targNum: int, targIsEnemy: bool) -> List[Ent]:
    untargeted = (targNum == -1)
    if untargeted:
        
        possessedHeroIndex, possessedHero = findMadnessedHero(state)
        possessed = (possessedHeroIndex == srcNum)
        sideType = state.players[srcNum].currentSide.type
        targets = []

        opposingUnits = state.players if possessed else state.enemies
        supportingUnits = state.enemies if possessed else state.players
        if sideType == SideType.DAMAGE_ALL:
            for hero in opposingUnits:
                if not hero.dead:
                    targets.append(hero)
        elif sideType == SideType.DAMAGE_EVERYONE:
            for hero in supportingUnits:
                if not hero.dead:
                    targets.append(hero)
            for mon in opposingUnits:
                if not mon.dead:
                    targets.append(mon)
        elif sideType in [SideType.HEAL_ALL, SideType.SHIELD_ALL]:
            for mon in supportingUnits:
                if not mon.dead:
                    targets.append(mon)
        else:
            return [] # Other untargeted sides need not have targets, such as mana or resurrect
        
        return targets

    else:
        playerSide = state.players[srcNum].currentSide
        hasCleave = hasCopycattedKeyword(state, playerSide, KeywordIDs.CLEAVE)
        hasDescend = hasCopycattedKeyword(state, playerSide, KeywordIDs.DESCEND)
        if not targIsEnemy:
            targets = [state.players[targNum]]
            if hasCleave:
                targets = findHeroCleaveTargets(state, state.players[targNum])
            elif hasDescend:
                targets = findHeroDescendTargets(state, state.players[targNum])
            return targets

        else:
            targets = [state.enemies[targNum]]
            if (hasCleave or hasDescend) and targNum < len(state.enemies) - 1:
                targets.append(state.enemies[targNum + 1])
            if hasCleave and targNum > 0:
                targets.append(state.enemies[targNum - 1])
            return targets

def castSpell(state : State, target: Ent, splNum : int):
    spellID = Spell.BURST
    if splNum > 0:
        spellID = state.players[splNum - 1].spellID

    spellCost = SpellCostMap[spellID] + max(0, state.spellData[splNum]) # positive values for splNum indicate DEPLETE costs 
    if SpellReqsMap[spellID] in SpellTargetReq.ALL_TACTICS:
        usedHeros = findTacticCosts(state, SpellReqsMap[spellID])
        for hero in usedHeros:
            hero.usedDie = True
    else:
        state.mana -= spellCost

    
    castAffectType = SpellCastAffectMap[spellID]
    if castAffectType == SpellCastAffect.COOLDOWN:
        state.spellData[splNum] = SpellData.COOLDOWN_USED
    elif castAffectType == SpellCastAffect.DEPLETE:
        state.spellData[splNum] += 1
    elif castAffectType == SpellCastAffect.SINGLE_CAST:
        state.spellData[splNum] = SpellData.SINGLE_CASTED

    if target and target.sourceID == THORN and not (SpellReqsMap[spellID] in SpellTargetReq.ALL_THORN_SPECIALS):
                                                # Cannot do anything to thorns, aside from repeatedly harvest them
                                                # However, untargeted spells can still be cast and are specifically edited to avoid hitting thorns
        return

    # Actually using the spell
    SpellFuncMap[spellID](state, target)

    # Mon Passives
    if state.bansheeD > 0:
        for hero in state.players:
            damage(state, None, hero, state.bansheeD)
        state.bansheeD *= -1 # negative bansheeD indicates that banshee has screamed
    if state.hexiaD:
        hero = findBottomPlayer(state)
        if hero:
            damage(state, None, hero, spellCost)

def useDice(state : State, source : Ent, targets: List[Ent]):
                                                                    # TargetIndex, TargetEnt
    backup = copy.deepcopy(source.currentSide) # backup in case of copycat
    side = source.currentSide
    if side.type != SideType.SUMMON and side.keywords[KeywordIDs.COPYCAT]:
        KeywordFuncMap[KeywordIDs.COPYCAT](state, source, None, 0)
    copyCattedSingleUse = False if side.type == SideType.SUMMON else side.keywords[KeywordIDs.SINGLEUSE]
    staticPips = findStaticPips(state, source)
    bonusCenterDynamicPips = (findDynamicPips(state, source, targets[(len(targets) - 1) // 2], staticPips) - staticPips) if len(targets) > 0 else 0
    if side.type in SIDES_AUTO_USABLE:
        staticPips = 0
    state.prevLastStrikeVal = state.lastStrikeVal
    state.lastStrikeVal = staticPips
    sourceIsHero = (source.sourceID >= 0)
    state.lastDie = copy.deepcopy(side) # for copycat, chain, etc. # Additional keywords appended below for inflicteds
     
    
    # Side usage (rampage and rescue in respective actions)
    useSide = True
    source.timesUsed += 1
    if sourceIsHero and ((side.keywords[KeywordIDs.DOUBLEUSE] and source.timesUsed < 2) or (side.keywords[KeywordIDs.QUADUSE] and source.timesUsed < 4)):
        useSide = False


    if (sourceIsHero or source.sourceID == GOLEM) and side.keywords[KeywordIDs.SELFSHIELD]:
        KeywordFuncMap[KeywordIDs.SELFSHIELD](state, source, None, staticPips + bonusCenterDynamicPips)

    # Handling inflicteds. Must occur first to prevent self-cleanse
    if source.inflictedDeathed:
        source.dead = True
        if sourceIsHero:
            state.lastDie.keywords[KeywordIDs.DEATH] = True
    if source.inflictedExerted:
        source.exerted = True
        source.toexert = True
        if sourceIsHero:
            state.lastDie.keywords[KeywordIDs.EXERT] = True
    if source.inflictedPained and not (sourceIsHero and side.keywords[KeywordIDs.PAIN]):
        if source.sourceID == DEMON: # Minor bug fix. Allows inflict pain to damage demonHP. All other "sourceless" damage(...) cannot hurt demon hp
            factions.damage(state, source, source, staticPips)
        else:
            factions.damage(state, None, source, staticPips)
        if sourceIsHero:
            state.lastDie.keywords[KeywordIDs.PAIN] = True

    

    # The actual action
    if len(targets) == 0: # Untargeted sides
        if (side.type == SideType.MANA):
            state.mana += staticPips
            if side.keywords[KeywordIDs.BONED]:
                summon(state, None, BONES)
        elif (side.type == SideType.SUMMON):
            eneID = -1 * ba2int(side.keywords) # enemy IDs are negative
            if eneID in [THORN, CAW, DRAGON]:
                source.dead = True # These particular enemies's seeds/eggs have Death
            
            for i in range(staticPips):
                summon(state, source, eneID)
        elif (side.type == SideType.RESURRECT):
            resPips = staticPips
            resurrect(state, resPips)
            
        elif side.type == SideType.DODGE:
            source.dodged = True
        elif side.type == SideType.SELF_DAMAGE:
            damage(state, None, source, staticPips)
        elif side.type == SideType.REROLL:
            state.rerolls += staticPips
        else:
            raise Exception("Side type needs a target: " + sidesReversedIDs[side.type])
    
    for target in targets:
        if source.sourceID < 0 and target.dodged:
            continue # Cannot apply affect keywords or anything else to a dodged target
        if source.sourceID < 0 and target.redirectTarget: # if enemy is hitting an ally with redirect, then all effects (including applyAffectKeywords) are redirected
            target = target.redirectTarget
            
        pips = findDynamicPips(state, source, target, staticPips)

        if (side.type in [SideType.DAMAGE, SideType.DAMAGE_ALL, SideType.DAMAGE_EVERYONE, SideType.DAMAGE_FLANKING]):
            damage(state, source, target, pips)
            if side.keywords[KeywordIDs.RAMPAGE] and target.hp <= 0:
                useSide = False   
            if side.keywords[KeywordIDs.GUILT] and target.hp <= 0:
                source.dead = True

        elif (side.type in [SideType.SHIELD, SideType.SHIELD_ALL]):
            previouslyDying = targetIsDying(state, target)
            target.shield += pips # shielding
            if side.keywords[KeywordIDs.RESCUE] and previouslyDying and not targetIsDying(state, target):
                useSide = False
        
        elif (side.type == SideType.HEALSHIELD):
            target.shield += pips
            heal(state, target, pips)

        elif (side.type in [SideType.HEAL, SideType.HEAL_ALL]):
            previouslyDying = targetIsDying(state, target)
            heal(state, target, pips)
            if side.keywords[KeywordIDs.RESCUE] and previouslyDying and not targetIsDying(state, target):
                useSide = False
                
        elif (side.type == SideType.RECHARGE):
            target.usedDie = False
            target.timesUsed = 0

        elif (side.type == SideType.REDIRECTINCOMING):
            if source != target:
                target.redirectTarget = source
                source.incomingDamage += target.incomingDamage
                source.incomingPoison += target.incomingPoison
                target.incomingDamage = 0
                target.incomingPoison = 0
    
        elif side.type == SideType.ENCHANT:
            target.currentSide.keywords = target.currentSide.keywords | side.keywords # enchant sides are ENCHANT, 0 pips, keywords=[things to enchant]
        # Bonus sidetypes
        elif side.type == SideType.KILL:
            target.dead = True # checking validity done in valid_actions function
        elif side.type == SideType.UNDYING:
            target.undead = True
        elif side.type == SideType.STUN:
            factions.modifyDamage(state, target, -999)
            target.usedDie = True
        else:
            raise Exception("Side type is not allowed for useDice() with targets: " + str(side.type))     # BLANK, REROLL
    
        applyAffectKeywords(state, source, target, pips) # weaken, poison, inflict affect every target, but vulnerable goes after

    # Using side. Must be after rampage and rescue checks
    if useSide:
        source.usedDie = True
    
    completeActionKeywords(state, source, staticPips + bonusCenterDynamicPips)
    source.currentSide = backup # restore side in case of copycat

    if (len(targets) <= 0 or len(targets) >= 4): # if no targets or 4+ targets, then focus is not applicable
        state.lastTarget = None
    else:
        state.lastTarget = targets[(len(targets) - 1) // 2]

    if (side.type != SideType.SUMMON and (source.currentSide.keywords[KeywordIDs.SINGLEUSE] or copyCattedSingleUse) or (source.sourceID >= 0 and state.brambleD) or source.inflictedSingleUse): # Mon passive
        source.currentSide.type = SideType.BLANK
        source.sides[source.currentSideNum].type = SideType.BLANK
        state.lastDie.keywords[KeywordIDs.SINGLEUSE] = True
        
def endTurn(state: State):
    # Step 0: Mana, can gain mana btwn turns which exceeds 3
    if state.mana > 3:
        state.mana = 3

    # Step 0.75: A few affects must be reset so enemies might reapply them. Enemies, on the other hand, have these affects cleared after attacking
    for mon in state.players:
        mon.weaken = min(0, mon.weaken)
        mon.inflictedDeathed = False
        mon.inflictedExerted = False
        mon.inflictedPained = False
        mon.inflictedSingleUse = False
    # Step 1: Enemy Turns including passives (bell, ...)
    enemyAttacks(state)
    # Step 1.25:
    for mon in state.enemies:
        mon.weaken = 0
        mon.inflictedDeathed = False
        mon.inflictedExerted = False
        mon.inflictedPained = False
        mon.inflictedSingleUse = False

    # Step 2: Applying poison/regen + bell/rotten
    resetHealths(state)
    for mon in state.players:
        delta = max(mon.poison, 0) - mon.regen
        if delta > 0:
            poisonDamage(state, None, mon, delta)
        else:
            heal(state, mon, -delta)
    for mon in state.enemies:
        delta = max(mon.poison, 0) - mon.regen
        if delta > 0:
            poisonDamage(state, None, mon, delta)
        else:
            heal(state, mon, -delta)
    # Mon passives
    bell = None
    rotten = None
    for mon in state.enemies:
        if mon.sourceID == BELL:
            bell = mon
        if mon.sourceID == ROTTEN:
            rotten = mon
    if bell is not None:
        for hero in state.players:
            damage(state, None, hero, 1)
        damage(state, None, bell, 5) # Bell self-damage
    if rotten is not None:
        for hero in state.players:
            hero.maxHP = max(1, hero.maxHP - 1)
            hero.hp = min(hero.hp, hero.maxHP)

    # Step 3: Deaths and summons round
    handleSDS(state)
    if len(state.enemies) == 0:
        endFight(state)
        return # endFight resets state properly

    # Step 4: Resetting all usedDie, exerted, dodged, intangibled, redirectTarget. Inflicted/weaken was handled above
    # 4.Ents
    for ent in state.players + state.enemies:
        ent.shield = 0 if ent.sourceID != GOLEM else ent.shield
        ent.undead = False
        ent.dodged = False
        ent.intangibled = False
        
        if not ent.dead:
            ent.usedDie = False
        else:
            ent.usedDie = True
        ent.timesUsed = 0
        ent.timesDamaged = 0
        
        ent.cleansed = False
        ent.poison = max(ent.poison, 0) # poison may be negative to represent remaining cleanse for poison
        ent.weaken = max(ent.weaken, 0)
        ent.petrifyAmount = max(ent.petrifyAmount, 0)


        if ent.toexert:
            ent.exerted = True
        else:
            ent.exerted = False
        ent.toexert = False

        ent.smith = 0
        ent.boost = 0
        ent.vulnerable = 0
        if ent.sourceID == CAW:
            ent.backrow = False

        ent.redirectTarget = None

        ent.incomingDamage = 0
        ent.incomingPoison = 0
        
        ent.currentSideNum = -1
        ent.currentSide = None
    
    # 4.State
    state.enemyTargets = [[] for _ in range(len(state.enemies))]
    state.mana += state.fortoldMana
    state.fortoldMana = 0
    state.rerolls = 3 - state.rerolls # state.rerolls becomes negative for gaze spell. Else, we force player to roll down.
    for v in range(6):
        if state.spellData[v] == SpellData.COOLDOWN_USED:
            state.spellData[v] = SpellData.STANDARD
    state.turn += 1
    state.lastTarget = None
    state.prevLastStrikeVal = -1
    state.lastStrikeVal = -1
    state.lastDie = None
    # Mon Passives
    state.enemyDamageD = False
    if state.bansheeD < 0:
        state.bansheeD *= -1 # Negative bansheeD indicates that banshee has screamed, so its reset for the next turn

    state.stateType = StateType.EMPTY_TURN
    # Steps 5 and 6 are done after intermediate empty state

def continueEndTurn(state: State):
    # Step 5: Rolling all enemy dice, assigning inflictedDamage, etc.
    rollEnemyDice(state)
    # Step 6: Rolling all player dice
    rerollAction(state, bitarray('11111')) # reroll all players

    state.stateType = StateType.BATTLE

def enemyAttacks(state: State):
    for i, mon in enumerate(state.enemies):
        if not mon.usedDie and not mon.exerted and findStaticPips(state, mon) > 0: # if the enemy has strength to use a die
            useDice(state, mon, state.enemyTargets[i])
        handleSDS(state) # Theoretically, a pained bones or a bandit could have death below them, stopping other enemies from attacking

    # More passives for fleeing
    logged = True
    for mon in state.enemies:
        if not state.enemyDamageD and mon.sourceID == BLIND: # Blind flees (dies, equivalently) if no enemy was damaged this turn, prior to handleDeaths
            mon.dead = True
        if state.turn == 1 and mon.sourceID == CHEST: # Autoflee
            mon.dead = True
        if state.turn == 3 and mon.sourceID == SARCOPHAGUS:
            mon.dead = True
        if logged and mon.sourceID != LOG:
            logged = False

    if logged: # Log flees if no other enemy types are present
        for mon in state.enemies:
            mon.dead = True


def initial() -> State:
    state = State()
    # Heros
    state.players = [None for i in range(NUM_HEROS)]
    for i in range(NUM_HEROS):
        newHeroId = np.random.choice(Lib.HERO_TREE[i][0])
        state.players[i] = copy.deepcopy(Lib.ALL_HEROS[newHeroId])
    
    state.level = 0
    state.playerLevels = [1 for _ in range(NUM_HEROS)]

    # Resets state, rolls enem dice, rolls player dice
    endFight(state) # resets all state values, including enemies, mana, etc. and rolls all dice
    continueEndFight(state) # rolls all player dice, levels up a hero if applicable, and rolls all enemy dice
    return state

def genState(playerIDs, enemIDs, autoAllyRolls=None, autoEnemyRolls=None) -> State:
    state = State()

    state.players = [None for i in range(NUM_HEROS)]
    for i in range(NUM_HEROS):
        state.players[i] = copy.deepcopy(Lib.ALL_HEROS[playerIDs[i]])
    resetState(state)
    
    state.enemies = []
    for monID in enemIDs:
        state.enemies.append(copy.deepcopy(Lib.ALL_MONSTERS[monIDtoPos(monID)]))
        if monID == HEXIA:
            state.hexiaD=True
        if monID == BANSHEE:
            state.bansheeD += 1
        if monID == BRAMBLE:
            state.brambleD=True
        if monID == MADNESS:
            state.madnessD=True
        if monID == THE_HAND:
            state.handD=True
        if monID == WARCHIEF:
            state.warchiefD += 1
    repositionEnemies(state)  
    
    if autoEnemyRolls is not None:
        rollEnemies(state, autoEnemyRolls)
    else:
        rollEnemyDice(state)
    
    if autoAllyRolls is not None:
        rollAllies(state, autoAllyRolls)
    else:
        rerollAction(state, bitarray('11111')) # reroll all players
    
    handleSDS(state)
    state.stateType = StateType.BATTLE
    return state

def endFight(state: State):
    # Step 1: Reset state and load enemies
    resetState(state)

    # Steps 1.5-4: After empty state
    state.stateType = StateType.EMPTY_FIGHT
    if state.level == 20:
        state.stateType = StateType.WON
        

def continueEndFight(state: State):
    # Step 1.5: Generate enemies
    generateEnemies(state)

    # Step 2: (Maybe) level-up a hero
    state.level += 1
    if state.level % 2 == 0: # Every 2 levels, level up a hero (Randomly for now, TODO)
        levelHero(state)

    # Step 3: Roll Enemies
    rollEnemyDice(state)

    # Step 4: Roll Players
    rerollAction(state, bitarray('11111')) # reroll all players
    
    state.stateType = StateType.BATTLE

def levelHero(state: State):
    tier = 2 if state.level <= 10 else 3
    levelableHeroCols = [i for i in range(5) if state.playerLevels[i] < tier]
    levelUpColor = np.random.choice(levelableHeroCols)
    state.playerLevels[levelUpColor] += 1
    newHeroId = np.random.choice(Lib.HERO_TREE[levelUpColor][tier - 1])
    state.players[levelUpColor] = copy.deepcopy(Lib.ALL_HEROS[newHeroId]) # loads character with no afflictions of any kind

def generateEnemies(state: State):
    data = generateFight(state.level) # Tuple of monster sourceIDs # Level hasnt increased yet, but also generateFight is 0-indexed, so +1-1=0
    for monID in data:
        state.enemies.append(copy.deepcopy(Lib.ALL_MONSTERS[monIDtoPos(monID)]))

        if monID == HEXIA:
            state.hexiaD=True
        if monID == BANSHEE:
            state.bansheeD += 1
        if monID == BRAMBLE:
            state.brambleD=True
        if monID == MADNESS:
            state.madnessD=True
        if monID == THE_HAND:
            state.handD=True
        if monID == WARCHIEF:
            state.warchiefD += 1
    repositionEnemies(state)  

def resetState(state: State):
    for i in range(5):
        state.players[i] = copy.deepcopy(Lib.ALL_HEROS[state.players[i].sourceID]) # loads character with no afflictions of any kind
    state.spellData = [SpellData.STANDARD for _ in range(6)]
    
    state.mana = 0
    state.fortoldMana = 0
    state.rerolls = 3
    
    #MonPassives:
    state.enemyDamageD = False
    state.hexiaD=False
    state.bansheeD=0
    state.brambleD=False
    state.madnessD=False
    state.handD=False
    state.warchiefD=0
    # ENEMIES
    state.enemies=[]        
    state.enemyTargets=[] # will be set once enemy dice are rolled
    
    state.imminentSummons=[]
    state.reinforcements=[]

    # General state data
    state.turn = 1
    state.lastTarget = None
    state.prevLastStrikeVal = -1 # for trio
    state.lastStrikeVal = -1 # like lastdie, but with keywords evaluated
    state.lastDie = None # for chain, copycat, etc.

    # MonPassives above enemy generation

def rollEnemies(state: State, rolls: List[int]):
    state.enemyTargets = [None for _ in range(len(state.enemies))]
    for i, mon in enumerate(state.enemies):
        rollEnemyDie(state, i, mon, rolls[i])

def rollEnemyDice(state: State):
    state.enemyTargets = [None for _ in range(len(state.enemies))]
    for i, mon in enumerate(state.enemies):
        num = np.random.randint(0,6)
        rollEnemyDie(state, i, mon, num)

def rollEnemyDie(state: State, monIndex: int, mon: Ent, num: int):
    mon.currentSideNum = num
    mon.currentSide = mon.sides[mon.currentSideNum]
    if mon.exerted:
        state.enemyTargets[monIndex] = [] # Exerted enemies cannot attack, so they have no targets
        return

    targets = getRandomTargetForEnemy(state, monIndex, mon) # List[Ent] of targets
    state.enemyTargets[monIndex] = targets

    factions.modifyDamage(state, mon, findStaticPips(state, mon)) # Mon passives that modify damage (like banshee) are applied before the attack

def getRandomTargetForEnemy(state: State, index: int, mon: Ent) -> List[Ent]:
    side = mon.currentSide
    if side.type in [SideType.HEAL, SideType.HEAL_ALL, SideType.BLANK, SideType.SUMMON, SideType.KILL, SideType.DAMAGE_ALL, SideType.DAMAGE_EVERYONE, SideType.DAMAGE_FLANKING]: # All of these have very specific required targets
        if side.type == SideType.HEAL:
            return [state.enemies[index]] # heal self
        elif side.type == SideType.HEAL_ALL:
            return [ene for ene in state.enemies] # heal all allies
        elif side.type == SideType.BLANK or side.type == SideType.SUMMON:
            return [] # blank has no target
        elif side.type == SideType.KILL:
            return [findTopPlayer(state)] # kill top player
        elif side.type == SideType.DAMAGE_ALL:
            return [hero for hero in state.players if not hero.dead]
        elif side.type == SideType.DAMAGE_EVERYONE:
            a = [hero for hero in state.players if not hero.dead]
            b = [ene for ene in state.enemies] # No enemies can be dead
            return a + b
        elif side.type == SideType.DAMAGE_FLANKING:
            a = findTopPlayer(state)
            b = findBottomPlayer(state)
            if a == b:
                return [a]
            return [a, b]
        raise Exception("Side type is not allowed for getRandomTargetForEnemy(): " + str(side.type))
    
    # If not above, only remaining sidetypes are damage (damage cleave, damage descend, eliminate/heavy)
    if (side.keywords[KeywordIDs.ELIMINATE]):
        return [getHpRestrictTargetForEnemy(state, False)]
    if (side.keywords[KeywordIDs.HEAVY]):
        return [getHpRestrictTargetForEnemy(state, True)]
    

    for i in range(4):
        potentialTarget: Ent = getRandomTargetForEnemyInternal(state, mon, (i & 1) > 0, (i & 2) > 0);
        if (potentialTarget == None):
            continue
        if (side.keywords[KeywordIDs.CLEAVE]):
            return findHeroCleaveTargets(state, potentialTarget)
        if (side.keywords[KeywordIDs.DESCEND]):
            return findHeroDescendTargets(state, potentialTarget)
        return [potentialTarget]
    
    return None

def getHpRestrictTargetForEnemy(state: State, isHeavy: bool):
    potentials = copy.copy(state.players) # Shallow copy is necessary to keep references properly
    np.random.shuffle(potentials)
    target = None
    bestHP = -999 if isHeavy else 999
    for i, hero in enumerate(potentials):
        if not hero.dead:
            futureHP = hero.hp - hero.incomingDamage - max((hero.poison + hero.incomingPoison), 0) # ignoring regen, since that appears to be what SAD does

            if ((futureHP > bestHP) != isHeavy):
                continue
            bestHP = futureHP
            target = hero
    return target

def getRandomTargetForEnemyInternal(state: State, mon: Ent, allowOverkill: bool, allowSuboptimal: bool):
    side = mon.currentSide
    validTargets = [hero for hero in state.players if not hero.dead]
    if not allowOverkill:
        for i in range(len(validTargets) - 1, -1, -1):
            
            hero = validTargets[i]
            if (not targetIsDying(state, hero)):
                continue
            validTargets.pop(i)
    
    if not allowSuboptimal:
        if side.keywords[KeywordIDs.CLEAVE]:
            if len(validTargets) < 3:
                return None
            
            validTargets.pop(0)
            validTargets.pop(len(validTargets) - 1)
        
        if (side.keywords[KeywordIDs.DESCEND]):
            if (len(validTargets) < 2):
                return None
            validTargets.pop(len(validTargets) - 1)

    if len(validTargets) > 0:
        idx = np.random.randint(0, len(validTargets))
        return validTargets[idx]
    return None

def rollAllies(state: State, rolls: List[int]):
    for i in range(NUM_HEROS):
        rollAllyDie(state, state.players[i], rolls[i])
    state.rerolls -= 1

def rerollAction(state : State, rerollBits : bitarray):
    if state.rerolls <= 0:
        raise Exception("Cannot reroll when rerolls is 0 or less")
    
    for i in range(NUM_HEROS):
        if rerollBits[i]:
            num = np.random.randint(0,6)
            rollAllyDie(state, state.players[i], num)

    state.rerolls -= 1
    return True

def rollAllyDie(state: State, player: Ent, num: int):
    player.currentSideNum = num
    newSide = player.sides[num]
    player.currentSide = copy.deepcopy(newSide)
    
    cantripped = hasCopycattedKeyword(state, newSide, KeywordIDs.CANTRIP)
    if not player.dead and cantripped and diceUsable(state, state.players.index(player)): # cantrip is only usable if the die is usable (not exerted, not singleuse, etc.)
        # First 2 ifs require targets
        possessed = (findMadnessedHero(state)[1] == player)
        
        if newSide.type == SideType.DAMAGE: # select random enemy
            target = state.enemies[np.random.randint(0, len(state.enemies))]
            if possessed: # if possessed, then target ally instead of enemy
                v = []
                for ent in state.players:
                    if not ent.dead:
                        v.append(ent)
                target = v[np.random.randint(0, len(v))]

            useDice(state, player, [target])
        elif newSide.type == SideType.SHIELD: # select non-random ALIVE ally that is not yet fully shielded
            v = []
            for ent in state.players:
                if not ent.dead and ent.shield < ent.incomingDamage:
                    v.append(ent)
            if len(v) == 0: # if no allies are alive and not fully shielded, then target self
                for ent in state.players:
                    if not ent.dead:
                        v.append(ent)
            if possessed:
                v = copy.copy(state.enemies)
            
            target = v[np.random.randint(0, len(v))]
            useDice(state, player, [target])

        elif newSide.type in [SideType.MANA, SideType.REROLL, SideType.DODGE, SideType.SELF_DAMAGE]:
            useDice(state, player, [])
        elif newSide.type == SideType.BLANK:
            pass
        else:
            raise Exception("Cantrip side that is not damage, shield, rerolls, or mana or DODGE. Instead, it is: " + sidesReversedIDs[newSide.type])
        # only damage, shield, dodge,and mana types exist

        player.usedDie = False
        player.timesUsed = 0 # useDice(...) uses the dice, but cantrip shouldnt do this when triggered via reroll
    
def isTerminal(state: State) -> bool:
    if state.stateType == StateType.WON or state.stateType == StateType.LOST:
        return True
    return False