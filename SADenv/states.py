from dataclasses import dataclass, field
from SADenv.ents import *

from typing import List, Callable, Tuple

"""
Dataclass for the State of the game. All functions have no persistent state, so this object fully represents a current moment in the game.
Games can be saved and loaded by pickling this object.
"""

class StateType:
    BATTLE = 0              # "Normal" state
    EMPTY_FIGHT = 1          # The momentary state between battles where the next battle is not yet generated
                                # This fits well for transposition tables, since w/o enemies or playerSides selected, there are few variables
                                # Specifically, the for a particular MCTS search, the only thing that could be different is which heros are defeated
    EMPTY_TURN = 2          # The momentary state between turns where the next turn's rolls are not completed, for transposition tables.
                                # Only available action is "Continue"
                                # Also for transposition tables, since w/o rolls, there are few variables.
    EMPTY_REROLL = 3        # A state indicating that the player has just finished rerolling, making it more 
                                # convenient to switch to searching for an optimal turn without randomness
    HERO_SELECT = 4         # Hero select, the next battle has been generated and is available for view to the player
    LOST = 5                # All heros have fallen
    WON = 6                 # Lvl 20 has been completed (or tied)

@dataclass(slots=True)
class State:
    # Players
    players: List[Ent]=None
    spellData: List[int]=None # 0 is burst, 1-5 are colors
    
    mana: int=None # current mana for team
    fortoldMana: int=None # mana that will be gained next turn from foretell
    rerolls: int=None # number of rerolls left for team. Negative values represent gaze value
    
    # Monsters
    enemies: List[Ent]=None
    enemyTargets: List[List[Ent]]=None # List of monsters: For each monster:
                                                    # a list of Ents for attacks. This list is 1 for each hero being attacked, ie, 
                                                        # 3 for cleave (typically), 2 for descend, 1 for heal, damage, 5 for damage_all, 10ish for damage_everyone

    imminentSummons: List[Ent]=field(default_factory=list) # enemies that will be summoned at the end of an action
    reinforcements: List[Ent]=field(default_factory=list) # queue. [0] is the next reinforcement to be added


    # Game:
    turn: int=None
    lastTarget: Ent=None # for focus
    prevLastStrikeVal: int=None # for trio
    lastStrikeVal: int=None # like lastdie, but with keywords evaluated
    lastDie: Side=None # for chain, copycat, etc.

    # Progression
    level: int=0
    playerLevels: List[int]=None # list of 5 levels, for each color

    # MonPassives
    enemyDamageD: bool=False # if any enemy was damaged this turn, for blind
    hexiaD:bool=False # if hexia is present
    bansheeD:int=0 # number of banshees present, negative if their scream has been cast, positive if not
    brambleD:bool=False # if bramble is present
    madnessD:bool=False # if madness is present
    handD:bool=False # if the hand is present
    warchiefD:int=0 # number of warchiefs present

    # State type
    stateType: int=StateType.EMPTY_FIGHT # Used to make it more convenient for RL agent to learn/search
    

    