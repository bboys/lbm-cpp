#ifndef INCLUDED_DOMAIN_Lid_Driven_Cavity
#define INCLUDED_DOMAIN_Lid_Driven_Cavity

struct Node;

#include "../BoundaryConditions/ZouHe.h"

// Currently we only save ZouHe boundary nodes

// struct ZouHe {
//     Node *node;
//     Direction direction;
// };

// struct ZouHeCorner {
//     Node *node;
//     Direction direction;
//     Node *interpolateFrom;
// };

#include "../VelocitySets/VelocitySet.h"
#include "../LBM/node.h"
#include "../Reporting/reporting.h"

// struct Node;
// struct VelocitySet;

namespace Domains { namespace LidDrivenCavity {

    void initializeNodeAt(VelocitySet &set, Node &node, size_t idx);
    void connectNodeToNeighbours(VelocitySet &set, Node *nodes, size_t x, size_t y, size_t dx, size_t dy);
    Node *initialize(VelocitySet &set, size_t &totalNodes, size_t dx, size_t dy);
    BoundaryNode *boundaryNodes(Node *nodes, size_t dx, size_t dy, size_t &totalBoundaryNodes);

    bool periodic(size_t x, size_t y, size_t dx, size_t dy);
    bool bounceback(size_t x, size_t y, size_t dx, size_t dy);
    // bool pressure(size_t x, size_t y, size_t dx, size_t dy);
    bool zouHeB(size_t x, size_t y, size_t dx, size_t dy);
    bool points_outwards(size_t x, size_t y, size_t dx, size_t dy);

    void applyBoundaryConditions(VelocitySet set, BoundaryNode *nodes, size_t totalBoundaryNodes);

}}

#endif



#ifndef INCLUDED_DOMAINS_LID_DRIVEN_CAVITY
#define INCLUDED_DOMAINS_LID_DRIVEN_CAVITY

#include "DomainInitializer.h"
#include "../VelocitySets/d2q9.h"

namespace Domains {
    class LidDrivenCavityDomain : public DomainInitializer {

        public:
            LidDrivenCavityDomain(VelocitySet &set, std::vector<size_t> domainSize);
            ~LidDrivenCavityDomain();

        protected:
            void connectNodeToNeighbours(Node &node) override;

            bool isInDomain(std::vector<int> position) override;
            bool isBounceBack(std::vector<int> position);
            bool isZouHe(std::vector<int> position);
            bool pointsOutwards(std::vector<int> position);

            void createBoundaryNodes() override;
            BoundaryNode &initializeBoundaryNode(Node &node) override;
    };
}

#endif