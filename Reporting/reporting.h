#ifndef INCLUDED_REPORTING
#define INCLUDED_REPORTING

#include "../config.h"

#include <string>
#include "../VelocitySets/VelocitySet.h"
#include "../LBM/node.h"

namespace Reporting {
    void reportOnInitialSetup(VelocitySet *set, Node *nodes, MY_SIZE_T dx, MY_SIZE_T dy);
    void reportOnNode(VelocitySet *set, Node &node);
    void reportOnDistributions(VelocitySet *set, Node &node);
    void report(VelocitySet *set, Node *nodes, MY_SIZE_T dx, MY_SIZE_T dy);
    void report(VelocitySet *set, Node *nodes, MY_SIZE_T totalNodes);
    void report(std::string outputFileName, VelocitySet *set, Node *nodes, MY_SIZE_T totalNodes);
}

#endif