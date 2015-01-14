#include "PointDomain.h"
#include "config.h"

namespace Domains {
    PointDomain::PointDomain(VelocitySet *set, std::vector<MY_SIZE_T> domainSize)
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
        if (
            (position[0] == 10 && position[1] == 10) ||
            (position[0] == 10 && position[1] == 30) ||
            (position[0] == 30 && position[1] == 30) ||
            (position[0] == 30 && position[1] == 10) ||
            (position[0] == 5 && position[1] == 5) ||
            (position[0] == 5 && position[1] == 25) ||
            (position[0] == 25 && position[1] == 25) ||
            (position[0] == 25 && position[1] == 5)
            )
        {
            MY_SIZE_T nDirections = d_set->nDirections;
            // Set distributions
            for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
            {
                node.distributions[dir].value *= 10;
                node.distributions[dir].nextValue *= 10;
            }
        }
        return node;
    }

    void PointDomain::connectNodeToNeighbours(MY_SIZE_T idx)
    {
        MY_SIZE_T nDirections = d_set->nDirections;

        for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
        {
            std::vector<int> neighbour;
            for (MY_SIZE_T dim = 0; dim < d_domain_size.size(); ++dim)
                neighbour.push_back(
                    d_nodes[idx].position[dim] + d_set->direction(dir)[dim]
                );

            if (isBounceBack(neighbour))
            {
                MY_SIZE_T op_dir =d_set->oppositeDirectionOf(dir);
                d_nodes[idx].distributions[dir].neighbour = &d_nodes[idx].distributions[op_dir].nextValue;
            }
            else
            {
                MY_SIZE_T neighbour_idx = idxOf(neighbour);
                d_nodes[idx].distributions[dir].neighbour = &d_nodes[neighbour_idx].distributions[dir].nextValue;
            }
        }
    }

    bool PointDomain::isInDomain(std::vector<int> position)
    {
        return (
            position[0] >= 0 && position[0] < d_domain_size[0] &&
            position[1] >= 0 && position[1] < d_domain_size[1]
        ) && true;
        //  not (
        //     position[0] == 65 && position[1] > 20 && position[1] < 65 ||
        //     position[1] == 65 && position[0] > 20 && position[0] < 65
        // );
    }

    bool PointDomain::isBounceBack(std::vector<int> position)
    {
        return not isInDomain(position);
    }
}