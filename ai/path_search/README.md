## Path search
This section concentrates on algorithms that are useful for finding a path from a given state to a goal state.

### Example
The popular sliding puzzle is a nice example of a path-finding problem. All the steps needed to solve the puzzle represent a path from the starting configuration to the goal.

### A*
A* is a popular informed search algorithm. It's noted for its good performance and simplicity.

I've implemented several versions of A*:
- Classic A*: fast, high memory consumption
- Iterative deepening A* (IDA*): slower, but uses a very small amount of memory
- Iterative expansion A* (IEA*): middle ground between the A* and IDA*
- Parallel bi-directional A*: improves A* speed by running two concurrent searches, from start to goal and from goal to start.
