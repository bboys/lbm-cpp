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
        {
            Node reported_node = nodes[x + y * dx];
            if (bounceback(x, y, dx, dy))
                std::cout << 'B';
            else
                std::cout << 'P';
        }
        std::cout << '\n';
    }
}

void reportOnNode(VelocitySet &set, Node node, size_t x, size_t y)
{
    double node_density = density(set, node);
    double *node_velocity = velocity(set, node);
    std::cout << std::fixed;
    // std::cout << std::setfill(' ') << std::setw(2);
    std::cout << "(" << x << ", " << y << ") | ("
              << node_density
              << ") | (" << node_velocity[0] << "," << node_velocity[1] << ")";
    // for (size_t dir = 0; dir < set.nDirections; ++dir)
    //     std::cout << "("
    //             << std::setfill('0') << std::setw(4) << node.distributions[dir].value << ", "
    //             << std::setfill('0') << std::setw(4) << *node.distributions[dir].neighbour << ", "
    //             << std::setfill('0') << std::setw(4) << node.distributions[dir].nextValue << ")" << " ";

    std::cout << '\n';
}

void report(VelocitySet &set, Node *nodes, size_t dx, size_t dy)
{
    std::cout << '\n' << '\n';
    for (size_t y = 0; y < dy; ++y)
        for (size_t x = 0; x < dx; ++   x)
            reportOnNode(set, nodes[x + dx * y], x, y);
}