#include "ZouHeVelocityBoundaryCondition.h"
#include <iostream>

namespace BoundaryConditions {
    ZouHeVelocityBoundary::ZouHeVelocityBoundary(std::vector<double> velocity, std::vector<Node *> acts_on)
    :
        d_acts_on(acts_on),
        d_velocity(velocity)
    {}

    ZouHeVelocityNorthBoundary::ZouHeVelocityNorthBoundary(std::vector<double> velocity, std::vector<Node *> acts_on)
    :
        ZouHeVelocityBoundary(velocity, acts_on)
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