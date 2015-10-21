#include "node.h"

#include "../config.h"

// Computes the total density on a node
double density(VelocitySet *set, Node node)
{
    double density = 0;
    for (MY_SIZE_T dir = 0; dir < set->nDirections; ++dir)
        density += node.distributions[dir].value;

    return density;
}

// Computes the projected velocity in each dimension
double *velocity(VelocitySet *set, Node node)
{
    MY_SIZE_T nDirections = set->nDirections;
    MY_SIZE_T nDimensions = set->nDimensions;
    double nodeDensity = density(set, node);
    double *velocity = new double[nDimensions]();

    // compute the velocity in each dimension taking in account
    // the form of our velocity set
    for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
    {
        double distribution = node.distributions[dir].value;
        for (MY_SIZE_T dim = 0; dim < nDimensions; ++dim)
            velocity[dim] += distribution * set->weight(dir) * set->direction(dir)[dim];
    }

    for (MY_SIZE_T dim = 0; dim < nDimensions; ++dim)
        velocity[dim] /= nodeDensity;

    return velocity;
}

double *equilibrium(VelocitySet *set, Node node)
{
    MY_SIZE_T nDirections = set->nDirections;
    MY_SIZE_T nDimensions = set->nDimensions;
    double speedOfSoundSquared = set->speedOfSoundSquared();
    double node_density = density(set, node);
    double *node_velocity = velocity(set, node);

    // for incompressible flow (?)
    // double initiali_density = 1.0;

    // Pre calculate the speed of the node
    double speedSquared = 0;
    for (MY_SIZE_T dim = 0; dim < nDimensions; ++dim)
        speedSquared += node_velocity[dim] * node_velocity[dim];
    speedSquared /= (2 * speedOfSoundSquared);

    double *equilibrium = new double[nDirections];
    for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
    {
        double cu = 0;
        for (MY_SIZE_T dim = 0; dim < nDimensions; ++dim)
            cu = set->direction(dir)[dim] * node_velocity[dim];
        cu /= speedOfSoundSquared;

        // compressible
        equilibrium[dir] = node_density * set->weight(dir) * (
            1.0 +
            cu +
            0.5 * cu * cu -
            speedSquared
        );

        // incompressible (?)
        // equilibrium[dir] = set->weight(dir) * (
        //     node_density + initiali_density * (
        //         cu +
        //         0.5 * cu * cu -
        //         speedSquared
        //     )
        // );
    }

    delete[] node_velocity;
    return equilibrium;
}