#include "LidDrivenCavity.h"

namespace Domains {
    LidDrivenCavityDomain::LidDrivenCavityDomain(VelocitySet *set, std::vector<size_t> domainSize)
    :
        DomainInitializer(set, domainSize)
    {}

    LidDrivenCavityDomain::~LidDrivenCavityDomain()
    {}

    void LidDrivenCavityDomain::connectNodeToNeighbours(Node &node)
    {
        size_t nDirections = d_set->nDirections;

        std::vector<int> position;
        for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
            position.push_back(node.position[dim]);

        for (size_t dir = 0; dir < nDirections; ++dir)
        {
            std::vector<int> neighbour;
            for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
                neighbour.push_back(
                    node.position[dim] + d_set->direction(dir)[dim]
                );

            if (isZouHe(position) && !pointsOutwards(neighbour))
                node.distributions[dir].neighbour = nullptr;
            else if (isBounceBack(neighbour))
            {
                size_t op_dir = d_set->oppositeDirectionOf(dir);
                node.distributions[dir].neighbour = &node.distributions[op_dir].nextValue;
            }
            else
            {
                // Periodic boundary
                for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
                    neighbour[dim] = (neighbour[dim] + d_domain_size[dim]) % d_domain_size[dim];

                size_t neighbour_idx = idxOf(neighbour);
                node.distributions[dir].neighbour = &d_nodes[neighbour_idx].distributions[dir].nextValue;
            }
        }
    }

    bool LidDrivenCavityDomain::isInDomain(std::vector<int> position)
    {
        // square domain
        return (
            position[0] >= 0 && position[0] < d_domain_size[0] &&
            position[1] >= 0 && position[1] < d_domain_size[1]
        );
    }

    bool LidDrivenCavityDomain::isBounceBack(std::vector<int> position)
    {
        return not isInDomain(position) && not isZouHe(position);
    }

    bool LidDrivenCavityDomain::isZouHe(std::vector<int> position)
    {
        return position[1] == 0; // top wall
    }

    bool LidDrivenCavityDomain::pointsOutwards(std::vector<int> position)
    {
        return position[1] < 0; // anything above our top wall is not periodic
    }

    void LidDrivenCavityDomain::createBoundaryNodes(std::vector<Node> &nodes)
    {

    }

    BoundaryNode &LidDrivenCavityDomain::initializeBoundaryNode(Node &node)
    {

    }
}