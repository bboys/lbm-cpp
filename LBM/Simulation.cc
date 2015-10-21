#include "../config.h"
#include "Simulation.h"
#include "node.h"
#include <iostream>
#include <iostream>
#include <sstream>

#include "../LBM/parallel.h"

namespace LBM {
    Simulation::Simulation(Domains::DomainInitializer *initializer)
    :
        d_domain(initializer->domain())
    {
        // we'll send both the local idx and the direction which we we'll change
        MCBSP_BYTESIZE_TYPE tag_size = sizeof(MY_SIZE_T[2]);
        bsp_set_tagsize(&tag_size);
        bsp_sync();
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
        stream(d_domain->set, d_domain->nodes);
        communicate(d_domain->messengers);
        postStreamProcess();
        collission(d_domain->set, d_domain->nodes);
    }

    void Simulation::stream(VelocitySet *set, std::vector<Node> &nodes)
    {
        MY_SIZE_T nDirections = set->nDirections;

        // Stream each distribution to the neighbouring nodes
        // since neighbour is a pointer to a boolean we only have to assign a new value
        // to the neighbour
        for (MY_SIZE_T idx = 0; idx < nodes.size(); ++idx)
            for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
                if (nodes[idx].distributions[dir].neighbour != nullptr)
                    *nodes[idx].distributions[dir].neighbour = nodes[idx].distributions[dir].value;
    }

    void Simulation::collission(VelocitySet *set, std::vector<Node> &nodes)
    {
        double omega = d_domain->omega;
        for (auto node : nodes)
        {
            MY_SIZE_T nDirections = set->nDirections;
            // switch to the newly streamed distribution values
            for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
            {
                node.distributions[dir].value = node.distributions[dir].nextValue;
                node.distributions[dir].nextValue = -1;
            }

            // apply BGK approximation
            auto node_equilibrium = equilibrium(set, node);
            for (MY_SIZE_T dir = 0; dir < nDirections; ++dir)
                node.distributions[dir].value = node.distributions[dir].value -
                    omega * (node.distributions[dir].value - node_equilibrium[dir]);

            delete[] node_equilibrium;
        }
    }

    void Simulation::postStreamProcess()
    {
        for (MY_SIZE_T idx = 0; idx < d_domain->post_processors.size(); ++idx)
            d_domain->post_processors[idx]->process();
    }


    void Simulation::report(::Reporting::MatlabReporter reporter)
    {
        // reporter.reportOnTimeStep(d_domain->set, d_domain->nodes);
    }

    void Simulation::communicate(std::vector<Messenger> messengers)
    {
        for (auto messenger : messengers)
            bsp_send(messenger.d_p, messenger.d_tag, &messenger.d_src, sizeof(double));
        bsp_sync();

        MCBSP_NUMMSG_TYPE nmessages = 0;
        MCBSP_BYTESIZE_TYPE nbytes = 0;
        bsp_qsize(&nmessages, &nbytes);
        for (MCBSP_NUMMSG_TYPE n = 0; n < nmessages; ++n)
        {
            MY_SIZE_T i[2];
            MCBSP_BYTESIZE_TYPE status;
            bsp_get_tag(&status,&i); // i[0] = idx, i[1] = dir
            if (status > 0)
            {
                double distribution = 0;
                bsp_move(&distribution, sizeof(double));
                d_domain->nodes[i[0]].distributions[i[1]].nextValue = distribution;
            }
        }
        // bsp_sync();
    }

    void Simulation::report()
    {
        MY_SIZE_T total_p = bsp_nprocs();
        MY_SIZE_T s = bsp_pid();
        double *densities = new double[total_p]();
        double current_density = 0;
        bsp_push_reg(densities,total_p * sizeof(double));
        bsp_sync();

        for (auto node : d_domain->nodes)
            current_density += density(d_domain->set, node);

        // send density to each processor
        for (MY_SIZE_T t = 0; t < total_p; t++)
            bsp_put(t, &current_density, densities, s * sizeof(double), sizeof(double));

        bsp_sync();
        // now calculate the total density
        double total_density = 0;
        for (MY_SIZE_T t = 0; t < total_p; t++)
            total_density += densities[t];

        bsp_pop_reg(densities);

        if (s == 0)
            std::cout << "Total density: " << total_density << '\n';

        delete[] densities;
    }

}