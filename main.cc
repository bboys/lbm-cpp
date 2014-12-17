#include "main.h"

bool isOnBoundary(size_t x, size_t y, size_t z)
{
    if (x == 0 && y == 0) {
        return true;
    }
    return false;
}


void collision()
{

}

void stream()
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
    for (size_t i = 0; i < totalNodes; ++i)
    {
        nodes[i].type = Cell;

        Distribution *distributions = new Distribution[nDirections];
        for (size_t j = 0; j < nDirections; ++j)
        {
            distributions[j].value = velocitySet.weights[j];
            distributions[j].neighbour = &nodes[i % 5].distributions[j].value;
        }
        nodes[i].distributions = distributions;
    }

    std::cout << "Done" << '\n';

    return nodes;
}

int main(int argc, char **argv)
{
    // A 2d lattice boltzmann simulation
    VelocitySet VSet;
    Node *nodes;
    size_t totalNodes = 0;
    nodes = init(VSet, totalNodes);

    size_t dimensions = VSet.nDimensions;
    std::cout << "Hoi" << '\n';
    // for (int i = 0; i < totalNodes; ++i)
    // {
    //     std::cout << nodes[i].type << '\t';
    //     std::cout << nodes[i].distributions[0].value << '\n';
    // }

    size_t iterations = 1000;
    for (size_t iter = 0; iter < iterations; ++iter)
    {
        collision();
        stream();

        // if (iter % 100 == 0)
        //     report();
    }

    // Free up the memory taken by our velocity set
    delete[] VSet.weights;
    for (size_t i = 0; i < dimensions; ++i)
        delete[] VSet.directions[i];
    delete[] VSet.directions;

    for (size_t i = 0; i < totalNodes; ++i)
        delete[] nodes[i].distributions;
    delete[] nodes;

    return 0;
}
