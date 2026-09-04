# Slice and Dice & Reinforcement Learning (SAD RL)

## About
This project contains two significant parts:

1) Slice and Dice Environment: This is a recreation of part of Tann's game, Slice and Dice. It is made in C++ and is hundreds or thousands of times faster, but does not have faithful graphics, items/modifiers (game mechanics), or any animations. The purpose of the environment is to allow an RL agent to train or search fast enough to make smart decisions in a reasonable amount of time.

2) Reinforcement Learning Algorithm: This is an algorithm meant to find the optimal action in any given state, specialized for Slice and Dice. The goal is to optimize probability of winning, which is near 100% for skilled players. Therefore, it aims to address distinguishing optimal actions in situations where multiple actions are very likely to be winning.

The game is typically abbreviated SnD, a fact I learned after naming this project.

## Running

Browser access: https://phoenix-314.github.io/SAD_RL/

Instructions:

0) Play and understand Slice and Dice by Tann (https://tann.itch.io/slice-dice). The display is not intended to be teach players how to play SnD. Further, it requires some knowledge of SnD to understand the display. The game also requires a keyboard, and it may require resizing the browser (use 3 dots in the top right if needed).

1) To play the game, enter commands into the terminal. The terminal is always selected.

The game is very similar to SnD, with the following modifications:

- No items, modifiers, hero select, or different game modes.
- No UNDO (would increase state memory size)
- All rerolls must be used before using dice or spells. Can 'reroll' 0 dice to end rerolling phase.
- Must use a continue action to proceed after rerolling phase, ending turn, or ending a fight.
- The current default RL agent is: TrialActor, using 500 simulations of each possible action with rolloutPolicy heuristicActor.

<br>

The following is a list of available commands:

Actions (case sensitive):

- (Empty input): Takes an action based on the current policy. The chosen action is printed to the console.
- DAXY: DA=Dice targeting Ally, uses hero X's dice for hero Y
- DEXY: DE=Dice targeting Enemy, uses hero X's dice against enemy Y
- DAX-1: Uses hero X's dice. Use when the dice is untargeted, such as mana, dodge, or damage-all. 
- SAXY: SA=Spell targeting Ally, uses spell X for hero Y. Spell 0 is burst, spell X is hero X's spell.
- SEAF: SE=Spell targeting Enemy, uses spell A against enemy F.
- SAX-1: Uses spell X if spell X is untargeted.
- RABCDE: Rerolls hero 1's dice if A=1, keeps hero 1's dice if A=0. Respectively, B-E for heros 2-5.
- E: Ends turn. Cannot be used during the rerolling phase.
- C: Continue action. Must be used after rerolling phase, after ending turn, and after winning a fight.
- R: Rerolls no heros (wastes a reroll to end rerolling phase)
- R?: Rerolls all heros, regardless of the value of ?

Debug Actions:

- a: prints a list of valid actions in the current state (note: user actions may differ by 1 due to 0-indexing)
- rABCDE: forces hero 1 to roll side A, etc.
- s: prints the current state
- seY: prints enemy number Y
- saY: prints hero number Y

Example Series of actions:
- R11011: Roll every hero but the gray (3rd) hero
- R00001: Roll the blue (5th) hero only
- C: Exit rolling phase, start using dice and spells
- DE12: Use orange (1st) hero to attack the second enemy
- DA35: Use the gray hero to shield the blue hero
- DA4-1: Use the red hero's untargeted side (probably mana)
- a: print a list of available actions to see what can be done
    - Prints the internal representation, where heros/enemies are 0-indexed instead of 1-indexed.
- SE01: burst the first enemy
- E: end turn
- C: exit the end turn phase, enter the rolling phase of the next turn
- ...

<br>

Other notes:
 Agent does not get his shifter sides. Very sad.

Keywords are applied, but are not displayed visually.

## Installation

This uses premake5 and conan for building.
Testing has been done when building for Visual Studio 2026 and with Emscripten.

Premake5 executable is available in the repository.
Conan 2.30.0 can be installed via pip/conda.
The conanfile and premake5.lua files are available in the repository.

1) Run ```python init.py```

2) If using visual studio 2026: Run the resulting ```SAD_CPP.slnx``` file.

If using a browser, run ```python init.py emscripten```. You will first need to edit the path to conan and the name of the gnumake package in init.py.

## Future Plans

Implement a full transposition table setup to improve training efficiency.

Implement and train a neural network for evaluation of leaf nodes (instead of using rollouts). Will likely research and use a setup similar to AlphaZero, although with more domain specific knowledge of Slice and Dice.

Allow users to query Gemini for the best possible move in a situation. Gemini will be able to access information about the state as well as the current search information from the RL Agent.

Potentially, I could try to modify the agent to work with the full version of SnD. This would also require building a fast environment for the full game.

## LLM Usage:

The following list consitutes a complete list of LLM usage when constructing this project:
- Copilot inline suggestions
- Copilot wrote the renderer and user input code (with some bug fixes by me). These functions are not used when training the RL Agent
- Copilot debugged library/dependency issues
- Users may be able to query Gemini for explanations of optimal moves

## References
- Tann. (2021). Slice And Dice (v. 3.2.13) [Desktop]. https://tann.fun/games/dice/
- J. S. B. Choe and J. -K. Kim, "Enhancing Monte Carlo Tree Search for Playing Hearthstone," 2019 IEEE Conference on Games (CoG), London, UK, 2019, pp. 1-7, doi: 10.1109/CIG.2019.8848034. keywords: {Games;Monte Carlo methods;Artificial intelligence;Directed acyclic graph;Search problems;Decision trees;Monte-Carlo tree search;Hearthstone;artificial intelligence for games},

- A. Couetoux, J.-B. Hoock, N. Sokolovska, O. Teytaud, and N. Bonnard, ¨
“Continuous upper confidence trees,” in International Conference on
Learning and Intelligent Optimization. Springer, 2011, pp. 433–445
- C. B. Browne et al., "A Survey of Monte Carlo Tree Search Methods," in IEEE Transactions on Computational Intelligence and AI in Games, vol. 4, no. 1, pp. 1-43, March 2012, doi: 10.1109/TCIAIG.2012.2186810.


