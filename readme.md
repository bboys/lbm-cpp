# LBM Simulation in C++

## Goal
To write a flexible Lattice Boltzmann Method simulation that is parallelizable.
This program is flexible in the sense that the streaming and collision methods are independent on the chosen velocity set and the chosen domain.

## Todo
Currently only a periodic and a bounce back boundary condition have been implemented.

1. Add Zou He boundary condition.
2. Rewrite how the program handles boundary conditions.
3. Rewrite how the program handles the initialization of lattice nodes.
4. Check if using `std::vector` instead of normal arrays decrease efficiency.
5. Parallelize the program using BSP.
6. Check if using objects decrease the efficiency of the program.

This program was written as a final assignement for the [Parallel Algorithms](http://www.staff.science.uu.nl/~bisse101/Education/PA/pa.html) mastermath course.
