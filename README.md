# pointenumerator
Several algorithms for enumerating points in an unbounded plane

This is not a library or a tool. I am not even providing a `Makefile` or build instructions.

Four different ways of mapping a single integer to a point in a plane.

 * _zigzag_ ( only block  0 <= x,y <= w )
 * _triangle_ shape ( includes only the x,y >=0 quadrant )
 * _diamond_ shape ( full plane )
 * _spiral_ ( full plane )
 
_zigzag_, and _spiral_ always make steps of distance `1`.
_diamond_ and _triangle_ make steps of either `1`, or `sqrt(2)`

 
TODO
====

 * add _block_ for enumerating entire plane in block shape.
 * add half-plane enumerator
 * add volume enumerators:
     * octant in tetrahedron shape
     * space in cube shape

AUTHOR
======

Willem Hengeveld <itsme@xs4all.nl>
