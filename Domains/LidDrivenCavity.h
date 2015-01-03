#ifndef INCLUDED_DOMAIN_Lid_Driven_Cavity
#define INCLUDED_DOMAIN_Lid_Driven_Cavity

struct Node;
enum class NodeType {
    Cell,
    BounceBack,
    Periodic,
    ZouHe
};

enum class Direction {
    N, NE, E, S, SE, SW, W, NW
};

// Currently we only save ZouHe boundary nodes
struct BoundaryNode {
    Node * node;
    Direction direction;
    union {
        double *velocity;
        double density;
    };
};

#include "../main.h"
#include "../Reporting/reporting.h"

struct Node;
struct VelocitySet;

namespace Domains { namespace LidDrivenCavity {

    void initializeNodeAt(VelocitySet &set, Node &node, size_t idx);
    void connectNodeToNeighbours(VelocitySet &set, Node *nodes, size_t x, size_t y, size_t dx, size_t dy);
    Node *initialize(VelocitySet &set, size_t &totalNodes, size_t dx, size_t dy);
    BoundaryNode *boundaryNodes(Node *nodes, size_t const dx, size_t dy, size_t &totalBoundaryNodes);

    bool periodic(size_t x, size_t y, size_t dx, size_t dy);
    bool bounceback(size_t x, size_t y, size_t dx, size_t dy);
    // bool pressure(size_t x, size_t y, size_t dx, size_t dy);
    bool zouHeB(size_t x, size_t y, size_t dx, size_t dy);
    bool points_outwards(size_t x, size_t y, size_t dx, size_t dy);

    void applyBoundaryConditions(VelocitySet set, BoundaryNode *nodes, size_t totalBoundaryNodes);

}}

#endif