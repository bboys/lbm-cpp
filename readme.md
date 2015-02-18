# LBM Simulation in C++

## Goal
To write a flexible Lattice Boltzmann Method simulation that is parallelizable.
This program is flexible in the sense that the streaming and collision methods are independent on the chosen velocity set and the chosen domain.
The current implementation uses the `MulticoreBSP-for-C` package for parallelization.

## Todo

- Refactor folder structure and use cmake
- Add tests
- Refactor the usage of `std::vector` into templates of fixed size.
- Refactor the DomainInitializer
- Add functionality to create different domain distributions (vertical / horizontal split, rectangular, dimaond, mondriaan)
- Make boundary conditions independent on the domain distribution
- Fix Zou He boundary condition.
- Add more boundary conditions
- Add an MPI implementation
- Make the usage of MPI and BSP interchangeable

This program was written as a final assignment for the [Parallel Algorithms](http://www.staff.science.uu.nl/~bisse101/Education/PA/pa.html) mastermath course.
