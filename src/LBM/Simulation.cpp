#include "Simulation.h"
#include "node.h"
#include <memory>

namespace LBM {

    Simulation::Simulation(Initializer_Ptr initializer)
    :
        d_domain(initializer->domain())
    {}

    Simulation::Simulation(std::unique_ptr<Domain> domain)
    :
        d_domain(std::move(domain))
    {}

    Simulation::~Simulation()
    {
        for (auto node : d_domain->nodes)
        {
            delete[] node.distributions;
            delete[] node.position;
        }
    }

    void Simulation::step()
    {
        stream(d_domain->set, d_domain->nodes);
        communicate(d_domain->messengers);
        postStreamProcess();
        collission(d_domain->set, d_domain->nodes);
    }

    void Simulation::stream(VelocitySet *set, std::vector<Node> &nodes)
    {
        size_t nDirections = set->nDirections;

        // Stream each distribution to the neighbouring nodes
        // since neighbour is a pointer to a boolean we only have to assign a new value
        // to the neighbour
        for (size_t idx = 0; idx < nodes.size(); ++idx)
            for (size_t dir = 0; dir < nDirections; ++dir)
                if (nodes[idx].distributions[dir].neighbour != nullptr)
                    *nodes[idx].distributions[dir].neighbour = nodes[idx].distributions[dir].value;
    }

    void Simulation::postStreamProcess()
    {
        for (size_t idx = 0; idx < d_domain->post_processors.size(); ++idx)
            d_domain->post_processors[idx]->process();
    }

    void Simulation::collission(VelocitySet *set, std::vector<Node> &nodes)
    {
        double omega = d_domain->omega;
        for (auto node : nodes)
        {
            size_t nDirections = set->nDirections;
            // switch to the newly streamed distribution values
            for (size_t dir = 0; dir < nDirections; ++dir)
            {
                node.distributions[dir].value = node.distributions[dir].nextValue;
                node.distributions[dir].nextValue = -1;
            }

            // apply BGK approximation
            auto node_equilibrium = equilibrium(set, node);
            for (size_t dir = 0; dir < nDirections; ++dir)
                node.distributions[dir].value = node.distributions[dir].value -
                    omega * (node.distributions[dir].value - node_equilibrium[dir]);

            delete[] node_equilibrium;
        }
    }
}