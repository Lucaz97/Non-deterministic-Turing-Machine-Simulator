# Non-deterministic-Turing-Machine-Simulator
University project for theoretical computer science and algorithms

The goal of this project was to make the most efficient possible NDTM simulator.
My solution takes advantage of the maximum number of steps imposed in the input file by implementig a Depth First Search algorithm to explore the tree of TM.
Transitions are stored in a double hash table implemented with linked lists. The first hash filters transitions by character read, the second hash by starting state.
The use of functions is limited to the least possible to use the stack as little as possible.
