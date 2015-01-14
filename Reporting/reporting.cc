#include "reporting.h"

#include "config.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <sstream>

#include "MatlabReporter.h"

namespace Reporting {

    void reportOnInitialSetup(VelocitySet *set, Node *nodes, MY_SIZE_T dx, MY_SIZE_T dy)
    {
        std::cout << "LBM D" << set->nDimensions << "Q" << set->nDirections << " simulation." << '\n'
            << "Grid size " << dx << "x" << dy
            << '\n' << '\n';
        // Shows a grid of letters representing the type of each node
        // for (MY_SIZE_T y = dy; y--;)
        // {
        //     for (MY_SIZE_T x = 0; x < dx; ++x)
        //     {
        //         MY_SIZE_T idx = x + dx * y;
        //         if (nodes[idx].type == NodeType::ZouHe)
        //             std::cout << 'Z';
        //         else if (nodes[idx].type == NodeType::BounceBack)
        //             std::cout << 'B';
        //         else
        //             std::cout << '.';
        //     }
        //     std::cout << '\n';
        // }
    }

    void reportOnNode(VelocitySet *set, Node &node)
    {
        double node_density = density(set, node);
        double *node_velocity = velocity(set, node);

        MY_SIZE_T nDimensions = set->nDimensions;
        // show position
        std::cout << "(";
        for (MY_SIZE_T dir = 0; dir < nDimensions; ++dir)
            std::cout << node.position[dir] << ", ";
        // show density
        std::cout << ") | (" << node_density << ") | (";
        // show velocity
        for (MY_SIZE_T dim = 0; dim < nDimensions; ++dim)
            std::cout << node_velocity[dim] << ", ";
        std::cout << ")" << '\n';
        delete[] node_velocity;
    }

    void reportOnDistributions(VelocitySet *set, Node &node)
    {
        MY_SIZE_T nDirections = set->nDirections;
        std::stringstream ss;



        ss << "Current:     (";
        ss << node.position[0] << ", " << node.position[1] << "), (";
        for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
            ss << node.distributions[dir].value << ", ";
        ss << ")" << '\n';

        ss << "Next:        (";
        ss << node.position[0] << ", " << node.position[1] << "), (";
        for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
            ss << node.distributions[dir].nextValue << ", ";
        ss << ")" << '\n';

        ss << "Neighbour:   (";
        ss << node.position[0] << ", " << node.position[1] << "), (";
        for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
            ss << *node.distributions[dir].neighbour << ", ";
        ss << ")" << '\n';
        std::cout << ss.str();
    }

    void report(VelocitySet *set, Node *nodes, MY_SIZE_T dx, MY_SIZE_T dy)
    {
        std::cout << '\n' << '\n';
        MY_SIZE_T totalNodes = dx * dy;
        for (MY_SIZE_T idx = 0; idx < totalNodes; ++idx)
            reportOnNode(set, nodes[idx]);
    }

    // Report on the total density and the total velocity
    void report(VelocitySet *set, Node *nodes, MY_SIZE_T totalNodes)
    {
        /*
        for (MY_SIZE_T p = 0; p < totalProcessors; ++p)
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
        MY_SIZE_T nDimensions = set->nDimensions;
        double *total_velocity = new double[nDimensions]();
        for (MY_SIZE_T idx = 0; idx < totalNodes; ++idx)
        {
            total_density += density(set, nodes[idx]);

            double *node_velocity = velocity(set, nodes[idx]);
            for (MY_SIZE_T dim = 0; dim < nDimensions; ++dim)
                total_velocity[dim] += node_velocity[dim];

            delete[] node_velocity;
        }

        std::cout << "Total density: " << total_density << '\n';


        return;
        std::cout << "Total velocity: ";
        for (MY_SIZE_T dim = 0; dim < nDimensions; ++dim)
            std::cout << total_velocity[dim] << '\t';
        std::cout << '\n';

        delete[] total_velocity;
    }

    void report(std::string outputFileName, VelocitySet *set, Node *nodes, MY_SIZE_T totalNodes)
    {
        std::ofstream out(outputFileName, std::ios::out | std::ios::app);
        Reporting::MatlabReporter reporter(out);
        reporter.reportOnTimeStep(set, nodes, totalNodes);
    }
}