#include "DomainInitializer.h"
#include <iostream>

namespace Domains {

    DomainInitializer::DomainInitializer(VelocitySet *set, std::vector<size_t> domainSize)
    :
        d_set(set),
        d_domain_size(domainSize)
    {}

    DomainInitializer::~DomainInitializer()
    {}

    std::unique_ptr<Domain> DomainInitializer::domain()
    {
        createNodes();
        // Return the given domain
        std::unique_ptr<Domain> domain(new Domain);
        domain->nodes = std::move(d_nodes);

        // Now that we've moved the nodes to our domain object, we can create post
        // processors which can point to these nodes
        // createBoundaryNodes(domain->nodes);

        //domain->post_processors = std::move(std::vector<PostProcessor *> d_post_processors);
        createPostProcessors(domain->nodes);
        domain->post_processors = std::move(d_post_processors);
        domain->set     = d_set;
        domain->omega = omega();

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
            if (isInDomain(position))
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
                neighbour.push_back((
                    node.position[dim] + d_set->direction(dir)[dim] + d_domain_size[dim]
                ) % d_domain_size[dim]);
            }
            size_t neighbour_idx = idxOf(neighbour);

            node.distributions[dir].neighbour = &d_nodes[neighbour_idx].distributions[dir].nextValue;
        }
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
    size_t DomainInitializer::hashIdxOf(std::vector<int> position)
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

        return hashIdx;
    }

    size_t DomainInitializer::idxOf(std::vector<int> position)
    {
        size_t hashIdx = hashIdxOf(position);
        return d_map_to_index[hashIdx];
    }
}