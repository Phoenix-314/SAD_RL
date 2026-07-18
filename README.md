Slice and Dice and Reinforcement Learning

The goal of this project is to build a RL agent that can play Slice and Dice (SAD, or SnD)
better than human players. The challenge of SAD is around winstreaking, which is the task
of winning many games consecutively (often 30-40 games). Because of this goal, it is critical
for players or AIs to recognize small optimizations that increase the chance of winning an individual
game by a fraction of a percent.

Completed Progress:
1) Implemented a simplified python environment of SAD for faster computation, and thus faster iteration of ideas.

Current Task:
- Use profiling, compilation, or vectorization to enhance the speed of the environment.

Future Plans:

2) Implement RL agent to play my environment with extremely high consistency.
Current plan is to use MCTS with DPW and a transposition table. I expect MCTS to be an effective choice
since the value function of this game has many sharp edges due to breakpoints. DPW is needed due to the relatively
large action space (up to 200 actions) and the stochastic nature of the environment. A transposition table will
be extremely effective for particular states, such as the points between turns and fights where many different simulations
will converge to the same state.

3) Implement the full version of SAD, including modifiers and items.
I may need to improve the efficiency of the RL agent around this time based on performance, since the full version of SAD
is not as efficient as my environment.

4) Modify RL agent to work with the full version of SAD.

AI Usage:
- Copilot autocomplete
- Copilot was wrote the renderer (with some bug fixes by me), since the RL agent will not use it

Resources
- https://ieee-cog.org/2020/papers2019/paper_257.pdf
- A. Couetoux, J.-B. Hoock, N. Sokolovska, O. Teytaud, and N. Bonnard, ¨
“Continuous upper confidence trees,” in International Conference on
Learning and Intelligent Optimization. Springer, 2011, pp. 433–445
- https://www.researchgate.net/publication/235985858_A_Survey_of_Monte_Carlo_Tree_Search_Methods
