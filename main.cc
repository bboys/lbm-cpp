#include "main.h"

double density(VelocitySet set, Node node)
{
    double density = 0;
    for (size_t i = 0; i < set.nDimensions; ++i)
        density += node.distributions[i].value;

    return density;
}

double *velocity(VelocitySet set, Node node)
{
    size_t nDirections = set.nDirections;
    size_t nDimensions = set.nDimensions;
    double nodeDensity = density(set, node);
    // bleh, lelijk
    double *velocity = new double[nDimensions];
    for (size_t dim = 0; dim < nDimensions; ++dim)
        velocity[dim] = 0;

    // compute the velocity in each dimension taking in account
    // the form of our velocity set
    for (size_t dir = 0; dir < nDirections; ++dir)
        for (size_t dim = 0; dim < nDimensions; ++dim)
            velocity[dim] += node.distributions[dir].value
                 * set.weights[dir] * set.directions[dir][dim];

    for (size_t dim = 0; dim < nDimensions; ++dim)
        velocity[dim] /= nodeDensity;

    std::cout << '\n';
    return velocity;
}

double *equilibrium(VelocitySet set, Node node)
{
    size_t nDirections = set.nDirections;
    size_t nDimensions = set.nDimensions;
    double speedOfSoundSquared = set.speedOfSoundSquared;
    double node_density = density(set, node);
    double *node_velocity = velocity(set, node);
    // double *distributions = node.distributions;

    // Pre calculate
    double speedSquared = 0;
    for (size_t dim = 0; dim < nDimensions; ++dim)
        speedSquared += node_velocity[dim] * node_velocity[dim];
    speedSquared /= (2 * speedOfSoundSquared);

    double *equilibrium = new double[nDimensions];
    for (size_t dir = 0; dir < nDirections; ++dir)
    {
        double cu = 0;
        for (size_t dim = 0; dim < nDimensions; ++dim)
            cu = set.directions[dir][dim] * node_velocity[dir];
        cu /= speedOfSoundSquared;

        equilibrium[dir] = node_density * set.weights[dir] * (
            1 +
            cu +
            cu * cu / 2 -
            speedSquared
        );
    }

    return equilibrium;
}

bool isOnBoundary(size_t x, size_t y, size_t z)
{
    if (x == 0 && y == 0) {
        return true;
    }
    return false;
}

void report(VelocitySet set, Node *nodes, size_t totalNodes)
{
    std::cout << "Did some iterations" << '\n';

    double total_density = 0;

    for (size_t idx = 0; idx < totalNodes; ++idx)
        total_density += density(set, nodes[idx]);

    std::cout << "Total density: " << total_density << '\n';

    size_t nDimensions = set.nDimensions;
    double *total_velocity = new double[nDimensions];
    for (size_t dim = 0; dim < nDimensions; ++dim)
        total_velocity[dim] = 0;

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

void collision(VelocitySet set, Node *nodes, size_t totalNodes)
{
    // for (size_t idx = 0; idx < totalNodes; ++idx)
    // {
    //     var equilibrium = this.getEquilibrium();
    //         var velocitySet = Config.get('velocity-set');
    //         // var force = 10;
    //         for (var k = 0; k < this.distributions.length; k++) {
    //                 this.distributions[k] = this.distributions[k] -
    //                     (this.distributions[k] - equilibrium[k]) / relaxationTime;
    //                      // + 3 * velocitySet[k].dy * velocitySet[k].w * force;

    //             if (this.distributions[k] < 0) {
    //                 // console.log("Distribution is negative!", this.distributions[k]);
    //             }
    //         };
    // }
}

void stream(VelocitySet set, Node *nodes, size_t totalNodes)
{

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

Node *init(VelocitySet &velocitySet, size_t &totalNodes)
{
    // Initialize the velocity set (note: should be available to all processors)
    initializeVelocitySet(velocitySet);
    size_t nDirections = velocitySet.nDirections;
    size_t dx = 10;
    size_t dy = 10;

    totalNodes = dx * dy;
    Node *nodes = new Node[totalNodes];
    for (size_t x = 0; x < dx; ++x)
    {
        for (size_t y = 0; y < dy; ++y)
        {
            size_t idx = x * dx + y;
            nodes[idx].type = Cell;

            Distribution *distributions = new Distribution[nDirections];
            for (size_t dir = 0; dir < nDirections; ++dir)
            {
                distributions[dir].value = velocitySet.weights[dir];
                // periodic boundary
                size_t neighbour_x = x + velocitySet.directions[dir][0];
                size_t neighbour_y = y + velocitySet.directions[dir][1];
                size_t neighbour_idx = (neighbour_x % dx) * dx + (neighbour_y % dy);
                distributions[dir].neighbour = &nodes[neighbour_idx].distributions[dir].value;
            }
            nodes[idx].distributions = distributions;
        }
    }

    // for (size_t i = 0; i < totalNodes; ++i)
    // {
    // }

    std::cout << "Done" << '\n';

    return nodes;
}

int main(int argc, char **argv)
{
    VelocitySet VSet;
    Node *nodes;
    size_t totalNodes = 0;    nodes = init(VSet, totalNodes);


    size_t iterations = 1000;
    for (size_t iter = 0; iter < iterations; ++iter)
    {
        collision(VSet, nodes, totalNodes);
        stream(VSet, nodes, totalNodes);

        if (iter % 1000 == 0)
            report(VSet, nodes, totalNodes);
    }

    // Free up the memory taken by our velocity set
    size_t dimensions = VSet.nDimensions;
    delete[] VSet.weights;
    for (size_t i = 0; i < dimensions; ++i)
        delete[] VSet.directions[i];
    delete[] VSet.directions;

    for (size_t i = 0; i < totalNodes; ++i)
        delete[] nodes[i].distributions;
    delete[] nodes;

    return 0;
}
