#include "DomainInitializer.h"
#include "config.h"
#include <iostream>
#include <sstream>
#include <math.h>       /* ceil */


#include "../LBM/parallel.h"

namespace Domains {

    DomainInitializer::DomainInitializer(VelocitySet *set, std::vector<MY_SIZE_T> domainSize, MY_SIZE_T p, MY_SIZE_T totalProcessors)
    :
        d_p(p),
        d_total_processors(totalProcessors),
        d_set(set),
        d_domain_size(domainSize)
    {}

    DomainInitializer::~DomainInitializer()
    {}

    std::unique_ptr<Domain> DomainInitializer::domain()
    {
        // setting the tagsize such that we can send the hash idx of a message
        MCBSP_BYTESIZE_TYPE tag_size = sizeof(MY_SIZE_T);
        bsp_set_tagsize(&tag_size);
        bsp_sync();
        createNodes();

        std::unique_ptr<Domain> domain(new Domain);
        domain->nodes = std::move(d_nodes);

        // Now that we've moved the nodes to our domain object, we can create post
        // processors which can point to these nodes
        createPostProcessors(domain->nodes);
        domain->post_processors = std::move(d_post_processors);

        domain->set    = d_set;
        domain->omega = omega();

        // setup messengers (for the parallelisation of the code)
        bsp_sync();
        // get the destination from bsp and apply it to the appropriate messenger
        MCBSP_NUMMSG_TYPE nmessages = 0;
        MCBSP_BYTESIZE_TYPE nbytes = 0;
        bsp_qsize(&nmessages, &nbytes);
        for (MCBSP_NUMMSG_TYPE n = 0; n < nmessages; ++n)
        {
            MY_SIZE_T idx; // the hashIdx of the current messenger
            MCBSP_BYTESIZE_TYPE status;
            bsp_get_tag(&status,&idx);

            if (status > 0)
            {
                MY_SIZE_T localIdx = 0;
                bsp_move(&localIdx, status);
                d_messengers[d_map_to_messenger[idx]].d_tag[0] = localIdx;
            }
            else
                throw "Couldn't move the local idx during initialization phase.";
        }

        domain->messengers = std::move(d_messengers);

        return domain;
    }

    double DomainInitializer::omega()
    {
        return 1.0;
    }

    void DomainInitializer::createNodes()
    {
        std::vector<Node> nodes;

        MY_SIZE_T potentialTotalNodes = 1;
        for (auto sizeOfDimension : d_domain_size)
            potentialTotalNodes *= sizeOfDimension;

        for (MY_SIZE_T idx = 0; idx < potentialTotalNodes; ++idx)
        {
            MY_SIZE_T currentIndex = idx;

            // Get the new position
            std::vector<int> position;
            position.push_back(currentIndex % d_domain_size[0]);
            for (MY_SIZE_T dim = 1; dim < d_domain_size.size(); ++dim)
            {
                currentIndex = (currentIndex - position[dim - 1]) / d_domain_size[dim - 1];
                position.push_back(currentIndex % d_domain_size[dim]);
            }

            // Add a new node if the position is in our domain
            if (isInDomain(position) && processorOfNode(position) == d_p)
            {
                d_map_to_index[idx] = d_nodes.size();
                d_nodes.push_back(initializeNodeAt(position));
            }
        }

        for (MY_SIZE_T idx = 0; idx < d_nodes.size(); ++idx)
            connectNodeToNeighbours(idx);

        // Connect the messengers to their nodes
        for (MY_SIZE_T idx = 0; idx < d_messengers.size(); ++idx)
        {
            MY_SIZE_T node_idx = d_messengers[idx].d_tag[0];
            d_nodes[node_idx].distributions[d_messengers[idx].d_tag[1]].neighbour = &d_messengers[idx].d_src;
        }
    }

    Node DomainInitializer::initializeNodeAt(std::vector<int> &position)
    {
        MY_SIZE_T nDirections = d_set->nDirections;
        MY_SIZE_T nDimensions = d_set->nDimensions;

        Node node;
        // Set position
        node.position = new MY_SIZE_T[nDimensions];
        for (MY_SIZE_T dim = 0; dim < nDimensions; ++dim)
            node.position[dim] = position[dim];

        // Set distributions
        Distribution *distributions = new Distribution[nDirections];
        for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
        {
            distributions[dir].value     = d_set->weight(dir);
            distributions[dir].nextValue = d_set->weight(dir);
        }
        node.distributions = distributions;

        return node;
    }

