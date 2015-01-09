# LBM Simulation in C++

## Goal
To write a flexible Lattice Boltzmann Method simulation that is parallelizable.
This program is flexible in the sense that the streaming and collision methods are independent on the chosen velocity set and the chosen domain.

## Todo
Currently only a periodic and a bounce back boundary condition have been implemented.

1. Add Zou He boundary condition.
2. Rewrite how the program handles boundary conditions.
3. Rewrite how the program handles the initialization of lattice nodes. (done?)
4. Parallelize the program using BSP.

This program was written as a final assignement for the [Parallel Algorithms](http://www.staff.science.uu.nl/~bisse101/Education/PA/pa.html) mastermath course.
