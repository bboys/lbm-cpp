#ifndef INCLUDED_DOMAIN_PERIODIC
#define INCLUDED_DOMAIN_PERIODIC

#include "../main.h"
#include "../Reporting/reporting.h"

struct Node;
struct VelocitySet;

namespace Domains { namespace Periodic {

    void initializeNodeAt(VelocitySet &set, Node &node, size_t idx);
    void connectNodeToNeighbours(VelocitySet &set, Node *nodes, size_t x, size_t y, size_t dx, size_t dy);
    Node *initialize(VelocitySet &set, size_t &totalNodes, size_t dx, size_t dy);
    BoundaryNode *boundaryNodes(Node *nodes, size_t dx, size_t dy, size_t &totalBoundaryNodes);

    bool periodic(size_t x, size_t y, size_t dx, size_t dy);
    bool bounceback(size_t x, size_t y, size_t dx, size_t dy);
    void applyBoundaryConditions(VelocitySet set, BoundaryNode *nodes, size_t totalBoundaryNodes);

} }

#endif