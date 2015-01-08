// inspiration:
// https://github.com/anders-dc/lbm-d3q19/blob/master/lbm.c

#include "main.h"
#include <ctime>
#include <fstream>

using namespace Domains::LidDrivenCavity;
// using namespace Domains::Periodic;
using namespace Reporting;

const std::string outputFileName = "logs/test8.txt";
const size_t ITERATIONS = 2;
const size_t REPORT_PER_ITERATION = 1;
const size_t dx = 3;
const size_t dy = 3;

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
    double RE = 25;
    double u_x = 0.05;
    // double relaxation = 3 * u_x * dy / RE + 0.5;

    double omega = 1.0 / (3 * u_x * dx / RE + 0.5);
    // relaxation = 2.0 / 3.0;
    // double relaxation = 0.05;
    double * node_equilibrium = equilibrium(set, node);

    // apply BGK approximation
    for (size_t dir = 0; dir < nDirections; ++dir)
        node.distributions[dir].value = node.distributions[dir].value - omega *
            (node.distributions[dir].value - node_equilibrium[dir]);

    delete[] node_equilibrium;
}

void collision(VelocitySet &set, Node *nodes, size_t totalNodes)
{
    for (size_t idx = 0; idx < totalNodes; ++idx)
    {
        ::Reporting::reportOnDistributions(set, nodes[idx]);
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
        for (size_t dir = 0; dir < nDirections; ++dir)
            if (nodes[idx].distributions[dir].neighbour != nullptr)
                *nodes[idx].distributions[dir].neighbour = nodes[idx].distributions[dir].value;
}

int main(int argc, char **argv)
{
    VelocitySet set;
    Node *nodes;
    size_t totalNodes = 0;
    size_t totalBoundaryNodes = 0;

    nodes = initialize(set, totalNodes, dx, dy);
    BoundaryNode *bNodes = boundaryNodes(nodes, dx, dy, totalBoundaryNodes);

    std::clock_t    start;
    start = std::clock();
    report(set, nodes, totalNodes);

    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        stream(set, nodes, totalNodes);
        /*
        #ifndef PARALLEL
            communicate(Messenger *messengers, size_t totalMessengers);
        #endif
        */
        applyBoundaryConditions(set, bNodes, totalBoundaryNodes);
        collision(set, nodes, totalNodes);

        if (iter % REPORT_PER_ITERATION == 0)
            report(set, nodes, totalNodes);
    }
    report(outputFileName, set, nodes, totalNodes);

    std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << '\n';

    // Free up the memory taken by our velocity set
    size_t nDirections = set.nDirections;
    delete[] set.weights;
    for (size_t dir = 0; dir < nDirections; ++dir)
        delete[] set.directions[dir];
    delete[] set.directions;

    // boundaryNodes
    delete[] bNodes;
    for (size_t idx = 0; idx < totalNodes; ++idx)
    {
        delete[] nodes[idx].distributions;
        delete[] nodes[idx].position;
    }
    delete[] nodes;

    return 0;
}
