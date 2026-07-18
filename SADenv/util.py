from bitarray import bitarray
from SADenv.keywordIDs import *
from SADenv.reversedIDs import *
from SADenv.ents import Ent
from SADenv.states import State
from SADenv.actions import ACTION_MAP

"""
A few utility functions that depend only on the dataclass/ID files
Includes printState and printEnt, which are used for debugging
"""

def findHeroCleaveTargets(state : State, ent : Ent):
    index = state.players.index(ent)
    center = index
    l = []
    index = center + 1
    while index < len(state.players):
        if not state.players[index].dead:
            l.append(state.players[index])
            break
        index += 1
    l.append(state.players[center])
    index = center - 1
    while index >= 0:
        if not state.players[index].dead:
            l.append(state.players[index])
            break
        index -= 1

    return l
def findHeroDescendTargets(state : State, ent : Ent):
    index = state.players.index(ent)
    center = index
    l = []
    index = center + 1
    while index < len(state.players):
        if not state.players[index].dead:
            l.append(state.players[index])
            break
        index += 1
    l.append(state.players[center])

    return l

def findEnemyCleaveTargets(state : State, target : Ent):
    index = state.enemies.index(target) # PositionID ensures uniqueness
    l = []
    if index + 1 < len(state.enemies):
        l.append(state.enemies[index + 1])
    l.append(state.enemies[index])
    if index - 1 >= 0:
        l.append(state.enemies[index - 1])
    return l
def findTopPlayer(state : State):
    index = 0
    while index < 5 and state.players[index].dead:
        index += 1
    if index < 5:
        return state.players[index]
    return None
def findBottomPlayer(state : State):
    index = 4
    while index >= 0 and state.players[index].dead:
        index -= 1
    if index >= 0:
        return state.players[index]
    return None
def findMiddlePlayer(state : State): # Technically inaccurate to finding the middle player, but close enough
    if not state.players[2].dead:
        return state.players[2]
    elif not state.players[1].dead:
        return state.players[1]
    elif not state.players[3].dead:
        return state.players[3]
    elif not state.players[0].dead:
        return state.players[0]
    elif not state.players[4].dead:
        return state.players[4]
    return None

def hasRoom(state : State, reinforceEnt : Ent):
    count = reinforceEnt.size
    for ene in state.enemies + state.imminentSummons:
        count += ene.size
    return count <= 165

def make_bitarray(keywords : list[int]):
    b = bitarray(KeywordIDs.NUM_KEYWORDS)
    b[keywords] = 1
    return b

def targetIsDying(state : State, ent : Ent):
    netPoison = max((ent.poison + ent.incomingPoison) - (ent.regen), 0) # ent.poison may be negative
    dyingToPoison = netPoison >= ent.hp
    dyingToDamage = (netPoison + ent.incomingDamage >= ent.hp + ent.shield)

    if ent.undead or ent.intangibled or ent.dead:
        return False
    if ent.dodged:
        return dyingToPoison
    return dyingToPoison or dyingToDamage
def hasCopycattedKeyword(state : State, side : Ent, keywordID : int):
    if side.keywords[keywordID]:
        return True
    if side.keywords[KeywordIDs.COPYCAT] and state.lastDie and state.lastDie.keywords[keywordID]:
        return True
    return False

def monIDtoPos(monID : int):
    return (monID + 1) * -1
def heroIDtoPos(heroID : int):
    return heroID

def printState(state : State):
    print("/STATE=====================================")
    for attrName in state.__dir__():
        if attrName.startswith("__"):
            continue
        #print(st.enemyTargets)
        #exit()
        attr = getattr(state, attrName)
        if isinstance(attr, list):
            if len(attr) > 0 and hasattr(attr[0], "sourceID"):
                print(f"{attrName}: " + str([(str(x.sourceID) + "-" + entsReversedIDs[x.sourceID]) for x in attr]))
            elif "enemyTargets" in attrName:
                print("Enemy Targets!")
                for i, monData in enumerate(state.enemyTargets):
                    print(f"\tEnemy {state.enemies[i].sourceID}-{entsReversedIDs[state.enemies[i].sourceID]} targets: ")
                    for i, target in enumerate(monData):
                        print(f"\t\tEnemytarget {i}: {target.sourceID}-{entsReversedIDs[target.sourceID]}")
                continue
            else:
                print(f"{attrName}: {attr}")
        else:
            if hasattr(attr, "sourceID"):
                print(f"{attrName}: {attr.sourceID}-{entsReversedIDs[attr.sourceID]}")
            else:
                if "stateType" in attrName:
                    print(f"{attrName}: {stateTypesReversedIDs[attr]}")
                else:
                    print(f"{attrName}: {attr}")
    print("===========================================")
def printEnt(state : State, ent : Ent):
    print(f"Entity {ent.sourceID}-{entsReversedIDs[ent.sourceID]}:")
    for attrName in ent.__dir__():
        if attrName.startswith("__"):
            continue

        attr = getattr(ent, attrName)
        if isinstance(attr, list):
            if len(attr) > 0 and hasattr(attr[0], "sourceID"):
                print(f"{attrName}: " + str([(str(x.sourceID) + "-" + entsReversedIDs[x.sourceID]) for x in attr]))
            elif "sides" in attrName:
                print("Sides:")
                for i, side in enumerate(attr):
                    print(f"\tSide {i}-{sidesReversedIDs[side.type]}: {side.value}; {side.keywords}")
            else:
                print(f"{attrName}: {attr}")
        else:
            if hasattr(attr, "sourceID"):
                print(f"{attrName}: {attr.sourceID}-{entsReversedIDs[attr.sourceID]}")
            else:    
                print(f"{attrName}: {attr}")

    print("===========================================")

def getActionStr(act : int):
    action, data = ACTION_MAP[act]
    if action == DICE_ALLY_ACTION:
        return "DA" + str(data[0]) + str(data[1])
    elif action == DICE_ENEMY_ACTION:
        return "DE" + str(data[0]) + str(data[1])
    elif action == SPELL_ALLY_ACTION:
        return "SA" + str(data[0]) + str(data[1])
    elif action == SPELL_ENEMY_ACTION:
        return "SE" + str(data[0]) + str(data[1])
    elif action == REROLL_ACTION:
        return "R" + data.to01()
    elif action == END_TURN_ACTION:
        return "E"
    elif action == CONTINUE_ACTION:
        return "C"
    return str(act)