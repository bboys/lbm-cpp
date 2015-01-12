#include "PointDomain.h"

namespace Domains {
    PointDomain::PointDomain(VelocitySet *set, std::vector<size_t> domainSize)
    :
        DomainInitializer(set, domainSize)
    {}

    PointDomain::~PointDomain()
    {}

    double PointDomain::omega()
    {
        return 1.0 / 0.6;
    }

    Node PointDomain::initializeNodeAt(std::vector<int> position)
    {
        Node node = DomainInitializer::initializeNodeAt(position);
        if (position[0] == 50 && position[1] == 50)
        {
            size_t nDirections = d_set->nDirections;
            // Set distributions
            for (size_t dir = 0; dir < nDirections; ++dir)
            {
                node.distributions[dir].value *= 2.2;
                node.distributions[dir].nextValue *= 2.2;
            }
        }
        return node;
    }

    void PointDomain::connectNodeToNeighbours(Node &node)
    {
        size_t nDirections = d_set->nDirections;

        for (size_t dir = 0; dir < nDirections; ++dir)
        {
            std::vector<int> neighbour;
            for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
                neighbour.push_back(
                    node.position[dim] + d_set->direction(dir)[dim]
                );

            if (isBounceBack(neighbour))
            {
                size_t op_dir =d_set->oppositeDirectionOf(dir);
                node.distributions[dir].neighbour = &node.distributions[op_dir].nextValue;
            }
            else
            {
                size_t neighbour_idx = idxOf(neighbour);
                node.distributions[dir].neighbour = &d_nodes[neighbour_idx].distributions[dir].nextValue;
            }
        }
    }

    bool PointDomain::isInDomain(std::vector<int> position)
    {
        return (
            position[0] >= 0 && position[0] < d_domain_size[0] &&
            position[1] >= 0 && position[1] < d_domain_size[1]
        );
    }

    bool PointDomain::isBounceBack(std::vector<int> position)
    {
        return not isInDomain(position);
    }
}