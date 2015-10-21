#ifndef INCLUDED_REPORTING
#define INCLUDED_REPORTING

#include <string>
#include "../VelocitySets/VelocitySet.h"
#include "../LBM/node.h"

namespace Reporting {
    void reportOnInitialSetup(VelocitySet *set, Node *nodes, size_t dx, size_t dy);
    void reportOnNode(VelocitySet *set, Node &node);
    void reportOnDistributions(VelocitySet *set, Node &node);
    void report(VelocitySet *set, Node *nodes, size_t dx, size_t dy);
    void report(VelocitySet *set, Node *nodes, size_t totalNodes);
    void report(std::string outputFileName, VelocitySet *set, Node *nodes, size_t totalNodes);
}

#endif