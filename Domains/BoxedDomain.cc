#include "BoxedDomain.h"

extern "C" {
    #include "mcbsp.h"
}

namespace Domains {
    BoxedDomain::BoxedDomain(VelocitySet *set, std::vector<size_t> domainSize, size_t p, size_t totalProcessors)
    :
        DomainInitializer(set, domainSize, p, totalProcessors)
    {}

    BoxedDomain::~BoxedDomain()
    {}

    void BoxedDomain::connectNodeToNeighbours(size_t idx)
    {
        size_t nDirections = d_set->nDirections;

        for (size_t dir = 0; dir < nDirections; ++dir)
        {
            std::vector<int> neighbour;
            for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
                neighbour.push_back(
                    d_nodes[idx].position[dim] + d_set->direction(dir)[dim]
                );

            if (isBounceBack(neighbour))
            {
                size_t op_dir =d_set->oppositeDirectionOf(dir);
                d_nodes[idx].distributions[dir].neighbour = &d_nodes[idx].distributions[op_dir].nextValue;
            }
            // else (todo)
            //     d_nodes[idx].distributions[dir].neighbour = destination(neighbour, dir, idx);
            sendLocationOfDistribution(d_nodes[idx], dir);
        }
    }

    // get the node pointing to this distribution and if it is not in
    // the current processor, then send the source of this distribution
    // to that processor
    void BoxedDomain::sendLocationOfDistribution(Node &node, size_t dir)
    {
        std::vector<int> neighbour;
        for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
        {
            // get the neighbour in this direction, using periodic boundary
            neighbour.push_back(node.position[dim] - d_set->direction(dir)[dim]);
        }

        size_t p = processorOfNode(neighbour);
        if (p == d_p || isBounceBack(neighbour))
            return;

        // we send the local index of the node to the messenger
        std::vector<int> position;
        for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
            position.push_back(node.position[dim]);

        // tag should contain the position and direction
        // the tag tells us where the messenger is located
        auto tag = hashIdxOf(position, dir);
        size_t src = idxOf(position);
        bsp_send(p, &tag, &src, sizeof(double *));
    }

    size_t BoxedDomain::processorOfNode(std::vector<int> position)
    {
        // TODO!
        if (d_total_processors < 2)
            return 0;
        // here we might want to use a distribution creator object or something alike
        if (position[0] < d_domain_size[0] / 2)
            return 0;
        else
            return 1;
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