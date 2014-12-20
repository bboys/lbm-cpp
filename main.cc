// https://github.com/anders-dc/lbm-d3q19/blob/master/lbm.c

#include "main.h"
#include <ctime>

const size_t ITERATIONS = 10000000;
const size_t REPORT_PER_ITERATION = 100000;

// Computes the total density on a node
double density(VelocitySet &set, Node node)
{
    double density = 0;
    for (size_t dir = 0; dir < set.nDirections; ++dir)
        density += node.distributions[dir].value;

    return density;
}

// Coputes the projected velocity in each dimension
double *velocity(VelocitySet &set, Node node)
{
    size_t nDirections = set.nDirections;
    size_t nDimensions = set.nDimensions;
    double nodeDensity = density(set, node);
    // 0 velocity
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
    // double *distributions = node.distributions;

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

void report(VelocitySet &set, Node *nodes, size_t totalNodes)
{
    std::cout << "Did some iterations" << '\n';

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

void collideNode(VelocitySet &set, Node &node)
{
    double * node_equilibrium = equilibrium(set, node);
    size_t nDirections = set.nDirections;

    // TODO: make dependent on either velocity set, or domain problem
    double relaxation = 1.0 / 2.0;

    /*
    this.dx = 128;
        this.dy = 128;
        this.vx = 0.005;
        this.config = config;
        var Reynolds = 100;// config.get('Re');
        var nu = this.vx * this.dx / Reynolds;
        this.relaxationTime = 3 * nu + 1 / 2;
        */

    for (size_t dir = 0; dir < nDirections; ++dir)
        node.distributions[dir].value = node.distributions[dir].value -
            (node.distributions[dir].value - node_equilibrium[dir]) / relaxation;

    delete[] node_equilibrium;
}

void collision(VelocitySet &set, Node *nodes, size_t totalNodes)
{
    for (size_t idx = 0; idx < totalNodes; ++idx)
        collideNode(set, nodes[idx]);
}

void stream(VelocitySet &set, Node *nodes, size_t totalNodes)
{
    size_t nDirections = set.nDirections;

    // Stream each distribution to the neighbouring nodes
    // since neighbour is a pointer to a boolean we only have to assign a new value
    // to the neighbour
    for (size_t idx = 0; idx < totalNodes; ++idx)
        for (size_t dir = 0; dir < nDirections; ++dir)
            *nodes[idx].distributions[dir].neighbour = nodes[idx].distributions[dir].value;
}

/*size_t neighbourIdxForDistribution(size_t nodeIdx, size_t distributionIdx, VelocitySet &set)
{
    // if isnt a boundary
    // get the 2D idx x, y for nodeIdx,
    // add set.directions[distributionIdx][0] to x and set.directions[distributionIdx][1] to y
    // return a transformation from 2d idx to 1d idx

    // if is a periodic boundary
    // do some modulo things

    // if other boundary, do other things, maybe this should be replaced with a set neighbours method?
}*/

int main(int argc, char **argv)
{
    VelocitySet set;
    Node *nodes;
    size_t totalNodes = 0;
    nodes = initialize(set, totalNodes);

    std::clock_t    start;
    start = std::clock();

    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        if (iter % REPORT_PER_ITERATION == 0)
            report(set, nodes, totalNodes);

        collision(set, nodes, totalNodes);
        stream(set, nodes, totalNodes);
    }
    report(set, nodes, totalNodes);
    std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << '\n';

    // Free up the memory taken by our velocity set
    size_t nDirections = set.nDirections;
    delete[] set.weights;
    for (size_t dir = 0; dir < nDirections; ++dir)
        delete[] set.directions[dir];
    delete[] set.directions;

    for (size_t idx = 0; idx < totalNodes; ++idx)
        delete[] nodes[idx].distributions;
    delete[] nodes;

    return 0;
}
