# LBM Simulation in C++

## Goal
To write a flexible Lattice Boltzmann Method simulation that is parallelizable.
This program is flexible in the sense that the streaming and collision methods are independent on the chosen velocity set and the chosen domain.

## Todo

1. Fix Zou He boundary condition.
2. Refactor the DomainInitializer
3. Add functionality to create different domain distributions (vertical / horizontal split, rectangular, dimaond, mondriaan)
4. Add more boundary conditions

This program was written as a final assignement for the [Parallel Algorithms](http://www.staff.science.uu.nl/~bisse101/Education/PA/pa.html) mastermath course.
