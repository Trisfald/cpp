## CSP
Constraint satisfaction problem is a problem defined by a set of variables, each one having a set of constraints associated to it. The aim is to assign a value to every variable in such a way that no constraint is violated. One advantage of CSP representation is genericity: a solver can work well for a wide range of problems without the need of fine-tuning the algorithm.  

### Example
Classic sudoku is a good exercise to show the power of a CSP solver. To translate a sudoku instance in a CSP puzzle it's sufficient to make each grid square a variable and imposing that its value must be different from all other variables in the same block.

### Backtracking
Backtracking is a generic technique useful to tackle many problems and also CSP instances. The idea is to pick a variable and assign a value to it, then do the same for the next variable; when there're no possible values to assign to the current variable, go back to the previous assignment and try another value. There are numerous enhancement to backtracking, for example: smart ways to choose the order of assignments, backjumping to the point of conflict between two variables or keeping some kind of consistency through inference routines. 