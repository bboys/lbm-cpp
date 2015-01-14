#include "test.h"

#include "config.h"
using namespace Reporting;
// using namespace VelocitySets::OLD::D2Q9;
using namespace Domains;

void testDomain(DomainInitializer *initializer)
{
    LBM::Simulation sim(initializer);
    sim.step();
}

int main(int argc, char **argv)
{
    D2Q9 *set = new D2Q9;
    std::vector<MY_SIZE_T> domainSize = {3, 3};
    DomainInitializer initializer(set, domainSize);
    testDomain(&initializer);

    BoxedDomain boxed(set, domainSize);
    testDomain(&boxed);
    delete set;
}