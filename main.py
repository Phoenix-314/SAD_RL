import os
import numpy as np
import time

np.random.seed(2)

from SADenv.reversedIDs import *
from render import *
from SADenv.transitions import *
from SADenv.util import *
from randomActor import randomAction

renderer = None

def runInputCycle(oldState : State, ancientState : State, actionGenerator=None):
    """
    Updates pygame display and reads from pygame console input. (If actionGenerator is provided, empty console input will be filled with actions from the generator)
    Returns the new state after executing the action, or returns the old state if the input was invalid.
    empty for actionGenerator
    q to quit,
    rXXXXX to reroll allies with the given dice values (X = 0-5),
    v to view valid actions (numeric)
    a to view valid actions (text)
    s to view state
    saX to view ally X (1-5)
    seX to view enemy X (1-10)
    b to go back to the previous state (or forward, if b was just used)
    x<code> to execute arbitrary python code (for testing)
    DAxy to use ally x's die on target y (x = 1-5, y = 1-10, or - for untargeted)
    DExy, SAxy, SExy for similar actions. SA0/SE0 is burst
    RXXXXX to reroll allies with the given dice values (X = 0-1, 1 = reroll, 0 = keep)
    E to end turn
    C to continue (if the game is in a non-interactive state)
    """
    state = copy.deepcopy(oldState)
    line = renderer.readConsoleLine(state=state)
    if line is None or line.strip() == "":
        if actionGenerator is not None:
            try:
                act = actionGenerator(state)
                transition(state, act)
                renderer.addActionToConsoleHistory(f"{getActionStr(act)}")
                renderer.render(state)
                return state
            except Exception as e:
                print("Error: " + str(e))
                import traceback
                traceback.print_exc()
                print(f"Some error has occurred. Returning oldstate")
                printState(state)
                for ent in state.players:
                    printEnt(state, ent)
                return oldState
        return oldState
    
    try:
        x = line.strip()
        if x[0] == "q":
            return None
        elif x[0] == "r":
            if len(x) != 6:
                print(f"INVALID INPUT. Try again: {x}")
                return oldState
            rollAllies(state, [int(c) for c in x[1:6]])
            handleSDS(state)
            return state
        elif x[0] == "v":
            print(validActions(state))
            return oldState
        elif x[0] == "a":
            for v in validActions(state):
                print(getActionStr(v))
            print()
            return oldState
        elif x[0] == "s":
            if x.startswith("sa"):
                printEnt(state, state.players[int(x[2]) - 1])
            elif x.startswith("se"):
                printEnt(state, state.enemies[int(x[2]) - 1])
            else:
                printState(state)
            return oldState
        elif x[0] == "b":
            return ancientState
        elif x[0] == "x":
            exec(x[1:]) # Local testing program
        else:
            if x.startswith("DA"):
                action = DICE_ALLY_ACTION
                if x[3] == "-":
                    data = (int(x[2]) - 1, -1)
                else:
                    data = (int(x[2]) - 1, int(x[3]) - 1)
            elif x.startswith("DE"):
                action = DICE_ENEMY_ACTION
                data = (int(x[2]) - 1, int(x[3]) - 1)
            elif x.startswith("SA"):
                action = SPELL_ALLY_ACTION
                if x[3] == "-":
                    data = (int(x[2]), -1)
                else:
                    data = (int(x[2]), int(x[3]) - 1)
            elif x.startswith("SE"):
                action = SPELL_ENEMY_ACTION
                data = (int(x[2]), int(x[3]) - 1)
            elif x.startswith("R"):
                action = REROLL_ACTION
                if len(x) == 1:
                    data = str(bitarray("0"*5))
                else:
                    if len(x) != 6 and len(x) != 2:
                        print(f"INVALID INPUT. Try again: {x}")
                        return oldState
                    if len(x) == 2:
                        data = str(bitarray("1"*5))
                    else:
                        data = str(bitarray(x[1:]))
            elif x.startswith("E"):
                if state.rerolls == 2: # Auto finishes turn for convenience, if one tries to end turn at the start of a turn
                    transition(state, actionsReversedIDs[(REROLL_ACTION, str(bitarray("00000")))])
                    transition(state, actionsReversedIDs[(REROLL_ACTION, str(bitarray("00000")))])
                action = END_TURN_ACTION
                data = 0
            elif x.startswith("C"):
                action = CONTINUE_ACTION
                data = 0
            else:
                print(f"INVALID INPUT. Try again: {x}")
                return oldState

            if (action, data) in actionsReversedIDs and isValidAction(state, actionsReversedIDs[(action, data)]):
                print("Executing: " + getActionStr(actionsReversedIDs[(action, data)]))
                transition(state, actionsReversedIDs[(action, data)])
                renderer.render(state)
                return state
            else:
                print(f"Illegal Action: {x}")
                return oldState
    except Exception as e:
        print("Error: " + str(e))
        import traceback
        traceback.print_exc()
        print(f"Some error has occurred. Returning oldstate")
        printState(state)
        return oldState
    return state

