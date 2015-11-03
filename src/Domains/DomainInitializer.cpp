#include "DomainInitializer.h"
#include <iostream>
#include <sstream>
#include <math.h>       /* ceil */


#include "../LBM/parallel_bsp.h"

namespace Domains {

    DomainInitializer::DomainInitializer(VelocitySet *set, std::vector<size_t> domainSize, size_t p, size_t totalProcessors)
    :
        d_p(p),
        d_total_processors(totalProcessors),
        d_set(set),
        d_domain_size(domainSize)
    {}

    std::unique_ptr<Domain> DomainInitializer::domain()
    {
        createNodes();

        std::unique_ptr<Domain> domain(new Domain);
        domain->nodes = std::move(d_nodes);

        // Now that we've moved the nodes to our domain object, we can create post
        // processors which can point to these nodes
        createPostProcessors(domain->nodes);
        domain->post_processors = std::move(d_post_processors);

        domain->set    = d_set;
        domain->omega = omega();
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

        size_t potentialTotalNodes = 1;
        for (auto sizeOfDimension : d_domain_size)
            potentialTotalNodes *= sizeOfDimension;

        for (size_t idx = 0; idx < potentialTotalNodes; ++idx)
        {
            size_t currentIndex = idx;

            // Get the new position
            std::vector<int> position;
            position.push_back(currentIndex % d_domain_size[0]);
            for (size_t dim = 1; dim < d_domain_size.size(); ++dim)
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

        // Connecting the nodes to their neighbours and set up the messengers

        // setting the tagsize such that we can send the hash idx of a message
        MCBSP_BYTESIZE_TYPE tag_size = sizeof(size_t);
        bsp_set_tagsize(&tag_size);
        bsp_sync();

        for (size_t idx = 0; idx < d_nodes.size(); ++idx)
            connectNodeToNeighbours(idx);

        // Connect the messengers to their nodes
        for (size_t idx = 0; idx < d_messengers.size(); ++idx)
        {
            size_t node_idx = d_messengers[idx].d_tag[0];
            d_nodes[node_idx].distributions[d_messengers[idx].d_tag[1]].neighbour = &d_messengers[idx].d_src;
        }

        retrieveMessengers();
    }

    Node DomainInitializer::initializeNodeAt(std::vector<int> &position)
    {
        size_t nDirections = d_set->nDirections;
        size_t nDimensions = d_set->nDimensions;

        Node node;
        // Set position
        node.position = new size_t[nDimensions];
        for (size_t dim = 0; dim < nDimensions; ++dim)
            node.position[dim] = position[dim];

        // Set distributions
        Distribution *distributions = new Distribution[nDirections];
        for (size_t dir = 0; dir < nDirections; ++dir)
        {
            distributions[dir].value     = d_set->weight(dir);
            distributions[dir].nextValue = d_set->weight(dir);
        }
        node.distributions = distributions;

        return node;
    }

    void DomainInitializer::connectNodeToNeighbours(size_t idx)
    {
        size_t nDirections = d_set->nDirections;

        for (size_t dir = 0; dir < nDirections; ++dir)
        {
            std::vector<int> neighbour;
            for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
            {
                // get the neighbour in this direction, using periodic boundary
                neighbour.push_back((
                    d_nodes[idx].position[dim] + d_set->direction(dir)[dim] + d_domain_size[dim]
                ) % d_domain_size[dim]);
            }

            sendLocationOfDistribution(d_nodes[idx], dir);

            size_t p = processorOfNode(neighbour);
            // if the node is in the current processor
            if (p == d_p)
            {
                size_t neighbour_idx = idxOf(neighbour);
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
    void DomainInitializer::sendLocationOfDistribution(Node &node, size_t dir)
    {
        std::vector<int> neighbour;
        for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
        {
            // get the neighbour in this direction, using periodic boundary
            neighbour.push_back((
                node.position[dim] - d_set->direction(dir)[dim] + d_domain_size[dim]
            ) % d_domain_size[dim]);
        }

        size_t p = processorOfNode(neighbour);
        if (p == d_p)
            return;

        // we send the local index of the node to the messenger
        std::vector<int> position;
        for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
            position.push_back(node.position[dim]);

        // tag should contain the position and direction
        // the tag tells us where the messenger is located
        auto tag = hashIdxOf(position, dir);
        size_t src = idxOf(position);// the local index of this node
        bsp_send(p, &tag, &src, sizeof(double *));
    }


    size_t DomainInitializer::processorOfNode(std::vector<int> &position)
    {
        // splitting vertically
        double p = static_cast<double>(d_total_processors * position[0]) / d_domain_size[0];
        return static_cast<size_t>(floor(p));
    }

    void DomainInitializer::createPostProcessors(std::vector<Node> &nodes)
    {
        // We don't need any post processors for this "dummy" domain
    }

    void DomainInitializer::retrieveMessengers()
    {
        // setup messengers (for the parallelisation of the code)
        bsp_sync();
        // get the destination from bsp and apply it to the appropriate messenger
        MCBSP_NUMMSG_TYPE nmessages = 0;
        MCBSP_BYTESIZE_TYPE nbytes = 0;
        bsp_qsize(&nmessages, &nbytes);
        for (MCBSP_NUMMSG_TYPE n = 0; n < nmessages; ++n)
        {
            size_t idx; // the hashIdx of the current messenger
            MCBSP_BYTESIZE_TYPE status = 0;
            bsp_get_tag(&status,&idx);

            if (status > 0)
            {
                size_t localIdx = 0;
                bsp_move(&localIdx, status);
                d_messengers[d_map_to_messenger[idx]].d_tag[0] = localIdx;
            }
            else
                throw "Couldn't move the local idx during initialization phase.";
        }
    }

    bool DomainInitializer::isInDomain(std::vector<int> &position)
    {
        return true;
    }

    // Creates an hash index from a vector
    // example: given bounds [20, 30, 40], position: [4, 3, 5]
    // idx = 4 + 20 * 3 + 20 * 30 * 5
    size_t DomainInitializer::hashIdxOf(std::vector<int> & position, size_t direction)
    {
        if (position.size() != d_domain_size.size())
            throw std::string("Position is not compatible with domain size");

        size_t hashIdx = position[0];
        size_t multiplier = d_domain_size[0];

        for (size_t dim = 1; dim < d_domain_size.size(); ++dim)
        {
            hashIdx += multiplier * position[dim];
            multiplier *= d_domain_size[dim];
        }
        // If a direction was given, then we add the additional information
        hashIdx += direction * multiplier;

        return hashIdx;
    }

    size_t DomainInitializer::idxOf(std::vector<int> &position)
    {
        size_t hashIdx = hashIdxOf(position);
        return d_map_to_index[hashIdx];
    }
}