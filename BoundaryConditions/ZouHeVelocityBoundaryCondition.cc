#include "ZouHeVelocityBoundaryCondition.h"
#include <iostream>

namespace BoundaryConditions {
    ZouHeVelocityBoundary::ZouHeVelocityBoundary(std::vector<double> velocity)
    :
        d_velocity(velocity)
    {}

    void ZouHeVelocityBoundary::add(Node *node)
    {
        d_acts_on.push_back(node);
    }

    void ZouHeVelocityNorthBoundary::add(Node *node)
    {
         ZouHeVelocityBoundary::add(node);
         // set node neighbours to nullptrs
         std::cout << d_acts_on.size() << ", ";
    }

    ZouHeVelocityNorthBoundary::ZouHeVelocityNorthBoundary(std::vector<double> velocity)
    :
        ZouHeVelocityBoundary(velocity)
    {}

    void ZouHeVelocityNorthBoundary::process()
    {
        double u_x = d_velocity[0];
        double u_y = d_velocity[1];
        double rho = 0;

        for (auto node : d_acts_on)
        {
            rho = (1 / (1 + u_y)) * (
                node->distributions[0].nextValue +
                node->distributions[1].nextValue +
                node->distributions[3].nextValue +
                2 * (
                    node->distributions[2].nextValue +
                    node->distributions[5].nextValue +
                    node->distributions[6].nextValue
                )
            );

            node->distributions[4].nextValue = node->distributions[2].nextValue;

            node->distributions[7].nextValue = node->distributions[5].nextValue +
                0.5 *(node->distributions[1].nextValue - node->distributions[3].nextValue ) -
                0.5 * rho * u_x;

            node->distributions[8].nextValue = node->distributions[6].nextValue +
                0.5 * (node->distributions[3].nextValue - node->distributions[1].nextValue) +
                0.5 * rho * u_x;
        }
    }
}