import pickle
picklerFolderName = "saveStates"
if not os.path.exists(picklerFolderName):
    os.makedirs(picklerFolderName)

def loadState():
    if os.path.isfile(os.path.join(picklerFolderName, "state_" + str(0) + ".pkl")):
        i = 0
        while os.path.isfile(os.path.join(picklerFolderName, "state_" + str(i) + ".pkl")): # Finds most recent saved state and loads it
            i += 1
        i -= 1
        state = pickle.load(open(os.path.join(picklerFolderName, "state_" + str(i) + ".pkl"), "rb"))
        return state, i + 1
    else:
        #state, i = genState([LUDUS, LEADER, VALKYRIE, MEDIC, ARTIFICER], [THORN]), 0
        #transition(state, actionsReversedIDs[(REROLL_ACTION, str(bitarray("00000")))])
        #transition(state, actionsReversedIDs[(REROLL_ACTION, str(bitarray("00000")))])
        #state.mana = 10

        state, i = initial(), 0
        return state, i

def playGame(actionGenerator=None):
    """
    Allows user to play game with console input in a pygame window. Saves all non-error states. 
    If a state has been saved, that state will be loaded when restarting. Else, starts a new state
    If actionGenerator is provided, empty console input will be filled with actions from the generator, allowing for automated play.
    """
    global renderer
    renderer = Render()
    state, i = loadState()
    lastState = None
    while True:
        ancient = lastState
        lastState = copy.deepcopy(state)
        state = runInputCycle(state, ancient, actionGenerator=actionGenerator)
        if not state:
            exit()
        if state != lastState:
            with open(os.path.join(picklerFolderName, "state_" + str(i) + ".pkl"), "wb") as f:
                pickle.dump(state, f)
            i += 1

        if state.stateType == StateType.WON or state.stateType == StateType.LOST:
            print(f"Game Over. Result: {stateTypesReversedIDs[state.stateType]}, Level: {state.level}")
            return 1 if state.stateType == StateType.WON else 0

def playManyGamesRandomly(actionGenerator, numGames=1000):
    """
    Does not render anything.
    Plays numGames games with (mostly) random actions, and displays winrate, average level, and the highest level reached.
    """
    wins = 0
    losses = 0
    levels = 0
    maxLevel = 0
    timer = time.time()
    for i in range(1, numGames + 1):
        # Progress bar with time estimate
        if i % max(numGames // 345, 1) == 0 or i == numGames - 1:
            elapsed = time.time() - timer
            estimatedTotal = elapsed / (i + 1) * numGames
            remaining = estimatedTotal - elapsed
            print(f"Progress: {i}/{numGames} ({i/numGames*100:.2f}%), Time elapsed: {elapsed:.2f}s, Estimated remaining: {remaining:.2f}s       ", end="\r")
        
        state = initial() # Random first fight & heroes
        actionCount = 0
        while not isTerminal(state):
            actionCount += 1
            try:
                # Infinite Loop Prevention, should never happen as the game causes instant loss after 30 turns
                if actionCount > 10000:
                    print("Action count exceeded 10000, breaking out of loop")
                    raise Exception("Action count exceeded 10000")
                    
                oldState = copy.deepcopy(state)
                act = actionGenerator(state)


                transition(state, act)

            except Exception as e:
                # Debugging information
                print()
                print("Error: " + str(e))
                import traceback
                traceback.print_exc()
                
                print(f"ActionError: {act}",end=" ")
                print(getActionStr(act))
                print()
                printState(oldState)
                print("ENTS\n\n")
                for ent in state.players:
                    printEnt(state, ent)

                with open(os.path.join(picklerFolderName, "errFile.pkl"), "wb") as f:
                    pickle.dump(oldState, f)
                exit()
        # Stat Collection
        levels += state.level
        maxLevel = max(maxLevel, state.level)
        if state.stateType == StateType.WON:
            wins += 1
        else:
            losses += 1
    print()
    print(f"Wins: {wins}, Losses: {losses}, Winrate: {wins/(wins+losses)}, Average Level: {levels/numGames}, Max Level: {maxLevel}")
    print(f"Time taken: {time.time() - timer}")

def main():
    # playGame(actionGenerator=randomAction)
    playManyGamesRandomly(actionGenerator=randomAction, numGames=10000)

if __name__ == "__main__":
	main()

