import copy

import numpy as np

"""
Fundamental actions, such as damage, heal, summon, res, handleDeaths, etc.
"""

from SADenv.lib import *
from SADenv.entIDs import *
from SADenv.ents import *
from SADenv.states import *
from SADenv.util import *
import SADenv.monPassives as monPassives
import SADenv.keywords as keywords

def damage(state : State, source : Ent, target : Ent, pips : int):
    if target.dodged or target.intangibled:
        return
    
    if target.redirectTarget:
        target = target.redirectTarget
    target.timesDamaged += 1
    pips += target.vulnerable
    
    if not source and target.sourceID == DEMON: # Mon Passive. Demons cannot take certain damage without a source (ie, a dice)
        for i in range(target.hp, max(0,target.hp - pips), -1): # start at hp. If 7 hp and 3 pips of damage, checck 7, 6, 5. Skip 7-3=4.
            if target.specialHP[i]:
                target.hp = i
                return
        # If no special hp, then can continue as normal

    if target.onHitFunction != -1:
        monPassives.PassiveFuncMap[target.onHitFunction](state, source, target, pips)


    # Edge Case:
    if target.sourceID == GOLEM and target.currentSide.keywords[KeywordIDs.STEEL]:
        index = state.enemies.index(target)
        hero = state.enemyTargets[index][0].redirectTarget or state.enemyTargets[index][0]
        hero.incomingDamage -= min(pips, target.shield)

    if pips > target.shield:
        target.hp -= max((pips - target.shield) - target.armor, 0)
        target.shield = 0
        if target.sourceID < 0: # if enemy
            state.enemyDamageD = True
    else:
        target.shield -= pips

    if target.undead and target.hp <= 0:
        target.hp = 1

def poisonDamage(state : State, source : Ent, target : Ent, pips : int):
    if target.intangibled:
        return
    if target.hp <= 0:
        return # Cant get bandit overkill damage from poison
    
    if target.sourceID == DEMON: # Mon Passive. Demons cannot take certain damage without a source (ie, a dice)
        for i in range(target.hp, max(0,target.hp - pips), -1): # start at hp. If 7 hp and 3 pips of damage, checck 7, 6, 5. Skip 7-3=4.
            if target.specialHP[i]:
                target.hp = i
                return
        # If no special hp, then can continue as normal
    
    target.timesDamaged += 1
    if target.onHitFunction != -1:
        monPassives.PassiveFuncMap[target.onHitFunction](state, source, target, pips)
    
    target.hp -= pips
    if target.undead and target.hp <= 0:
        target.hp = 1

def heal(state : State, target : Ent, pips : int):
    target.hp += pips # At end of action, hp is reset
    
def resurrect(state : State, pips : int):
    # Resurrecting Hero
    for i in range(5):
        if state.players[i].dead:
            pips -= 1
            resHero(state, i)
            if pips <= 0:
                break

def resHero(state: State, index: int):
    retargeting = []
    for i, monTargetData in enumerate(state.enemyTargets):
        for j, target in enumerate(monTargetData):
            if target == state.players[index]:
                retargeting.append((i, j))

    state.players[index] = copy.deepcopy(Lib.ALL_HEROS[state.players[index].sourceID]) # loads character with no afflictions of any kind
    state.players[index].usedDie = True
    
    num = np.random.randint(0, 6)
    state.players[index].currentSideNum = num
    state.players[index].currentSide = copy.deepcopy(state.players[index].sides[num])
    
    for i, j in retargeting:
        state.enemyTargets[i][j] = state.players[index]

def summon(state : State, source : Ent, entID : int):
    newEnemy = copy.deepcopy(Lib.ALL_MONSTERS[monIDtoPos(entID)]) # All enemies have negative sourceIDs, so we convert to positive
    newEnemy.usedDie = True

    if source:
        newEnemy.positionID = - source.positionID
    else:
        newEnemy.positionID = 0

    if hasRoom(state, newEnemy) and len(state.reinforcements) == 0: # Cannot summon if reinforcements are queued
        state.imminentSummons.append(newEnemy)
    else:
        state.reinforcements.append(newEnemy)

def repositionEnemies(state : State):
    for i, mon in enumerate(state.enemies):
        mon.positionID = i + 1

def modifyDamage(state : State, target : Ent, amt : int):
    if target.sourceID >= 0:
        return
    if target.usedDie:
        return
    
    if amt < 0:
        amt = max(amt, -keywords.findStaticPips(state, target))

    index = state.enemies.index(target)
    for hero in state.enemyTargets[index]:
        if hero.sourceID < 0:
            continue
        if hero.redirectTarget:
            hero = hero.redirectTarget
        hero.incomingDamage += amt
        if target.currentSide.keywords[KeywordIDs.POISON]:
            hero.incomingPoison += amt

def resetHealths(state : State):
    for ent in state.players + state.enemies:
        ent.hp = min(ent.maxHP, ent.hp)

