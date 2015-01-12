#ifndef INCLUDED_DOMAINS_DOMAIN
#define INCLUDED_DOMAINS_DOMAIN

#include "../LBM/node.h"
#include "../LBM/Messenger.h"
#include "../VelocitySets/VelocitySet.h"
#include "../BoundaryConditions/BoundaryNode.h"
#include "../BoundaryConditions/PostProcessor.h"
#include <memory>

struct Domain {
    std::vector<Node> nodes;
    // Change this to PostProcessor *
    std::vector<BoundaryNode> b_nodes;
    VelocitySet *set;
    double omega;
    std::vector<std::unique_ptr<BoundaryConditions::PostProcessor>> post_processors;

    // maybe add an ifdef statement ?
    std::vector<Messenger> messengers;
};

#endif