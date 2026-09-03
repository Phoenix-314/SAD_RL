# Slice and Dice & Reinforcement Learning (SAD RL)

## About
This project contains two significant parts:

1) Slice and Dice Environment: This is a recreation of part of Tann's game, Slice and Dice. It is made in C++ and is hundreds or thousands of times faster, but does not have faithful graphics, items/modifiers (game mechanics), or any animations. The purpose of the environment is to allow an RL agent to train or search fast enough to make smart decisions in a reasonable amount of time.

2) Reinforcement Learning Algorithm: This is an algorithm meant to find the optimal action in any given state, specialized for Slice and Dice. The goal is to optimize probability of winning, which is near 100% for skilled players. Therefore, it aims to address distinguishing optimal actions in situations where multiple actions are very likely to be winning.

The game is typically abbreviated SnD, a fact I learned after naming this project.

## Running

A link to a browser version of this project will be attached here shortly.

Instructions;


## Installation

This uses premake5 and conan for building.
Testing has been done when building for Visual Studio 2026 and with Emscripten.

Premake5 executable is available in the repository.
Conan 2.30.0 can be installed via pip/conda.
The conanfile and premake5.lua files are available in the repository.

1) Run ```python init.py```

2) If using visual studio 2026: Run the resulting SAD_CPP.slnx file.

If using a browser, run the 2 following commands in a terminal.

cmd /d /c "call dependencies\emscripten\conanbuild.bat && [pathToConan]\\.conan2\p\\[packageToGnumake]\p\bin\gnumake.exe -f Makefile config=release -j1"

cmd /d /c "call dependencies\emscripten\conanbuild.bat && emrun docs\SAD_CPP.html"

## Future Plans

Implement a full transposition table setup to improve training efficiency.

Implement and train a neural network for evaluation of leaf nodes (instead of using rollouts). Will likely research and use a setup similar to AlphaZero, although with more domain specific knowledge of Slice and Dice.

Allow users to query Gemini for the best possible move in a situation. Gemini will be able to access information about the state as well as the current search information from the RL Agent.

Potentially, I could try to modify the agent to work with the full version of SnD.

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


