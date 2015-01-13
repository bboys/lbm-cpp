#include "Simulation.h"
#include "node.h"
#include <iostream>
#include <iostream>
#include <sstream>

extern "C" {
    #include "mcbsp.h"
}

namespace LBM {
    Simulation::Simulation(Domains::DomainInitializer *initializer)
    :
        d_domain(initializer->domain())
    {

    }

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
        // if (bsp_pid() == 0)
        //     std::cout << "\n\nBefore streaming\n";

        // bsp_sync();
        // report();

        stream(d_domain->set, d_domain->nodes);
        // if (bsp_pid() == 0)
        //     std::cout << "\n\nAfter streaming\n";

        // // bsp_sync();
        // // report();
        // // bsp_sync();
        // std::cout << "\n\n";
        communicate(d_domain->messengers);
        // if (bsp_pid() == 0)
        //     std::cout << "\n\nAfter communicating\n";

        // bsp_sync();
        // report();
        postStreamProcess();
        collission(d_domain->set, d_domain->nodes);
        // if (bsp_pid() == 0)
        //     std::cout << "\n\nAfter collission\n\n\n";

        // bsp_sync();
        // report();
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

    void Simulation::postStreamProcess()
    {
        for (size_t idx = 0; idx < d_domain->post_processors.size(); ++idx)
            d_domain->post_processors[idx]->process();
    }

    void Simulation::report()
    {
        size_t total_p = bsp_nprocs();
        size_t s = bsp_pid();
        double *densities = new double[total_p]();
        double current_density = 0;
        bsp_push_reg(densities,total_p * sizeof(double));
        bsp_sync();

        for (auto node : d_domain->nodes)
            current_density += density(d_domain->set, node);

        // std::cout << "Denstiy from processor " << s << ": " << current_density << '\n';
        // send density to each processor
        for (size_t t = 0; t < total_p; t++)
            bsp_put(t, &current_density, densities, s * sizeof(double), sizeof(double));

        bsp_sync();
        // now calculate the total density
        double total_density = 0;
        for (size_t t = 0; t < total_p; t++)
            total_density += densities[t];

        bsp_pop_reg(densities);

        if (s == 0)
            std::cout << "Total density: " << total_density << '\n';
    }

    void Simulation::report(::Reporting::MatlabReporter reporter)
    {
        // reporter.reportOnTimeStep(d_domain->set, d_domain->nodes);
    }

    void Simulation::communicate(std::vector<Messenger> messengers)
    {
        // we'll send both the local idx and the direction which we we'll change
        size_t tag_size = sizeof(size_t[2]);
        bsp_set_tagsize(&tag_size);
        bsp_sync();

        std::stringstream ss;
        size_t s = bsp_pid();
        for (auto messenger : messengers)
        {
            ss << "Message from: " << s << " to: " << messenger.d_p <<
                " with direction: " << messenger.d_tag[1] <<
                " to local idx: " << messenger.d_tag[0] <<
                " and source: " << messenger.d_src;
            auto tag = messenger.d_tag;

            ss << " with tag: " << tag[0] << ", " << tag[1] << '\n';
            ss << "The tags have size: " << sizeof(tag[0]) << " and " << sizeof(tag[1]) << '\n';
            bsp_send(messenger.d_p, messenger.d_tag, &messenger.d_src, sizeof(double));
        }
        // std::cout << ss.str();

        // ss.clear();
        // ss.str("");

        bsp_sync();

        unsigned int nmessages = 0;
        size_t nbytes = 0;
        bsp_qsize(&nmessages, &nbytes);

        ss << "\nProcessor " << s << " received " << nmessages << " messages totalling " << nbytes << " bytes\n";

        for (size_t n = 0; n < nmessages; ++n)
        {
            size_t i[2];
            size_t status;
            double distribution = 0;

            bsp_get_tag(&status,&i);

            size_t idx = i[0];
            size_t dir = i[1];

            if (status > 0)
            {
                bsp_move(&distribution, sizeof(double));
                ss << "Idx: " << i[0] << "trying something: "  << " with status: " << status << " and payload: " << distribution << '\n';
                d_domain->nodes[idx].distributions[dir].nextValue = distribution;

            }
        }
        ss << '\n';
        // std::cout << ss.str();
        ss.clear();
        ss.str("");

        bsp_sync();
    }
}