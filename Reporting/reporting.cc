#include "reporting.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>

namespace Reporting {

    void reportOnInitialSetup(VelocitySet &set, Node *nodes, size_t dx, size_t dy)
    {
        std::cout << "LBM D" << set.nDimensions << "Q" << set.nDirections << " simulation." << '\n'
            << "Grid size " << dx << "x" << dy
            << '\n' << '\n';
        // Shows a grid of letters representing the type of each node
        for (size_t y = dy; y--;)
        {
            for (size_t x = 0; x < dx; ++x)
            {
                size_t idx = x + dx * y;
                if (nodes[idx].type == NodeType::ZouHe)
                    std::cout << 'Z';
                else if (nodes[idx].type == NodeType::BounceBack)
                    std::cout << 'B';
                else
                    std::cout << '.';
            }
            std::cout << '\n';
        }
    }

    void reportOnNode(VelocitySet &set, Node &node, size_t x, size_t y)
    {
        double node_density = density(set, node);
        double *node_velocity = velocity(set, node);

        size_t nDimensions = set.nDimensions;

        // show position
        std::cout << "(";
        for (size_t dir = 0; dir < nDimensions; ++dir)
            std::cout << node.position[dir] << ", ";
        // show density
        std::cout << ") | (" << node_density << ") | (";
        // show velocity
        for (size_t dim = 0; dim < nDimensions; ++dim)
            std::cout << node_velocity[dim] << ", ";
        std::cout << ")" << '\n';
        delete[] node_velocity;
    }

    void reportOnDistributions(VelocitySet &set, Node &node)
    {
        size_t nDirections = set.nDirections;

        std::cout << "Current: \n(";
        for (size_t dir = 0; dir < nDirections; ++dir)
            std::cout << node.distributions[dir].value << ", ";
        std::cout << ")" << '\n';

        std::cout << "Next: \n(";
        for (size_t dir = 0; dir < nDirections; ++dir)
            std::cout << node.distributions[dir].nextValue << ", ";
        std::cout << ")" << '\n';
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
        /*
        for (size_t p = 0; p < totalProcessors; ++p)
        {
            if (p == P)
            {
                std::ofstream out("logs/test.txt");
                Reporting::MatlabReporter reporter(out);
                reporter.reportOnTimeStep(set, nodes, totalNodes);
                bsp_sycn();

            }
        }
        */


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
        return;
        std::cout << "Total velocity: ";
        for (size_t dim = 0; dim < nDimensions; ++dim)
            std::cout << total_velocity[dim] << '\t';
        std::cout << '\n';

        delete[] total_velocity;
    }

    void report(std::string outputFileName, VelocitySet &set, Node *nodes, size_t totalNodes)
    {
        std::ofstream out(outputFileName, std::ios::out | std::ios::app);
        Reporting::MatlabReporter reporter(out);
        reporter.reportOnTimeStep(set, nodes, totalNodes);
    }
}