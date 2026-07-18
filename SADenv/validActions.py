import copy
from typing import Tuple
import numpy as np

from bitarray import bitarray

import SADenv.spells as spells
from SADenv.states import *
from SADenv.ents import *
from SADenv.keywords import *
from SADenv.actions import *
from SADenv.entIDs import *
from SADenv.factions import *
from SADenv.util import *

"""
Returns a list of valid actions for the current state with validActions(state)
 - All checking for valid actions is done here. Transition will accept an invalid action without throwing an error, but may produce odd results

Also includes diceUsable
"""

def diceUsable(state: State, sourceNum: int) -> bool:
    # Doesn't check side types
    if state.players[sourceNum].dead or state.players[sourceNum].usedDie or state.players[sourceNum].exerted:
        return False
    if state.players[sourceNum].petrified[state.players[sourceNum].currentSideNum] and not state.players[sourceNum].currentSide.keywords[KeywordIDs.STASIS]: # Stasis overrides petrify
        return False
    if state.players[sourceNum].currentSide.type == SideType.BLANK:
        return False

    autoUsableSide = state.players[sourceNum].currentSide.type in SIDES_AUTO_USABLE
    if not autoUsableSide:
        staticPips = findStaticPips(state, state.players[sourceNum])
        if staticPips <= 0:
            return False
    
    return True

def checkBasicValidity(state: State, sourceNum: int, targetNum: int, targIsEnemy: bool) -> bool:
    """
    Checks that are irrelevant to side type
    """
    if not diceUsable(state, sourceNum):
        return False
    
    curSide = state.players[sourceNum].currentSide
    currSideKeywords = curSide.keywords
    if targIsEnemy:
        if targetNum >= len(state.enemies):
            return False
        
        if state.enemies[targetNum].backrow:
            allAreBackrow = True
            for enemy in state.enemies:
                if not enemy.backrow:
                    allAreBackrow = False
                    break
            hasRanged = hasCopycattedKeyword(state, curSide, KeywordIDs.RANGED)
            if not (hasRanged or allAreBackrow):
                return False
        if currSideKeywords[KeywordIDs.HEAVY]:
            if (targetNum not in findMaxHP(state, True, True)):
                return False
        if currSideKeywords[KeywordIDs.ELIMINATE]:
            if (targetNum not in findMaxHP(state, False, True)):
                return False
    else:
        if targetNum != -1 and state.players[targetNum].dead:
            return False
        if targetNum == -1:
            return state.players[sourceNum].currentSide.type in [SideType.MANA, SideType.RESURRECT, SideType.DODGE, SideType.DAMAGE_ALL, SideType.DAMAGE_EVERYONE, SideType.HEAL_ALL, SideType.SHIELD_ALL, SideType.SELF_DAMAGE]
        elif state.players[sourceNum].currentSide.type in [SideType.MANA, SideType.RESURRECT, SideType.DODGE, SideType.DAMAGE_ALL, SideType.DAMAGE_EVERYONE, SideType.HEAL_ALL, SideType.SHIELD_ALL, SideType.SELF_DAMAGE]:
            return False
        
        if currSideKeywords[KeywordIDs.HEAVY]: # Technically only relevant if the source is possessed
            return targetNum in findMaxHP(state, True, False)
        if currSideKeywords[KeywordIDs.ELIMINATE]:
            return targetNum in findMaxHP(state, False, False)
    
    return True

def canUsedPossessedDie(state: State, sourceNum: int) -> bool: # Used to see if mandatory should be applied
    if not diceUsable(state, sourceNum):
        return False
    sideType = state.players[sourceNum].currentSide.type
    if sideType == SideType.BLANK or sideType == SideType.REROLL or sideType == SideType.RECHARGE:
        return False
    if sideType == SideType.RESURRECT:
        for hero in state.players:
            if hero.dead:
                return True
        return False
    if sideType == SideType.KILL:
        for hero in state.players:
            if hero.hp <= findStaticPips(state, state.players[sourceNum]):
                return True
        return False
    if sideType == SideType.STUN:
        return True # Can always self-stun
    
    return True # Most sides can be used

