#include "BoxedDomain.h"

namespace Domains {
    BoxedDomain::BoxedDomain(VelocitySet &set, std::vector<size_t> domainSize)
    :
        DomainInitializer(set, domainSize)
    {}

    BoxedDomain::~BoxedDomain()
    {}

    void BoxedDomain::connectNodeToNeighbours(Node &node)
    {
        size_t nDirections = d_set.nDirections;

        for (size_t dir = 0; dir < nDirections; ++dir)
        {
            std::vector<int> neighbour;
            for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
                neighbour.push_back(
                    node.position[dim] + d_set.directions[dir][dim]
                );

            if (isBounceBack(neighbour))
            {
                size_t op_dir = ::VelocitySets::D2Q9::oppositeDirectionOf(dir);
                node.distributions[dir].neighbour = &node.distributions[op_dir].nextValue;
            }
            else
            {
                size_t neighbour_idx = idxOf(neighbour);
                node.distributions[dir].neighbour = &d_nodes[neighbour_idx].distributions[dir].nextValue;
            }
        }
    }

    bool BoxedDomain::isInDomain(std::vector<int> position)
    {
        return (
            position[0] >= 0 && position[0] < d_domain_size[0] &&
            position[1] >= 0 && position[1] < d_domain_size[1]
        );
    }

    bool BoxedDomain::isBounceBack(std::vector<int> position)
    {
        return not isInDomain(position);
    }
}