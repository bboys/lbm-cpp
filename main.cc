// inspiration:
// https://github.com/anders-dc/lbm-d3q19/blob/master/lbm.c

#include "main.h"
#include <ctime>

// using namespace Domains::LidDrivenCavity;
using namespace Domains::Periodic;
using namespace Reporting;

const size_t ITERATIONS = 10;
const size_t REPORT_PER_ITERATION = 1;

// Computes the total density on a node
double density(VelocitySet &set, Node node)
{
    double density = 0;
    for (size_t dir = 0; dir < set.nDirections; ++dir)
        density += node.distributions[dir].value;

    return density;
}

// Computes the projected velocity in each dimension
double *velocity(VelocitySet &set, Node node)
{
    size_t nDirections = set.nDirections;
    size_t nDimensions = set.nDimensions;
    double nodeDensity = density(set, node);
    double *velocity = new double[nDimensions]();

    // compute the velocity in each dimension taking in account
    // the form of our velocity set
    for (size_t dir = 0; dir < nDirections; ++dir)
    {
        double distribution = node.distributions[dir].value;
        for (size_t dim = 0; dim < nDimensions; ++dim)
            velocity[dim] += distribution * set.weights[dir] * set.directions[dir][dim];
    }

    for (size_t dim = 0; dim < nDimensions; ++dim)
        velocity[dim] /= nodeDensity;

    return velocity;
}

double *equilibrium(VelocitySet &set, Node node)
{
    size_t nDirections = set.nDirections;
    size_t nDimensions = set.nDimensions;
    double speedOfSoundSquared = set.speedOfSoundSquared;
    double node_density = density(set, node);
    double *node_velocity = velocity(set, node);

    // Pre calculate the speed of the node
    double speedSquared = 0;
    for (size_t dim = 0; dim < nDimensions; ++dim)
        speedSquared += node_velocity[dim] * node_velocity[dim];
    speedSquared /= (2 * speedOfSoundSquared);

    double *equilibrium = new double[nDirections];
    for (size_t dir = 0; dir < nDirections; ++dir)
    {
        double cu = 0;
        for (size_t dim = 0; dim < nDimensions; ++dim)
            cu = set.directions[dir][dim] * node_velocity[dim];
        cu /= speedOfSoundSquared;

        equilibrium[dir] = node_density * set.weights[dir] * (
            1.0 +
            cu +
            0.5 * cu * cu -
            speedSquared
        );
    }

    delete[] node_velocity;
    return equilibrium;
}

void collideNode(VelocitySet &set, Node &node)
{
    size_t nDirections = set.nDirections;

    // switch to the newly streamed distribution values
    for (size_t dir = 0; dir < nDirections; ++dir)
    {
        node.distributions[dir].value = node.distributions[dir].nextValue;
        node.distributions[dir].nextValue = 0;
    }

    // TODO: make dependent on either velocity set, or domain problem
    double relaxation = 1.0 / 3.0;
    double * node_equilibrium = equilibrium(set, node);

    // apply BGK approximation
    for (size_t dir = 0; dir < nDirections; ++dir)
        node.distributions[dir].value = node.distributions[dir].value -
            (node.distributions[dir].value - node_equilibrium[dir]) / relaxation;

    delete[] node_equilibrium;
}

void collision(VelocitySet &set, Node *nodes, size_t totalNodes)
{
    for (size_t idx = 0; idx < totalNodes; ++idx)
    {
        if (nodes[idx].type == BounceBack)
            continue;
        collideNode(set, nodes[idx]);
    }
}

void stream(VelocitySet &set, Node *nodes, size_t totalNodes)
{
    size_t nDirections = set.nDirections;

    // Stream each distribution to the neighbouring nodes
    // since neighbour is a pointer to a boolean we only have to assign a new value
    // to the neighbour
    for (size_t idx = 0; idx < totalNodes; ++idx)
    {
        if (nodes[idx].type == BounceBack)
            continue;
        for (size_t dir = 0; dir < nDirections; ++dir)
            *nodes[idx].distributions[dir].neighbour = nodes[idx].distributions[dir].value;
    }
}

int main(int argc, char **argv)
{
    VelocitySet set;
    Node *nodes;
    size_t totalNodes = 0;
    size_t totalBoundaryNodes = 0;
    size_t dx = 5;
    size_t dy = 5;
    nodes = initialize(set, totalNodes, dx, dy);
    Node ** bNodes = boundaryNodes(nodes, dx, dy, totalBoundaryNodes);

    std::clock_t    start;
    start = std::clock();
    report(set, nodes, totalNodes);
    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        collision(set, nodes, totalNodes);
        stream(set, nodes, totalNodes);
        applyBoundaryConditions(set, bNodes, totalBoundaryNodes);

        if (iter % REPORT_PER_ITERATION == 0)
            report(set, nodes, totalNodes);
            // report(set, nodes, dx, dy);
    }
    report(set, nodes, dx, dy);
    report(set, nodes, totalNodes);
    std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << '\n';

    // Free up the memory taken by our velocity set
    size_t nDirections = set.nDirections;
    delete[] set.weights;
    for (size_t dir = 0; dir < nDirections; ++dir)
        delete[] set.directions[dir];
    delete[] set.directions;

    delete[] bNodes;
    for (size_t idx = 0; idx < totalNodes; ++idx)
        delete[] nodes[idx].distributions;
    delete[] nodes;

    return 0;
}