def isValidDiceAction(state: State, targIsEnemy: bool, diceAction: Tuple[int, int]) -> bool:
    sourceNum, targetNum = diceAction

    if not checkBasicValidity(state, sourceNum, targetNum, targIsEnemy):
        return False

    possessed = (findMadnessedHero(state)[0] == sourceNum)
    if possessed:
        targIsEnemy = not targIsEnemy # Pretending to be an enemy handles most logic:
            # Untargeted sides still just check targetNum, the different targets will be handled in transition
            # Sides that typically target enemies will only accept initial inputs that were verified to target a valid ally
            # Similar for target allies.
            # RECHARGE cannot be used, RESURRECT still targets allies, and KILL & STUN have new requirements

    
    
    # Side Types
    staticPips = findStaticPips(state, state.players[sourceNum])
    sideType = state.players[sourceNum].currentSide.type
    
    if sideType == SideType.DAMAGE:
        return targIsEnemy
    if sideType == SideType.SHIELD:
        return not targIsEnemy
    if sideType == SideType.HEAL:
        return not targIsEnemy
    if sideType == SideType.HEALSHIELD:
        return not targIsEnemy
    if sideType == SideType.MANA:
        return targetNum == -1 # untargeted
    if sideType == SideType.BLANK:
        return False
    if sideType == SideType.REROLL:
        return False
    if sideType == SideType.REDIRECTINCOMING:
        return not targIsEnemy
    if sideType == SideType.SUMMON:
        raise ValueError(f"Invalid input in isValidDiceAction for sourceNum {sourceNum} and targetNum {targetNum}")
    if sideType == SideType.RECHARGE:
        if possessed:
            return False
        return (not targIsEnemy) and state.players[targetNum].usedDie and state.players[targetNum].currentSide.type != SideType.RECHARGE
    if sideType == SideType.RESURRECT:
        if targetNum != -1:
            return False
        for hero in state.players:
            if hero.dead:
                return True
        return False
    if sideType == SideType.ENCHANT:
        return not targIsEnemy
    if sideType == SideType.KILL:
        if possessed:
            return targIsEnemy and state.players[targetNum].hp <= staticPips
        return targIsEnemy and state.enemies[targetNum].hp <= staticPips
    if sideType == SideType.UNDYING:
        return not targIsEnemy
    if sideType == SideType.DODGE:
        return targetNum == -1 # untargeted
    if sideType == SideType.SELF_DAMAGE:
        return targetNum == -1 # untargeted
    if sideType == SideType.DAMAGE_ALL:
        return targetNum == -1 # untargeted
    if sideType == SideType.DAMAGE_EVERYONE:
        return targetNum == -1 # untargeted
    if sideType == SideType.DAMAGE_FLANKING:
        raise ValueError(f"Invalid input in isValidDiceAction for sourceNum {sourceNum} and targetNum {targetNum}")
    if sideType == SideType.SHIELD_ALL:
        return targetNum == -1 # untargeted
    if sideType == SideType.HEAL_ALL:
        return targetNum == -1 # untargeted
    if sideType == SideType.STUN:
        if possessed:
            return targIsEnemy and state.players[targetNum].hp <= state.players[sourceNum].hp
        return targIsEnemy and state.enemies[targetNum].hp <= state.players[sourceNum].hp
    
    raise ValueError(f"Unhandled case in isValidDiceAction for sourceNum {sourceNum} and targetNum {targetNum}")
    return True or False
    
    

def isValidSpellAction(state: State, targIsEnemy: bool, spellAction: Tuple[int, int]) -> bool:
    spellColor, targetNum = spellAction
    
    if state.spellData[spellColor] < 0: # Spell not available # SpellData.COOLDOWN_USED=-1, SpellData.SINGLE_CASTED=-2
        return False
    if spellColor != 0:
        if state.players[spellColor - 1].spellID == -1: # Character has no spell
            return False
        if state.players[spellColor - 1].dead: # Dead characters cannot cast spells
            return False
    
    
    if targIsEnemy and targetNum >= len(state.enemies):
        return False
    if targIsEnemy and state.enemies[targetNum].backrow:
        allAreBackrow = True
        for enemy in state.enemies:
            if not enemy.backrow:
                allAreBackrow = False
                break
        if not allAreBackrow:
            return False # Cannot target backrow enemies with spells (except for untargeted/cleave spells, which do not have targIsEnemy=True/target a backrow)
    if not targIsEnemy and (targetNum != -1 and state.players[targetNum].dead):
        return False
    
    
    spellID = state.players[spellColor - 1].spellID if spellColor > 0 else spells.Spell.BURST
    spellReq = spells.SpellReqsMap[spellID]
    spellCost = spells.SpellCostMap[spellID] + max(0, state.spellData[spellColor]) # positive values for spellColor indicate DEPLETE costs
    canCastCost = state.mana >= spellCost
    if not canCastCost and not spellReq in spells.SpellTargetReq.ALL_TACTICS: # Tactics can be cast without mana, but still need to meet targeting requirements
        return False
    
    if targetNum == -1:
        if not spellReq in [spells.SpellTargetReq.UNTARGETED, spells.SpellTargetReq.RESURRECT, spells.SpellTargetReq.DAMAGE_N_SHIELD_3]:
            return False


    if spellReq == spells.SpellTargetReq.UNTARGETED:
        return targetNum == -1
    elif spellReq == spells.SpellTargetReq.ENEMY:
        return targIsEnemy
    elif spellReq == spells.SpellTargetReq.ALLY:
        return not targIsEnemy
    elif spellReq == spells.SpellTargetReq.EITHER:
        return True # General checks done above for targeting alive/existant enemies/allies
    elif spellReq == spells.SpellTargetReq.RESURRECT:
        if targetNum != -1:
            return False
        
        for hero in state.players:
            if hero.dead:
                return True
        return False
    elif spellReq == spells.SpellTargetReq.HP1:
        if targIsEnemy:
            return state.enemies[targetNum].hp == 1
        return False
    elif spellReq == spells.SpellTargetReq.HP2:
        if targIsEnemy:
            return state.enemies[targetNum].hp == 2
        return False
    elif spellReq == spells.SpellTargetReq.HP3:
        if targIsEnemy:
            return state.enemies[targetNum].hp == 3
        return False
    elif spellReq == spells.SpellTargetReq.RECHARGE:
        if targIsEnemy:
            return False
        return state.players[targetNum].usedDie and (state.players[targetNum].currentSide is not None)
    elif spellReq in spells.SpellTargetReq.ALL_TACTICS:
        if spells.findTacticCosts(state, spellReq) is None:  # TODO: add caching all over the place
            return False
        
        if spellReq == spells.SpellTargetReq.DAMAGE_N_SHIELD_3: # Must be UNTARGETED
            return targetNum == -1
        elif spellReq == spells.SpellTargetReq.DAMAGE_2: # ALLY
            return not targIsEnemy
        elif spellReq == spells.SpellTargetReq.ONE_OF_ALL: # ENEMY
            return targIsEnemy
        elif spellReq == spells.SpellTargetReq.AN_X: # ALLY
            return not targIsEnemy
        
    else:
        raise ValueError(f"Unknown spell requirement: {spellReq}")
    
    raise ValueError(f"Unhandled case in isValidSpellAction for spellColor {spellColor} and targetNum {targetNum}")
    return False or True
    

