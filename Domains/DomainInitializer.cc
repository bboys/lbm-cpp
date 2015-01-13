#include "DomainInitializer.h"
#include <iostream>
#include <sstream>

extern "C" {
    #include "mcbsp.h"
}

namespace Domains {

    DomainInitializer::DomainInitializer(VelocitySet *set, std::vector<size_t> domainSize, size_t p, size_t totalProcessors)
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
        size_t tag_size = sizeof(size_t);
        bsp_set_tagsize(&tag_size);
        bsp_sync();

        createNodes();
        // Return the given domain
        std::unique_ptr<Domain> domain(new Domain);
        domain->nodes = std::move(d_nodes);
        // Now that we've moved the nodes to our domain object, we can create post
        // processors which can point to these nodes
        createPostProcessors(domain->nodes);
        domain->post_processors = std::move(d_post_processors);

        domain->set    = d_set;
        domain->omega = omega();

        // setup messengers
        std::cout << "Even kijken hoeveel messengers er zijn: " << d_messengers.size() << '\n';

        bsp_sync();
        // get the destination from bsp and apply it to the appropriate messenger
        unsigned int nmessages = 0;
        size_t nbytes = 0;
        bsp_qsize(&nmessages, &nbytes);
        std::stringstream ss;
        ss << "Processor " << d_p << " received " << nmessages << " messages totalling " << nbytes << " bytes\n";
        for (size_t n = 0; n < nmessages; ++n)
        {
            size_t i;
            size_t status;
            size_t localIdx = 0;

            bsp_get_tag(&status,&i);

            if (status > 0)
            {
                bsp_move(&localIdx, status);
                ss << "Tag: " << i << " with index: " << d_map_to_messenger[i] << " and payload: " << localIdx;

                d_messengers[d_map_to_messenger[i]].setLocalIdx(localIdx);
                ss << " set local index of " << d_map_to_messenger[i] << " to " << localIdx << '\n';
                ss << "Confirmation: " << d_messengers[d_map_to_messenger[i]].localIdx() << '\n';
            }
        }
        std::cout << ss.str();
        ss.clear();
        ss.str("");
        size_t s = bsp_pid();
        for (auto messenger : d_messengers)
        {
            ss << "Message from: " << s << " to: " << messenger.processor() <<
                " with direction: " << messenger.direction() <<
                " to local idx: " << messenger.localIdx() << '\n';
        }
        std::cout << ss.str() << "\n\n\n\n\n\n";

        // puur voor mooie debug messages
        domain->messengers = d_messengers;
        bsp_sync();

        return domain;
    }

    double DomainInitializer::omega()
    {
        double RE = 50;
        double u_x = 0.25;
        int d_x = 1; //d_domain_size[0];
        return 1.0 / (3 * u_x * d_x / RE + 0.5);
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

        for (auto node : d_nodes)
            connectNodeToNeighbours(node);
    }

    Node DomainInitializer::initializeNodeAt(std::vector<int> position)
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

            // Commented distributions are here for easy testing purposes
            // distributions[dir].value     = d_nodes.size() * 10 + dir;
            // distributions[dir].nextValue = -1;
        }
        node.distributions = distributions;

        return node;
    }

    void DomainInitializer::connectNodeToNeighbours(Node &node)
    {
        size_t nDirections = d_set->nDirections;

        for (size_t dir = 0; dir < nDirections; ++dir)
        {
            std::vector<int> neighbour;
            for (size_t dim = 0; dim < d_domain_size.size(); ++dim)
            {
                // get the neighbour in this direction, using periodic boundary
                neighbour.push_back((
                    node.position[dim] + d_set->direction(dir)[dim] + d_domain_size[dim]
                ) % d_domain_size[dim]);
            }

            node.distributions[dir].neighbour = destination(neighbour, dir);
            sendLocationOfDistribution(node, dir);
        }
    }

    // Gets the
    double *DomainInitializer::destination(std::vector<int> neighbour, size_t direction)
    {
        size_t p = processorOfNode(neighbour);
        // if the node is in the current processor
        if (p == d_p)
        {
            size_t neighbour_idx = idxOf(neighbour);
            return &d_nodes[neighbour_idx].distributions[direction].nextValue;
        }
        // Create a new messenger and remember its location based on the
        // position and direction of the current node
        Messenger messenger(p, direction);
        d_map_to_messenger[hashIdxOf(neighbour, direction)] = d_messengers.size();
        d_messengers.push_back(messenger);
        std::stringstream ss;
        size_t s = bsp_pid();

        ss << "Creating hash: " << hashIdxOf(neighbour, direction) << " to " << d_messengers.size() - 1 << " from p: " << s << '\n';
        std::cout << ss.str();
        return messenger.source();
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

        // send the destination of the distribution to the processor that streams
        // to this distribution
        // double *src = &node.distributions[dir].nextValue;


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

    size_t DomainInitializer::processorOfNode(std::vector<int> position)
    {
        // TODO!
        if (d_total_processors < 2)
            return 0;
        // here we might want to use a distribution creator object or something alike
        if (position[0] < d_domain_size[0] / 2)
            return 0;
        else
            return 1;
        // here we might want to use a distribution creator object or something alike
        return 0;
    }

    void DomainInitializer::createPostProcessors(std::vector<Node> &nodes)
    {

    }

    //
    bool DomainInitializer::isInDomain(std::vector<int> position)
    {
        return true;
    }

    // Creates an hash index from a vector
    // example: given bounds [20, 30, 40], position: [4, 3, 5]
    // idx = 4 + 20 * 3 + 20 * 30 * 5
    size_t DomainInitializer::hashIdxOf(std::vector<int> position, size_t direction)
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

    size_t DomainInitializer::idxOf(std::vector<int> position)
    {
        size_t hashIdx = hashIdxOf(position);
        return d_map_to_index[hashIdx];
    }
}