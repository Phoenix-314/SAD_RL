from SADenv.states import *
from SADenv.actions import *
from SADenv.validActions import *
from SADenv.reversedIDs import *

def randomAction(state : State):
    """
    Slightly improved random action generator
    """
    while True:
        act = np.random.choice(validActions(state))

        # Picking slightly better than random play
        if act == actionsReversedIDs[(END_TURN_ACTION, 0)] and len(validActions(state)) > 1:
            continue
        if act in [80, 81, 82, 83, 84]: # Bursting defensively (SA01-SA05) is almost always a waste of mana for random actions
            continue
        if state.rerolls > 0 and state.stateType == StateType.BATTLE: # Reroll if blank, else dont.
            rr = bitarray("00000")
            for i in range(5):
                if not diceUsable(state, i) and not (state.players[i].dead or state.players[i].currentSide.keywords[KeywordIDs.STICKY]):
                    rr[i] = 1
                else:
                    rr[i] = 0
            if (actionsReversedIDs[(REROLL_ACTION, str(rr))] in validActions(state)):
                act = actionsReversedIDs[(REROLL_ACTION, str(rr))]
        break
    
    return act