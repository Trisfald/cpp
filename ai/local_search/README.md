## Local search
Local search is a method for solving problems for which we care only about the actual solution, not the steps needed to reach it.

### Example
The example problem I chose to demonstrate the local search algorithms is the eight queens puzzle.
The aim is to place 8 queens on a checkboard in a way that no queen threatens another. 

### Simulated annealing
**N_queens_sa.cpp** is a simple program that solves the 8-queens puzzle using a generic simulated annealing algorithm. 

The idea is to start from a certain state, then traversing the state-set towards 'better' solutions. To check if a state is better or worst than another we use an heuristic function.
If it reaches a local maximum, a naive algorithm gets stuck because it can't see a better alternative to its current state. To solve this issue, simulated annealing has a small probability to take a bad step instead of remaining idle.

### Genetic algorithm
In a genetic algorithm a population of 'individuals' (in this case these are problem states) is repeatedly combined with each other. The best specimen are more likely to reproduce thus improving the chances of finding a solution. The algorithm may also make random mutations to the individuals. 

I wrote a basic implementation of a generic genetic algoritm (GA) and another one that makes use of parallelism to improve performance (PGA). 
**N_queens_ga.pp** uses both versions of the algorithm to solve the queens puzzle.
