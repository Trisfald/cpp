## Games
Games are a kind of problem in which there are at least two 'players' that compete with each other. In the context of artificial intelligence we speak of adversarial search because an agent plans its moves not on a static world, but in one where other agents are taking decisions against it.

### Example
I chose chess as an example to illustrate adversarial search. I'm in no way an experienced chess player, thus the game rules' implementation is not complete and the heuristic functions used are rather simple.

### Alpha-beta search
The alpha-beta pruning is an algorithm widely used to solve two player games. It's core strategy is to select the move leading to the best possible outcome, taking for granted that the opponent will always make the best choice at his disposal.