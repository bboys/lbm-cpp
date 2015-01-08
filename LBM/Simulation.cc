#include "Simulation.h"

#include "node.h"
#include "../Reporting/reporting.h"

namespace LBM {
    Simulation::Simulation(Domains::DomainInitializer *initializer)
    :
        d_domain(initializer->domain())
    {}

    Simulation::~Simulation()
    {
        for (auto node : d_domain->nodes)
        {
            delete[] node.distributions;
            delete[] node.position;
        }
    }

    void Simulation::run()
    {
        report();
        stream(d_domain->set, d_domain->nodes);
        collission(d_domain->set, d_domain->nodes);
        report();
    }

    void Simulation::stream(VelocitySet &set, std::vector<Node> &nodes)
    {
        size_t nDirections = set.nDirections;

        // Stream each distribution to the neighbouring nodes
        // since neighbour is a pointer to a boolean we only have to assign a new value
        // to the neighbour
        for (size_t idx = 0; idx < nodes.size(); ++idx)
            for (size_t dir = 0; dir < nDirections; ++dir)
                if (nodes[idx].distributions[dir].neighbour != nullptr)
                    *nodes[idx].distributions[dir].neighbour = nodes[idx].distributions[dir].value;
    }

    void Simulation::collission(VelocitySet &set, std::vector<Node> &nodes)
    {
        for (auto node : nodes)
        {
            size_t nDirections = set.nDirections;
            // switch to the newly streamed distribution values
            for (size_t dir = 0; dir < nDirections; ++dir)
            {
                node.distributions[dir].value = node.distributions[dir].nextValue;
                node.distributions[dir].nextValue = 0;
            }
            // TODO: make dependent on either velocity set, or domain problem
            // TODO : get omega / relaxation from domain
            double RE = 25;
            double u_x = 0.05;
            double dx = 100;
            double omega = 1.0 / (3 * u_x * dx / RE + 0.5);

            // apply BGK approximation
            double * node_equilibrium = equilibrium(set, node);
            for (size_t dir = 0; dir < nDirections; ++dir)
                node.distributions[dir].value = node.distributions[dir].value - omega *
                    (node.distributions[dir].value - node_equilibrium[dir]);

            delete[] node_equilibrium;
        }
    }

    void Simulation::report()
    {
        for (auto node : d_domain->nodes)
            ::Reporting::reportOnDistributions(d_domain->set, node);
    }

    void postStreamProcess()
    {

    }
}