    void DomainInitializer::connectNodeToNeighbours(MY_SIZE_T idx)
    {
        MY_SIZE_T nDirections = d_set->nDirections;

        for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
        {
            std::vector<int> neighbour;
            for (MY_SIZE_T dim = 0; dim < d_domain_size.size(); ++dim)
            {
                // get the neighbour in this direction, using periodic boundary
                neighbour.push_back((
                    d_nodes[idx].position[dim] + d_set->direction(dir)[dim] + d_domain_size[dim]
                ) % d_domain_size[dim]);
            }

            sendLocationOfDistribution(d_nodes[idx], dir);

            MY_SIZE_T p = processorOfNode(neighbour);
            // if the node is in the current processor
            if (p == d_p)
            {
                MY_SIZE_T neighbour_idx = idxOf(neighbour);
                d_nodes[idx].distributions[dir].neighbour = &d_nodes[neighbour_idx].distributions[dir].nextValue;
            }
            else
            {
                // Create a new messenger and remember its location based on the
                // position and dir of the current node
                d_map_to_messenger[hashIdxOf(neighbour, dir)] = d_messengers.size();
                d_messengers.push_back(create_messenger(p, dir));
                // save the index of the node connected to this messenger such that we can connect them once we've created all messengers
                d_messengers.back().d_tag[0] = idx;
            }
        }
    }

    // get the node pointing to this distribution and if it is not in
    // the current processor, then send the source of this distribution
    // to that processor
    void DomainInitializer::sendLocationOfDistribution(Node &node, MY_SIZE_T dir)
    {
        std::vector<int> neighbour;
        for (MY_SIZE_T dim = 0; dim < d_domain_size.size(); ++dim)
        {
            // get the neighbour in this direction, using periodic boundary
            neighbour.push_back((
                node.position[dim] - d_set->direction(dir)[dim] + d_domain_size[dim]
            ) % d_domain_size[dim]);
        }

        MY_SIZE_T p = processorOfNode(neighbour);
        if (p == d_p)
            return;

        // we send the local index of the node to the messenger
        std::vector<int> position;
        for (MY_SIZE_T dim = 0; dim < d_domain_size.size(); ++dim)
            position.push_back(node.position[dim]);

        // tag should contain the position and direction
        // the tag tells us where the messenger is located
        auto tag = hashIdxOf(position, dir);
        MY_SIZE_T src = idxOf(position);// the local index of this node
        bsp_send(p, &tag, &src, sizeof(double *));
    }

    MY_SIZE_T DomainInitializer::processorOfNode(std::vector<int> &position)
    {
        // splitting vertically
        double p = static_cast<double>(d_total_processors * position[0]) / d_domain_size[0];
        return static_cast<MY_SIZE_T>(floor(p));
    }

    void DomainInitializer::createPostProcessors(std::vector<Node> &nodes)
    {
        // We don't need any post processors for this "dummy" domain
    }

    bool DomainInitializer::isInDomain(std::vector<int> &position)
    {
        return true;
    }

    // Creates an hash index from a vector
    // example: given bounds [20, 30, 40], position: [4, 3, 5]
    // idx = 4 + 20 * 3 + 20 * 30 * 5
    MY_SIZE_T DomainInitializer::hashIdxOf(std::vector<int> &position, MY_SIZE_T direction)
    {
        if (position.size() != d_domain_size.size())
            throw std::string("Position is not compatible with domain size");

        MY_SIZE_T hashIdx = position[0];
        MY_SIZE_T multiplier = d_domain_size[0];

        for (MY_SIZE_T dim = 1; dim < d_domain_size.size(); ++dim)
        {
            hashIdx += multiplier * position[dim];
            multiplier *= d_domain_size[dim];
        }
        // If a direction was given, then we add the additional information
        hashIdx += direction * multiplier;

        return hashIdx;
    }

    MY_SIZE_T DomainInitializer::idxOf(std::vector<int> &position)
    {
        MY_SIZE_T hashIdx = hashIdxOf(position);
        return d_map_to_index[hashIdx];
    }
}