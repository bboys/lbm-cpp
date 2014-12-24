#include "reporting.h"
#include <iostream>
#include <iomanip>
#include <limits>

void reportOnInitialSetup(VelocitySet &set, Node *nodes, size_t dx, size_t dy)
{
    std::cout << "LBM D" << set.nDimensions << "Q" << set.nDirections << " simulation." << '\n'
        << "Grid size " << dx << "x" << dy
        << '\n' << '\n';
    // Shows a grid of letters representing the type of each node
    for (size_t x = 0; x < dx; ++   x)
    {
        for (size_t y = 0; y < dy; ++y)
            if (bounceback(x, y, dx, dy))
                std::cout << 'B';
            else
                std::cout << 'P';
        std::cout << '\n';
    }
}

void reportOnNode(VelocitySet &set, Node node, size_t x, size_t y)
{
    double node_density = density(set, node);
    double *node_velocity = velocity(set, node);

    size_t nDimensions = set.nDimensions;

    std::cout << "(" << x << ", " << y << ") | ("
              << node_density
              << ") | (";
    for (size_t dim = 0; dim < nDimensions; ++dim)
        std::cout << node_velocity[0] << ", ";
    std::cout << ")" << '\n';
    delete[] node_velocity;
}

void report(VelocitySet &set, Node *nodes, size_t dx, size_t dy)
{
    std::cout << '\n' << '\n';
    for (size_t y = 0; y < dy; ++y)
        for (size_t x = 0; x < dx; ++   x)
            reportOnNode(set, nodes[x + dx * y], x, y);
}

// Report on the total density and the total velocity
void report(VelocitySet &set, Node *nodes, size_t totalNodes)
{
    double total_density = 0;
    for (size_t idx = 0; idx < totalNodes; ++idx)
        total_density += density(set, nodes[idx]);

    std::cout << "Total density: " << total_density << '\n';

    size_t nDimensions = set.nDimensions;
    double *total_velocity = new double[nDimensions]();

    for (size_t idx = 0; idx < totalNodes; ++idx)
    {
        double *node_velocity = velocity(set, nodes[idx]);
        for (size_t dim = 0; dim < nDimensions; ++dim)
            total_velocity[dim] += node_velocity[dim];

        delete[] node_velocity;
    }

    std::cout << "Total velocity: ";
    for (size_t dim = 0; dim < nDimensions; ++dim)
        std::cout << total_velocity[dim] << '\t';
    std::cout << '\n';

    delete[] total_velocity;
}
