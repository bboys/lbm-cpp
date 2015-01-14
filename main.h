#ifndef INCLUDED_MAIN
#define INCLUDED_MAIN
#include "config.h"
/*
// #define PARALLEL (TODO)
#ifndef PARALLEL
    struct Messenger
    {
        double *source;
        MY_SIZE_T p;
        double *dest;
    };
    // als dest naar een distriubion wijst, dan hebben we hier geen nodes nodig
    communicate(Messenger *messengers, MY_SIZE_T totalMessengers);
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

std::string createFileName(MY_SIZE_T iteration, std::string setName, std::string domainName, std::vector<MY_SIZE_T> domainSize);

int main(int argc, char **argv);

#endif