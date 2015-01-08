#include "test.h"

using namespace Reporting;
using namespace VelocitySets::D2Q9;
using namespace Domains;

void testDomain(DomainInitializer *initializer)
{
    LBM::Simulation sim(initializer);
    sim.run();
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