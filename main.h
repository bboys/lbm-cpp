#ifndef INCLUDED_MAIN
#define INCLUDED_MAIN
/*
// #define PARALLEL (TODO)
#ifndef PARALLEL
    struct Messenger
    {
        double *source;
        size_t p;
        double *dest;
    };
    // als dest naar een distriubion wijst, dan hebben we hier geen nodes nodig
    communicate(Messenger *messengers, size_t totalMessengers);
#endif
*/
#include <iostream>
#include <string>
#include <vector>

#include "VelocitySets/VelocitySet.h"
#include "LBM/node.h"
#include "VelocitySets/d2q9.h"
#include "Reporting/MatlabReporter.h"

#include "Domains/DomainInitializer.h"
#include "Domains/BoxedDomain.h"
#include "Domains/LidDrivenCavity.h"
#include "Domains/PointDomain.h"

#include "LBM/Simulation.h"

std::string createFileName(size_t iteration, std::string setName, std::string domainName, std::vector<size_t> domainSize);

int main(int argc, char **argv);

#endif