def handleSDS(state : State):
    handleSummons(state)
    handleDeaths(state) # handleDeaths includes a summonsRound (ex: a bones breaking a summonHP)

def handleSummons(state : State):
    for newEnemy in sorted(state.imminentSummons, key=lambda x: x.positionID):
        posID = -newEnemy.positionID
        state.enemies.insert(posID, newEnemy)
        state.enemyTargets.insert(posID, [])
    state.imminentSummons = []
    repositionEnemies(state)

def handleDeaths(state : State):
    for hero in state.players:
        if hero.hp <= 0 and not hero.dead:
            hero.dead = True # nothing needs to be reset, since the hero will be reloaded from Lib.ALL_HEROS if resurrected
        if hero.undead:
            hero.dead = False
            if hero.hp <= 0:
                hero.hp = 1
    
    for i, mon in enumerate(state.enemies):
        if mon.hp <= 0: # This check is uncessary, but helps with clarity
            mon.dead = True
        if mon.sourceID == MILITIA:
            if len(state.enemyTargets[i]) > 0:
                targ = state.enemyTargets[i][0].redirectTarget or state.enemyTargets[i][0]
                if targ.shield >= 5: # if the target has 5 or more shield, then militia flees (dies, is equivalent)
                    mon.dead = True     # Im going to just check shields at the end of every action, instead of in the middle of a shielding action for convenience. Technically,
                                        # militias still flee on stuff like selfshield pain or selfshield spikymon, but this is one of the few things for which I really dont care

        

    index = len(state.enemies) - 1 # Must go in reverse for bone/barrel damage to apply properly
    processedIndices = []
    while index >= 0:
        dyingMon = state.enemies[index]
        if (dyingMon.dead or dyingMon.hp <= 0) and not (index in processedIndices):
            processedIndices.append(index)
            dyingMon.dead = True
            initIndex = index
            overkillHP = dyingMon.hp
            sourceID = dyingMon.sourceID

            if sourceID == BONES or sourceID == BARREL: # Bone damage applies to nearby enemies
                dmgAmt = 1 if sourceID == BONES else 5
                if initIndex > 0:
                    damage(state, None, state.enemies[initIndex - 1], dmgAmt)
                bonusIndex = initIndex + 1
                while bonusIndex < len(state.enemies) and state.enemies[bonusIndex].dead:
                    bonusIndex += 1
                if bonusIndex < len(state.enemies):
                    damage(state, None, state.enemies[bonusIndex], dmgAmt)
                    index = bonusIndex + 1 # ensures reprocessing of dead bone/barrel
            if overkillHP <= -2:
                if initIndex > 0 and state.enemies[initIndex - 1].sourceID == BANDIT:
                    state.enemies[initIndex - 1].dead = True
                bonusIndex = initIndex + 1
                while bonusIndex < len(state.enemies) and state.enemies[bonusIndex].dead:
                    bonusIndex += 1
                if bonusIndex < len(state.enemies) and state.enemies[bonusIndex].sourceID == BANDIT:
                    state.enemies[bonusIndex].dead = True
                    index = bonusIndex + 1 # ensures reprocessing of dead bandit
            
            # Mon state passives
            if sourceID == HEXIA:
                state.hexiaD = False
            elif sourceID == BANSHEE:
                if state.bansheeD > 0:
                    state.bansheeD -= 1
                else: # Cannot be 0, since a banshee was present to be killed
                    state.bansheeD += 1
            elif sourceID == BRAMBLE:
                state.brambleD = False
            elif sourceID == MADNESS:
                state.madnessD = False
            elif sourceID == THE_HAND:
                state.handD = False
            elif sourceID == WARCHIEF:
                state.warchiefD -= 1


            #state.enemies.pop(initIndex) # Done after a summons round, so that summons are added in the correct places
            if state.lastTarget == dyingMon:
                state.lastTarget = None
            
            modifyDamage(state, dyingMon, -999) # Remove all damage from the dead mon, so that it doesnt affect other heroes. This is done after the mon's onHitFunction is called, so that the damage can still be applied to other heroes if needed
            

            for i, monTargetData in enumerate(state.enemyTargets):
                for target in monTargetData:
                    if target == dyingMon:
                        monTargetData.remove(target)

        index -= 1
        if index >= len(state.enemies):
            index = len(state.enemies) - 1

    handleSummons(state)
    # Remove dead enemies
    index = len(state.enemies) - 1
    while index >= 0:
        if state.enemies[index].dead:
            state.enemies.pop(index)
            state.enemyTargets.pop(index)
        index -= 1

    # Handle Reinforcements
    while len(state.reinforcements) > 0 and hasRoom(state, state.reinforcements[0]):
        state.enemyTargets.insert(0, [])
        state.enemies.insert(0, state.reinforcements.pop(0))
    repositionEnemies(state)
    

    if len(state.enemies) == 1 and state.enemies[0].sourceID == GOBLIN:
        state.enemies.pop(0)

    return len(state.enemies) == 0 # returns true if all enemies are dead