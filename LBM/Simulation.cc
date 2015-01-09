#include "Simulation.h"

#include "node.h"
#include <iostream>
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

    void Simulation::step()
    {
        // report();
        stream(d_domain->set, d_domain->nodes);
        // report();
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

    void Simulation::collission(VelocitySet *set, std::vector<Node> &nodes)
    {
        double omega = d_domain->omega;
        for (auto node : nodes)
        {
            size_t nDirections = set->nDirections;
            // switch to the newly streamed distribution values
            for (size_t dir = 0; dir < nDirections; ++dir)
                node.distributions[dir].value = node.distributions[dir].nextValue;

            // apply BGK approximation
            double * node_equilibrium = equilibrium(set, node);
            for (size_t dir = 0; dir < nDirections; ++dir)
                node.distributions[dir].value = node.distributions[dir].value - omega *
                    (node.distributions[dir].value - node_equilibrium[dir]);

            delete[] node_equilibrium;
        }
    }

    void Simulation::postStreamProcess()
    {
        for (size_t idx = 0; idx < d_domain->post_processors.size(); ++idx)
            d_domain->post_processors[idx]->process();
    }

    void Simulation::report()
    {
        // for (auto node : d_domain->nodes)
        //     ::Reporting::reportOnDistributions(d_domain->set, node);

        ::Reporting::report(d_domain->set, &d_domain->nodes[0], d_domain->nodes.size());
    }

    void Simulation::report(::Reporting::MatlabReporter reporter)
    {
        reporter.reportOnTimeStep(d_domain->set, d_domain->nodes);
    }
}