def findMaxHP(state: State, heavy: bool, isEnemy: bool) -> Tuple[int, Ent]:
    maxHP = -999 if heavy else 999
    maxIndices = []
    targetChars = state.enemies if isEnemy else state.players
    for i, char in enumerate(targetChars):
        if (char.hp >= maxHP and heavy) or (char.hp <= maxHP and not heavy):
            if char.hp != maxHP:
                maxIndices = []
                maxHP = char.hp
            maxIndices.append(i)
    return maxIndices

def findMadnessedHero(state: State) -> Tuple[int, Ent]:
    if not state.madnessD:
        return -1, None
    
    for i, hero in enumerate(state.players):
        if not hero.dead and not (hero.sourceID in Lib.MAGIC_HEROS):
            return i, hero
    return -1, None

def isValidReroll(state: State, rerollBits: bitarray) -> bool:
    # Check if the rerollBits is a valid reroll action based on the current state
    # For example, check if the number of rerolls left is sufficient for the number of bits set in rerollBits
    if state.rerolls <= 0:
        return False
    
    for i in range(5):
        if rerollBits[i] and (state.players[i].dead or state.players[i].currentSide.keywords[KeywordIDs.STICKY]):
            return False
    
    return True

def isValidEndTurn(state: State) -> bool:
    possessedHeroIndex, possessedHero = findMadnessedHero(state)
    for i, player in enumerate(state.players):
        if i == possessedHeroIndex:
            return player.usedDie or not canUsedPossessedDie(state, i)
        if player.currentSide.keywords[KeywordIDs.MANDATORY] and not player.usedDie and diceUsable(state, i):
            return False
    
    return True

def isValidAction(state : State, action : int) -> bool:
    actionType, actionData = ACTION_MAP[action]
    if actionType == CONTINUE_ACTION:
        return state.stateType in [StateType.EMPTY_FIGHT, StateType.EMPTY_TURN, StateType.EMPTY_REROLL]
    else:
        if state.stateType in [StateType.EMPTY_FIGHT, StateType.EMPTY_TURN, StateType.EMPTY_REROLL, StateType.WON, StateType.LOST]:
            return False # Only CONTINUE_ACTION is valid in these states
        
        if actionType == REROLL_ACTION:
            return isValidReroll(state, actionData)
        else:
            if state.rerolls > 0:
                return False # If rerolls are present, force AI to use them for training speed. It has the option of '00000' for a reroll
            
            if actionType == DICE_ALLY_ACTION or actionType == DICE_ENEMY_ACTION:
                return isValidDiceAction(state, actionType == DICE_ENEMY_ACTION, actionData)
            elif actionType == SPELL_ALLY_ACTION or actionType == SPELL_ENEMY_ACTION:
                return isValidSpellAction(state, actionType == SPELL_ENEMY_ACTION, actionData)
            elif actionType == END_TURN_ACTION:
                return isValidEndTurn(state)
    raise ValueError(f"Unknown action type: {actionType}")

def validActions(state : State) -> List[int]: # TODO - profile to see if this is a bottleneck. Optimize by not repeating checks for the same characters, and maybe caching actions
    validActions = []
    for i in range(len(ACTION_MAP)):
        if isValidAction(state, i):
            validActions.append(i)

    if len(validActions) == 0:
        raise ValueError("No valid actions found for the current state. This should not happen.")
    
    return validActions
