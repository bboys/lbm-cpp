#include "test.h"

using namespace Reporting;
using namespace VelocitySets::D2Q9;
using namespace Domains;

void stream(VelocitySet &set, std::vector<Node> nodes)
{
    size_t nDirections = set.nDirections;

    // Stream each distribution to the neighbouring nodes
    // since neighbour is a pointer to a boolean we only have to assign a new value
    // to the neighbour
    for (size_t idx = 0; idx < nodes.size(); ++idx)
        for (size_t dir = 0; dir < nDirections; ++dir)
            if (nodes[idx].distributions[dir].neighbour != nullptr)
                *nodes[idx].distributions[dir].neighbour = nodes[idx].distributions[dir].value;
}

void testDomain(DomainInitializer *initializer)
{
    auto domain = initializer->domain();

    // std::cout << "Before streaming: " << '\n' << '\n' << '\n';
    // for (auto node : domain->nodes)
    //     ::Reporting::reportOnDistributions(domain->set, node);

    stream(domain->set, domain->nodes);

    std::cout << "After streaming: " << '\n' << '\n' << '\n';
    for (auto node : domain->nodes)
        ::Reporting::reportOnDistributions(domain->set, node);


    for (auto node : domain->nodes)
    {
        delete[] node.distributions;
        delete[] node.position;
    }
}


int main(int argc, char **argv)
{
    VelocitySet set;
    initializeVelocitySet(set);
    std::vector<size_t> domainSize = {3, 3};
    DomainInitializer initializer(set, domainSize);
    testDomain(&initializer);

    BoxedDomain boxed(set, domainSize);
    testDomain(&boxed);

    // Free up the memory taken by our velocity set
    size_t nDirections = set.nDirections;
    delete[] set.weights;
    for (size_t dir = 0; dir < nDirections; ++dir)
        delete[] set.directions[dir];
    delete[] set.directions;
}