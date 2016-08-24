# pointenumerator
Several algorithms for enumerating points in an unbounded plane

This is not a library or a tool. I am not even providing a `Makefile` or build instructions.

Four different ways of mapping a single integer to a point in a plane.

 * zigzag ( only block  0 <= x,y <= w )
 * triangle shape ( includes only the x,y >=0 quadrant )
 * diamond shape ( full plane )
 * spiral ( full plane )
 
zigzag, and spiral always make steps of distance `1`.
diamond and triangle make steps of either `1`, or `sqrt(2)`

 
